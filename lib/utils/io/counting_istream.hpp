#ifndef GENESIS_UTILS_IO_COUNTING_ISTREAM_H_
#define GENESIS_UTILS_IO_COUNTING_ISTREAM_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief Header of CountingIstreamIterator class.
 *
 * @file
 * @ingroup utils
 */

#include <assert.h>
#include <iterator>
#include <stdexcept>
#include <streambuf>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Counting Istream
// =================================================================================================

/**
 * @brief Wrapper class to iterate over an `std::istream` while keeping track of lines and columns.
 *
 * It provides similar functionality to `std::istream`, but has a different way of handling the
 * stream and characters. The main differences are:
 *
 *   * The stream is not automatically advanced after reading a char. This is because otherwise
 *     the line and column would already point to the next char while processing the last.
 *     Thus, advance() or the increment operator++() have to be called to get to the next char in
 *     the stream.
 *   * The handling of line feed chars (LF or `\n`, as used in Unix-like systems) and carriage
 *     return chars (CR or `\r`, which are the new line delimiters in many Mac systems, and which
 *     are part of the CR+LF new lines as used in Windows) is different.
 *     Both, CR and LF chars (and the whole CR+LF combination), are turned into single line feed
 *     chars (`\n`) in this iterator. This ensures that all new lines delimiters are internally
 *     represented as one LF, independently of the file format. That makes parsing way easier.
 *
 * It has two member functions line() and column() that return the corresponding values for the
 * current iterator position. Also, at() can be used to get a textual representation of the current
 * position. The member function current() furthermore provides a checked version of the
 * dereference operator.
 */
class CountingIstream
{
public:

    // -------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------

    using self_type         = CountingIstream;
    using value_type        = char;

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    /**
     * @brief Default constructor. Creates an empty stream that cannot be read from.
     */
    CountingIstream()
        : it_()
        , eos_()
        , current_('\0')
        , line_(0)
        , column_(0)
    {}

    /**
     * @brief Constructor that takes an `std::istream` reference as streaming input.
     */
    explicit CountingIstream(std::istream& in)
        : it_(in)
        , eos_()
        , line_(1)
        , column_(1)
    {
        if( eos() ) {
            current_ = '\0';
            line_    = 0;
            column_  = 0;
        } else if( *it_ == '\r') {
            current_ = '\n';
        } else {
            current_ = *it_;
        }
    }

    /**
     * @brief Constructor that takes an `std::streambuf` pointer as streaming input.
     */
    explicit CountingIstream(std::streambuf* in_buf)
        : it_(in_buf)
        , eos_()
        , line_(1)
        , column_(1)
    {
        if( eos() ) {
            current_ = '\0';
            line_    = 0;
            column_  = 0;
        } else if( *it_ == '\r') {
            current_ = '\n';
        } else {
            current_ = *it_;
        }
    }

    /**
     * @brief Default destructor.
     */
    ~CountingIstream() = default;

    /**
     * @brief Copy constructor. Deleted to ensure correct line/column counting.
     */
    CountingIstream(self_type const&) = delete;

    /**
     * @brief Move constructor.
     */
    CountingIstream(self_type&&)      = default;

    /**
     * @brief Copy assignment. Deleted to ensure correct line/column counting.
     */
    self_type& operator= (self_type const&) = delete;

    /**
     * @brief Move assignment.
     */
    self_type& operator= (self_type&&)      = default;

    void swap (self_type& other) noexcept
    {
        using std::swap;
        swap(it_,     other.it_);
        swap(line_,   other.line_);
        swap(column_, other.column_);
    }

    // -------------------------------------------------------------
    //     Stream Operations
    // -------------------------------------------------------------

    /**
     * @brief Dereference operator. Return the current char.
     */
    char operator * () const
    {
        return current_;
    }

    /**
     * @brief Return the current char, with char check.
     *
     * This function is similar to the dereference operator, but additionally performs a check of
     * the current char: This iterator is meant for ASCII (or similar) text format encodings, and
     * its output should be usable for lookup tables etc. Thus, this function ensures that the char
     * is in the range [0, 127]. If not, an `std::domain_error` is thrown.
     */
    char current() const
    {
        if( eos() ) {
            throw std::runtime_error( "Unexpected end of file at " + at() + "." );
        }
        if( current_ < 0 ) {
            throw std::domain_error( "Invalid input char at " + at() + "." );
        }
        return current_;
    }

    /**
     * @brief Move to the next char in the stream and advance the counters.
     */
    void advance()
    {
        ++column_;

        // Check whether we marked the current char as a line break (remember: all \r chars are
        // turned into \n for the outside world, i.e., in current_).
        if( current_ == '\n' ) {

            // Adjust counters. The current (not yet advanced) char is a line break, so the next
            // one will be on a new line.
            ++line_;
            column_ = 1;

            // We need to further check whether the char actually was an \r (before being
            // internally changed to an \n - so we need to check the stream again). If so, we have
            // either a Mac or a Windows file and need to act accordingly.
            // What follows is a bit of code duplication, but this seemed easier and probably faster
            // than introducing a flag variable and checking it later.
            if( *it_ == '\r' ) {

                // Read and store next char.
                ++it_;
                current_ = *it_;

                // If the next char (after the \r that we just had) is another \r, that means we
                // have a Mac file and there are two consecutive new lines, so treat that.
                if( current_ == '\r' ) {
                    current_ = '\n';
                    return;

                // If it is just a normal char, we also have a Mac file, and simply continue
                // with the first char of the next line.
                } else if( current_ != '\n' )  {
                    return;
                }

                // Else (that is, if both previous conditions fail), it must be an \n char.
                // That means, we came from an \r, now are at an \n, so it's a Windows line break.
                // We then simply continue to read the next char as usual, so that in effect,
                // the \n is skipped.
                assert( current_ == '\n' );
            }
        }

        // Read and store next char.
        ++it_;
        current_ = *it_;

        // Turn \r into \n (for the outside world - internally, *it_ is still pointing to \r, which
        // will be used in the next call of this function to determine how to proceed).
        if( current_ == '\r' ) {
            current_ = '\n';
        }
    }

    /**
     * @brief Move to the next char in the stream. Shortcut for advance().
     */
    self_type& operator ++ ()
    {
        advance();
        return *this;
    }

    // -------------------------------------------------------------
    //     State
    // -------------------------------------------------------------

    /**
     * @brief Return the current line of the input stream.
     *
     * The counter starts with line 1 for input stream. The value for default constructed objects
     * (no stream provided) is 0, to be able to distinguish it from actual streams.
     */
    size_t line() const
    {
        return line_;
    }

    /**
    * @brief Return the current column of the input stream.
    *
    * The counter starts with column 1 for each line of the input stream. The value for default
    * constructed objects (no stream provided) is 0, to be able to distinguish it from actual
    * streams. New line characters `\n` are included in counting and count as the last character
    * of a line.
    */
    size_t column() const
    {
        return column_;
    }

    /**
     * @brief Return a textual representation of the current input position in the form
     * "line:column".
     */
    std::string at() const
    {
        return std::to_string( line_ ) + ":" + std::to_string( column_ );
    }

    /**
     * @brief Return true iff the input is good (not eof) and can be read from.
     */
    bool good() const
    {
        return it_ != eos_;
    }

    /**
     * @brief Return true iff the input is good (not eof) and can be read from. Shortcut for good().
     */
    explicit operator bool() const
    {
        return it_ != eos_;
    }

    /**
    * @brief Return true iff the input reached its end.
    */
    bool eos() const
    {
        return it_ == eos_;
    }

    // -------------------------------------------------------------
    //     Comparators
    // -------------------------------------------------------------

    /**
     * @brief Compare two CountingIstream%s for equality.
     *
     * This function compares the underlying buffers for equality. That is, two CountingIstream%s
     * are considered equal if either both of them are at the end of their stream or neither is.
     * Thus, iterators on different streams or with different current chars can still be considered
     * equal. This equality is hence mostly useful to test for end-of-stream in loops reading
     * from the stream.
     */
    bool operator == ( self_type const& other ) const
    {
        return it_ == other.it_;
    }

    /**
     * @brief Compare two CountingIstream%s for inequality.
     *
     * See operator==() for details.
     */
    bool operator != ( self_type const& other ) const
    {
        return !( *this == other );
    }

    // -------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------

private:

    std::istreambuf_iterator<char> it_;
    std::istreambuf_iterator<char> eos_;

    char   current_;
    size_t line_;
    size_t column_;
};

} // namespace utils
} // namespace genesis

#endif // include guard

#ifndef GENESIS_UTILS_IO_COUNTING_ISTREAM_ITERATOR_H_
#define GENESIS_UTILS_IO_COUNTING_ISTREAM_ITERATOR_H_

/**
 * @brief Header of CountingIstreamIterator class.
 *
 * @file
 * @ingroup utils
 */

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
 * @brief Wrapper class to iterate over an std::istream while keeping track of lines and columns.
 *
 * It provides similar functionality to std::istream, but has a different way of handling the
 * stream and characters. The main differences are:
 *
 *   * The stream is not automatically advanced after reading a char. This is because otherwise
 *     the line and column would already point to the next char while processing the last.
 *     Thus, advance() or the increment operator have to be called to get to the next char in the
 *     stream.
 *   * The handling of '\r' characters (part of the CR+LF new lines as used in Windows). Those are
 *     completely skipped in this iterator. This ensures that all new lines are simply represented
 *     as '\n' independent of the file format.
 *
 * It has two member functions line() and column() that return the corresponding values for the
 * current iterator position and a member function current() that provides a checked version of the
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

    CountingIstream()
        : it_()
        , eof_()
        , current_('\0')
        , line_(0)
        , column_(0)
    {}

    explicit CountingIstream(std::istream& in)
        : it_(in)
        , eof_()
        , line_(1)
        , column_(1)
    {
        if( eof() ) {
            current_ = '\0';
        } else {
            current_ = *it_;
        }
    }

    explicit CountingIstream(std::streambuf* in_buf)
        : it_(in_buf)
        , eof_()
        , line_(1)
        , column_(1)
    {
        if( eof() ) {
            current_ = '\0';
        } else {
            current_ = *it_;
        }
    }

    ~CountingIstream() = default;

    // Copy construction deleted to ensure correct line/column counting!
    CountingIstream(self_type const&) = delete;
    CountingIstream(self_type&&)      = default;

    // Copy construction deleted to ensure correct line/column counting!
    self_type& operator= (self_type const&) = delete;
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
        if( eof() ) {
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

        // Treat new lines always as a simple '\n' internally.
        // This works for both unix and Win formatted files. It does not work for Mac OS <= 9,
        // as this uses a single \r for delimiting lines, thus the line and column counter will
        // never be increased in this case and the whitespace not outputted.
        // Maybe in the future we could provide some compile time handling of this issue, but for
        // now, this solution should work in most cases.

        if (current_ == '\r') {
            ++it_;
        }
        if (current_ == '\n') {
            ++line_;
            column_ = 1;
        }

        ++it_;
        current_ = *it_;
    }

    /**
     * @brief Move to the next char in the stream. Shortcut for advance().
     */
    self_type& operator ++ ()
    {
        // Override line counting for speed:
        // ++it_;
        // current_ = *it_;

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
        return it_ != eof_;
    }

    /**
     * @brief Return true iff the input is good (not eof) and can be read from. Shortcut for good().
     */
    explicit operator bool() const
    {
        return it_ != eof_;
    }

    /**
    * @brief Return true iff the input reached its end.
    */
    bool eof() const
    {
        return it_ == eof_;
    }

    // -------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------

private:

    std::istreambuf_iterator<char> it_;
    std::istreambuf_iterator<char> eof_;

    char   current_;
    size_t line_;
    size_t column_;
};

} // namespace utils
} // namespace genesis

#endif // include guard

#ifndef GENESIS_UTILS_IO_INPUT_STREAM_H_
#define GENESIS_UTILS_IO_INPUT_STREAM_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/input_reader.hpp"
#include "genesis/utils/io/input_source.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <utility>

namespace genesis {
namespace utils {

// =================================================================================================
//     Input Stream
// =================================================================================================

/**
 * @brief Stream interface for reading data from an @link BaseInputSource InputSource@endlink,
 * that keeps track of line and column counters.
 *
 * This class provides similar functionality to `std::istream`, but has a different way of handling
 * the stream and characters. The main differences are:
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
 *
 * Implementation details inspired by
 * [fast-cpp-csv-parser](https://github.com/ben-strasser/fast-cpp-csv-parser) by Ben Strasser,
 * see also @link supplement_acknowledgements_code_reuse_input_stream Acknowledgements@endlink.
 */
class InputStream
{
public:

    // -------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------

    /**
     * @brief Block length for internal buffering.
     *
     * The buffer uses three blocks of this size (4MB each).
     * This is also the maximum line length that can be read at a time with get_line().
     * If this is too short, change the BlockLength.
     */
    static const size_t BlockLength = 1 << 22;

    using self_type         = InputStream;
    using value_type        = char;

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    InputStream()
        : source_name_( "invalid source" )
        , buffer_(   nullptr )
        , data_pos_( 0 )
        , data_end_( 0 )
        , current_( '\0' )
        , line_(     0 )
        , column_(   0 )
    {}

    explicit InputStream( std::shared_ptr<BaseInputSource> input_source )
        : line_(   1 )
        , column_( 1 )
    {
        init_( input_source );
    }

    ~InputStream()
    {
        delete[] buffer_;
        buffer_ = nullptr;
    }

    InputStream( self_type const& ) = delete;
    InputStream( self_type&& other )
        : buffer_( nullptr )
    {
        *this = std::move( other );
    }

    self_type& operator= ( self_type const& ) = delete;
    self_type& operator= ( self_type&& other );

    // -------------------------------------------------------------
    //     Char Operations
    // -------------------------------------------------------------

    /**
     * @brief Dereference operator. Return the current char.
     */
    char operator* () const
    {
        return current_;
    }

    /**
     * @brief Return the current char, with some checks.
     *
     * This function is similar to the dereference operator, but additionally performs two checks:
     *
     *  *  End of input: If this function is called when there is no more data left in the input,
     *     it throws an `runtime_error`.
     *  *  Current char: This iterator is meant for ASCII (or similar) text format encodings with
     *     single bytes, and its output should be usable for lookup tables etc. Thus, this function
     *     ensures that the char is in the range [0, 127]. If not, an `std::domain_error` is thrown.
     *
     * Usually, those two conditions are checked in the parser anyway, so in most cases it is
     * preferred to use the @link operator*() dereference operator@endlink instead.
     */
    char current() const;

    /**
     * @brief Move to the next char in the stream and advance the counters.
     */
    self_type& advance();

    /**
     * @brief Move to the next char in the stream. Shortcut for advance().
     */
    self_type& operator ++ ()
    {
        advance();
        return *this;
    }

    /**
     * @brief Extract a single char from the input.
     *
     * Return the current char and move to the next one.
     */
    char get_char()
    {
        char ret = current_;
        advance();
        return ret;
    }

    // -------------------------------------------------------------
    //     Line Operations
    // -------------------------------------------------------------

    /**
     * @brief Read the current line, append it to the @p target, and move to the beginning of the
     * next line.
     *
     * The function finds the end of the current line, starting from the current position,
     * and appends the content to the given @p target, excluding the trailing new line char(s).
     * The stream is left at the first char of the next line.
     */
    void get_line( std::string& target );

    /**
     * @brief Read the current line and move to the beginning of the next.
     *
     * The function finds the end of the current line, starting from the current position,
     * and returns the content, excluding the trailing new line char(s).
     * The stream is left at the first char of the next line.
     */
    std::string get_line()
    {
        std::string result;
        get_line( result );
        return result;
    }

    // -------------------------------------------------------------
    //     State
    // -------------------------------------------------------------

    /**
     * @brief Return the current line of the input stream.
     *
     * The counter starts with line 1 for input stream.
     */
    size_t line() const
    {
        return line_;
    }

    /**
    * @brief Return the current column of the input stream.
    *
    * The counter starts with column 1 for each line of the input stream.
    * New line characters `\n` are included in counting and count as the last character of a line.
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
     * @brief Return true iff the input is good (not end of data) and can be read from.
     */
    bool good() const
    {
        return data_pos_ < data_end_;
    }

    /**
     * @brief Return true iff the input is good (not end of data) and can be read from.
     * Shortcut for good().
     */
    explicit operator bool() const
    {
        return data_pos_ < data_end_;
    }

    /**
    * @brief Return true iff the input reached its end.
    */
    bool eof() const
    {
        return data_pos_ >= data_end_;
    }

    /**
     * @brief Get the input source name where this stream reads from.
     *
     * Depending on the type of input, this is either
     *
     *  *  "input string",
     *  *  "input stream" or
     *  *  "input file <filename>"
     *
     * This is mainly useful for user output like log and error messages.
     */
    std::string source_name() const
    {
        return source_name_;
    }

    // -------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------

private:

    /**
     * @brief Set the column, line and currenct char to zero values.
     */
    void reset_();

    /**
     * @brief Refill the buffer blocks if necessary.
     */
    void update_blocks_();

    /**
     * @brief Helper function that does some checks on the current char and sets it to what
     * is at `data_pos_` in the `buffer_`.
     */
    void set_current_char_();

    /**
     * @brief Init the buffers and the state of this object.
     */
    void init_( std::shared_ptr<BaseInputSource> input_source );

    // -------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------

private:

    // Input data comes from here...
    // (we use a unique ptr to make the class movable)
    std::unique_ptr<InputReader> input_reader_ = nullptr;
    std::string source_name_;

    // ...and is buffered here.
    char*  buffer_;
    size_t data_pos_;
    size_t data_end_;

    // Also, keep track of the current char and position in the data.
    char   current_;
    size_t line_;
    size_t column_;
};

} // namespace utils
} // namespace genesis

#endif // include guard

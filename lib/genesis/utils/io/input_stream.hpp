#ifndef GENESIS_UTILS_IO_INPUT_STREAM_H_
#define GENESIS_UTILS_IO_INPUT_STREAM_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
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
#include "genesis/utils/text/char.hpp"

#include <cassert>
#include <cstdint>
#include <functional>
#include <memory>
#include <stdexcept>
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

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    /**
     * @brief Block length for internal buffering.
     *
     * The buffer uses three blocks of this size (4MB each).
     */
    static const size_t BlockLength = 1 << 22;

    using self_type         = InputStream;
    using value_type        = char;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

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

    // -------------------------------------------------------------------------
    //     Stream Iterator Operations
    // -------------------------------------------------------------------------

    /**
     * @brief Dereference operator. Return the current char.
     */
    inline char operator* () const
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
    inline char current() const
    {
        if( data_pos_ >= data_end_ ) GENESIS_UNLIKELY {
            throw std::runtime_error(
                "Unexpected end of " + source_name() + " at " + at() + "."
            );
        }
        if( current_ < 0 ) GENESIS_UNLIKELY {
            throw std::domain_error(
                "Invalid input char in " + source_name() + " at " + at() + "."
            );
        }
        return current_;
    }

    /**
     * @brief Move to the next char in the stream and advance the counters.
     */
    inline self_type& advance()
    {
        operator++();
        return *this;
    }

    /**
     * @brief Move to the next char in the stream. Shortcut for advance().
     */
    inline self_type& operator ++ ()
    {
        // If we were already at the end, set counter so zero.
        if( data_pos_ >= data_end_ ) GENESIS_UNLIKELY {
            reset_();
            return *this;
        }

        // Read data if necessary.
        if( data_pos_ >= BlockLength ) GENESIS_UNLIKELY {
            update_blocks_();
        }
        assert( data_pos_ < BlockLength );

        // In case we are moving to a new line, set the counters accordingly.
        if( current_ == '\n' ) {
            ++line_;
            column_ = 1;
        } else {
            ++column_;
        }

        // Next position.
        ++data_pos_;

        // Set the char.
        set_current_char_();
        return *this;
    }

    /**
     * @brief Extract a single char from the input.
     *
     * Return the current char and move to the next one.
     */
    inline char get_char()
    {
        char ret = current_;
        operator++();
        return ret;
    }

    // -------------------------------------------------------------------------
    //     Line Operations
    // -------------------------------------------------------------------------

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

    // -------------------------------------------------------------------------
    //     Char Operations
    // -------------------------------------------------------------------------

    /**
     * @brief Lexing function that reads a single char from the stream and checks whether it equals
     * the provided one.
     *
     * If not, the function throws `std::runtime_error`. The stream is advanced by one position and
     * the char is returned. For a similar function that checks the value of the current char but
     * does not advance in the stream, see affirm_char_or_throw().
     */
    inline char read_char_or_throw( char const criterion )
    {
        // Check char and move to next.
        if( data_pos_ >= data_end_ || current_ != criterion ) GENESIS_UNLIKELY {
            throw std::runtime_error(
                std::string("In ") + source_name() + ": " +
                "Expecting " + char_to_hex( criterion ) + " at " + at() + ", " +
                "but received " + char_to_hex( current_ ) + " instead."
            );
        }
        assert( good() && current_ == criterion );
        operator++();
        return criterion;
    }

    /**
     * @brief Lexing function that reads a single char from the stream and checks whether it
     * fulfills the provided criterion.
     *
     * If not, the function throws `std::runtime_error`. The stream is advanced by one position and
     * the char is returned. For a similar function that checks the value of the current char but
     * does not advance in the stream, see affirm_char_or_throw().
     */
    inline char read_char_or_throw( std::function<bool (char)> criterion )
    {
        // Check char and move to next.
        if( data_pos_ >= data_end_ || !criterion( current_ )) GENESIS_UNLIKELY {
            throw std::runtime_error(
                std::string("In ") + source_name() + ": " +
                "Unexpected char " + char_to_hex( current_ ) + " at " + at() + "."
            );
        }
        assert( good() );
        auto const chr = current_;
        operator++();
        return chr;
    }

    // -------------------------------------------------------------------------
    //     State
    // -------------------------------------------------------------------------

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
    inline bool good() const
    {
        return data_pos_ < data_end_;
    }

    /**
     * @brief Return true iff the input is good (not end of data) and can be read from.
     * Shortcut for good().
     */
    inline explicit operator bool() const
    {
        return data_pos_ < data_end_;
    }

    /**
    * @brief Return true iff the input reached its end.
    */
    inline bool eof() const
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

    /**
     * @brief Direct access to the internal buffer.
     *
     * This function returns a pointer to the internal buffer, as well as the length (past the end)
     * that is currently buffered. This is meant for special file parsers that can optimize better
     * when using this - but it is highly dangerous to use if you do not know what you are doing!
     *
     * The idea is as follows: With access to the buffer, parse data as needed, keeping track of
     * how many chars have been processed. Then, use the jump_unchecked() function to update this
     * stream to the new position of the stream (the char after the last one being processed by
     * the parsing).
     *
     * Caveat: Never parse and jump across new line characters (or, for that matter, carriage return
     * characters, which won't be automatically converted when using the buffer directly)! This
     * would invalidate the line counting!
     *
     * Caveat: Never read after the end of the buffer, that is, never access the char at the
     * returned last position `buffer + length` or after!
     */
    std::pair<char const*, size_t> buffer()
    {
        assert( data_pos_ <= data_end_ );
        return { &buffer_[ data_pos_ ], data_end_ - data_pos_ };
    }

    /**
     * @brief Jump forward in the stream by a certain amount of chars.
     *
     * This is meant to update the stream position after using buffer() for direct parsing.
     * See the caveats there!
     *
     * In particular, we can never jump behind the current buffer end, and shall not jump across
     * new lines. That is, this function is not meant as a way to jump to an arbitrary (later)
     * position in a file!
     */
    void jump_unchecked( size_t n )
    {
        // Safety first! We do a single check here, so that in the default case,
        // we only branch once - assuming that the compiler doesn't optimize that even better anway.
        if( data_pos_ + n >= data_end_ ) {
            if( data_pos_ + n == data_end_ ) {
                // Lazy approach to make sure that all functions are called as expected
                // when reaching the end of the input data.
                assert( data_pos_ < data_end_ );
                assert( n > 0 );
                data_pos_ += n - 1;
                column_   += n - 1;
                advance();
                return;
            }

            // We try to jump past the end
            assert( data_pos_ + n > data_end_ );
            throw std::runtime_error(
                "Invalid InputStream jump to position after buffer end."
            );
        }

        // Update the position as neeeded.
        data_pos_ += n;
        column_ += n;
        if( data_pos_ >= BlockLength ) {
            update_blocks_();
        }
        set_current_char_();
    }

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    /**
     * @brief Init the buffers and the state of this object.
     */
    void init_( std::shared_ptr<BaseInputSource> input_source );

    /**
     * @brief Set the column, line and currenct char to zero values.
     */
    void reset_()
    {
        line_    = 0;
        column_  = 0;
        current_ = '\0';
    }

    /**
     * @brief Refill the buffer blocks if necessary.
     */
    void update_blocks_();

    /**
     * @brief Helper function that does some checks on the current char and sets it to what
     * is at `data_pos_` in the `buffer_`.
     */
    inline void set_current_char_()
    {
        // Check end of stream conditions.
        if( data_pos_ >= data_end_ ) GENESIS_UNLIKELY {
            // We do not expect to overshoot. Let's assert this, but if it still happens
            // (in release build), we can also cope, and will just set \0 as the current char.
            assert( data_pos_ == data_end_ );

            if( data_pos_ == data_end_ && data_pos_ > 0 && buffer_[ data_pos_ - 1 ] != '\n' ) {
                // If this is the end of the data, but there was no closing \n, add one.
                buffer_[ data_pos_ ] = '\n';
                ++data_end_;
            } else {
                // If we reached the end, do not fully reset the line and column counters.
                // They might be needed in some parser.
                current_ = '\0';
                return;
            }
        }

        // Treat stupid Windows and Mac lines breaks. Set them to \n, so that downstream parsers
        // don't have to deal with this.
        if( buffer_[ data_pos_ ] == '\r' ) {
            buffer_[ data_pos_ ] = '\n';

            // If this is a Win line break \r\n, skip one of them, so that only a single \n
            // is visible to the outside. We do not treat \n\r line breaks properly here!
            // If any system still uses those, we'd have to change code here.
            if( data_pos_ + 1 < data_end_ && buffer_[ data_pos_ + 1 ] == '\n' ) {
                ++data_pos_;
            }
        }

        // Set the char.
        current_ = buffer_[ data_pos_ ];
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

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

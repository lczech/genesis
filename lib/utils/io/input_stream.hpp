#ifndef GENESIS_UTILS_IO_INPUT_STREAM_H_
#define GENESIS_UTILS_IO_INPUT_STREAM_H_

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
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "utils/io/input_source.hpp"
#include "utils/io/input_reader.hpp"

#include <assert.h>
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
 * [fast-cpp-csv-parser](https://github.com/ben-strasser/fast-cpp-csv-parser) by Ben Strasser.
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
     * The buffer uses three blocks of this size (16MB each).
     * This is also the maximum line length that can be read at a time with get_line().
     * If this is too short, change the BlockLength.
     */
    static const size_t BlockLength = 1 << 24;

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

    explicit InputStream( std::unique_ptr<BaseInputSource> input_source )
        : line_(   1 )
        , column_( 1 )
    {
        init_( std::move( input_source ));
    }

    ~InputStream()
    {
        delete[] buffer_;
        buffer_ = nullptr;
    }

    InputStream(self_type const&) = delete;
    InputStream(self_type&&)      = delete;

    self_type& operator= (self_type const&) = delete;
    self_type& operator= (self_type&&)      = delete;

    // -------------------------------------------------------------
    //     Char Operations
    // -------------------------------------------------------------

    /**
     * @brief Dereference operator. Return the current char.
     */
    char operator * () const
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
    char current() const
    {
        if( data_pos_ >= data_end_ ) {
            throw std::runtime_error(
                "Unexpected end of " + source_name() + " at " + at() + "."
            );
        }
        if( current_ < 0 ) {
            throw std::domain_error(
                "Invalid input char in " + source_name() + " at " + at() + "."
            );
        }
        return current_;
    }

    /**
     * @brief Move to the next char in the stream and advance the counters.
     */
    self_type& advance()
    {
        // If we were already at the end, set counter so zero.
        if( data_pos_ >= data_end_ ) {
            reset_();
            return *this;
        }

        // Read data if necessary.
        update_blocks_();

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
     * @brief Return the current line and move to the beginning of the next.
     *
     * The function finds the end of the current line, starting from the current position.
     * It returns a pointer to the current position and the length of the line. Furthermore,
     * a null char is set at the end of the line, replacing the new line char.
     * This allows downstream parses to directly use the returned pointer as a c-string.
     *
     * The stream is left at the first char of the next line.
     */
    std::pair< char*, size_t > get_line()
    {
        if( data_pos_ >= data_end_ ) {
            return { nullptr, 0 };
        }

        // Read data if necessary.
        update_blocks_();

        // Find the end of the line.
        size_t line_end = data_pos_;
        while( line_end != data_end_       &&
               buffer_[ line_end ] != '\n' &&
               buffer_[ line_end ] != '\r'
        ) {
            ++line_end;
        }

        // If the line is too long, throw.
        if( line_end - data_pos_ + 1 > BlockLength ) {
            throw std::runtime_error( "Input line too long in " + source_name() + " at " + at() );
        }

        // Set the end of the line to \0, so that downstream parses can work with it.
        if( buffer_[ line_end ] == '\n' ) {
            buffer_[ line_end ] = '\0';

        } else if( buffer_[ line_end ] == '\r' ) {
            buffer_[ line_end ] = '\0';

            // Treat stupid Windows \r\n lines breaks.
            if( line_end + 1 < data_end_ && buffer_[ line_end + 1 ] == '\n' ) {
                ++line_end;
                buffer_[ line_end ] = '\0';
            }
        } else {

            // Files might be missing the line break at the end of the last line. Add it.
            assert( line_end == data_end_ );
            ++data_end_;
            buffer_[ line_end ] = '\0';
        }

        // Get pointer to beginning of the line, and length of the line, for returning it.
        char*  ret_ptr = buffer_ + data_pos_;
        size_t ret_len = line_end - data_pos_;

        // Move to the first char of the next line, so that future calls for reading a line or
        // char start at the right position.
        data_pos_ = line_end + 1;
        set_current_char_();

        // Set counters.
        ++line_;
        column_ = 1;

        return { ret_ptr, ret_len };
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
    void reset_()
    {
        line_    = 0;
        column_  = 0;
        current_ = '\0';
    }

    /**
     * @brief Refill the buffer blocks if necessary.
     */
    void update_blocks_()
    {
        // This function is only called from the read char and line methods, which both beforehand
        // check the following condition. So, if it breaks, this function is invalidly called
        // from somewhere else.
        assert( data_pos_ <  data_end_ );

        // If this assertion breaks, someone tempered with our internal invariants.
        assert( data_end_ <= BlockLength * 2 );

        // If we are past the first block, we need to load more data into the blocks.
        if( data_pos_ >= BlockLength ) {

            // Move the second to the first block.
            std::memcpy( buffer_, buffer_ + BlockLength, BlockLength );
            data_pos_ -= BlockLength;
            data_end_ -= BlockLength;

            // If we are not yet at the end of the data, start the reader again:
            // Copy the third block to the second, and read into the third one.
            if( input_reader_.valid() ) {
                data_end_ += input_reader_.finish_reading();
                std::memcpy( buffer_ + BlockLength, buffer_ + 2 * BlockLength, BlockLength );
                input_reader_.start_reading( buffer_ + 2 * BlockLength, BlockLength );
            }
        }
    }

    /**
     * @brief Helper function that does some checks on the current char and sets it to what
     * is at `data_pos_` in the `buffer_`.
     */
    void set_current_char_()
    {
        if( data_pos_ >= data_end_ ) {
            // If we just reached the end, do not fully reset the line and column counters.
            // They might be needed in some parser.
            current_ = '\0';
            return;
        }

        // Treat stupid Windows and Mac lines breaks. Set them to \n, so that downstream parsers
        // don't have to deal with this.
        if( buffer_[ data_pos_ ] == '\r' ) {
            buffer_[ data_pos_ ] = '\n';

            // If this is a Win line break \r\n, skip one of them, so that only a single \n
            // is visible to the outside.
            if( data_pos_ + 1 < data_end_ && buffer_[ data_pos_ + 1 ] == '\n' ) {
                ++data_pos_;
            }
        }

        // If this is the last char of the data, but there is no closing \n, add one.
        if( data_pos_ + 1 == data_end_ && buffer_[ data_pos_ ] != '\n' ) {
            ++data_end_;
            buffer_[ data_pos_ + 1 ] = '\n';
        }

        // Set the char.
        current_ = buffer_[ data_pos_ ];
    }

    /**
     * @brief Init the buffers and the state of this object.
     */
    void init_( std::unique_ptr<BaseInputSource> input_source )
    {
        // Set to empty defaults if there is no input.
        if( input_source == nullptr ) {
            source_name_ = "invalid source";

            buffer_   = nullptr;
            data_pos_ = 0;
            data_end_ = 0;

            current_ = '\0';
            line_    = 0;
            column_  = 0;
            return;
        }

        // We use three buffer blocks: one and two for the current line. The max line length is
        // one buffer length, so the beginning of the line is always in the first block, while its
        // end can reach into the second block, but never exeed it.
        // The third block is for the async reading.
        buffer_ = new char[ 3 * BlockLength ];

        try {
            // Set source name.
            source_name_ = input_source->source_name();

            // Read up to two blocks.
            data_pos_ = 0;
            data_end_ = input_source->read( buffer_, 2 * BlockLength );

            // Skip UTF-8 BOM, if found.
            if( data_end_  >= 3      &&
                buffer_[0] == '\xEF' &&
                buffer_[1] == '\xBB' &&
                buffer_[2] == '\xBF'
            ) {
                data_pos_ = 3;
            }

            // If there was no data, set to "empty" values.
            if( data_pos_ == data_end_ ) {
                reset_();

            // If there is data, set char value.
            } else {
                set_current_char_();
            }

            // If there is more data after the two blocks that we just read, start the
            // reading process (possibly async, if pthreads is available).
            if( data_end_ == 2 * BlockLength ) {
                input_reader_.init( std::move( input_source ));
                input_reader_.start_reading( buffer_ + 2 * BlockLength, BlockLength );
            }

        } catch( ... ) {
            delete[] buffer_;
            throw;
        }
    }

    // -------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------

private:

    // Input data comes from here...
    InputReader input_reader_;
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

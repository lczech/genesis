#ifndef GENESIS_UTILS_CONTAINERS_MATRIX_SIMPLE_READER_H_
#define GENESIS_UTILS_CONTAINERS_MATRIX_SIMPLE_READER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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

#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/io/parser.hpp"
#include "genesis/utils/io/scanner.hpp"
#include "genesis/utils/text/char.hpp"
#include "genesis/utils/text/convert.hpp"
#include "genesis/utils/text/string.hpp"

#include <cassert>
#include <cstdlib>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     MatrixSimpleReader
// =================================================================================================

template <typename T>
class MatrixSimpleReader
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    explicit MatrixSimpleReader( char separator_char = '\t' )
        : separator_char_( separator_char )
    {}
    ~MatrixSimpleReader() = default;

    MatrixSimpleReader(MatrixSimpleReader const&) = default;
    MatrixSimpleReader(MatrixSimpleReader&&)      = default;

    MatrixSimpleReader& operator= (MatrixSimpleReader const&) = default;
    MatrixSimpleReader& operator= (MatrixSimpleReader&&)      = default;

    // -------------------------------------------------------------
    //     Reading
    // -------------------------------------------------------------

    Matrix<T> read( std::shared_ptr<BaseInputSource> source ) const
    {
        utils::InputStream is( source );
        return parse_( is );
    }

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    char separator_char() const
    {
        return separator_char_;
    }

    bool skip_first_col() const
    {
        return skip_first_col_;
    }

    bool skip_first_row() const
    {
        return skip_first_row_;
    }

    MatrixSimpleReader& separator_char( char value )
    {
        separator_char_ = value;
        return *this;
    }

    MatrixSimpleReader& skip_first_col( bool value )
    {
        skip_first_col_ = value;
        return *this;
    }

    MatrixSimpleReader& skip_first_row( bool value )
    {
        skip_first_row_ = value;
        return *this;
    }

    /**
     * @brief Provide a function to parse an InputStream and return a value of the Matrix.
     *
     * The parser functor is expected to leave the stream at the next character, that is,
     * either the separator char, end of line, or end of file.
     */
    MatrixSimpleReader& parse_value_functor( std::function<T( utils::InputStream& )> functor )
    {
        parse_value_ = functor;
        return *this;
    }

    MatrixSimpleReader& convert_value_functor( std::function<T( std::string const& )> functor )
    {
        convert_value_ = functor;
        return *this;
    }

    // -------------------------------------------------------------
    //     Internal Functions
    // -------------------------------------------------------------

private:

    Matrix<T> parse_( utils::InputStream& input_stream ) const
    {
        auto& it = input_stream;

        // We collect data in a vector first, because resizing a Matrix is hard.
        std::vector<T> table;
        size_t cols = 0;

        // Early stop.
        if( ! it ) {
            return {};
        }

        // Skip first line if needed.
        if( skip_first_row_ ) {
            it.get_line();
        }

        // Read the whole input
        while( it ) {

            // Skip first column of needed.
            if( skip_first_col_ ) {
                while( it && *it != separator_char_ && *it != '\n' ) {
                    ++it;
                }
                assert( !it || *it == separator_char_ || *it == '\n' );
                ++it;
            }

            // Read the rest of the line into the table.
            auto const line_length = parse_line_( it, table );

            // Check that line length is consisent. Cols == 0 means we just started.
            if( cols == 0 ) {
                // Edge case, no columns found at all.
                // A Matrix with zero length colums is empty, no matter how many rows it has.
                if( line_length == 0 ) {
                    return {};
                }

                // Store the col length.
                cols = line_length;
            } else if( cols != line_length ) {
                throw std::runtime_error(
                    "In " + it.source_name() + " at " + it.at() + ": " +
                    "Different line lengths. Stareted with " + std::to_string( cols ) +
                    ", now found " + std::to_string( line_length )
                );
            }
        }

        // We cannot properly calculate dimensions of an empty matrix. So better return here.
        if( table.size() == 0 ) {
            assert( cols == 0 );
            return {};
        }

        // Make sure that the table as a matrix shape.
        if( table.size() % cols != 0 ) {
            // I'm pretty sure this could be an assertion. But better check it all the time,
            // and throw some confusing incomprehensible error message.
            throw std::runtime_error( "Matrix is not rectangluar." );
        }

        // Make a proper Matrix.
        size_t const rows = table.size() / cols;
        return Matrix<T>( rows, cols, std::move(table) );
    }

    size_t parse_line_( utils::InputStream& input_stream, std::vector<T>& table ) const
    {
        auto& it = input_stream;
        size_t cnt = 0;
        while( it && *it != '\n' ) {
            // Parse the next field.
            table.push_back( parse_field_( it ));
            ++cnt;

            // Check that everything is in order.
            if( it && *it != separator_char_ && *it != '\n' ) {
                throw std::runtime_error(
                    "In " + it.source_name() + " at " + it.at() + ": " +
                    "Unexpected char " + char_to_hex( *it )
                );
            }
            assert( !it || *it == separator_char_ || *it == '\n' );
            if( it && *it == separator_char_ ) {
                ++it;
            }
        }
        assert( !it || *it == '\n' );
        ++it;
        return cnt;
    }

    T parse_field_( utils::InputStream& input_stream ) const
    {
        T result;
        auto& it = input_stream;
        if( parse_value_ ) {
            // If we are given a complete parser function by the user, use that.
            result = parse_value_( it );
        } else {
            // Otherwise, we read to string first...
            std::string value;
            while( it && *it != separator_char_ && *it != '\n' ) {
                value += *it;
                ++it;
            }

            // ... and then convert to the value type we want.
            if( convert_value_ ) {
                // If we are given a conversion function by the user, use that.
                result = convert_value_( value );
            } else {
                // Otherwise, use a generic one, that utilizes stream input.
                // Expensive, but works.
                try {
                    result = convert_from_string<T>( value );
                } catch(...) {
                    throw std::runtime_error(
                        "In " + it.source_name() + " at " + it.at() + ": " +
                        "Invalid value \"" + value + "\""
                    );
                }
            }
        }
        return result;
    }

    // -------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------

private:

    char separator_char_ = '\t';
    bool skip_first_row_ = false;
    bool skip_first_col_ = false;

    std::function<T( utils::InputStream& )> parse_value_;
    std::function<T( std::string const& )> convert_value_;

};

} // namespace utils
} // namespace genesis

#endif // include guard

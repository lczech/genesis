#ifndef GENESIS_UTILS_CONTAINERS_MATRIX_READER_H_
#define GENESIS_UTILS_CONTAINERS_MATRIX_READER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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

#include "genesis/utils/core/std.hpp"
#include "genesis/utils/formats/csv/reader.hpp"
#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/containers/matrix.hpp"

#include <functional>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     MatrixReader
// =================================================================================================

template <typename T>
class MatrixReader
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    MatrixReader( char separator_char = '\t' )
    {
        reader_.separator_chars( std::string( 1, separator_char ));
    }

    MatrixReader( CsvReader const& reader )
        : reader_( reader )
    {}

    ~MatrixReader() = default;

    MatrixReader(MatrixReader const&) = default;
    MatrixReader(MatrixReader&&)      = default;

    MatrixReader& operator= (MatrixReader const&) = default;
    MatrixReader& operator= (MatrixReader&&)      = default;

    // -------------------------------------------------------------
    //     Reading
    // -------------------------------------------------------------

    Matrix<T> from_stream( std::istream&      is ) const
    {
        utils::InputStream it( utils::make_unique< utils::StreamInputSource >( is ));
        return parse_( it );
    }

    Matrix<T> from_file  ( std::string const& fn ) const
    {
        utils::InputStream it( utils::make_unique< utils::FileInputSource >( fn ));
        return parse_( it );
    }

    Matrix<T> from_string( std::string const& fs ) const
    {
        utils::InputStream it( utils::make_unique< utils::StringInputSource >( fs ));
        return parse_( it );
    }

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    bool skip_first_col() const
    {
        return skip_first_col_;
    }

    bool skip_first_row() const
    {
        return skip_first_row_;
    }

    MatrixReader& skip_first_col( bool value )
    {
        skip_first_col_ = value;
        return *this;
    }

    MatrixReader& skip_first_row( bool value )
    {
        skip_first_row_ = value;
        return *this;
    }

    CsvReader& csv_reader()
    {
        return reader_;
    }

    CsvReader const& csv_reader() const
    {
        return reader_;
    }

    MatrixReader& parse_value_functor( std::function<T( std::string const& )> functor )
    {
        parse_value_ = functor;
    }

    // -------------------------------------------------------------
    //     Internal Functions
    // -------------------------------------------------------------

private:

    Matrix<T> parse_( utils::InputStream& input_stream ) const
    {
        // We collect data in a vector first, because resizing a Matrix is hard.
        std::vector<T> table;
        size_t cols = 0;

        // Early stop.
        if( ! input_stream ) {
            return {};
        }

        // Skip first line if needed.
        if( skip_first_row_ ) {
            reader_.parse_line( input_stream );
        }

        while( input_stream ) {
            auto const line = reader_.parse_line( input_stream );

            // Get the measurements of the interesting part of the line.
            auto first = 0;
            auto len   = line.size();
            if( len > 0 && skip_first_col_ ) {
                first = 1;
                --len;
            }

            // Check that line length is consisent. Cols == 0 means we just started.
            if( cols == 0 ) {

                // Matrix with zero length colums is empty, no matter how many rows it has.
                if( len == 0 ) {
                    return {};
                    // throw std::runtime_error( "Cannot read Matrix with empty lines." );
                }

                // Store the col length.
                cols = len;

            } else if( cols != len ) {
                throw std::runtime_error( "Matrix has different line lengths." );
            }

            // Parse and transfer the data. User specified parser or default one.
            if( parse_value_ ) {
                for( size_t i = first; i < line.size(); ++i ) {
                    table.push_back( parse_value_( line[i] ) );
                }
            } else {
                for( size_t i = first; i < line.size(); ++i ) {
                    table.push_back( parse_value_stringstream_( line[i] ) );
                }
            }
        }

        // We cannot properly calculate dimensions of an empty matrix. So better return here.
        if( table.size() == 0 ) {
            assert( cols == 0 );
            return {};
        }

        // Make sure that the table as a matrix shape.
        if( table.size() % cols != 0 ) {
            // I'm pretty sure this should be an assertion. But better check it all the time,
            // and throw some confusing incomprehensible error message.
            throw std::runtime_error( "Matrix is not rectangluar." );
        }

        // Make a proper Matrix.
        size_t const rows = table.size() / cols;
        return Matrix<T>( rows, cols, std::move(table) );
    }

    inline T parse_value_stringstream_( std::string const& cell ) const
    {
        std::stringstream ss( cell );
        T value;
        ss >> value;
        return value;
    }

    // -------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------

private:

    bool skip_first_row_ = false;
    bool skip_first_col_ = false;

    CsvReader reader_;

    std::function<T( std::string const& )> parse_value_;

};

} // namespace utils
} // namespace genesis

#endif // include guard

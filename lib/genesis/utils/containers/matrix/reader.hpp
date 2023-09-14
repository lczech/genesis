#ifndef GENESIS_UTILS_CONTAINERS_MATRIX_READER_H_
#define GENESIS_UTILS_CONTAINERS_MATRIX_READER_H_

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
#include "genesis/utils/formats/csv/reader.hpp"
#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/text/convert.hpp"
#include "genesis/utils/text/string.hpp"

#include <cstdlib>
#include <functional>
#include <sstream>
#include <stdexcept>
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

    explicit MatrixReader( std::string const& separator = "\t" )
    {
        reader_.separator_chars( separator );
    }

    explicit MatrixReader( CsvReader const& reader )
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

    Matrix<T> read( std::shared_ptr<BaseInputSource> source ) const
    {
        utils::InputStream is( source );
        return parse_( is );
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

    MatrixReader& convert_value_functor( std::function<T( std::string const& )> functor )
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
            if( convert_value_ ) {
                for( size_t i = first; i < line.size(); ++i ) {
                    table.push_back( convert_value_( line[i] ) );
                }
            } else {
                for( size_t i = first; i < line.size(); ++i ) {
                    try {
                        table.push_back( convert_from_string<T>( line[i] ));
                    } catch(...) {
                        throw std::runtime_error(
                            "In " + input_stream.source_name() + " line " +
                            std::to_string( input_stream.line() - 1 ) + ": "
                            "Cannot parse value \"" + line[i] + "\" into Matrix. "
                        );
                    }
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

    // -------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------

private:

    bool skip_first_row_ = false;
    bool skip_first_col_ = false;

    CsvReader reader_;

    std::function<T( std::string const& )> convert_value_;

};

} // namespace utils
} // namespace genesis

#endif // include guard

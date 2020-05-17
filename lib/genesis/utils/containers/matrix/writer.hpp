#ifndef GENESIS_UTILS_CONTAINERS_MATRIX_WRITER_H_
#define GENESIS_UTILS_CONTAINERS_MATRIX_WRITER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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

#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/output_target.hpp"

#include <cassert>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     MatrixWriter
// =================================================================================================

template <typename T>
class MatrixWriter
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    enum class Format
    {
        kMatrix,
        kList,
        kTriangular
    };

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    explicit MatrixWriter( std::string const& separator = "\t", Format format = Format::kMatrix )
        : separator_(separator)
        , format_(format)
    {}

    ~MatrixWriter() = default;

    MatrixWriter(MatrixWriter const&) = default;
    MatrixWriter(MatrixWriter&&)      = default;

    MatrixWriter& operator= (MatrixWriter const&) = default;
    MatrixWriter& operator= (MatrixWriter&&)      = default;

    // -------------------------------------------------------------
    //     Reading
    // -------------------------------------------------------------

    /**
     * @brief Write a Matrix to an output target, using a specific MatrixWriter::Format and
     * separator string.
     *
     * The format and separator string can either be set in the constructor, or using the methods
     * format( Format ) and separator_string( std::string const& ).
     *
     * The write() function takes optional row and column names, and a corner element that is used
     * if both row and column names are givenm, as the upper left entry of the written output.
     * Alternatively, either the @p row_names or the @p col_names can contain an additional
     * element in the beginning, which is then used as the "corner" top left element when writing.
     * This allows maximum flexibility in the setup when using this writer class.
     */
    void write(
        Matrix<T> const& matrix,
        std::shared_ptr<utils::BaseOutputTarget> target,
        std::vector<std::string> row_names = {},
        std::vector<std::string> col_names = {},
        std::string corner = ""
    ) const {
        if( row_names.size() == matrix.rows() + 1 ) {
            if( corner.empty() ) {
                corner = row_names[0];
                row_names.erase( row_names.begin() );
            } else {
                throw std::runtime_error( "Number of row names is different from Matrix row size." );
            }
        } else if( col_names.size() == matrix.cols() + 1 ) {
            if( corner.empty() ) {
                corner = col_names[0];
                col_names.erase( col_names.begin() );
            } else {
                throw std::runtime_error( "Number of col names is different from Matrix col size." );
            }
        }
        to_stream_( matrix, target->ostream(), row_names, col_names, corner );
    }

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    std::string const& separator_string() const
    {
        return separator_;
    }

    MatrixWriter& separator_string( std::string const& value )
    {
        separator_ = value;
        return *this;
    }

    Format format() const
    {
        return format_;
    }

    MatrixWriter& format( Format value )
    {
        format_ = value;
        return *this;
    }

    MatrixWriter& write_value_functor( std::function<std::string( T const& )> functor )
    {
        write_value_ = functor;
    }

    // -------------------------------------------------------------
    //     Internal Functions
    // -------------------------------------------------------------

private:

    void to_stream_(
        Matrix<T> const& mat,
        std::ostream& os,
        std::vector<std::string> const& row_names,
        std::vector<std::string> const& col_names,
        std::string const& corner
    ) const {

        // Checks.
        if( ! row_names.empty() && row_names.size() != mat.rows() ) {
            throw std::invalid_argument( "Number of row names is different from Matrix row size." );
        }
        if( ! col_names.empty() && col_names.size() != mat.cols() ) {
            throw std::invalid_argument( "Number of col names is different from Matrix col size." );
        }

        // Format switch
        switch( format_ ) {
            case Format::kMatrix: {
                to_matrix_( mat, os, row_names, col_names, corner );
                break;
            }
            case Format::kList: {
                to_list_( mat, os, row_names, col_names );
                break;
            }
            case Format::kTriangular: {
                to_triangular_( mat, os, row_names, col_names, corner );
                break;
            }
            default: {
                throw std::invalid_argument( "Invalid enum value for MatrixWriter::Format" );
            }
        }
    }

    void to_matrix_(
        Matrix<T> const& mat,
        std::ostream& os,
        std::vector<std::string> const& row_names,
        std::vector<std::string> const& col_names,
        std::string const& corner
    ) const {

        // Write top corner cell if needed.
        if( ! row_names.empty() && ! col_names.empty() ) {
            os << corner << separator_;
        }

        // Write col names.
        if( ! col_names.empty() ) {
            for( size_t c = 0; c < col_names.size(); ++c ) {
                if( c > 0 ) {
                    os << separator_;
                }
                os << col_names[c];
            }
            os << "\n";
        }

        // Write lines.
        for( size_t r = 0; r < mat.rows(); ++r ) {
            if( ! row_names.empty() ) {
                os << row_names[r] << separator_;
            }

            for( size_t c = 0; c < mat.cols(); ++c ) {
                if( c > 0 ) {
                    os << separator_;
                }
                if( write_value_ ) {
                    os << write_value_( mat( r, c ));
                } else {
                    os << mat( r, c );
                }
            }

            os << "\n";
        }
    }

    void to_list_(
        Matrix<T> const& mat,
        std::ostream& os,
        std::vector<std::string> const& row_names,
        std::vector<std::string> const& col_names
    ) const {

        // Simple: one line per cell
        for( size_t r = 0; r < mat.rows(); ++r ) {
            for( size_t c = 0; c < mat.cols(); ++c ) {
                if( ! row_names.empty() ) {
                    os << row_names[r] << separator_;
                }
                if( ! col_names.empty() ) {
                    os << col_names[c] << separator_;
                }
                os << mat( r, c );
                os << "\n";
            }
        }
    }

    void to_triangular_(
        Matrix<T> const& mat,
        std::ostream& os,
        std::vector<std::string> const& row_names,
        std::vector<std::string> const& col_names,
        std::string const& corner
    ) const {

        // Check
        if( mat.rows() != mat.cols() ) {
            throw std::invalid_argument( "Cannot write triangular matrix, as it is not quadratic." );
        }

        // Write top corner cell if needed.
        if( ! row_names.empty() && ! col_names.empty() ) {
            os << corner << separator_;
        }

        // Write col names backwards.
        if( ! col_names.empty() ) {
            for( size_t cb = 0; cb < col_names.size(); ++cb ) {
                auto const c = col_names.size() - cb - 1;
                if( cb > 0 ) {
                    os << separator_;
                }
                os << col_names[c];
            }
            os << "\n";
        }

        // Write lines, with backwards columns to get a nice looking triangluar matrix.
        for( size_t r = 0; r < mat.rows(); ++r ) {
            if( ! row_names.empty() ) {
                os << row_names[r] << separator_;
            }

            // Write the trinangular columns, starting at col = row
            for( size_t cb = 0; cb < mat.cols() - r; ++cb ) {
                auto const c = mat.cols() - cb - 1;
                assert( c < mat.cols() );

                if( cb > 0 ) {
                    os << separator_;
                }
                if( write_value_ ) {
                    os << write_value_( mat( r, c ));
                } else {
                    os << mat( r, c );
                }
            }

            os << "\n";
        }
    }

    // -------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------

private:

    std::string separator_ = std::string( "\t" );
    Format      format_ = Format::kMatrix;

    std::function<std::string( T const& )> write_value_;

};

} // namespace utils
} // namespace genesis

#endif // include guard

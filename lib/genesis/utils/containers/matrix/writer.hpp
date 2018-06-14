#ifndef GENESIS_UTILS_CONTAINERS_MATRIX_WRITER_H_
#define GENESIS_UTILS_CONTAINERS_MATRIX_WRITER_H_

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
#include "genesis/utils/io/output_stream.hpp"
#include "genesis/utils/containers/matrix.hpp"

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
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    MatrixWriter() = default;
    ~MatrixWriter() = default;

    MatrixWriter(MatrixWriter const&) = default;
    MatrixWriter(MatrixWriter&&)      = default;

    MatrixWriter& operator= (MatrixWriter const&) = default;
    MatrixWriter& operator= (MatrixWriter&&)      = default;

    // -------------------------------------------------------------
    //     Reading
    // -------------------------------------------------------------

    void to_stream(
        Matrix<T> const& mat,
        std::ostream& os,
        std::vector<std::string> const& row_names = {},
        std::vector<std::string> const& col_names = {},
        std::string const& corner = ""
    ) const {

        // Checks.
        if( ! row_names.empty() && row_names.size() != mat.rows() ) {
            throw std::invalid_argument( "Number of row names is different from Matrix row size." );
        }
        if( ! col_names.empty() && col_names.size() != mat.cols() ) {
            throw std::invalid_argument( "Number of col names is different from Matrix col size." );
        }

        // Writer top corner cell if needed.
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

    void to_file(
        Matrix<T> const& mat,
        std::string const& fn,
        std::vector<std::string> const& row_names = {},
        std::vector<std::string> const& col_names = {},
        std::string const& corner = ""
    ) const {
        std::ofstream ofs;
        utils::file_output_stream( fn, ofs );
        to_stream( mat, ofs, row_names, col_names, corner );
    }

    std::string to_string(
        Matrix<T> const& mat,
        std::vector<std::string> const& row_names = {},
        std::vector<std::string> const& col_names = {},
        std::string const& corner = ""
    ) const {
        std::ostringstream oss;
        to_stream( mat, oss, row_names, col_names, corner );
        return oss.str();
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

    MatrixWriter& write_value_functor( std::function<std::string( T const& )> functor )
    {
        write_value_ = functor;
    }

    // -------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------

private:

    std::string separator_ = "\t";

    std::function<std::string( T const& )> write_value_;

};

} // namespace utils
} // namespace genesis

#endif // include guard

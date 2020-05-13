#ifndef GENESIS_UTILS_IO_GZIP_H_
#define GENESIS_UTILS_IO_GZIP_H_

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

#include "genesis/utils/core/exception.hpp"

#include <stdexcept>
#include <string>

namespace genesis {
namespace utils {

// ================================================================================================
//     General gzip/zlib Functions
// ================================================================================================

/**
 * @brief Return whether a given file is gzip-compressed.
 *
 * Returns true only iff the file exists and is gzip-compressed.
 * If there is a mismatch between the gzip magic bytes and the file ending, a warning is issued
 * via LOG_WARN.
 */
bool is_gzip_compressed_file( std::string const& file_name );

// ================================================================================================
//     Gzip Exception Class
// ================================================================================================

} // namespace utils
namespace except {

/**
 * @brief Exception class thrown by failed gzip/zlib operations.
 *
 * If compiled without zlib support, the exepction has no use and contains a dummy message.
 */
class GzipError
    : public Exception
{
public:

    GzipError( std::string const& z_stream_message, int error_code );

    GzipError( std::string const msg )
        : except::Exception(msg)
    {}

};

} // namespace except
} // namespace genesis

#endif // include guard

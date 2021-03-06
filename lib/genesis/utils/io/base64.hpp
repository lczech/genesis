#ifndef GENESIS_UTILS_IO_BASE64_H_
#define GENESIS_UTILS_IO_BASE64_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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

#include <cstdint>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// ================================================================================================
//     Base 64 Encode/Decode
// ================================================================================================

std::string base64_encode( std::vector<std::uint8_t> const& input, size_t line_length = 76 );
std::string base64_encode( std::string const& input, size_t line_length = 76 );

std::vector<std::uint8_t> base64_decode_uint8( std::string const& input );
std::string base64_decode_string( std::string const& input );

} // namespace utils
} // namespace genesis

#endif // include guard

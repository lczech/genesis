#ifndef GENESIS_UTILS_IO_FUNCTIONS_H_
#define GENESIS_UTILS_IO_FUNCTIONS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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

#include <genesis/utils/io/input_source.hpp>

#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Read Input Source
// =================================================================================================

/**
 * @brief Return the contents of an input @p source as a string.
 *
 * This simply reads all characters from the @p source into a `std::string`. It is similar to
 * file_read(), but takes the more general BaseInputSource as input.
 */
std::string read_input_source( std::shared_ptr<BaseInputSource> source );

/**
 * @brief Return the contents of an input @p source as a string, split into individual lines.
 *
 * This simply reads all lines from the @p source into a `std::string`. It is similar to
 * file_read_lines(), but takes the more general BaseInputSource as input.
 */
std::vector<std::string> read_input_source_lines( std::shared_ptr<BaseInputSource> source );

} // namespace utils
} // namespace genesis

#endif // include guard

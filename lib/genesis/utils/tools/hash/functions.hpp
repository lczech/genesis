#ifndef GENESIS_UTILS_TOOLS_HASH_FUNCTIONS_H_
#define GENESIS_UTILS_TOOLS_HASH_FUNCTIONS_H_

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

#include "genesis/utils/io/input_source.hpp"

#include <iosfwd>
#include <string>

namespace genesis {
namespace utils {

// ================================================================================================
//     Hashing Types
// ================================================================================================

/**
 * @brief List of the currently implemented hashing functions.
 *
 * This is useful in order to select the used hashing function at runtime for some algorithms.
 *
 * @see hash_hex()
 */
enum class HashingFunctions
{
    /**
     * @brief Use MD5 for hashing.
     */
    kMD5,

    /**
     * @brief Use SHA1 for hashing.
     */
    kSHA1,

    /**
     * @brief Use SHA256 for hashing.
     */
    kSHA256
};

// ================================================================================================
//     Hashing
// ================================================================================================

/**
 * @brief Calculate the hash of an input source, using a given hashing function, and return its hex
 * representation as a string.
 *
 * See ::HashingFunctions for the list of available hashing functions.
 */
std::string hash_hex( std::shared_ptr<BaseInputSource> source, HashingFunctions hash_fct );

} // namespace utils
} // namespace genesis

#endif // include guard

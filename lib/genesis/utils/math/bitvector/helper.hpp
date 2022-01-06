#ifndef GENESIS_UTILS_MATH_BITVECTOR_HELPER_H_
#define GENESIS_UTILS_MATH_BITVECTOR_HELPER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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

#include <vector>
#include <cstddef>

namespace genesis {
namespace utils {

// =================================================================================================
//     Bitvector Helper
// =================================================================================================

/**
 * @brief Helper function to create a bool vector from a set of indices to be set to `true`.
 *
 * The function expectes a list of indices. It returns a bool vector with the size of the largest
 * index, or the provided @p size (if set to a value > 0), where all positions of these indices are
 * `true`, and all other positions are `false`.
 */
std::vector<bool> make_bool_vector_from_indices(
    std::vector<size_t> const& indices,
    size_t size = 0
);

} // namespace utils
} // namespace genesis

#endif // include guard

#ifndef GENESIS_PLACEMENT_FUNCTION_SAMPLE_SET_H_
#define GENESIS_PLACEMENT_FUNCTION_SAMPLE_SET_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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
 * @ingroup placement
 */

#include "genesis/placement/sample.hpp"
#include "genesis/placement/sample_set.hpp"
#include "genesis/tree/tree_set.hpp"

#include <iosfwd>
#include <string>

namespace genesis {
namespace placement {

// =================================================================================================
//     Sample Set Functions
// =================================================================================================

/**
 * @brief Get the first Sample in a SampleSet that has a given name, or `nullptr` if not found.
 */
Sample      * find_sample( SampleSet      & sset, std::string const& name );

/**
 * @brief Get the first Sample in a SampleSet that has a given name, or `nullptr` if not found.
 */
Sample const* find_sample( SampleSet const& sset, std::string const& name );

/**
 * @brief Returns true iff all Trees of the Samples in the set are identical.
 *
 * This is the case if they have the same topology, node names and edge_nums. However, branch
 * lengths are not checked, because usually those differ slightly.
 */
bool all_identical_trees( SampleSet const& sset );

/**
 * @brief Return a TreeSet containing all the trees of the SampleSet.
 */
tree::TreeSet tree_set( SampleSet const& sset );

/**
 * @brief Returns a Sample where all maps of this set have been merged into.
 *
 * For this method to succeed, all Samples need to have the same topology, including identical
 * edge_nums and node names. The Tree of the returned Sample has the average branch lenghts
 * from the input trees, using TreeSet::average_branch_length_tree().
 */
Sample merge_all( SampleSet const& sset );

/**
 * @brief Return the total number of @link Pquery Pqueries@endlink in the Sample%s of the SampleSet.
 */
size_t total_pquery_count( SampleSet const& sset );

// =================================================================================================
//     Output
// =================================================================================================

std::ostream& operator << ( std::ostream& out, SampleSet const& sset );

} // namespace placement
} // namespace genesis

#endif // include guard

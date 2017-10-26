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
Sample      * find_sample( SampleSet      & sample_set, std::string const& name );

/**
 * @brief Get the first Sample in a SampleSet that has a given name, or `nullptr` if not found.
 */
Sample const* find_sample( SampleSet const& sample_set, std::string const& name );

/**
 * @brief Returns a Sample where all Sample%s of a SampleSet have been merged into.
 *
 * For this method to succeed, all Samples need to have the same topology, including identical
 * edge_nums and node names. The Tree of the returned Sample has the average branch lenghts
 * from the input trees, using TreeSet::average_branch_length_tree().
 */
Sample merge_all( SampleSet const& sample_set );

/**
 * @brief Return the total number of @link Pquery Pqueries@endlink in the Sample%s of the SampleSet.
 */
size_t total_pquery_count( SampleSet const& sample_set );

// =================================================================================================
//     Tree Functions
// =================================================================================================

/**
 * @brief Return the Tree that has edges with the average branch length of the respective edges
 * of the Trees in the Samples of the given SampleSet.
 */
tree::Tree average_branch_length_tree( SampleSet const& sample_set );

/**
 * @brief Returns true iff all Trees of the Samples in the set are identical.
 *
 * This is the case if they have the same topology, node names and edge_nums. However, branch
 * lengths are not checked, because usually those differ slightly.
 */
bool all_identical_trees( SampleSet const& sample_set );

/**
 * @brief Return a TreeSet containing all the trees of the SampleSet.
 */
tree::TreeSet tree_set( SampleSet const& sample_set );

/**
 * @brief Take the branch lengths of the @p source @link tree::Tree Tree@endlink
 * and use them as the new branch lengths of the Samples in the @p sample_set.
 *
 * This function simply calls adjust_branch_lengths( Sample&, tree::Tree const& ) for all
 * Sample%s in the set. See there for details.
 *
 * All involved Tree%s need to have identical topology. This is not checked.
 */
void adjust_branch_lengths( SampleSet& sample_set, tree::Tree const& source );

/**
 * @brief Set the branch lengths of all Sample%s in the @p sample_set to the respecitve average
 * branch length of the Samples.
 *
 * That is, for each @link tree::TreeEdge edge@endlink of the tree, find the average branch length
 * over all Samples, and use this for the Samples.
 * This means, all Samples in the SampleSet need to have identical tree topologies.
 */
void adjust_to_average_branch_lengths( SampleSet& sample_set );

// =================================================================================================
//     Output
// =================================================================================================

std::ostream& operator << ( std::ostream& out, SampleSet const& sample_set );

} // namespace placement
} // namespace genesis

#endif // include guard

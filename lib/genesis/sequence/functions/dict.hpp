#ifndef GENESIS_SEQUENCE_FUNCTIONS_DICT_H_
#define GENESIS_SEQUENCE_FUNCTIONS_DICT_H_

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
 * @ingroup sequence
 */

#include "genesis/sequence/sequence_dict.hpp"
#include "genesis/sequence/sequence_set.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Sequence Dict
// =================================================================================================

/**
 * @brief Get the sequence dict/index information of a given set of Sequence%s.
 *
 * This is a helper function that can be used to obtain an overview of the names and lengths
 * of a set of sequences, for some downstream processing, or as information about a reference genome.
 */
SequenceDict sequence_set_to_dict( SequenceSet const& set );

/**
 * @brief Verify that a SequenceDict fits a SequenceSet.
 *
 * The function return `true` iff the @p dict has sequences of the same name and length,
 * in the same order, as the @p set, and `false` otherwise.
 *
 * If @p lenient is set, we cut off the sequence names after the first tab character, in case there
 * are any, as this is what typical fasta indexing tools also seem to do. In that case, anything
 * after the first tab is not used for the comparison.
 */
bool verify( SequenceDict const& dict, SequenceSet const& set, bool lenient = false );

} // namespace sequence
} // namespace genesis

#endif // include guard

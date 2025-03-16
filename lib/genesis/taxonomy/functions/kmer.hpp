#ifndef GENESIS_TAXONOMY_FUNCTIONS_KMER_H_
#define GENESIS_TAXONOMY_FUNCTIONS_KMER_H_

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
 * @ingroup taxonomy
 */

#include "genesis/taxonomy/functions/kmer_data.hpp"
#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/io/output_target.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Forwad Declarations
// =================================================================================================

class Taxon;
class Taxonomy;

// =================================================================================================
//     Taxon Kmer Counting
// =================================================================================================

// -------------------------------------------------------------------------
//     Functions
// -------------------------------------------------------------------------

/**
 * @brief Given a @p taxonomy with data entries of type KmerTaxonData, propagate and accumulate
 * count values towards the root.
 *
 * This function takes the KmerTaxonData::num_sequences and KmerTaxonData::sum_seq_lengths of each
 * taxon, and accumulates them towards the root, storing the data in KmerTaxonData::clade_num_sequences
 * and KmerTaxonData::clade_sum_seq_lengths respectively. In other words, after this functions,
 * each clade contains the number of sequences and their total length (in number of bases)
 * of itself and all its children, stored in the two `total` values of KmerTaxonData.
 *
 * The given @p taxonomy is assumed to have data type KmerTaxonData, and throws an exception
 * otherwise. See reset_taxonomy_data() to set this data type for all Taxa in a Taxonomy.
 */
void accumulate_taxon_sizes( Taxonomy& tax );

/**
 * @brief Write a Taxonomy with KmerTaxonData, in our internal Json format.
 *
 * This is meant to store some data for later, and for visual inspection of the results.
 * By default, we just write the basic counts per taxon. However, @p with_group_data also
 * writes the result of group_by_taxon_sizes() to the file. If then additionally @p only_trunk
 * is set, we only write the "inner" taxa, i.e., the ones that are expanded and are thus not part
 * of a group, and the first (top-most) taxa that are grouped. All taxa below the group are then
 * omitted. This means, the resulting json will only represent the scaffold/trunk of the taxonomy
 * of groups, which is most relevant for inspection, while leaving out the details of all clades
 * that are grouped.
 */
void write_kmer_taxonomy_to_json(
    Taxonomy const& tax,
    std::shared_ptr<utils::BaseOutputTarget> target,
    bool with_group_data = false,
    bool only_trunk = false
);

/**
 * @brief Read a Taxonomy with KmerTaxonData, from our internal Json format.
 *
 * This reads back the data produced by write_kmer_taxonomy_to_json(). See there for details.
 * Note that if write_kmer_taxonomy_to_json() was called with `only_trunk`, only the
 * scaffold/trunk of the taxonomy will be read as well.
 */
Taxonomy read_kmer_taxonomy_from_json(
    std::shared_ptr<utils::BaseInputSource> source
);

} // namespace taxonomy
} // namespace genesis

#endif // include guard

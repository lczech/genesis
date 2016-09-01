#ifndef GENESIS_TAXONOMY_FUNCTIONS_ENTROPY_H_
#define GENESIS_TAXONOMY_FUNCTIONS_ENTROPY_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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
 * @ingroup taxonomy
 */

#include "taxonomy/functions/entropy_data.hpp"

#include <string>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Forwad Declarations
// =================================================================================================

class Taxon;
class Taxonomy;

// =================================================================================================
//     Entropy Functions
// =================================================================================================

// =================================================================================================
//     Prune Functions
// =================================================================================================

void prune_by_entropy_with_target_size(
    Taxonomy& taxonomy,
    size_t    target_taxonomy_size,
    size_t    max_subtaxonomy_size = 0,
    size_t    min_subtaxonomy_size = 0,
    bool      allow_approximation  = false
);

void expand_small_subtaxonomies(
    Taxonomy& taxonomy,
    size_t    min_subtaxonomy_size
);

size_t count_taxa_with_prune_status(
    Taxonomy const&               taxonomy,
    EntropyTaxonData::PruneStatus status
);

void remove_pruned_taxonomy_children( Taxonomy& taxonomy );

std::string print_pruned_taxonomy(    Taxonomy const& taxonomy );
bool        validate_pruned_taxonomy( Taxonomy const& taxonomy );

} // namespace taxonomy
} // namespace genesis

#endif // include guard

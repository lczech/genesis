#ifndef GENESIS_TAXONOMY_FUNCTIONS_SPLIT_H_
#define GENESIS_TAXONOMY_FUNCTIONS_SPLIT_H_

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

#include <string>
#include <unordered_map>
#include <unordered_set>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Forwad Declarations
// =================================================================================================

class Taxon;
class Taxonomy;

// =================================================================================================
//     Split
// =================================================================================================

std::unordered_set< Taxon const* > split_taxonomy_by_entropy_threshold(
    Taxonomy const&                                   taxonomy,
    std::unordered_map< Taxon const*, double > const& entropies,
    double                                            entropy_threshold
);

std::unordered_set< Taxon const* > split_taxonomy_by_entropy_nested_invervals(
    Taxonomy const&                                   taxonomy,
    std::unordered_map< Taxon const*, double > const& entropies,
    size_t                                            target_taxonomy_size
);

std::unordered_set< Taxon const* > split_taxonomy_by_entropy_with_target_size(
    Taxonomy const&                                   taxonomy,
    std::unordered_map< Taxon const*, double > const& entropies,
    size_t                                            target_taxonomy_size
);

std::unordered_set< Taxon const* > fill_splitted_entropy_parents(
    std::unordered_set< Taxon const* >                split_list
);

size_t count_splitted_taxonomy_total_size(
    Taxonomy const&                                   taxonomy,
    std::unordered_set< Taxon const* >                full_split_list
);

void remove_splitted_taxonomy_children(
    Taxonomy&                                         taxonomy,
    std::unordered_set< Taxon const* >                split_list
);

std::string print_splitted_taxonomy(
    Taxonomy const&                                   taxonomy,
    std::unordered_set< Taxon const* > const&         split_list
);

std::string print_splitted_taxonomy(
    Taxonomy const&                                   taxonomy,
    std::unordered_set< Taxon const* > const&         split_list,
    std::unordered_map< Taxon const*, double > const& entropies
);

bool validated_splitted_taxonomy(
    Taxonomy const&                                   taxonomy,
    std::unordered_set< Taxon const* > const&         split_list
);

} // namespace taxonomy
} // namespace genesis

#endif // include guard

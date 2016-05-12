#ifndef GENESIS_TAXONOMY_FUNCTIONS_H_
#define GENESIS_TAXONOMY_FUNCTIONS_H_

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

#include <functional>
#include <iosfwd>
#include <string>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Forwad Declarations
// =================================================================================================

class Taxonomy;
class Taxon;

// =================================================================================================
//     Accessors
// =================================================================================================

Taxon const* find_taxon( Taxonomy const& tax, std::string const& name );
Taxon*       find_taxon( Taxonomy&       tax, std::string const& name );

size_t taxon_level( Taxon const& taxon );
size_t total_taxa_count( Taxonomy const& tax );

void levelorder_for_each(
    Taxonomy& tax,
    std::function< void( Taxon& )> fn,
    bool include_inner_taxa = true
);

void preorder_for_each(
    Taxonomy& tax,
    std::function< void( Taxon& )> fn,
    bool include_inner_taxa = true
);

void postorder_for_each(
    Taxonomy& tax,
    std::function< void( Taxon& )> fn,
    bool include_inner_taxa = true
);

// =================================================================================================
//     Modifiers
// =================================================================================================

Taxon& add_children_from_string(
    Taxonomy&          taxonomy,
    std::string const& children,
    std::string const& delimiters = ";",
    bool               trim_whitespaces = true
);

void remove_taxa_at_level( Taxonomy& tax, size_t level );

// =================================================================================================
//     Print and Output
// =================================================================================================

std::ostream& operator << ( std::ostream& out, Taxonomy const& tax );

std::string taxonomic_string(
    Taxon const& taxon,
    std::string delimiter = ";",
    bool trim_nested_duplicates = true
);

} // namespace taxonomy
} // namespace genesis

#endif // include guard

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

#include <iosfwd>
#include <string>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Forwad Declarations
// =================================================================================================

class Taxonomy;
class Rank;

// =================================================================================================
//     Accessors
// =================================================================================================

Rank const* find_taxon( Taxonomy const& tax, std::string const& name );
Rank*       find_taxon( Taxonomy&       tax, std::string const& name );

size_t total_taxa_count( Taxonomy const& tax );

// =================================================================================================
//     Modifiers
// =================================================================================================

Rank& add_children_from_string(
    Taxonomy&          taxonomy,
    std::string const& children,
    std::string const& delimiters = ";",
    bool               trim = true
);

// =================================================================================================
//     Print and Output
// =================================================================================================

std::ostream& operator << ( std::ostream& out, Taxonomy const& tax );

} // namespace taxonomy
} // namespace genesis

#endif // include guard

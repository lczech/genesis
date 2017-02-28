#ifndef GENESIS_TAXONOMY_ITERATOR_POSTORDER_H_
#define GENESIS_TAXONOMY_ITERATOR_POSTORDER_H_

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
 * @ingroup taxonomy
 */

#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/taxonomy/taxon.hpp"
#include "genesis/utils/core/range.hpp"

#include <functional>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Postorder For Each
// =================================================================================================

/**
 * @brief Apply a function to all taxa of the Taxonomy, traversing it in postorder.
 *
 * The given Taxonomy is traversed in postorder (i.e., a variant of depth-first search). If
 * `include_inner_taxa` is set to true (default), the provided functional is called for all
 * @link Taxon Taxa @endlink. Otherwise, the functional is only called for the taxa of lowest rank,
 * that is, for each Taxon that does not have sub-taxa.
 *
 * This is the non-const version of the function.
 */
inline void postorder_for_each(
    Taxonomy& tax,
    std::function< void( Taxon& )> fn,
    bool include_inner_taxa = true
) {
    for( auto& t : tax ) {
        postorder_for_each( t, fn, include_inner_taxa );

        if( include_inner_taxa || t.size() == 0 ) {
            fn( t );
        }
    }
}

/**
 * @brief Apply a function to all taxa of the Taxonomy, traversing it in postorder.
 *
 * The given Taxonomy is traversed in postorder (i.e., a variant of depth-first search). If
 * `include_inner_taxa` is set to true (default), the provided functional is called for all
 * @link Taxon Taxa @endlink. Otherwise, the functional is only called for the taxa of lowest rank,
 * that is, for each Taxon that does not have sub-taxa.
 *
 * This is the const version of the function.
 */
inline void postorder_for_each(
    Taxonomy const& tax,
    std::function< void( Taxon const& )> fn,
    bool include_inner_taxa = true
) {
    for( auto const& t : tax ) {
        postorder_for_each( t, fn, include_inner_taxa );

        if( include_inner_taxa || t.size() == 0 ) {
            fn( t );
        }
    }
}

} // namespace taxonomy
} // namespace genesis

#endif // include guard

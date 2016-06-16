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

#include "taxonomy/functions/taxscriptor.hpp"

#include "taxonomy/taxon.hpp"
#include "taxonomy/taxonomy.hpp"
#include "taxonomy/taxscriptor.hpp"
#include "utils/text/string.hpp"

#include <stdexcept>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Taxscriptor
// =================================================================================================

/**
 * @brief Add a Taxon to a Taxonomy, using the taxonomic elements of a Taxscriptor.
 *
 * For example, given a Taxscriptor like
 *
 *     [ "Animalia", "Vertebrata", "Mammalia", "Carnivora" ]
 *
 * this functions adds the following hierarchy to the Taxonomy:
 *
 *     Animalia
 *         Vertebrata
 *             Mammalia
 *                 Carnivora
 *
 * For any existing Taxa, nothing happens. If any (parent) Taxon in the hierarchy does not exist,
 * it is created by default.
 *
 * @param taxonomy Taxonomy to add the Taxon to.
 * @param taxscriptor A Taxscriptor object from which the Taxon and its parents are taken.
 * @param expect_parents Optional, defaults to `false`. If set to true, the function expects
 *     all super-taxa of the added Taxon to exists, that is, all taxa except for the last one
 *     in the hierachry.
 *     If this expectation is not met, that is, if not all super-taxa exist, an `std::runtime_error`
 *     exception is thrown. If left at the default (`false`), all necessary super-taxa are created
 *     if they do not exists yet.
 *
 * @return The function returns a reference to the newly created Taxon. This is the deepest Taxon
 *     of the Taxscriptor; in other words, its last element.
 */
Taxon& add_from_taxscriptor(
    Taxonomy&          taxonomy,
    Taxscriptor const& taxscriptor,
    bool               expect_parents
) {
    // The return value of this function is the added Taxon. If we don't add anything, we
    // cannot return anything. So better throw. This might need to change in the future...
    if( taxscriptor.empty() ) {
        throw std::runtime_error(
            "Cannot add empty Taxscriptor to Taxonomy."
        );
    }

    // Prepare: we need a Taxonomy to add children to. This pointer is updated in the loop so that
    // we go deeper and deeper into the taxonomy.
    Taxonomy* cur_taxon = &taxonomy;

    // Add the names to the Taxonomy.
    for( size_t i = 0; i < taxscriptor.size(); ++i ) {

        // If we expect parents to exists, we need to check if the child exists
        // (but only when we are still considering parents, and not the last taxon itself,
        // that is, if we are not at the last element of the taxscriptor).
        if( ( expect_parents ) &&
            ( i < taxscriptor.size() - 1 ) &&
            ( ! cur_taxon->has_child( taxscriptor[i] ))
        ) {
            throw std::runtime_error(
                "Not all super-taxa of the Taxon in the Taxscripyot are present in the given Taxonomy."
            );
        }

        cur_taxon = &cur_taxon->add_child( taxscriptor[i] );
    }

    // Convert to Taxon. This is always legal because we have ensured that we are adding at least
    // one sub-taxon to the taxonomy, and in the loop set cur_taxon to this taxon, so that it is
    // actually of dynamic type Taxon.
    return dynamic_cast< Taxon& >( *cur_taxon );
}

/**
 * @brief Find a Taxon in a Taxonomy, given its Taxscriptor.
 */
Taxon const* find_taxon_by_taxscriptor( Taxonomy const& tax, Taxscriptor const& taxscriptor )
{
    // Border condition: nothing to search for.
    if( taxscriptor.empty() ) {
        return nullptr;
    }

    // Use a pointer that is updated in the loop while we go deeper and deeper into the taxonomy.
    Taxonomy const* cur_taxon = &tax;

    // Find the elements of the Taxscriptor in the Taxonomy.
    for( auto const& name : taxscriptor ) {
        if( ! cur_taxon->has_child( name )) {
            return nullptr;
        }
        cur_taxon = &cur_taxon->get_child( name );
    }

    // Convert to Taxon. This is always legal because we have ensured that we execute the loop at
    // least once. If we thus reach this point, we have set cur_taxon to a child of the Taxonomy,
    // which is a Taxon.
    return dynamic_cast< Taxon const* >( cur_taxon );
}

/**
 * @brief Find a Taxon in a Taxonomy, given its Taxscriptor.
 */
Taxon* find_taxon_by_taxscriptor( Taxonomy& tax, Taxscriptor const& taxscriptor )
{
    // Avoid code duplication according to Scott Meyers.
    auto const& ctax = static_cast< Taxonomy const& >( tax );
    return const_cast< Taxon* >( find_taxon_by_taxscriptor( ctax, taxscriptor ));
}

} // namespace taxonomy
} // namespace genesis

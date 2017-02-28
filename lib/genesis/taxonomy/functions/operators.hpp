#ifndef GENESIS_TAXONOMY_FUNCTIONS_OPERATORS_H_
#define GENESIS_TAXONOMY_FUNCTIONS_OPERATORS_H_

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

#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/taxonomy/functions/taxonomy.hpp"

#include <stdexcept>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Data Types
// =================================================================================================

/**
 * @brief Check whether the data of a Taxonomy are exactly of the specified data type.
 *
 * This function returns true iff all @link Taxon Taxa@endlink have data of the given type,
 * using typeid() for this matching.
 */
template< class TaxonDataType >
bool taxonomy_data_is( Taxonomy const& taxonomy )
{
    // Slow, because we traverse the whole thing even if the first one is already false...
    // Works for now.
    bool correct = true;
    preorder_for_each( taxonomy, [&] ( Taxon const& taxon ) {
        if( ! taxon.has_data() ) {
            correct = false;
        }
        auto const& ref = *taxon.data_ptr();
        if( typeid( ref ) != typeid( TaxonDataType ) ) {
            correct = false;
        }
    });
    return correct;
}

/**
 * @brief Check whether the data of a Taxonomy are derived from the specified data type.
 *
 * This function returns true iff all @link Taxon Taxa@endlink have data whose types
 * are derived from the specified type. It uses dynamic_cast() for this.
 */
template< class TaxonDataType >
bool taxonomy_data_is_derived_from( Taxonomy const& taxonomy )
{
    // Slow, because we traverse the whole thing even if the first one is already false...
    // Works for now.
    bool correct = true;
    preorder_for_each( taxonomy, [&] ( Taxon const& taxon ) {
        if( dynamic_cast< TaxonDataType const* >( taxon.data_ptr() ) == nullptr ) {
            return false;
        }
    });
    return correct;
}

/**
 * @brief (Re-)set all Taxon data of a Taxonomy to a specified data type.
 *
 * The data is created empty, using BaseTaxonData::create(). If the optional parameter
 * `allow_overwrite` is set to `false` (instead of the default `true`), the function throws an
 * exception if a Taxon already has data assigned to it.
 */
template< class TaxonDataType >
void reset_taxonomy_data( Taxonomy& taxonomy, bool allow_overwrite = true )
{
    preorder_for_each( taxonomy, [&] ( Taxon& taxon ) {
        if( taxon.has_data() && ! allow_overwrite ) {
            throw std::runtime_error(
                "Trying to overwrite Taxonomy data that already has a value."
            );
        }
        taxon.reset_data(
            TaxonDataType::create()
        );
    });
}

} // namespace taxonomy
} // namespace genesis

#endif // include guard

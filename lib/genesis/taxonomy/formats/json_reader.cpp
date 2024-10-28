/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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

#include "genesis/taxonomy/formats/json_reader.hpp"

#include "genesis/utils/formats/json/document.hpp"
#include "genesis/utils/formats/json/reader.hpp"

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Reading
// =================================================================================================

Taxonomy TaxonomyJsonReader::read( std::shared_ptr<utils::BaseInputSource> source ) const
{
    auto doc = utils::JsonReader().read( source );
    return read( doc );
}

Taxonomy TaxonomyJsonReader::read( utils::JsonDocument& doc ) const
{
    using namespace utils;
    Taxonomy tax;
    auto& arr = doc.get_array();
    for( auto& child : arr ) {
        Taxon child_tax;
        object_to_taxon_( child.get_object(), child_tax );
        tax.add_child( std::move( child_tax ));
    }
    return tax;
}

void TaxonomyJsonReader::object_to_taxon_( utils::JsonDocument::ObjectType& obj, Taxon& tax ) const
{
    // Get the basic properties of a Taxon.
    tax.name( obj[ "name" ].get_string() );
    if( obj.count( "rank" ) > 0 ) {
        tax.rank( obj[ "rank" ].get_string() );
    }
    if( obj.count( "id" ) > 0 ) {
        tax.id( obj[ "id" ].get_number_unsigned() );
    }

    // If we have a special function to process extra data, apply that one as well.
    if( json_to_taxon ) {
        json_to_taxon( obj, tax );
    }

    // Finally, recursive call to get the children, if any,
    // and then delete them from the document to save memory.
    if( obj.count( "children" ) > 0 ) {
        auto& children = obj[ "children" ].get_array();
        for( auto& child : children ) {
            Taxon child_tax;
            object_to_taxon_( child.get_object(), child_tax );
            tax.add_child( std::move( child_tax ));
        }
        children.clear();
    }
}

} // namespace taxonomy
} // namespace genesis

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

#include "genesis/taxonomy/formats/json_writer.hpp"

#include "genesis/taxonomy/iterator/preorder.hpp"
#include "genesis/utils/formats/json/document.hpp"
#include "genesis/utils/formats/json/writer.hpp"
#include "genesis/utils/io/output_stream.hpp"

#include <iostream>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Writing
// =================================================================================================

void TaxonomyJsonWriter::write(
    Taxonomy const& tax, std::shared_ptr<utils::BaseOutputTarget> target
) const {
    utils::JsonWriter().write( to_document( tax ), target );
}

utils::JsonDocument TaxonomyJsonWriter::to_document(
    Taxonomy const& tax
) const {
    using namespace utils;
    JsonDocument doc = JsonDocument::array();
    for( auto const& child : tax ) {
        doc.push_back( taxon_to_object_( child ));
    }
    return doc;
}

utils::JsonDocument::ObjectType TaxonomyJsonWriter::taxon_to_object_(
    Taxon const& tax
) const {
    using namespace utils;
    auto obj = utils::JsonDocument::ObjectType();

    // Set the basic properties of a Taxon.
    obj[ "name" ] = tax.name();
    if( !tax.rank().empty() ) {
        obj[ "rank" ] = tax.rank();
    }
    if( tax.id() != 0 ) {
        obj[ "id" ] = JsonDocument::number_unsigned( tax.id() );
    }

    // If we have a special function to process extra data, apply that one as well.
    if( taxon_to_json ) {
        taxon_to_json( tax, obj );
    }

    // Finally, recursive call for the children, if there are any, and the condition is fulfilled.
    // Even the NCBI taxonomy is only ~30 levels deep, so that is fine to do by recursion.
    if( tax.size() > 0 ) {
        if( ! recurse_taxon_condition || recurse_taxon_condition( tax )) {
            obj[ "children" ] = JsonDocument::array();
            for( auto const& child : tax ) {
                obj[ "children" ].push_back( taxon_to_object_( child ));
            }
        }
    }

    return obj;
}

} // namespace taxonomy
} // namespace genesis

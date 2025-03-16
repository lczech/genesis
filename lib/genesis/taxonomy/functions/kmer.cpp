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

#include "genesis/taxonomy/functions/kmer.hpp"

#include "genesis/taxonomy/formats/json_reader.hpp"
#include "genesis/taxonomy/formats/json_writer.hpp"
#include "genesis/taxonomy/formats/taxopath_generator.hpp"
#include "genesis/taxonomy/functions/operators.hpp"
#include "genesis/taxonomy/functions/taxonomy.hpp"
#include "genesis/taxonomy/iterator/levelorder.hpp"
#include "genesis/taxonomy/iterator/postorder.hpp"
#include "genesis/taxonomy/iterator/preorder.hpp"
#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/formats/json/document.hpp"
#include "genesis/utils/formats/json/reader.hpp"
#include "genesis/utils/formats/json/writer.hpp"
#include "genesis/utils/text/string.hpp"
#include "genesis/utils/text/table.hpp"

#include <algorithm>
#include <cassert>
#include <functional>
#include <limits>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Taxon Kmer Counting
// =================================================================================================

// --------------------------------------------------------------------------
//     accumulate_taxon_sizes
// --------------------------------------------------------------------------

void accumulate_taxon_sizes( Taxonomy& tax )
{
    // Postorder travesal, so that we accumulate values from the outside in,
    // i.e., from lower ranks upwards to the higher ranks.
    postorder_for_each( tax, []( Taxon& taxon )
    {
        // Cast to our data type. Throws an exception if this is not the data type of the taxon.
        auto& data = taxon.data<KmerTaxonData>();

        // Initialize the totals to the values found in the taxon itself.
        data.clade_num_sequences = data.num_sequences;
        data.clade_sum_seq_lengths = data.sum_seq_lengths;

        // Now traverse the immediate children, and add their sums.
        // As we do postorder traversal here, those have already been set in previous iterations.
        // For leaves of the taxonomy, this just does nothing.
        for( auto const& child : taxon ) {
            auto const& child_data = child.data<KmerTaxonData>();
            data.clade_num_sequences += child_data.clade_num_sequences;
            data.clade_sum_seq_lengths += child_data.clade_sum_seq_lengths;
        }
    });
}

// --------------------------------------------------------------------------
//     write_kmer_taxonomy_to_json
// --------------------------------------------------------------------------

void write_kmer_taxonomy_to_json(
    Taxonomy const& tax,
    std::shared_ptr<utils::BaseOutputTarget> target,
    bool with_group_data,
    bool only_trunk
) {
    using utils::JsonDocument;

    auto json_writer = TaxonomyJsonWriter();
    json_writer.taxon_to_json = [with_group_data](
        Taxon const& tax, JsonDocument::ObjectType& obj
    ) {
        auto const& data = tax.data<KmerTaxonData>();
        obj[ "num_sequences" ] = JsonDocument::number_unsigned(
            data.num_sequences
        );
        obj[ "sum_seq_lengths" ] = JsonDocument::number_unsigned(
            data.sum_seq_lengths
        );
        if( with_group_data ) {
            obj[ "clade_num_sequences" ] = JsonDocument::number_unsigned(
                data.clade_num_sequences
            );
            obj[ "clade_sum_seq_lengths" ] = JsonDocument::number_unsigned(
                data.clade_sum_seq_lengths
            );
            switch( data.status ) {
                case KmerTaxonData::Status::kUnprocessed: {
                    obj[ "status" ] = JsonDocument::string( "unprocessed" );
                    break;
                }
                case KmerTaxonData::Status::kGroupAssigned: {
                    obj[ "status" ] = JsonDocument::string( "assigned" );
                    obj[ "index" ] = JsonDocument::number_unsigned(
                        data.index
                    );
                    break;
                }
                case KmerTaxonData::Status::kGroupExpanded: {
                    obj[ "status" ] = JsonDocument::string( "expanded" );
                    break;
                }
                default: {
                    throw std::invalid_argument( "Invalid KmerTaxonData::GroupStatus" );
                }
            }
        }
    };
    if( only_trunk ) {
        json_writer.recurse_taxon_condition = []( Taxon const& tax )
        {
            // If we only want to print the scaffold of the taxonomy after grouping,
            // we set a condition on the recursion such that we only recurse if the taxon
            // is expanded. If it is not expanded, we still write it (as it will be visited
            // in the TaxonomyJsonWriter), but do not recurse.
            auto const& data = tax.data<KmerTaxonData>();
            if(
                data.status != KmerTaxonData::Status::kGroupAssigned &&
                data.status != KmerTaxonData::Status::kGroupExpanded
            ) {
                throw std::invalid_argument(
                    "Taxonomy has not been grouped, as the group status of a taxon is set to "
                    "KmerTaxonData::Status::kUnprocessed"
                );
            }
            return ( data.status == KmerTaxonData::Status::kGroupExpanded );
        };
    }
    json_writer.write( tax, target );
}

// --------------------------------------------------------------------------
//     read_kmer_taxonomy_from_json
// --------------------------------------------------------------------------

Taxonomy read_kmer_taxonomy_from_json(
    std::shared_ptr<utils::BaseInputSource> source
) {
    auto json_reader = TaxonomyJsonReader();
    json_reader.json_to_taxon = []( utils::JsonDocument::ObjectType const& obj, Taxon& tax )
    {
        tax.reset_data( KmerTaxonData::create() );
        auto& data = tax.data<KmerTaxonData>();

        // We are lenient in what we expect, and only set what is found in the json.
        auto it = obj.begin();
        if(( it = obj.find( "num_sequences" )) != obj.end() ) {
            data.num_sequences = it->second.get_number_unsigned();
        }
        if(( it = obj.find( "sum_seq_lengths" )) != obj.end() ) {
            data.sum_seq_lengths = it->second.get_number_unsigned();
        }
        if(( it = obj.find( "clade_num_sequences" )) != obj.end() ) {
            data.clade_num_sequences = it->second.get_number_unsigned();
        }
        if(( it = obj.find( "clade_sum_seq_lengths" )) != obj.end() ) {
            data.clade_sum_seq_lengths = it->second.get_number_unsigned();
        }
        if(( it = obj.find( "index" )) != obj.end() ) {
            data.index = it->second.get_number_unsigned();
        }
        if(( it = obj.find( "status" )) != obj.end() ) {
            auto const& status = it->second.get_string();
            if( status == "unprocessed" ) {
                data.status = KmerTaxonData::Status::kUnprocessed;
            } else if( status == "assigned" ) {
                data.status = KmerTaxonData::Status::kGroupAssigned;
            } else if( status == "expanded" ) {
                data.status = KmerTaxonData::Status::kGroupExpanded;
            } else {
                throw std::invalid_argument(
                    "Invalid KmerTaxonData::GroupStatus == " + status
                );
            }
        }
    };
    return json_reader.read( source );
}

} // namespace taxonomy
} // namespace genesis

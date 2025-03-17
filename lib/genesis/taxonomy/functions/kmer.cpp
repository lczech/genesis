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
#include "genesis/utils/math/statistics.hpp"
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

// =================================================================================================
//     User Reporting
// =================================================================================================

// --------------------------------------------------------------------------
//     count_taxon_groups_or_partitions
// --------------------------------------------------------------------------

size_t count_taxon_groups_or_partitions( Taxonomy const& tax )
{
    // Iterate the taxonomy, recursing on expanded taxa, and counting
    // unique group/partition indices for the taxa that are assigned to groups.
    std::unordered_set<size_t> indices;
    std::function<void(Taxonomy const&)> recursion_ = [&]( Taxonomy const& taxon )
    {
        for( auto const& child : taxon ) {
            auto const& data = child.data<KmerTaxonData>();
            switch( data.status ) {
                case KmerTaxonData::Status::kGroupAssigned:
                case KmerTaxonData::Status::kPartitionMonophyletic:
                {
                    // For taxa that have been assigned to a group or are monophyletic partitions,
                    // collect their indices. As multiple taxa can be assigned to the same group,
                    // we need to count unique group indices here.
                    if( data.index == std::numeric_limits<size_t>::max() ) {
                        throw std::invalid_argument(
                            "Invalid KmerTaxonData::Status, invalid group index"
                        );
                    }
                    indices.insert( data.index );
                    break;
                }
                case KmerTaxonData::Status::kGroupExpanded:
                case KmerTaxonData::Status::kPartitionParaphyletic:
                {
                    // For taxa that have been expanded (because they are too big), we recurse.
                    // If there is an index (only for partitions), store it as well.
                    if( data.status == KmerTaxonData::Status::kPartitionParaphyletic ) {
                        if( data.index == std::numeric_limits<size_t>::max() ) {
                            throw std::invalid_argument(
                                "Invalid KmerTaxonData::Status, invalid group index"
                            );
                        }
                        indices.insert( data.index );
                    }
                    recursion_( child );
                    break;
                }
                case KmerTaxonData::Status::kUnprocessed:
                default: {
                    throw std::invalid_argument(
                        "Invalid KmerTaxonData::Status, Taxonomy not properly processed"
                    );
                }
            }
        }
    };
    recursion_( tax );

    // Make sure that group indices are consecutive.
    for( size_t i = 0; i < indices.size(); ++i ) {
        if( indices.count( i ) == 0 ) {
            throw std::runtime_error( "Taxonomy contains non-consecutive group/partition indices" );
        }
    }

    // The number of groups is given by the number of unique group indices.
    return indices.size();
}

// --------------------------------------------------------------------------
//     taxononmy_group_or_partition_taxa_list
// --------------------------------------------------------------------------

std::vector<std::vector<Taxon const*>> taxononmy_group_or_partition_taxa_list( Taxonomy const& tax )
{
    // Prepare the result vector with entries for each group.
    auto const num_groups = count_taxon_groups_or_partitions( tax );
    auto result = std::vector<std::vector<Taxon const*>>( num_groups );

    // Iterate the taxonomy, recursing on expanded taxa,
    // and collecting pointers to the top-most assigned taxa.
    std::function<void(Taxonomy const&)> recursion_ = [&]( Taxonomy const& taxon )
    {
        for( auto const& child : taxon ) {
            auto const& data = child.data<KmerTaxonData>();
            switch( data.status ) {
                case KmerTaxonData::Status::kGroupAssigned:
                case KmerTaxonData::Status::kPartitionMonophyletic:
                case KmerTaxonData::Status::kPartitionParaphyletic:
                {
                    // For taxa that have been assigned to a group, collect their taxa.
                    // As multiple taxa can be assigned to the same group (if the combined
                    // sizes are still within the limits of TaxonGroupingLimits),
                    // we need to add all those taxa to the vector for that group.
                    if( data.index == std::numeric_limits<size_t>::max() ) {
                        throw std::invalid_argument(
                            "Invalid KmerTaxonData::Status, invalid group index"
                        );
                    }
                    assert( data.index < result.size() );
                    result[ data.index ].push_back( &child );

                    // For paraphyletic taxa, we recurse, as not all their children
                    // are in the same partition.
                    if( data.status == KmerTaxonData::Status::kPartitionParaphyletic ) {
                        recursion_( child );
                    }
                    break;
                }
                case KmerTaxonData::Status::kGroupExpanded:  {
                    // For taxa that have been expanded (because they are too big), we recurse.
                    recursion_( child );
                    break;
                }
                case KmerTaxonData::Status::kUnprocessed:
                default: {
                    throw std::invalid_argument(
                        "Invalid KmerTaxonData::Status, Taxonomy not properly processed"
                    );
                }
            }
        }
    };
    recursion_( tax );

    // Assert that we have found all groups, i.e., there are no empty inner vectors left.
    assert( std::none_of(
        result.begin(), result.end(),
        []( std::vector<Taxon const*> const& taxa )
        {
            return taxa.empty();
        }
    ));

    return result;
}

// --------------------------------------------------------------------------
//     grouped_or_partitioned_taxonomy_trunk
// --------------------------------------------------------------------------

void grouped_or_partitioned_taxonomy_trunk_( Taxonomy const& tax, Taxonomy& result )
{
    // Parallel recursion on the original Taxonomy and the resulting one:
    // If we decend into a taxon of the input, we also do so in the result,
    // and so at each stage we are in the same rank in both.
    // Basically, we are using the function call stack as our recursion pointer.
    // This way, in each step, we only have to deal with entries at the same level.
    for( auto const& child : tax ) {
        auto& data = child.data<KmerTaxonData>();
        switch( data.status ) {
            case KmerTaxonData::Status::kGroupAssigned:
            case KmerTaxonData::Status::kPartitionMonophyletic:
            {
                // For a taxon that is part of a group, we want to add a group as a pseudo-taxon
                // to the resulting taxonomy. This is a bit cumbersome, as it does not really fit
                // well with how taxonomies usually work...

                // First, make sure that a pseudo-taxon with the group name exists.
                std::string name;
                if( data.status == KmerTaxonData::Status::kGroupAssigned ) {
                    name = "group_" + std::to_string( data.index );
                } else {
                    name = "partition_" + std::to_string( data.index );
                }
                Taxon* group_tax = nullptr;
                if( result.has_child( name )) {
                    group_tax = &result.get_child( name );
                    assert( group_tax->data<KmerTaxonData>().index == data.index );
                } else {
                    group_tax = &result.add_child( name );
                    group_tax->reset_data( KmerTaxonData::create() );
                    auto& group_tax_data = group_tax->data<KmerTaxonData>();
                    group_tax_data.status = KmerTaxonData::Status::kGroupAssigned;
                    group_tax_data.index = data.index;
                }

                // Then, accumulate the sizes of the current taxon to the group.
                auto& group_tax_data = group_tax->data<KmerTaxonData>();
                group_tax_data.clade_num_sequences += data.clade_num_sequences;
                group_tax_data.clade_sum_seq_lengths += data.clade_sum_seq_lengths;
                break;
            }
            case KmerTaxonData::Status::kGroupExpanded:
            case KmerTaxonData::Status::kPartitionParaphyletic:
            {
                std::string prefix;
                if( data.status == KmerTaxonData::Status::kPartitionParaphyletic ) {
                    assert( data.index != std::numeric_limits<size_t>::max() );
                    prefix = "partition_" + std::to_string( data.index ) + " ";
                }
                assert( !result.has_child( prefix + child.name() ));
                auto& exp_tax = result.add_child( prefix + child.name() );
                exp_tax.reset_data( child.data<KmerTaxonData>().clone() );
                grouped_or_partitioned_taxonomy_trunk_( child, exp_tax );
                break;
            }
            case KmerTaxonData::Status::kUnprocessed:
            default: {
                throw std::invalid_argument(
                    "Invalid KmerTaxonData::Status, Taxonomy not properly processed"
                );
            }
        }
    }
}

Taxonomy grouped_or_partitioned_taxonomy_trunk( Taxonomy const& tax )
{
    Taxonomy result;
    grouped_or_partitioned_taxonomy_trunk_( tax, result );
    return result;
}

// --------------------------------------------------------------------------
//     grouped_or_partitioned_taxonomy_report
// --------------------------------------------------------------------------

std::string grouped_or_partitioned_taxonomy_report( Taxonomy const& tax )
{
    // Group statistics
    size_t num_groups = count_taxon_groups_or_partitions( tax );
    auto num_sequences   = std::vector<size_t>( num_groups, 0 );
    auto sum_seq_lengths = std::vector<size_t>( num_groups, 0 );

    // Ungrouped (expanded) statistics
    size_t num_ungrouped_taxa = 0;
    size_t ungrouped_num_sequences = 0;
    size_t ungrouped_sum_seq_lengths = 0;

    // Traverse the taxonomy to get all data
    for( auto const& taxon : preorder( tax )) {
        auto& data = taxon.data<KmerTaxonData>();
        switch( data.status ) {
            case KmerTaxonData::Status::kGroupAssigned:
            case KmerTaxonData::Status::kPartitionMonophyletic:
            case KmerTaxonData::Status::kPartitionParaphyletic:
            {
                // The input to this function shall come from grouped_taxonomy_trunk(),
                // which creates pseudo-groups with this name prefix. Check this.
                if(
                    ! utils::starts_with( taxon.name(), "group_" ) &&
                    ! utils::starts_with( taxon.name(), "partition_" )
                ) {
                    throw std::invalid_argument( "Taxonomy is not the trunk" );
                }

                // Accumulate the statistics
                if( data.status == KmerTaxonData::Status::kPartitionParaphyletic ) {
                    num_sequences[data.index]   += data.num_sequences;
                    sum_seq_lengths[data.index] += data.sum_seq_lengths;
                } else {
                    num_sequences[data.index]   += data.clade_num_sequences;
                    sum_seq_lengths[data.index] += data.clade_sum_seq_lengths;
                }
                break;
            }
            case KmerTaxonData::Status::kGroupExpanded:
            {
                ++num_ungrouped_taxa;
                ungrouped_num_sequences   += data.num_sequences;
                ungrouped_sum_seq_lengths += data.sum_seq_lengths;
                break;
            }
            case KmerTaxonData::Status::kUnprocessed:
            default:
            {
                throw std::invalid_argument(
                    "Invalid KmerTaxonData::Status, Taxonomy not properly processed"
                );
            }
        }
    }

    // Quartiles computation needs sorted ranges
    std::sort( num_sequences.begin(), num_sequences.end() );
    std::sort( sum_seq_lengths.begin(), sum_seq_lengths.end() );

    // Produce a report from the accumulated statistics.
    size_t const grouped_num_sequences = std::accumulate(
        num_sequences.begin(), num_sequences.end(), static_cast<size_t>(0)
    );
    size_t const grouped_sum_seq_lengths = std::accumulate(
        sum_seq_lengths.begin(), sum_seq_lengths.end(), static_cast<size_t>(0)
    );
    auto const arithmetic_mean_ns = utils::arithmetic_mean(
        num_sequences.begin(), num_sequences.end()
    );
    auto const arithmetic_mean_sl = utils::arithmetic_mean(
        sum_seq_lengths.begin(), sum_seq_lengths.end()
    );
    auto const quartiles_ns = utils::quartiles(
        num_sequences.begin(), num_sequences.end()
    );
    auto const quartiles_sl = utils::quartiles(
        sum_seq_lengths.begin(), sum_seq_lengths.end()
    );
    std::stringstream ss;
    ss << num_groups << " groups/partitions:\n\n";
    utils::Table summary;
    summary.add_column( "stat" );
    summary.add_column( "num_sequences",   utils::Table::Column::Justification::kRight );
    summary.add_column( "sum_seq_lengths", utils::Table::Column::Justification::kRight );
    summary << "sum" << grouped_num_sequences << grouped_sum_seq_lengths;
    summary << "avg" << arithmetic_mean_ns << arithmetic_mean_sl;
    summary << "q0"  << quartiles_ns.q0 << quartiles_sl.q0;
    summary << "q1"  << quartiles_ns.q1 << quartiles_sl.q1;
    summary << "q2"  << quartiles_ns.q2 << quartiles_sl.q2;
    summary << "q3"  << quartiles_ns.q3 << quartiles_sl.q3;
    summary << "q4"  << quartiles_ns.q4 << quartiles_sl.q4;
    ss << summary.to_string( utils::simple_grid() );
    ss << "\n" << num_ungrouped_taxa << " ungrouped taxa:\n\n";
    summary.clear_content();
    summary << "sum" << ungrouped_num_sequences << ungrouped_sum_seq_lengths;
    ss << summary.to_string( utils::simple_grid() );
    ss << "\n";
    return ss.str();
}

// =================================================================================================
//     Json Format
// =================================================================================================

// --------------------------------------------------------------------------
//     write_taxonomy_grouping_or_partitioning_to_json
// --------------------------------------------------------------------------

void fill_json_array_with_taxonomy_groups_or_partitions_(
    Taxonomy const& tax,
    utils::JsonDocument::ArrayType& array
) {
    using utils::JsonDocument;

    // Iterate the children, adding their data to the groups they were assigned to.
    for( auto const& child : tax ) {
        auto& data = child.data<KmerTaxonData>();
        switch( data.status ) {
            case KmerTaxonData::Status::kGroupAssigned:
            case KmerTaxonData::Status::kPartitionMonophyletic:
            case KmerTaxonData::Status::kPartitionParaphyletic:
            {
                // Create entries in the resulting json array for the indices of groups up until
                // we have an entry for the group of the current taxon, and init as needed.
                while( array.size() <= data.index ) {
                    array.push_back( JsonDocument::object() );
                    auto& obj = array.back().get_object();
                    obj["index"] = JsonDocument::number_unsigned( array.size() - 1 );
                    obj["taxa"] = JsonDocument::array();
                    obj["num_sequences"] = JsonDocument::number_unsigned( 0 );
                    obj["sum_seq_lengths"] = JsonDocument::number_unsigned( 0 );
                }

                // We now have an entry in the array for the group index we need.
                // Amend that entry with the data of the current taxon.
                assert( array.size() > data.index );
                auto& entry = array[data.index].get_object();
                assert( entry["index"].get_number_unsigned() == data.index );
                entry["taxa"].get_array().push_back( JsonDocument::string(
                    TaxopathGenerator().to_string( child )
                ));
                if( data.status == KmerTaxonData::Status::kPartitionParaphyletic ) {
                    entry["num_sequences"].get_number_unsigned() += data.num_sequences;
                    entry["sum_seq_lengths"].get_number_unsigned() += data.sum_seq_lengths;
                } else {
                    entry["num_sequences"].get_number_unsigned() += data.clade_num_sequences;
                    entry["sum_seq_lengths"].get_number_unsigned() += data.clade_sum_seq_lengths;
                }

                // No recursion here for the assigned/monophyletic clases - we already have all the
                // information we need from the clade sum variables, and we only want to store
                // the highest tax rank of the taxon. For the paraphyletic case, we recurse.
                if( data.status == KmerTaxonData::Status::kPartitionParaphyletic ) {
                    fill_json_array_with_taxonomy_groups_or_partitions_( child, array );
                }
                break;
            }
            case KmerTaxonData::Status::kGroupExpanded:
            {
                // If the child is not in a group itself, but was expanded, we recurse.
                fill_json_array_with_taxonomy_groups_or_partitions_( child, array );
                break;
            }
            case KmerTaxonData::Status::kUnprocessed:
            default:
            {
                throw std::invalid_argument(
                    "Invalid KmerTaxonData::Status, Taxonomy not properly processed"
                );
            }
        }
    }
}

void write_taxonomy_grouping_or_partitioning_to_json(
    Taxonomy const& tax,
    std::shared_ptr<utils::BaseOutputTarget> target
) {
    auto doc = utils::JsonDocument::array();
    fill_json_array_with_taxonomy_groups_or_partitions_( tax, doc.get_array() );
    utils::JsonWriter().write( doc, target );
}

// --------------------------------------------------------------------------
//     read_taxonomy_grouping_or_partitioning_from_json
// --------------------------------------------------------------------------

std::vector<TaxonomyGroupPartitionData> read_taxonomy_grouping_or_partitioning_from_json(
    std::shared_ptr<utils::BaseInputSource> source
) {
    std::vector<TaxonomyGroupPartitionData> result;
    auto doc = utils::JsonReader().read( source );
    auto& arr = doc.get_array();
    result.reserve( arr.size() );
    for( auto& child : arr ) {
        TaxonomyGroupPartitionData elem;
        elem.index = child.at( "index" ).get_number_unsigned();
        if( elem.index != result.size() ) {
            throw std::runtime_error(
                "Taxonomy grouping/partitioning json file contains " +
                std::to_string( result.size() ) +
                "entries, but with non-consecutive group indices. "
                "Found index" + std::to_string( elem.index )
            );
        }
        elem.num_sequences = child.at( "num_sequences" ).get_number_unsigned();
        elem.sum_seq_lengths = child.at( "sum_seq_lengths" ).get_number_unsigned();
        for( auto const& tax : child.at( "taxa" ).get_array() ) {
            elem.taxa.push_back( tax.get_string() );
        }
        result.push_back( std::move( elem ));
        child.clear();
    }
    return result;
}

// --------------------------------------------------------------------------
//     write_kmer_taxonomy_to_json
// --------------------------------------------------------------------------

void write_kmer_taxonomy_to_json(
    Taxonomy const& tax,
    std::shared_ptr<utils::BaseOutputTarget> target,
    bool with_group_or_partition_data,
    bool only_trunk
) {
    using utils::JsonDocument;

    auto json_writer = TaxonomyJsonWriter();
    json_writer.taxon_to_json = [with_group_or_partition_data](
        Taxon const& tax, JsonDocument::ObjectType& obj
    ) {
        auto const& data = tax.data<KmerTaxonData>();
        obj[ "num_sequences" ] = JsonDocument::number_unsigned(
            data.num_sequences
        );
        obj[ "sum_seq_lengths" ] = JsonDocument::number_unsigned(
            data.sum_seq_lengths
        );
        if( with_group_or_partition_data ) {
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
                    obj[ "index" ] = JsonDocument::number_unsigned( data.index );
                    break;
                }
                case KmerTaxonData::Status::kGroupExpanded: {
                    obj[ "status" ] = JsonDocument::string( "expanded" );
                    break;
                }
                case KmerTaxonData::Status::kPartitionMonophyletic: {
                    obj[ "status" ] = JsonDocument::string( "monophyletic" );
                    obj[ "index" ] = JsonDocument::number_unsigned( data.index );
                    break;
                }
                case KmerTaxonData::Status::kPartitionParaphyletic: {
                    obj[ "status" ] = JsonDocument::string( "paraphyletic" );
                    obj[ "index" ] = JsonDocument::number_unsigned( data.index );
                    break;
                }
                default: {
                    throw std::invalid_argument( "Invalid KmerTaxonData::Status" );
                }
            }
        }
    };
    if( only_trunk ) {
        json_writer.recurse_taxon_condition = []( Taxon const& tax )
        {
            // If we only want to print the scaffold of the taxonomy after grouping,
            // we set a condition on the recursion such that we only recurse if the taxon
            // is expanded or paraphyletic. If it is not expanded, we still write it
            // (as it will be visited in the TaxonomyJsonWriter), but do not recurse.
            auto const& data = tax.data<KmerTaxonData>();
            if( data.status == KmerTaxonData::Status::kUnprocessed ) {
                throw std::invalid_argument(
                    "Taxonomy has not been grouped/partitioned, as the status of a taxon \"" +
                    tax.name() + "\" is set to KmerTaxonData::Status::kUnprocessed"
                );
            }
            return (
                data.status == KmerTaxonData::Status::kGroupExpanded ||
                data.status == KmerTaxonData::Status::kPartitionParaphyletic
            );
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
            } else if( status == "monophyletic" ) {
                data.status = KmerTaxonData::Status::kPartitionMonophyletic;
            } else if( status == "paraphyletic" ) {
                data.status = KmerTaxonData::Status::kPartitionParaphyletic;
            } else {
                throw std::invalid_argument(
                    "Invalid KmerTaxonData::Status == " + status
                );
            }
        }
    };
    return json_reader.read( source );
}

} // namespace taxonomy
} // namespace genesis

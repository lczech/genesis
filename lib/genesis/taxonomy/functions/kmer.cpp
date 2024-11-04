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
#include "genesis/utils/formats/json/document.hpp"
#include "genesis/utils/formats/json/reader.hpp"
#include "genesis/utils/formats/json/writer.hpp"
#include "genesis/utils/math/statistics.hpp"
#include "genesis/utils/text/string.hpp"
#include "genesis/utils/text/table.hpp"

#include <algorithm>
#include <cassert>
#include <limits>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Taxon Count Grouping
// =================================================================================================

// --------------------------------------------------------------------------
//     Grouping Algorithm
// --------------------------------------------------------------------------

void accumulate_taxon_sizes( Taxonomy& tax )
{
    // Postorder travesal, so that we accumulate values from the outside in,
    // i.e., from lower ranks upwards to the higher ranks.
    postorder_for_each( tax, []( Taxon& taxon ){
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

/**
 * @brief Local helper function to check if a given taxon size exceeds the limits.
 */
bool exceeds_group_sizes_(
    TaxonGroupingSettings const& settings,
    size_t num_sequences,
    size_t sum_seq_lengths
) {
    bool const exceed_num_sequences = (
        settings.max_group_num_sequences > 0 &&
        num_sequences > settings.max_group_num_sequences
    );
    bool const exceed_sum_seq_lengths = (
        settings.max_group_sum_seq_lengths > 0 &&
        sum_seq_lengths > settings.max_group_sum_seq_lengths
    );
    return exceed_num_sequences || exceed_sum_seq_lengths;
}

/**
 * @brief Local helper function that recursively assigns the group of the parent taxon
 * to all children.
 */
void assign_children_to_group_(
    Taxon& taxon
) {
    // Get the data and assert that we are in the right type of taxon
    auto& data = taxon.data<KmerTaxonData>();
    assert( data.group_status = KmerTaxonData::GroupStatus::kAssigned );
    assert( data.group_index != std::numeric_limits<size_t>::max() );

    // In some previous iteration of the procedur, we already decided that this taxon here is
    // small enough to fit in a group. So all we have to do is to assign that same group to its
    // children here. That will then also be repeated (in the later iterations of the recursion)
    // for their children, and so on, so that the whole subtree/clade of this taxon gets their
    // data set to point to the same group index.
    for( auto& child : taxon ) {
        auto& child_data = child.data<KmerTaxonData>();
        child_data.group_status = data.group_status;
        child_data.group_index = data.group_index;
        assign_children_to_group_( child );
    }
}

/**
 * @brief Local helper function to process the children of a Taxon for which we already
 * know that it is too big for a single group, and hence was expanded.
 */
void group_by_taxon_sizes_process_taxon_(
    TaxonGroupingSettings const& settings,
    Taxonomy& tax,
    size_t& next_index
) {
    // We are going to process all children of the current taxon. We are either deciding
    // to expand them if they are too big, or to group them with their siblings up until
    // they reach the max size. To keep track of the latter, we need a list of siblings
    // that have been processed already: This vector stores the group indicies used already
    // for the children of the current taxon, and their cumulative sizes.
    struct GroupSummary
    {
        size_t index = 0;
        size_t clade_num_sequences = 0;
        size_t clade_sum_seq_lengths = 0;
    };
    std::vector<GroupSummary> sibling_groups;

    // Process the children of the taxon, grouping them as needed. There are two outcomes of that:
    // Either we decid to break it apart, because it exceeded some size limit, or we assign
    // it to a group.
    for( auto& child_taxon : tax ) {
        auto& child_data = child_taxon.data<KmerTaxonData>();

        // Determine if this taxon is too big or small enough to be (part of) a single group.
        // If it is too big, and has children, we expand it. It can however happen that a taxon
        // is too big for our limits, but has no children (meaning, all ref sequences are assigned
        // to that one leaf taxon). In that case, we just have to live with a large group.
        bool const exceeds_limits = exceeds_group_sizes_(
            settings, child_data.clade_num_sequences, child_data.clade_sum_seq_lengths
        );
        if( exceeds_limits && child_taxon.size() > 0 ) {
            child_data.group_status = KmerTaxonData::GroupStatus::kExpanded;
            group_by_taxon_sizes_process_taxon_( settings, child_taxon, next_index );

        } else {
            // If the child taxon is not too big (or a leaf), we try to combine it into a group
            // with its other small siblings, in order to reduce the number of groups.
            // Find the first index of the child group list that we can merge this taxon into
            // without exceeding the sizes.
            // In a sense, out of all siblings that are smaller than the max size, we are
            // solving the knappsack problem here, greedily by assigning groups on a first come
            // first served basis to the lowest index group that has space for the new sibling.
            auto sibling_group_index = std::numeric_limits<size_t>::max();
            if( settings.merge_sibling_taxa ) {
                for( size_t i = 0; i < sibling_groups.size(); ++i ) {
                    auto const& sibling_group = sibling_groups[i];
                    auto const proposed_num_seq = (
                        sibling_group.clade_num_sequences +
                        child_data.clade_num_sequences
                    );
                    auto const proposed_seq_len = (
                        sibling_group.clade_sum_seq_lengths +
                        child_data.clade_sum_seq_lengths
                    );
                    if( ! exceeds_group_sizes_( settings, proposed_num_seq, proposed_seq_len )) {
                        sibling_group_index = i;
                        break;
                    }
                }
            }

            // If there was no such index, either we do not want to merge siblings, or all groups
            // are already filled to a point where adding this taxon would exceed the limit, or
            // the list is still empty. Either way, we want to make a new group then for the taxon.
            if( sibling_group_index == std::numeric_limits<size_t>::max() ) {
                sibling_group_index = sibling_groups.size();
                sibling_groups.emplace_back();
                sibling_groups.back().index = next_index;
                ++next_index;
            }

            // Now we know the group that we want to put this taxon into.
            // Set this, and then recursively set this group for all its children as well.
            auto& sibling_group = sibling_groups[sibling_group_index];
            sibling_group.clade_num_sequences += child_data.clade_num_sequences;
            sibling_group.clade_sum_seq_lengths += child_data.clade_sum_seq_lengths;
            child_data.group_status = KmerTaxonData::GroupStatus::kAssigned;
            child_data.group_index = sibling_group.index;
            assign_children_to_group_( child_taxon );
        }
    }
}

void group_by_taxon_sizes(
    TaxonGroupingSettings const& settings,
    Taxonomy& tax
) {
    // Check that the taxonoy has the correct data type everywhere.
    if( ! taxonomy_data_is<KmerTaxonData>( tax ) ) {
        throw std::invalid_argument( "Cannot group taxonomy without it having KmerTaxonData" );
    }

    // Accumulate all sizes towards the higher ranks.
    accumulate_taxon_sizes( tax );

    // Initialize all group assignments to the unprocessed status.
    preorder_for_each( tax, []( Taxon& taxon ){
        auto& data = taxon.data<KmerTaxonData>();
        data.group_status = KmerTaxonData::GroupStatus::kUnprocessed;
        data.group_index = std::numeric_limits<size_t>::max();
    });

    // We will be assining consecutive indices to groups.
    // This variable indicates the next index to be used, and is incremented for each new group.
    size_t next_index = 0;

    // Now run the main recursion. This effectively is a preorder traversal of the taxonomy, i.e.,
    // start at the root (highest rank), and decent into the lower ranks one by one.
    // At each taxon we visit, we assess its children and decide how to group them.
    group_by_taxon_sizes_process_taxon_( settings, tax, next_index );
}

// --------------------------------------------------------------------------
//     User Reporting
// --------------------------------------------------------------------------

void grouped_taxonomy_trunk_( Taxonomy const& tax, Taxonomy& result )
{
    // Parallel recursion on the original Taxonomy and the resulting one:
    // If we decend into a taxon of the input, we also do so in the result,
    // and so at each stage we are in the same rank in both.
    // Basically, we are using the function call stack as our recursion pointer.
    // This way, in each step, we only have to deal with entries at the same level.
    for( auto const& child : tax ) {
        auto& data = child.data<KmerTaxonData>();
        switch( data.group_status ) {
            case KmerTaxonData::GroupStatus::kAssigned: {
                // For a taxon that is part of a group, we want to add a group as a pseudo-taxon
                // to the resulting taxonomy. This is a bit cumbersome, as it does not really fit
                // well with how taxonomies usually work...

                // First, make sure that a pseudo-taxon with the group name exists.
                std::string const group_name = "group_" + std::to_string( data.group_index );
                Taxon* group_tax = nullptr;
                if( result.has_child( group_name )) {
                    group_tax = &result.get_child( group_name );
                    assert( group_tax->data<KmerTaxonData>().group_index == data.group_index );
                } else {
                    group_tax = &result.add_child( group_name );
                    group_tax->reset_data( KmerTaxonData::create() );
                    auto& group_tax_data = group_tax->data<KmerTaxonData>();
                    group_tax_data.group_status = KmerTaxonData::GroupStatus::kAssigned;
                    group_tax_data.group_index = data.group_index;
                }

                // Then, accumulate the sizes of the current taxon to the group.
                auto& group_tax_data = group_tax->data<KmerTaxonData>();
                group_tax_data.clade_num_sequences += data.clade_num_sequences;
                group_tax_data.clade_sum_seq_lengths += data.clade_sum_seq_lengths;
                break;
            }
            case KmerTaxonData::GroupStatus::kExpanded: {
                assert( !result.has_child( child.name() ));
                auto& exp_tax = result.add_child( child.name() );
                exp_tax.reset_data( child.data<KmerTaxonData>().clone() );
                grouped_taxonomy_trunk_( child, exp_tax );
                break;
            }
            case KmerTaxonData::GroupStatus::kUnprocessed: {
                throw std::invalid_argument(
                    "Taxonomy has not been grouped, as the group status of a taxon "
                    "is set to KmerTaxonData::GroupStatus::kUnprocessed"
                );
            }
            default: {
                throw std::invalid_argument(
                    "Invalid KmerTaxonData::GroupStatus"
                );
            }
        }
    }
}

Taxonomy grouped_taxonomy_trunk( Taxonomy const& tax )
{
    Taxonomy result;
    grouped_taxonomy_trunk_( tax, result );
    return result;
}

std::string grouped_taxonomy_report( Taxonomy const& tax )
{
    // Group statistics
    size_t num_groups = 0;
    std::vector<size_t> group_num_sequences;
    std::vector<size_t> group_sum_seq_lengths;

    // Ungrouped (expanded) statistics
    size_t num_ungrouped_taxa = 0;
    size_t ungrouped_num_sequences = 0;
    size_t ungrouped_sum_seq_lengths = 0;

    // Traverse the taxonomy to get all data
    for( auto const& taxon : preorder( tax )) {
        auto& data = taxon.data<KmerTaxonData>();
        switch( data.group_status ) {
            case KmerTaxonData::GroupStatus::kAssigned: {
                // The input to this function shall come from grouped_taxonomy_trunk(),
                // which creates pseudo-groups with this name prefix. Check this.
                if( !utils::starts_with( taxon.name(), "group_" )) {
                    throw std::invalid_argument( "Taxonomy is not the trunk" );
                }

                // Accumulate the statistics
                ++num_groups;
                group_num_sequences.push_back( data.clade_num_sequences );
                group_sum_seq_lengths.push_back( data.clade_sum_seq_lengths );
                break;
            }
            case KmerTaxonData::GroupStatus::kExpanded: {
                ++num_ungrouped_taxa;
                ungrouped_num_sequences += data.num_sequences;
                ungrouped_sum_seq_lengths += data.sum_seq_lengths;
                break;
            }
            case KmerTaxonData::GroupStatus::kUnprocessed: {
                throw std::invalid_argument(
                    "Taxonomy has not been grouped, as the group status of a taxon "
                    "is set to KmerTaxonData::GroupStatus::kUnprocessed"
                );
            }
            default: {
                throw std::invalid_argument(
                    "Invalid KmerTaxonData::GroupStatus"
                );
            }
        }
    }

    // Quartiles computation needs sorted ranges
    std::sort( group_num_sequences.begin(), group_num_sequences.end() );
    std::sort( group_sum_seq_lengths.begin(), group_sum_seq_lengths.end() );

    // Produce a report from the accumulated statistics.
    size_t const grouped_num_sequences = std::accumulate(
        group_num_sequences.begin(), group_num_sequences.end(), static_cast<size_t>(0)
    );
    size_t const grouped_sum_seq_lengths = std::accumulate(
        group_sum_seq_lengths.begin(), group_sum_seq_lengths.end(), static_cast<size_t>(0)
    );
    auto const quartiles_ns = utils::quartiles(
        group_num_sequences.begin(), group_num_sequences.end()
    );
    auto const quartiles_sl = utils::quartiles(
        group_sum_seq_lengths.begin(), group_sum_seq_lengths.end()
    );
    std::stringstream ss;
    ss << num_groups << " groups:\n\n";
    utils::Table summary;
    summary.add_column( "stat" );
    summary.add_column( "num_sequences",   utils::Table::Column::Justification::kRight );
    summary.add_column( "sum_seq_lengths", utils::Table::Column::Justification::kRight );
    summary << "sum" << grouped_num_sequences << grouped_sum_seq_lengths;
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
//     Group List
// --------------------------------------------------------------------------

void fill_json_array_with_taxonomy_groups_(
    Taxonomy const& tax,
    utils::JsonDocument::ArrayType& array
) {
    using utils::JsonDocument;

    // Iterate the children, adding their data to the groups they were assigned to.
    for( auto const& child : tax ) {
        auto& data = child.data<KmerTaxonData>();
        switch( data.group_status ) {
            case KmerTaxonData::GroupStatus::kAssigned: {
                // Create entries in the resulting json array for the indices of groups up until
                // we have an entry for the group of the current taxon, and init as needed.
                while( array.size() <= data.group_index ) {
                    array.push_back( JsonDocument::object() );
                    auto& obj = array.back().get_object();
                    obj["group_index"] = JsonDocument::number_unsigned( array.size() - 1 );
                    obj["taxa"] = JsonDocument::array();
                    obj["num_sequences"] = JsonDocument::number_unsigned( 0 );
                    obj["sum_seq_lengths"] = JsonDocument::number_unsigned( 0 );
                }

                // We now have an entry in the array for the group index we need.
                // Amend that entry with the data of the current taxon.
                assert( array.size() > data.group_index );
                auto& entry = array[data.group_index].get_object();
                assert( entry["group_index"].get_number_unsigned() == data.group_index );
                entry["taxa"].get_array().push_back( JsonDocument::string(
                    TaxopathGenerator().to_string( child )
                ));
                entry["num_sequences"].get_number_unsigned() += data.clade_num_sequences;
                entry["sum_seq_lengths"].get_number_unsigned() += data.clade_sum_seq_lengths;

                // No recursion here - we already have all the information we need from the clade
                // sum variables, and we only want to store the highest tax rank of the taxon.
                break;
            }
            case KmerTaxonData::GroupStatus::kExpanded: {
                // If the child is not in a group itself, but was expanded, we recurse.
                fill_json_array_with_taxonomy_groups_( child, array );
                break;
            }
            case KmerTaxonData::GroupStatus::kUnprocessed: {
                throw std::invalid_argument(
                    "Taxonomy has not been grouped, as the group status of a taxon "
                    "is set to KmerTaxonData::GroupStatus::kUnprocessed"
                );
            }
            default: {
                throw std::invalid_argument(
                    "Invalid KmerTaxonData::GroupStatus"
                );
            }
        }
    }
}

void write_taxonomy_grouping_to_json(
    Taxonomy const& tax,
    std::shared_ptr<utils::BaseOutputTarget> target
) {
    auto doc = utils::JsonDocument::array();
    fill_json_array_with_taxonomy_groups_( tax, doc.get_array() );
    utils::JsonWriter().write( doc, target );
}

// --------------------------------------------------------------------------
//     Write Taxonomy
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
    ){
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
            switch( data.group_status ) {
                case KmerTaxonData::GroupStatus::kUnprocessed: {
                    obj[ "group_status" ] = JsonDocument::string( "unprocessed" );
                    break;
                }
                case KmerTaxonData::GroupStatus::kAssigned: {
                    obj[ "group_status" ] = JsonDocument::string( "assigned" );
                    obj[ "group_index" ] = JsonDocument::number_unsigned(
                        data.group_index
                    );
                    break;
                }
                case KmerTaxonData::GroupStatus::kExpanded: {
                    obj[ "group_status" ] = JsonDocument::string( "expanded" );
                    break;
                }
                default: {
                    throw std::invalid_argument( "Invalid KmerTaxonData::GroupStatus" );
                }
            }
        }
    };
    if( only_trunk ) {
        json_writer.recurse_taxon_condition = []( Taxon const& tax ){
            // If we only want to print the scaffold of the taxonomy after grouping,
            // we set a condition on the recursion such that we only recurse if the taxon
            // is expanded. If it is not expanded, we still write it (as it will be visited
            // in the TaxonomyJsonWriter), but do not recurse.
            auto const& data = tax.data<KmerTaxonData>();
            if(
                data.group_status != KmerTaxonData::GroupStatus::kAssigned &&
                data.group_status != KmerTaxonData::GroupStatus::kExpanded
            ) {
                throw std::invalid_argument(
                    "Taxonomy has not been grouped, as the group status of a taxon is set to "
                    "KmerTaxonData::GroupStatus::kUnprocessed"
                );
            }
            return ( data.group_status == KmerTaxonData::GroupStatus::kExpanded );
        };
    }
    json_writer.write( tax, target );
}

// --------------------------------------------------------------------------
//     Read Taxonomy
// --------------------------------------------------------------------------

Taxonomy read_kmer_taxonomy_from_json(
    std::shared_ptr<utils::BaseInputSource> source
) {
    auto json_reader = TaxonomyJsonReader();
    json_reader.json_to_taxon = []( utils::JsonDocument::ObjectType const& obj, Taxon& tax ){
        tax.reset_data( KmerTaxonData::create() );
        auto& data = tax.data<KmerTaxonData>();
        data.num_sequences = obj.at( "num_sequences" ).get_number_unsigned();
        data.sum_seq_lengths = obj.at( "sum_seq_lengths" ).get_number_unsigned();

        // Check if the optional grouping-related elements are there, and set them.
        auto it = obj.begin();
        if(( it = obj.find( "clade_num_sequences" )) != obj.end() ) {
            data.clade_num_sequences = it->second.get_number_unsigned();
        }
        if(( it = obj.find( "clade_sum_seq_lengths" )) != obj.end() ) {
            data.clade_sum_seq_lengths = it->second.get_number_unsigned();
        }
        if(( it = obj.find( "group_index" )) != obj.end() ) {
            data.group_index = it->second.get_number_unsigned();
        }
        if(( it = obj.find( "group_status" )) != obj.end() ) {
            auto const& group_status = it->second.get_string();
            if( group_status == "unprocessed" ) {
                data.group_status = KmerTaxonData::GroupStatus::kUnprocessed;
            } else if( group_status == "assigned" ) {
                data.group_status = KmerTaxonData::GroupStatus::kAssigned;
            } else if( group_status == "expanded" ) {
                data.group_status = KmerTaxonData::GroupStatus::kExpanded;
            } else {
                throw std::invalid_argument(
                    "Invalid KmerTaxonData::GroupStatus == " + group_status
                );
            }
        }
    };
    return json_reader.read( source );
}

} // namespace taxonomy
} // namespace genesis

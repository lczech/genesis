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

#include "genesis/taxonomy/functions/entropy.hpp"

#include "genesis/taxonomy/formats/taxopath_generator.hpp"
#include "genesis/taxonomy/functions/taxonomy.hpp"
#include "genesis/taxonomy/iterator/preorder.hpp"
#include "genesis/taxonomy/iterator/levelorder.hpp"
#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"

#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/math/common.hpp"
#include "genesis/utils/text/style.hpp"

#include <assert.h>
#include <algorithm>
#include <functional>
#include <limits>
#include <numeric>
#include <map>
#include <stdexcept>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Prune
// =================================================================================================

void prune_by_entropy(
    Taxonomy&              taxonomy,
    size_t                 target_taxonomy_size,
    PruneByEntropySettings settings
) {

    // -------------------------------------------------------------------------
    //     Init
    // -------------------------------------------------------------------------

    // Basic check.
    if( taxa_count_lowest_levels( taxonomy ) < target_taxonomy_size ) {
        LOG_INFO << "Taxonomy only has " + std::to_string( taxa_count_lowest_levels( taxonomy ) )
                 << " leaf Taxa. Pruning with " + std::to_string( target_taxonomy_size )
                 << " leaves thus includes the whole taxonomy.";

        // Expand fully.
        for( auto it : preorder( taxonomy ) ) {
            auto& status = it.taxon().data<EntropyTaxonData>().status;
            if( it.taxon().size() == 0 ) {
                status = EntropyTaxonData::PruneStatus::kBorder;
            } else {
                status = EntropyTaxonData::PruneStatus::kInside;
            }
        }

        return;

        // throw std::runtime_error(
        //     "Taxonomy only has " + std::to_string( taxa_count_lowest_levels( taxonomy ) ) +
        //     " leaf Taxa. Cannot prune it with " + std::to_string( target_taxonomy_size ) + " leaves."
        // );
    }

    // Init all taxa to be outside of the pruned taxonomy.
    preorder_for_each( taxonomy, [] ( Taxon& t ) {
        t.data<EntropyTaxonData>().status = EntropyTaxonData::PruneStatus::kOutside;
    });

    // Current taxa of the border of pruning, sorted by their entropy.
    // We will do a preorder traversal of the Taxonomy, in order to decide which taxa make up the
    // border of the pruning. We do not go deeper into taxa that should end up being the border.
    // We use this list to decide which taxa to expand: Always go deeper (i.e., set status to inner)
    // into the taxon with the highest entropy.
    std::multimap< double, Taxon* > border_candidates;

    // Keep track of how many border taxa we have. This value should approach the target size.
    size_t border_taxa_count = 0;

    // -------------------------------------------------------------------------
    //     Helper Functions
    // -------------------------------------------------------------------------

    // Helper function that adds a taxon to the border and sets it as a border candidate,
    // unless it has too few leaves. In this case, it and its children will become inner and
    // the leaves border taxa. That is, this taxon will be fully contained and not pruned.
    std::function< void ( Taxon& ) > add_taxon_to_border = [&] ( Taxon& taxon ) {

        if( taxon.data<EntropyTaxonData>().status != EntropyTaxonData::PruneStatus::kOutside ) {
            LOG_WARN << "Taxon added to border not outside ("
                     << EntropyTaxonData::status_abbreviation( taxon.data<EntropyTaxonData>().status )
                     <<  "): " << TaxopathGenerator()( taxon );
        }

        // If the taxon has fewer leaves than the threshold (but is not a leaf itself),
        // we make the whole sub-taxonomy a part of the pruned taxonomy.
        if(
            taxon.size() > 0 &&
            settings.min_subtaxonomy_size > 0 &&
            taxa_count_lowest_levels( taxon ) < settings.min_subtaxonomy_size
        ) {

            // First, the taxon itself is inside.
            taxon.data<EntropyTaxonData>().status = EntropyTaxonData::PruneStatus::kInside;

            // Then, iterate it, and set all taxa in it to inner or
            // border, depending on whether they are inner of leaf taxa.
            for( auto it : preorder( taxon ) ) {
                auto& sub_taxon_data = it.taxon().data<EntropyTaxonData>();
                if( it.taxon().size() == 0 ) {
                    sub_taxon_data.status = EntropyTaxonData::PruneStatus::kBorder;
                    ++border_taxa_count;
                } else {
                    sub_taxon_data.status = EntropyTaxonData::PruneStatus::kInside;
                }
            }

        } else if( taxon.size() == 1 ) {

            // If a Taxon has only one child, there is no need in adding this Taxon as a border
            // candidate.
            // We can instead directly add its child. This will not increase the resulting
            // number of border taxa, as we still add only one. This mainly avoids to stop too
            // early, which would result in branches of the taxonomy that only contain one
            // child anyway, so it would make little sense to prune there.

            taxon.data<EntropyTaxonData>().status = EntropyTaxonData::PruneStatus::kInside;
            add_taxon_to_border( taxon.at(0) );

        } else {

            // If the taxon has more leaves than the min subtax threshold, make it a border taxon.
            taxon.data<EntropyTaxonData>().status = EntropyTaxonData::PruneStatus::kBorder;
            ++border_taxa_count;

            // Also, if it has children, add the taxon as a new candidate for expanding.
            // Thus, in the pruning algorithm, it can be used to go deeper into the taxonomy.
            if( taxon.size() > 0 ) {
                border_candidates.emplace( taxon.data<EntropyTaxonData>().entropy, &taxon );
            }
        }
    };

    // Helper function to expand a taxon, make it inner and its children new border candidates.
    std::function< void ( Taxon& ) > expand_taxon = [&] (
        Taxon& taxon
    ) {
        auto& status = taxon.data<EntropyTaxonData>().status;

        if( status != EntropyTaxonData::PruneStatus::kBorder ) {
            LOG_WARN << "Expanding Taxon with status "
                     << EntropyTaxonData::status_abbreviation( status )
                     << ": " << TaxopathGenerator()( taxon );
        }

        // The taxon is expanded, thus it is inner now.
        status = EntropyTaxonData::PruneStatus::kInside;
        --border_taxa_count;

        // All its children then form the new border.
        for( auto& child : taxon ) {
            add_taxon_to_border( child );
        }
    };

    // -------------------------------------------------------------------------
    //     Min Border Level
    // -------------------------------------------------------------------------

    // If we want to have a certain minimum level of the taxonomy fully inside the final taxonomy,
    // do this before we start with the actual pruning algorithm.
    if( settings.min_border_level > 0 ) {

        // Helper function that sets a taxon to inside (or border, if it is a leaf) if it has a
        // too low level. Thus, low level taxa are fully expaneded.
        auto include_min_level_taxa = [&] ( Taxon& taxon ) {

            // If the taxon has a lower level than the min...
            if( taxon_level( taxon ) < settings.min_border_level ) {

                // Make it inside, if it has childern, or a border, if it is a leaf.
                if( taxon.size() > 0 ) {
                    taxon.data<EntropyTaxonData>().status = EntropyTaxonData::PruneStatus::kInside;
                } else {
                    taxon.data<EntropyTaxonData>().status = EntropyTaxonData::PruneStatus::kBorder;
                    ++border_taxa_count;
                }

            // If it is just at the level, make it a border, and make it a candidate for
            // further expansion.
            } else if( taxon_level( taxon ) == settings.min_border_level ) {
                add_taxon_to_border( taxon );
            }
        };

        levelorder_for_each( taxonomy, include_min_level_taxa );
    }

    // LOG_DBG << "after min border level. num cands " << border_candidates.size();
    // LOG_DBG1 << "valid " << ( validate_pruned_taxonomy( taxonomy ) ? "1" : "nooooooooooooooo" );
    // for( auto const& elem : border_candidates ) {
    //     LOG_DBG1 << "H\t" << elem.first << "\tname\t" << TaxopathGenerator()( *elem.second ) << "\tlevel\t"
    //              << taxon_level( *elem.second ) << "\tchildren\t" << taxa_count_lowest_levels( *elem.second  );
    // }

    // -------------------------------------------------------------------------
    //     Max Subtaxonomy Size
    // -------------------------------------------------------------------------

    // If we want to avoid taxa that are too big, do this in a preprocessing step and only
    // after this start the actual entropy pruning phase.
    if( settings.max_subtaxonomy_size > 0 ) {

        // Helper function that recursively visits taxa and makes them inside taxa if they are
        // too big. Small enough taxa will end up being the border.
        std::function< void ( Taxon& ) > resolve_big_subtaxa = [&] (
            Taxon& taxon
        ) {
            auto& status = taxon.data<EntropyTaxonData>().status;

            if(
                taxa_count_lowest_levels( taxon ) > settings.max_subtaxonomy_size
                || taxon.size() == 1
                || status == EntropyTaxonData::PruneStatus::kInside
            ) {

                // If the taxon has too many leaves, make it an inside taxon and recurse this
                // function on its children (until they are all small enough).
                // Also, if it has exactly one child, do this - for the resons explained in
                // add_taxon_to_border().
                // Furthermore, if it already is an inside taxon (from the min border level init),
                // we want to keep it this way, so we simply recurse.

                // If we turn a border taxon into inner, we need to descrese the counter, and
                //  if it was an entry in the border candidates, remove it.
                if( status == EntropyTaxonData::PruneStatus::kBorder ) {

                    auto cand = std::find_if(
                        border_candidates.begin(),
                        border_candidates.end(),
                        [ &taxon ] ( std::pair< const double, Taxon* >& entry ) {
                            return &taxon == entry.second;
                        }
                    );

                    if( cand != border_candidates.end() ) {
                        // If we found the taxon in the border cand list, it means that it has
                        // children, otherwise it would never have been added there.
                        assert( taxon.size() > 0 );

                        // Now, remove it.
                        border_candidates.erase( cand );
                    }
                    --border_taxa_count;
                }

                status = EntropyTaxonData::PruneStatus::kInside;
                for( auto& child : taxon ) {
                    resolve_big_subtaxa( child );
                }

            } else if(
                status == EntropyTaxonData::PruneStatus::kOutside
            ) {

                // If the taxon is small enough, do not recurse.
                // Instead, add the taxon as a border candidate.
                // We do not want to change the state of their children then - thus, those stay
                // outside for the moment. Later, the entropy pruning phase can then start from there.
                add_taxon_to_border( taxon );

            } else {

                // The only case that is not treated so far is a taxon that is already border,
                // but is smaller than the max subtax size, so we do not need to do anything, as
                // this taxon is already properly in the list.
                assert( status == EntropyTaxonData::PruneStatus::kBorder );
            }
        };

        // Run the resolve helper function for the taxonomy.
        for( auto& taxon : taxonomy ) {
            resolve_big_subtaxa( taxon );
        }
    }

    // LOG_DBG << "after max subtax size. num cands " << border_candidates.size();
    // LOG_DBG1 << "valid " << ( validate_pruned_taxonomy( taxonomy ) ? "1" : "nooooooooooooooo" );
    // for( auto const& elem : border_candidates ) {
    //     LOG_DBG1 << "H\t" << elem.first << "\tname\t" << TaxopathGenerator()( *elem.second ) << "\tlevel\t"
    //              << taxon_level( *elem.second ) << "\tchildren\t" << taxa_count_lowest_levels( *elem.second  );
    // }

    // -------------------------------------------------------------------------
    //     Default Init
    // -------------------------------------------------------------------------

    // If we use neither of the min border level or max subtax size settings, we need
    // to init the border front with just the first level of the taxonomy.
    if( settings.min_border_level == 0 && settings.max_subtaxonomy_size == 0 ) {

        // Init with first level. See expand_taxon() for details.
        for( auto& child : taxonomy ) {
            add_taxon_to_border( child );
        }
    }

    // -------------------------------------------------------------------------
    //     Main Loop
    // -------------------------------------------------------------------------

    // Loop until we have done enough pruning, i.e., if we exceeded the target size.
    while( border_taxa_count < target_taxonomy_size ) {

        // We already checked that we will have enough leaf taxa to achieve the target size.
        // So, there should always be candidates to choose for pruning.
        assert( border_candidates.size() > 0 );

        // Get the taxon with the highest entropy from the candidates list. This will be the front
        // for going deeper into the taxonomy, i.e., for going from border to inner.
        // Assert that the iterators do their thing correctly and the last element is the last.
        // (Those reverse iterations freak me out. They provoke off-by-one errors.)
        auto cur_front = *border_candidates.rbegin();
        assert( *std::prev(border_candidates.end()) == *border_candidates.rbegin() );

        // Remove the current front from the candidates, because we are about to use it for pruning.
        // Assert that we removed it - the removal of reverse iterators is a bit weird, so better
        // make sure.
        border_candidates.erase( --border_candidates.rbegin().base() );
        assert( cur_front != *border_candidates.rbegin() );

        // The current front taxon was considered a border taxon before.
        assert(
            cur_front.second->data<EntropyTaxonData>().status ==
            EntropyTaxonData::PruneStatus::kBorder
        );

        // Also, it has to have children, otherwise we would not have wanted it in the candidate
        // list in the first place.
        assert( cur_front.second->size() > 0 );

        // If we go into the front taxon, but achieve a new size that is further away from
        // our target size, we don't do go deeper.
        if( utils::abs_diff( border_taxa_count, target_taxonomy_size ) <
            utils::abs_diff( border_taxa_count + cur_front.second->size(), target_taxonomy_size )
        ) {
            // If we allow approximation, we will continue with the loop, which means, we will use
            // taxa with a lower entropy as pruning border. If we don't allow this, we are done here.
            if( settings.allow_approximation ) {
                continue;
            } else {
                break;
            }
        }

        // Prune at the front by making it an inside taxon, and its children the new border.
        expand_taxon( *cur_front.second );
    }

    // Output the entropy of the next split candidate. This is an indicator where the
    // entropy threshold for splitting is.
    // if( border_candidates.size() > 0 ) {
    //     auto cur_front = *border_candidates.rbegin();
    //     LOG_DBG << "entropy end: " << cur_front.first;
    // }

    // Output the rest of the candidates, for debugging.
    // LOG_DBG << "end of function. num candidates " << border_candidates.size();
    // LOG_DBG1 << "prune border branches count " << count_taxa_with_prune_status(
    //     taxonomy, EntropyTaxonData::PruneStatus::kBorder
    // );
    // LOG_DBG1 << "valid " << ( validate_pruned_taxonomy( taxonomy ) ? "1" : "nooooooooooooooo" );
    // for( auto const& elem : border_candidates ) {
    //     LOG_DBG1 << "H\t" << elem.first << "\tname\t" << TaxopathGenerator()( *elem.second ) << "\tlevel\t"
    //              << taxon_level( *elem.second ) << "\tchildren\t" << taxa_count_lowest_levels( *elem.second  );
    // }
    // LOG_DBG << "out";
}

// =================================================================================================
//     Helper Functions
// =================================================================================================

void expand_small_subtaxonomies(
    Taxonomy& taxonomy,
    size_t    min_subtaxonomy_size
) {
    for( auto& taxon : taxonomy ) {
        auto status = taxon.data<EntropyTaxonData>().status;

        // Recurse
        if( status == EntropyTaxonData::PruneStatus::kInside ) {
            expand_small_subtaxonomies( taxon, min_subtaxonomy_size );
        }

        // If the taxon has fewer leaves than the threshold (but is not a leaf itself),
        // we make the whole sub-taxonomy a part of the pruned taxonomy.
        if(
            status == EntropyTaxonData::PruneStatus::kBorder &&
            taxon.size() > 0 &&
            taxa_count_lowest_levels( taxon ) < min_subtaxonomy_size
        ) {

            // First, the taxon itself is inside.
            taxon.data<EntropyTaxonData>().status = EntropyTaxonData::PruneStatus::kInside;

            // Then, iterate it, and set all taxa in it to inner or
            // border, depending on whether they are inner of leaf taxa.
            for( auto it : preorder( taxon ) ) {
                auto& sub_taxon_data = it.taxon().data<EntropyTaxonData>();
                if( it.taxon().size() == 0 ) {
                    sub_taxon_data.status = EntropyTaxonData::PruneStatus::kBorder;
                } else {
                    sub_taxon_data.status = EntropyTaxonData::PruneStatus::kInside;
                }
            }
        }
    }
}

size_t count_taxa_with_prune_status(
    Taxonomy const&               taxonomy,
    EntropyTaxonData::PruneStatus status
) {
    size_t count = 0;
    auto do_count = [&] ( Taxon const& taxon ) {
        if( taxon.data<EntropyTaxonData>().status == status ) {
            ++count;
        }
    };
    preorder_for_each( taxonomy, do_count );
    return count;
}

void remove_pruned_taxonomy_children( Taxonomy& taxonomy )
{
    auto do_removal = [&] ( Taxon& taxon ) {
        if( taxon.data<EntropyTaxonData>().status == EntropyTaxonData::PruneStatus::kOutside ) {
            taxon.clear_children();
        }
    };
    preorder_for_each( taxonomy, do_removal );
}
std::string print_pruned_taxonomy( Taxonomy const& taxonomy )
{
    std::string result;
    auto print_taxon = [&] ( Taxon const& taxon ) {
        result += std::string( taxon_level(taxon) * 4, ' ' );
        if( taxon.data<EntropyTaxonData>().status == EntropyTaxonData::PruneStatus::kBorder ) {
            result += utils::Style("Red")(taxon.name());
        } else {
            result += taxon.name();
        }
        if( taxon.data<EntropyTaxonData>().entropy > 0 ) {
            result += " (" + std::to_string( taxon.data<EntropyTaxonData>().entropy ) + ")";
        }
        result += "\n";
    };
    preorder_for_each( taxonomy, print_taxon );
    return result;
}

bool validate_pruned_taxonomy( Taxonomy const& taxonomy )
{
    // Currently, because of the iterators, we need to always traverse the whole taxonomy.
    // Works for now, but should be speed up in the future with proper iterators.
    bool correct = true;

    auto check_parents = [&] ( Taxon const& taxon ) {

        // Need to have correct data type. We check it here. In the while loop later, we don't
        // have to: we are doing preorder traversal, so for each call of check_parents(), the
        // parents were already checked.
        if( taxon.data_cast<EntropyTaxonData>() == nullptr ) {
            auto name = TaxopathGenerator()( taxon );
            LOG_INFO << "Taxon with incorrect data type (not EntropyTaxonData): " << name;

            correct = false;
            return;
        }

        // Store the status of the current child. We'll move up the taxonomic hierarchy and check
        // whether all parents of this child are conform with the prune status rules.
        auto child_status = taxon.data<EntropyTaxonData>().status;

        // Check leaf state.
        if( taxon.size() == 0 && child_status == EntropyTaxonData::PruneStatus::kInside ) {
            auto name = TaxopathGenerator()( taxon );
            LOG_INFO << "Taxon is a leaf but has status 'kInside': " << name;
            correct = false;
        }

        auto cur_ptr = taxon.parent();
        while( cur_ptr != nullptr ) {
            auto cur_status = cur_ptr->data<EntropyTaxonData>().status;

            if(
                cur_status   == EntropyTaxonData::PruneStatus::kOutside &&
                child_status == EntropyTaxonData::PruneStatus::kOutside
            ) {
                // Do nothing, all good.
            } else if(
                cur_status   == EntropyTaxonData::PruneStatus::kInside &&
                child_status == EntropyTaxonData::PruneStatus::kInside
            ) {
                // Do nothing, all good.
            } else if(
                cur_status   == EntropyTaxonData::PruneStatus::kBorder &&
                child_status == EntropyTaxonData::PruneStatus::kOutside
            ) {
                child_status = cur_status;
            } else if(
                cur_status   == EntropyTaxonData::PruneStatus::kInside &&
                child_status == EntropyTaxonData::PruneStatus::kBorder
            ) {
                child_status = cur_status;
            } else {
                auto name = TaxopathGenerator()( taxon );
                LOG_INFO << "Taxon and child with wrong pruning status ("
                         << EntropyTaxonData::status_abbreviation( cur_status )  << "/"
                         << EntropyTaxonData::status_abbreviation( child_status ) << "): "
                         << name;

                correct = false;
                return;
            }

            cur_ptr = cur_ptr->parent();
        }
    };
    preorder_for_each( taxonomy, check_parents );

    return correct;
}


/*

    Old functions for testing. Did not use the EntropyTaxonData, but lists with pointers into
    the Taxonomy. Thus, ugly and and error prone. If needed, refactor first!

/ **
 * @brief Split a Taxonomy at Taxa that exceed a certain entropy threshold.
 *
 * This is mainly a test method, as it is currently not further used.
 * /
std::unordered_set< Taxon const* > split_taxonomy_by_entropy_threshold(
    Taxonomy const&                                   taxonomy,
    std::unordered_map< Taxon const*, double > const& entropies,
    double                                            entropy_threshold
) {
    // Resulting list of Taxa where to split.
    std::unordered_set< Taxon const* > crop_list;

    // Fill a stack of taxa with the first level of the Taxonomy.
    // We will do a preorder traversal, but do not go deeper into branches that we do not want
    // to split further. Thus, use this stack to keep track of which Taxa still need to be visited.
    std::stack< Taxon const* > taxa_stack;
    for( auto const& t : taxonomy ) {
        taxa_stack.push( &t );
    }

    // Iterate the taxonomy and either decide to not split but keep a certain Taxon
    // (if its entropy is below threshold), or go deeper into the Taxon by adding it to the stack,
    // so that it is also iterated and split at a deeper level.
    while( ! taxa_stack.empty() ) {
        auto const& cur = *taxa_stack.top();
        taxa_stack.pop();

        // Make sure we only process each element once. Not all taxa end of in the split list,
        // but none should be in there more than once.
        assert( crop_list.count( &cur ) == 0 );

        // Make sure that the entropy has entries that belong to the taxonomy.
        if( entropies.count( &cur ) == 0 ) {
            auto name = TaxopathGenerator()( cur );
            throw std::runtime_error( "Entropy list not complete. Missing Taxon " + name );
        }

        // If the Taxon has a low entropy, it's sequences are similar to each other, so we can
        // keep it as it is. Thus, no need to split it further, so add it to the list.
        // Also, if it is a leaf of the taxonomy, we will not further traverse it, so add it.
        if( entropies.at( &cur ) <= entropy_threshold || cur.size() == 0 ) {
            crop_list.insert( &cur );

        // If the entropy is high, go deepter into it.
        } else {
            for( auto& t : cur ) {
                taxa_stack.push( &t );
            }
        }
    }

    return crop_list;
}

/ **
 * @brief Test method for splitting a Taxonomy using nested intervals.
 *
 * This method is a test whether a Taxonomy can be splitted into low entropy regions using
 * nested intervals. Did not work, as the entropy per Taxon is not monotonic in the hierarchy.
 * /
std::unordered_set< Taxon const* > split_taxonomy_by_entropy_nested_invervals(
    Taxonomy const&                                   taxonomy,
    std::unordered_map< Taxon const*, double > const& entropies,
    size_t                                            target_taxonomy_size
) {
    // Init the entropy limits used for the nested intervals loop.
    // We will approach the best threshold from the min and max value, startig at the average.
    double lower_limit = std::numeric_limits<double>::infinity();
    double upper_limit = 0.0;
    double average     = 0.0;
    for( auto const& elem : entropies ) {
        if( elem.second < 0.0 ) {
            throw std::runtime_error( "Invalid entropy value < 0.0." );
        }

        lower_limit  = std::min( lower_limit, elem.second );
        upper_limit  = std::max( upper_limit, elem.second );
        average     += elem.second;
    }
    average /= static_cast< double >( entropies.size() );

    // Check invariants for the limits.
    assert( lower_limit <= average     );
    assert( average     <= upper_limit );

    // Start the iterative process with the average threshold.
    double threshold = average;

    // Target list: store the leaf taxa of the taxonomy.
    std::unordered_set< Taxon const* > crop_list;

    while( true ) {
        // Split the taxonomy using the current threshold.
        auto cand_list = split_taxonomy_by_entropy_threshold( taxonomy, entropies, threshold );

        // If we are closer to our target size, update the list.
        if( utils::abs_diff( cand_list.size(), target_taxonomy_size ) <
            utils::abs_diff( crop_list.size(), target_taxonomy_size )
        ) {
            crop_list = cand_list;
        }

        // Adjust the nested invervals, or finish.
        // If the list is too big, use a higher threshold,
        // if it is too small, use a lower one.
        // If we hit the target size, we can stop.
        if( cand_list.size() > target_taxonomy_size ) {
            lower_limit = threshold;
            threshold   = ( threshold + upper_limit ) / 2.0;

        } else
        if( cand_list.size() < target_taxonomy_size ) {
            upper_limit = threshold;
            threshold   = ( threshold + lower_limit ) / 2.0;

        } else {
            break;
        }

        // Check invariants for the limits.
        assert( lower_limit <= threshold   );
        assert( threshold   <= upper_limit );

        // Last resort: exit condition based on nesting depth.
        // Only in rare cases, we will exaclty hit the target size. Usually, we will jump back
        // and forth between a value too low and one too high. Then, at some point, the interval
        // converges at the entropy value that separates those two split candidates.
        // If we converged enough, we can stop, there won't be a better split candidate.
        if( upper_limit - lower_limit < 1.0E-10 * average ) {
            break;
        }

        // Test: Instead of nested intervals, simply try every n-th interval.
        // threshold += (upper_limit - lower_limit) / 200.0;
        // if( threshold > upper_limit ) {
        //     break;
        // }
    }

    return crop_list;
}
*/

} // namespace taxonomy
} // namespace genesis

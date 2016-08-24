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

#include "taxonomy/functions/entropy.hpp"

#include "taxonomy/formats/taxopath_generator.hpp"
#include "taxonomy/functions/taxonomy.hpp"
#include "taxonomy/taxon.hpp"
#include "taxonomy/taxonomy.hpp"

#include "utils/core/logging.hpp"
#include "utils/core/std.hpp"
#include "utils/text/style.hpp"

#include <assert.h>
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

/**
 * @brief Prune a Taxonomy so that the result (approximately) contains a desired number of "leaf"
 * @link Taxon Taxa@endlink, using the entropy of the Taxa as indicator where to prune.
 *
 * The function takes a Taxonomy with data type EntropyTaxonData and a target size which indicates
 * the desired number of "leaf" Taxa after pruning the Taxonomy. In the pruned Taxonomy, some Taxa
 * are considered as belonging to the Taxonomy (have status EntropyTaxonData::PruneStatus::kInside
 * or EntropyTaxonData::PruneStatus::kBorder), while others (with higher ranks) are excluded (have
 * status EntropyTaxonData::PruneStatus::kOutside). The number of border taxa (or "leaves")
 * of the included Taxa then is aimed to be as close as possible to the target size.
 *
 * That means, this function sets the @link EntropyTaxonData::status status@endlink of the
 * @link Taxon Taxa@endlink, but does not remove them. All Taxa with status
 * EntropyTaxonData::PruneStatus::kOutside are then considered to be pruned from the taxonomy.
 *
 * Example: The Taxonomy
 *
 *     Tax_1
 *         Tax_2
 *             Tax_3
 *             Tax_4
 *         Tax_5
 *             Tax_6
 *     Tax_7
 *         Tax_8
 *         Tax_9
 *
 * contains 5 "leaf" taxa, i.e., `Tax_3`, `Tax_4`, `Tax_6`, `Tax_8` and `Tax_9`. If we want to
 * prune it with a target size of 3, we might end up with either
 *
 *     Tax_1
 *         Tax_2
 *         Tax_5
 *     Tax_7
 *
 * or
 *
 *     Tax_1
 *     Tax_7
 *         Tax_8
 *         Tax_9
 *
 * as both contain 3 "leaves": `Tax_2`, `Tax_5` and `Tax_7` in the former case and `Tax_1`, `Tax_8`
 * and `Tax_9` in the latter. Which of those two is used depends on the entropies of the Taxa.
 *
 * In the former case, `Tax_1` is considered inside, `Tax_2`, `Tax_5` and `Tax_7` are border, and
 * all other taxa are outside of the pruned Taxonomy. In the latter case, `Tax_7` is inside,
 * `Tax_1`, `Tax_8` and `Tax_9` are border, and again all others are outside.
 *
 * It is not always possible to prune a Taxonomy in a way the we exaclty hit the target size. The
 * function then ends at a number of border Taxa that is closest (either below or above the target
 * size).
 *
 * In order to decide which Taxa to set to inside (i.e., not include as leaves, but further resolve
 * into their children), we use the entropies of the Taxa: We choose to split up at a current border
 * Taxon with the highest entropy value, as long as this brings us closer to the target size.
 *
 * If the optional parameter `allow_approximation` (default is `false`) is set to `true`,
 * we also allow to split up a border Taxon that has not the currently highest entropy of all
 * border Taxa, as long as this brings us closer to the target size.
 *
 * This means that the above case where we had two possible ways of splitting should be rare, as
 * the entropies will rarely be identical with real world data sets. If this happens nonetheless,
 * it is random with of the Taxa with equal entropy will be used.
 */
void prune_by_entropy_with_target_size(
    Taxonomy& taxonomy,
    size_t    target_taxonomy_size,
    bool      allow_approximation
) {
    // Basic check.
    if( taxa_count_lowest_levels( taxonomy ) < target_taxonomy_size ) {
        throw std::runtime_error(
            "Taxonomy only has " + std::to_string( taxa_count_lowest_levels( taxonomy ) ) +
            " leaf Taxa. Cannot prune it with " + std::to_string( target_taxonomy_size ) + " leaves."
        );
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

    // Helper function to fill the lists of taxa with the children of a Taxonomy.
    std::function< void ( Taxonomy& ) > fill_lists_with_children = [&] (
        Taxonomy& parent
    ) {
        for( auto& child : parent ) {
            if( child.size() == 1 ) {
                // If a Taxon has only one child, there is no need in adding this Taxon.
                // We can instead directly add its child. This will not increase the resulting
                // number of border taxa, as we still add only one. This mainly avoids to stop too
                // early, which would result in branches of the taxonomy that only contain one
                // child anyway, so it would make little sense to prune there.

                child.data<EntropyTaxonData>().status = EntropyTaxonData::PruneStatus::kInside;
                fill_lists_with_children( child );

            } else {
                // If we have either zero children, or more than one, this is a potential leaf
                // of the pruned taxonomy, so add it to the list. Also, if it has children, add those
                // as border candidates, in case we want to go deeper there.

                child.data<EntropyTaxonData>().status = EntropyTaxonData::PruneStatus::kBorder;
                ++border_taxa_count;

                if( child.size() > 0 ) {
                    border_candidates.emplace( child.data<EntropyTaxonData>().entropy, &child );
                }
            }
        }
    };

    // Init with first level.
    fill_lists_with_children( taxonomy );

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

        // If we go into the front taxon, but achieve a new size that is further away from
        // our target size, we don't do go deeper.
        if( utils::abs_diff( border_taxa_count, target_taxonomy_size ) <
            utils::abs_diff( border_taxa_count + cur_front.second->size(), target_taxonomy_size )
        ) {
            // If we allow approximation, we will continue with the loop, which means, we will use
            // taxa with a lower entropy as pruning border. If we don't allow this, we are done here.
            if( allow_approximation ) {
                continue;
            } else {
                break;
            }
        }

        // Prune at the front by making it an inside taxon, and its children the new border.
        cur_front.second->data<EntropyTaxonData>().status = EntropyTaxonData::PruneStatus::kInside;
        --border_taxa_count;
        fill_lists_with_children( *cur_front.second );
    }

    // Output the entropy of the next split candidate. This is an indicator where the
    // entropy threshold for splitting is.
    // if( border_candidates.size() > 0 ) {
    //     auto cur_front = *border_candidates.rbegin();
    //     LOG_DBG << "entropy end: " << cur_front.first;
    // }
}

/**
 * @brief Return the number of @link Taxon Taxa@endlink that have a certain
 * @link EntropyTaxonData::PruneStatus prune status@endlink.
 */
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

/**
 * @brief Remove the children of all @link Taxon Taxa@endlink that are pruned, i.e, that have
 * @link EntropyTaxonData::PruneStatus prune status@endlink `==`
 * @link EntropyTaxonData::PruneStatus::kOutside kOutside@endlink.
 *
 * The function does not validate the status before. Use validate_pruned_taxonomy() if you are
 * unsure whether the status is correct for all Taxa.
 */
void remove_pruned_taxonomy_children( Taxonomy& taxonomy )
{
    auto do_removal = [&] ( Taxon& taxon ) {
        if( taxon.data<EntropyTaxonData>().status == EntropyTaxonData::PruneStatus::kOutside ) {
            taxon.clear_children();
        }
    };
    preorder_for_each( taxonomy, do_removal );
}

/**
 * @brief Print a Taxonomy, highlighting those @link Taxon Taxa@endlink that are the pruning border,
 * i.e., where we cut off the sub-taxa, and print their entropies next to them.
 */
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

/**
 * @brief Validate that the pruning status of a Taxonomy is valid.
 *
 * This function expects the @link Taxon Taxa@endlink of the Taxonomy to have data type
 * EntropyTaxonData. It then checks whether the pruning states are all correctly set.
 *
 * That means:
 *
 *   * Taxa with status @link EntropyTaxonData::PruneStatus::kInside kInside@endlink can only have
 *     children of the same status or of @link EntropyTaxonData::PruneStatus::kBorder kBorder@endlink.
 *   * Taxa with status @link EntropyTaxonData::PruneStatus::kBorder kBorder@endlink can only have
 *     children of status @link EntropyTaxonData::PruneStatus::kOutside kOutside@endlink.
 *   * Taxa with status @link EntropyTaxonData::PruneStatus::kOutside kOutside@ can only have
 *     children of the same status.
 *
 * If any of those conditions is not met, an information about the faulty Taxon is written to
 * LOG_INFO, and the function returns `false`.
 */
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

        // Store the status of the current child. We'll move up the taoxnomic hierarchy and check
        // whether all parents of this child are conform with the prune status rules.
        auto child_status = taxon.data<EntropyTaxonData>().status;

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
                LOG_INFO << "Taxon with wrong pruning status: " << name;

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

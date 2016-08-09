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

#include "taxonomy/functions/split.hpp"

#include "taxonomy/taxon.hpp"
#include "taxonomy/taxonomy.hpp"
#include "taxonomy/functions/taxonomy.hpp"

#include "utils/core/std.hpp"
#include "taxonomy/formats/taxscriptor_generator.hpp"
#include "utils/core/logging.hpp"
#include "utils/text/style.hpp"

#include <algorithm>
#include <assert.h>
#include <functional>
#include <limits>
#include <map>
#include <numeric>
#include <stack>
#include <stdexcept>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Split
// =================================================================================================

/**
 * @brief Split a Taxonomy at Taxa that exceed a certain entropy threshold.
 *
 * This is mainly a test method, as it is currently not further used.
 */
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
            auto name = TaxscriptorGenerator()( cur );
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

/**
 * @brief Test method for splitting a Taxonomy using nested intervals.
 *
 * This method is a test whether a Taxonomy can be splitted into low entropy regions using
 * nested intervals. Did not work, as the entropy per Taxon is not monotonic in the hierarchy.
 */
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

/**
 * @brief Split a Taxonomy so that the result (approximately) contains a desired number of "leaf"
 * @link Taxon Taxa@endlink, using the entropy of the Taxa as indicator where to split.
 *
 * The function takes a Taxonomy and a list of entropy values for each Taxon of the Taxonomy.
 * This list can e.g. be obtained from the Sequence%s that belong to each Taxon, using
 * sequence::absolute_entropy() or sequence::averaged_entropy().
 *
 * The function further takes a target size which indicates the desired number of "leaf" Taxa after
 * splitting the Taxonomy. In the splitted Taxonomy, some Taxa are considered as belonging to the
 * Taxonomy, while other (with higher ranks) are excluded. The number of "endpoints" or "leaves"
 * of the included Taxa then is aimed to be as close as possible to the target size.
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
 * contains 5 "leaf" taxa, i.e., `Tax_3`, `Tax_4`, `Tax_6`, `Tax_8` and `Tax_9`. If we want to split
 * it with a target size of 3, we might end up with either
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
 * and `Tax_9` in the latter.
 *
 * It is not always possible to split a Taxonomy in a way the we exaclty hit the target size. The
 * function then chooses the split that is closest (either below or above the target size).
 *
 * In order to decide which Taxa to split (i.e., not include as leaves, but further resolve into
 * their children), we use the given entropy list: We choose to split at the (not yet splitted)
 * Taxon with the highest entropy value, as long as this split brings us closer to the target size.
 *
 * This means that the above case where we had two possible ways of splitting should be rare, as
 * the entropies will rarely be identical with real world data sets. If this happens nonetheless,
 * it is random with of the Taxa with equal entropy will be used.
 *
 * The resultung list then contains all leaf/endpoint Taxa, that is, those, which we do not want
 * to further split. The size of that list is then as close as possible to the target size, given
 * the method of splitting.
 */
std::unordered_set< Taxon const* > split_taxonomy_by_entropy_with_target_size(
    Taxonomy const&                                   taxonomy,
    std::unordered_map< Taxon const*, double > const& entropies,
    size_t                                            target_taxonomy_size
) {
    // Basic check.
    if( taxa_count_lowest_levels( taxonomy ) < target_taxonomy_size ) {
        throw std::runtime_error(
            "Taxonomy only has " + std::to_string( taxa_count_lowest_levels( taxonomy ) ) +
            " leaf Taxa. Cannot split it into " + std::to_string( target_taxonomy_size ) + " parts."
        );
    }

    // Resulting list of Taxa where to split. The list contains all those taxa where we want to
    // stop going deeper and take this taxon as leaf instead.
    std::unordered_set< Taxon const* > crop_list;

    // Candidate list of Taxa to split.
    // We will do a preorder traversal of the Taxonomy, but do not go deeper into branches that we
    // do not want to split further. We use this list to decide which taxa to further split:
    // Always go deeper (i.e., split) into the taxon with the highest entropy.
    std::multimap< double, Taxon const* > split_candidates;

    // Helper function to fill the lists of taxa with the children of a Taxonomy.
    std::function< void ( Taxonomy const& ) > fill_lists_with_children = [&] (
        Taxonomy const& parent
    ) {
        for( auto const& child : parent ) {

            // Make sure that the entropy has entries that belong to the taxonomy.
            if( entropies.count( &child ) == 0 ) {
                auto name = TaxscriptorGenerator()( child );
                throw std::runtime_error( "Entropy list not complete. Missing Taxon " + name );
            }

            // If a Taxon has only one child, there is no need in adding this Taxon. We can instead
            // directly add its child. This will not increase the resulting list, as we still add
            // only one. This mainly avoids to stop too early, which would result in branches of the
            // taxonomy that only contain one child anyway, so it would make little sense to
            // split there.
            if( child.size() == 1 ) {
                fill_lists_with_children( child );

            // If we have either zero children, or more than one, this is a potential leaf
            // of the taxonomy, so add it to the list. Also, if it has children, add those as
            // split candidates, in case we want to do more splitting.
            } else {
                crop_list.insert( &child );
                if( child.size() > 0 ) {
                    split_candidates.emplace( entropies.at( &child ), &child );
                }
            }
        }
    };

    // Init with first level.
    fill_lists_with_children( taxonomy );

    // Loop until we have done enough splitting, i.e., if we exceeded the target size.
    while( crop_list.size() < target_taxonomy_size ) {

        // We already checked that we will have enough leaf taxa to achieve the target size.
        // So, there should always be candidates to choose for splitting.
        assert( split_candidates.size() > 0 );

        // Get the taxon with the highest entropy from the candidates list.
        // Assert that the iterators do their thing correctly and the last element is the last.
        // Remove it from there, because we are about to use it for splitting.
        // Assert that we removed it - the removal of reverse iterators is a bit weird, so better
        // make sure.
        auto cur_split = *split_candidates.rbegin();
        assert( *std::prev(split_candidates.end()) == *split_candidates.rbegin() );
        split_candidates.erase( --split_candidates.rbegin().base() );
        assert( cur_split != *split_candidates.rbegin() );

        // The taxon where we want to split was considered a leaf taxon before (it was added to
        // crop_list at some point in fill_lists_with_children). So it should be in that list.
        assert( crop_list.count( cur_split.second ) > 0 );

        // If we split at the candidate taxon, but achieve a new size that is further away from
        // our target size, we don't do the split but stop here.
        if( utils::abs_diff( crop_list.size(), target_taxonomy_size ) <
            utils::abs_diff( crop_list.size() + cur_split.second->size(), target_taxonomy_size )
        ) {
            break;
        }

        // Do the split by removing it from the crop list but adding its children to the list
        // instead. Also, add those children as further candidates for the next rounds.
        crop_list.erase( cur_split.second );
        fill_lists_with_children( *cur_split.second );
    }

    // Helper code that also adds the inner taxa to the list. Might be useful if we want this
    // as output at some point, instead of just the leaf taxa.
    // auto crop_copy = crop_list;
    // for( auto const& elem : crop_copy ) {
    //     auto const* parent = elem->parent();
    //     while( parent != nullptr ) {
    //         crop_list.insert( parent );
    //         parent = parent->parent();
    //     }
    // }

    // Output the entropy of the next split candidate. This is an indicator where the
    // entropy threshold for splitting is.
    // if( split_candidates.size() > 0 ) {
    //     auto cur_split = *split_candidates.rbegin();
    //     LOG_DBG << "entropy end: " << cur_split.first;
    // }

    return crop_list;
}

/**
 * @brief Add the parents of the split candidates and return the result.
 *
 * The entropy split functions return a list of @link Taxon Taxa@endlink that only contains those
 * at the boundary of the splitted Taxonomy, i.e., the "leaf" Taxa. This function takes such a list
 * and also adds the "inner" Taxa to it. This is for example useful for printing the Taxonomy.
 */
std::unordered_set< Taxon const* > fill_splitted_entropy_parents(
    std::unordered_set< Taxon const* >                split_list
) {
    auto full_split_list = split_list;
    for( auto const& elem : split_list ) {
        auto const* parent = elem->parent();
        while( parent != nullptr ) {
            full_split_list.insert( parent );
            parent = parent->parent();
        }
    }
    return full_split_list;
}

size_t count_splitted_taxonomy_total_size(
    Taxonomy const&                                   taxonomy,
    std::unordered_set< Taxon const* >                full_split_list
) {
    size_t count = 0;
    auto do_count = [&] ( Taxon const& t ) {
        if( full_split_list.count( &t ) > 0 ) {
            ++count;
        }
    };
    preorder_for_each( taxonomy, do_count );
    return count;
}

/**
 * @brief Remove the children of all Taxa that are in the split list.
 */
void remove_splitted_taxonomy_children(
    Taxonomy&                                         taxonomy,
    std::unordered_set< Taxon const* >                split_list
) {
    // First check that only the leaves are marked in the split list.
    for( auto const& elem : split_list ) {
        auto const* parent = elem->parent();
        while( parent != nullptr ) {
            if( split_list.count( parent ) > 0 ) {
                auto name = TaxscriptorGenerator()( *elem );
                throw std::runtime_error(
                    "Removing splitted Taxa from Taxonomy where inner Taxa are also in the list "
                    "is not possible. This occured at Taxon " + name
                );
            }
            parent = parent->parent();
        }
    }

    // If all is good (not thrown), remove the children.
    auto do_removal = [&] ( Taxon& t ) {
        if( split_list.count( &t ) > 0 ) {
            t.clear_children();
        }
    };
    preorder_for_each( taxonomy, do_removal );
}

/**
 * @brief Print a Taxonomy, highlighting those @link Taxon Taxa@endlink that are used for splitting,
 * i.e., where we cut off the sub-taxa.
 */
std::string print_splitted_taxonomy(
    Taxonomy const&                                   taxonomy,
    std::unordered_set< Taxon const* > const&         crop_list
) {
    return print_splitted_taxonomy(
        taxonomy,
        crop_list,
        std::unordered_map< Taxon const*, double >()
    );
}

/**
 * @brief Print a Taxonomy, highlighting those @link Taxon Taxa@endlink that are used for splitting,
 * i.e., where we cut off the sub-taxa, and print their entropies next to them.
 */
std::string print_splitted_taxonomy(
    Taxonomy const&                                   taxonomy,
    std::unordered_set< Taxon const* > const&         crop_list,
    std::unordered_map< Taxon const*, double > const& entropies
) {
    std::string result;
    auto print_taxon = [&] ( Taxon const& t ) {
        result += std::string( taxon_level(t) * 4, ' ' );
        if( crop_list.count( &t ) > 0 ) {
            result += utils::Style("Red")(t.name());
        } else {
            result += t.name();
        }
        if( entropies.count( &t ) > 0 ) {
            result += " (" + std::to_string( entropies.at( &t )) + ")";
        }
        result += "\n";
    };
    preorder_for_each( taxonomy, print_taxon );
    return result;
}

/**
 * @brief Helper function for validating the internal validity of a splitted Taxonomy.
 *
 * See split_taxonomy_by_entropy_with_target_size() for the method behind this.
 */
bool validated_splitted_taxonomy(
    Taxonomy const&                            taxonomy,
    std::unordered_set< Taxon const* > const&  crop_list
) {
    size_t wrong_parents = 0;
    auto check_parents = [&] ( Taxon const& t ) {
        size_t split_count = 0;
        auto cur = &t;
        while( cur != nullptr ) {
            if( crop_list.count( cur ) > 0 ) {
                ++split_count;
            }
            cur = cur->parent();
        }
        if( split_count > 1 ) {
            auto name = TaxscriptorGenerator()( t );
            LOG_INFO << "Taxon has " << split_count << " splitted parents: " << name;
            ++wrong_parents;
        }
    };
    preorder_for_each( taxonomy, check_parents );

    return wrong_parents == 0;
}

} // namespace taxonomy
} // namespace genesis

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
#include <limits>
#include <numeric>
#include <stack>
#include <stdexcept>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Split
// =================================================================================================

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


    // LOG_DBG1 << "thresh  " << entropy_threshold;

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

        // auto name = TaxscriptorGenerator()( cur );
        // LOG_DBG1 << "at " << name;
        // LOG_DBG1 << "entropy " << entropies[ &cur ];

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

std::unordered_set< Taxon const* > split_taxonomy_by_entropy_with_target_size(
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

    // Some safety.
    assert( lower_limit <= average     );
    assert( average     <= upper_limit );

    // Start the iterative process with the average threshold.
    // double threshold = average;
    double threshold = lower_limit;

    std::unordered_set< Taxon const* > crop_list;

    while( true ) {
        LOG_DBG << "------------------------------------";
        LOG_DBG << "lower_limit " << lower_limit;
        LOG_DBG << "upper_limit " << upper_limit;
        LOG_DBG << "threshold   " << threshold;
        LOG_DBG << "run split...";

        auto cand_list = split_taxonomy_by_entropy_threshold( taxonomy, entropies, threshold );

        // If we are closer to our target size, update the list.
        if( utils::absolute_difference( cand_list.size(), target_taxonomy_size ) <
            utils::absolute_difference( crop_list.size(), target_taxonomy_size )
        ) {
            LOG_DBG << "  updating";
            crop_list = cand_list;
        }

        // // Adjust the nested invervals, or finish.
        // if( cand_list.size() > target_taxonomy_size ) {
        //     lower_limit = threshold;
        //     threshold   = ( threshold + upper_limit ) / 2.0;
        //
        // } else
        // if( cand_list.size() < target_taxonomy_size ) {
        //     upper_limit = threshold;
        //     threshold   = ( threshold + lower_limit ) / 2.0;
        //
        // } else {
        //     break;
        // }

        assert( lower_limit <= threshold   );
        assert( threshold   <= upper_limit );

        // Last resort: exit condition based on nesting depth.
        // Only in rare cases, we will exaclty hit the target size. Usually, we will jump back
        // and forth between a value too low and one too high. Then, at some point, the interval
        // converges at the entropy value that separates those two split candidates.
        // If we converged enough, we can stop, there won't be a better split candidate.
        // if( upper_limit - lower_limit < 1.0E-10 * average ) {
        //     break;
        // }

        // test
        threshold += (upper_limit - lower_limit) / 200.0;
        if( threshold > upper_limit ) {
            break;
        }

        LOG_DBG << "done.";
        LOG_DBG << "target      " << target_taxonomy_size;
        LOG_DBG << "cand        " << utils::Style("Blue")( std::to_string( cand_list.size() ));
        LOG_DBG << "splits      " << utils::Style("Red")( std::to_string( crop_list.size() ));
        LOG_DBG << "new:";
        LOG_DBG << "lower_limit " << lower_limit;
        LOG_DBG << "upper_limit " << upper_limit;
        LOG_DBG << "threshold   " << threshold;
    }

    LOG_DBG << "------------------------------------";
    LOG_DBG << "finished. final:";
    LOG_DBG << "splits      " << crop_list.size();
    LOG_DBG << "lower_limit " << lower_limit;
    LOG_DBG << "upper_limit " << upper_limit;
    LOG_DBG << "threshold   " << threshold;

    return crop_list;
}

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

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
 * @ingroup sequence
 */

#include "genesis/sequence/kmer/color_gamut_functions.hpp"
#include "genesis/taxonomy/functions/kmer_data.hpp"
#include "genesis/taxonomy/functions/kmer_grouping.hpp"
#include "genesis/taxonomy/functions/kmer.hpp"
#include "genesis/taxonomy/functions/taxonomy.hpp"
#include "genesis/taxonomy/iterator/preorder.hpp"
#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/utils/containers/matrix/operators.hpp"
#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/io/deserializer.hpp"
#include "genesis/utils/io/serializer.hpp"
#include "genesis/utils/bit/bitvector.hpp"
#include "genesis/utils/bit/bitvector/functions.hpp"
#include "genesis/utils/bit/bitvector/operators.hpp"
#include "genesis/utils/text/string.hpp"

// The KmerColorGamut class is only available from C++17 onwards.
#if GENESIS_CPP_STD >= GENESIS_CPP_STD_17

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <ios>
#include <limits>
#include <queue>
#include <set>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include <vector>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Color Gamut Initialization
// =================================================================================================

// -------------------------------------------------------------------------
//     add_secondary_colors_with_binary_reduction
// -------------------------------------------------------------------------

void add_secondary_colors_with_binary_reduction(
    KmerColorGamut& gamut
) {
    // In the absence of a phylogeny or taxonomy to group our primary colors by,
    // we use a binary reduction, essentially creating a complete binary tree
    // through recursive pairwise grouping of the elements. We currently only allow
    // to call it if no other secondary colors have been added to the set yet.
    if( gamut.get_color_list().size() != 1 + gamut.get_element_count() ) {
        throw std::invalid_argument(
            "Cannot initialize Kmer Color Gamut with binary reduction "
            "if other colors have already been added"
        );
    }

    // Define a recursive lambda that takes the first index and one-past-the-last index
    // for the current block of colors that we want to merge and create secondary colors from.
    size_t leftover_index = 0;
    std::function<void(size_t, size_t)> recursive_merge_ = [&]( size_t first, size_t last )
    {
        // Base case: no merge needed if no more elements are left.
        assert( first <= last );
        assert( last == gamut.get_color_list().size() );
        if( last - first == 0 ) {
            return;
        }

        // Record the starting index of the new block that will be appended,
        // and process elements pairwise in the current block [first, last).
        size_t new_start = gamut.get_color_list().size();
        for( size_t i = first; i < last; i += 2 ) {

            // Default case: two groups to merge into a new color.
            if( i + 1 < last ) {
                gamut.add_merged_color( i, i+1 );
                continue;
            }

            // Edge case: If there is an odd number of elements in the current group,
            // we note down the last element as a leftover, as it cannot be merged,
            // and we do not want to add duplicates. It will be merged later.
            // Alternatively, if there is already a leftover from a previous uneven group,
            // we merge this into our current last (uneven) element, to even the odds.
            // At any time, there can be at most one leftover element, and it will
            // always be merged once another uneven sized group is found.
            // This corresponds to building a binary tree where the last branch is always
            // attached at the earliest possibility.
            assert( i + 1 == last );
            if( leftover_index == 0 ) {
                leftover_index = i;
            } else {
                gamut.add_merged_color( i, leftover_index );
                leftover_index = 0;
            }
        }

        // Recursively merge the newly created block.
        recursive_merge_( new_start, gamut.get_color_list().size() );
    };

    // Start the recursive merging using the entire initial range of primary colors.
    // That is, we start after the empty color, and stop at the color index that corresponds
    // to  the last primary (single bit set) color. This is an ugly hard-coded dependency
    // on the inner workings of the class... but well, good enough for now.
    recursive_merge_( 1, gamut.get_color_list().size() );

    // At the end, there should be no leftover color; all should be in some secondary color.
    // Due to the exit condition of the recursive function, the leftover is set to the last
    // element that has all colors set (which is hence not really a leftover).
    assert( leftover_index == gamut.get_color_list().size() - 1 );

    // A binary tree has one fewer inner nodes than tips. We have added those as colors,
    // and have the empty color, so our total is now double the number of primary colors.
    // Also, the last secondary color should contain _all_ primary colors.
    assert( gamut.get_color_list().size() == 2 * gamut.get_element_count() );
    assert( all_set( gamut.get_color_list().back().elements ));
}

// -------------------------------------------------------------------------
//     add_secondary_colors_from_bitvectors
// -------------------------------------------------------------------------

void add_secondary_colors_from_bitvectors(
    KmerColorGamut& gamut,
    std::vector<utils::Bitvector> const& bitvecs,
    bool test_for_all_set_color
) {
    // We need at least one fully set bitvector, to guarantee that our imaginary colors
    // work as expected, i.e, such that they can always find at least that one as a match.
    bool found_all_set = false;

    // Add all bitvectors as secondary colors
    for( auto const& bv : bitvecs ) {
        // Validity checks
        if( bv.size() != gamut.get_element_count() ) {
            throw std::invalid_argument(
                "Cannot initialize Kmer Color Gamut with Bitvectors of size "
                "that does not match the element count"
            );
        }
        if( utils::pop_count( bv ) < 2 ) {
            throw std::invalid_argument(
                "Cannot initialize Kmer Color Gamut with Bitvectors representing "
                "the empty color or primary colors (i.e., zero or single bit set)"
            );
        }
        if( test_for_all_set_color && utils::all_set( bv )) {
            found_all_set = true;
        }

        // Add a new color for the bitvector, which also checks that it is not a duplicate.
        auto cp = bv;
        gamut.add_color( std::move( cp ));
    }

    // Final check for the all-set color, such that our minimal superset always succeeds.
    if( test_for_all_set_color && ! found_all_set ) {
        throw std::invalid_argument(
            "Cannot initialize Kmer Color Gamut with Bitvectors "
            "that do not contain an all-set bitvector"
        );
    }
}

// -------------------------------------------------------------------------
//     add_secondary_colors_from_groups
// -------------------------------------------------------------------------

void add_secondary_colors_from_groups(
    KmerColorGamut& gamut,
    std::vector<std::vector<size_t>> const& groups,
    bool test_for_all_set_color
) {
    // Quick and dirty: create bitvectors from the indices, then forward to the bitvector function.
    // We are currently not expecting more than a few hundred or thousand initial colors, so this
    // should be fine for initialization. If extended to more colors, might need optimization.
    std::vector<utils::Bitvector> bitvecs;
    for( auto const& group_indices : groups ) {
        bitvecs.emplace_back( utils::Bitvector( gamut.get_element_count(), group_indices ));
    }
    add_secondary_colors_from_bitvectors( gamut, bitvecs, test_for_all_set_color );
}

// -------------------------------------------------------------------------
//     make_secondary_colors_from_taxonomy_bottom_up
// -------------------------------------------------------------------------

std::vector<utils::Bitvector> make_secondary_colors_from_taxonomy_bottom_up(
    taxonomy::Taxonomy const& tax,
    size_t power_set_limit,
    bool add_primary_colors
) {
    using namespace utils;
    using namespace taxonomy;

    // Cautionary check of computational limits within this function. If this function ever
    // gets called with a power set limit of even close to 64, it would explode the memory
    // anyway... but we leave that to the user, and only check the technical limits here.
    if( power_set_limit == 0 || power_set_limit > 63 ) {
        throw std::invalid_argument( "Invalid power_set_limit" );
    }

    // Prepare the result and get the number of bits per bitvector, i.e., the number of groups.
    std::vector<utils::Bitvector> colors;
    auto const num_groups = count_taxon_groups_or_partitions( tax );
    if( num_groups == 0 ) {
        return colors;
    }

    // Recurse the taxonomy. At each stage, we collect all group indices, and build bitvectors.
    std::function<void(Taxonomy const&, std::vector<size_t>&)> recursion_ = [&](
        Taxonomy const& taxon,
        std::vector<size_t>& group_indices
    ) {
        // For later, we need to know if we used grouping (1) or partitioning (-1) on the taxnomoy.
        // We store this here as an int with the two meanings above. Ugly and hacky.
        int type = 0;

        // Collect all grouped children of the current taxon. For extended taxa, we collect all
        // group indices below them, and treat them as a single group for the power set.
        // The vector contains an entry for every child of the current taxon, and that entry
        // lists all group or partition indices found in that child.
        std::vector<std::vector<size_t>> child_indices;
        for( auto const& child : taxon ) {
            auto const& data = child.data<KmerTaxonData>();
            if( data.index != std::numeric_limits<size_t>::max() && data.index > num_groups ) {
                throw std::invalid_argument(
                    "Invalid KmerTaxonData::GroupStatus, invalid group index > num groups"
                );
            }
            switch( data.status ) {
                case KmerTaxonData::Status::kGroupAssigned:
                case KmerTaxonData::Status::kPartitionMonophyletic:
                {
                    type = data.status == KmerTaxonData::Status::kGroupAssigned ? 1 : -1;

                    // For taxa that have been assigned to a group, collect their indices.
                    // As multiple taxa can be assigned to the same group (if the combined
                    // sizes are still within the limits of TaxonGroupingLimits), we need
                    // to count unique group indices here. Could be made more efficient,
                    // but good enough for now.
                    bool found_index = false;
                    for( auto const& ci : child_indices ) {
                        assert( !ci.empty() );

                        // Check that we do not have the group index already. For the assigned
                        // groups, we only need to check the first entry, as there cannot be
                        // more than one entry per group anyway (as a consequence of only having
                        // shared group indices within the same taxon - i.e., siblings).
                        if( ci.size() == 1 && ci[0] == data.index ) {
                            found_index = true;
                        }
                    }
                    if( ! found_index ) {
                        child_indices.emplace_back( std::vector<size_t>( 1, data.index ));

                        // This is the first time that we are processing this particular group,
                        // and it is a singleton, i.e., a taxon without further children.
                        // We hence might want to add it as a primary color.
                        if( add_primary_colors ) {
                            auto group_elements = Bitvector( num_groups );
                            group_elements.set( data.index );
                            colors.push_back( std::move( group_elements ));
                        }
                    }
                    break;
                }
                case KmerTaxonData::Status::kGroupExpanded:
                {
                    assert( type == 0 || type == 1 );
                    type = 1;

                    // For taxa that have been expanded (because they are too big), we recurse,
                    // which processes all groups below them, and then collect these groups.
                    child_indices.emplace_back();
                    recursion_( child, child_indices.back() );
                    break;
                }
                case KmerTaxonData::Status::kPartitionParaphyletic:
                {
                    assert( type == 0 || type == -1 );
                    type = -1;

                    // For partitioning, we need to add the inner partition nodes as well,
                    // so that we include all of them along all taxa (nodes) of the taxonomy.
                    child_indices.emplace_back( std::vector<size_t>( 1, data.index ));
                    recursion_( child, child_indices.back() );
                    break;
                }
                case KmerTaxonData::Status::kUnprocessed:
                default: {
                    throw std::invalid_argument(
                        "Invalid KmerTaxonData::GroupStatus, Taxonomy not properly processed"
                    );
                }
            }
        }

        // Sort all entries and remove duplicates. Not really necessary, as the above already should
        // avoid duplicates at the level of the current taxon anyway, but doesn't hurt and makes
        // debugging output a bit nicer. We likely end up with final duplicate bitvectors anyway,
        // as the partitions at different taxa might have the same indices, and so in different
        // iterations of this recursive function, we end up adding the same colors... so we have
        // to filter them out later anyway when adding them to the color list.
        for( auto& child : child_indices ) {
            std::sort( child.begin(), child.end() );
        }
        std::sort( child_indices.begin(), child_indices.end() );
        child_indices.erase(
            std::unique( child_indices.begin(), child_indices.end() ), child_indices.end()
        );
        // std::sort(
        //     child_indices.begin(), child_indices.end(),
        //     []( std::vector<size_t> const& lhs, std::vector<size_t> const& rhs ){
        //         if( lhs.empty() || rhs.empty() ) {
        //             throw std::runtime_error( "Empty taxon child group" );
        //         }
        //         return lhs[0] < rhs[0];
        //     }
        // );

        // Now we have a list of all child group indices of the current taxon.
        // We can build their power set (or just flag all-set bitvector, if too big).
        if( child_indices.empty() ) {
            auto const* taxon_ptr = dynamic_cast<Taxon const*>( &taxon );
            if( taxon_ptr ) {
                throw std::runtime_error( "Empty group at taxon " + taxon_ptr->name() );
            } else {
                throw std::runtime_error( "Empty group at root" );
            }
        } else if( child_indices.size() <= power_set_limit ) {
            // For each child, we prepare a bitvector with elements set according to all indices
            // found in that child. That way, we do not have to compute them each time below.
            auto child_elements = std::vector<Bitvector>( child_indices.size() );
            for( size_t i = 0; i < child_indices.size(); ++i ) {
                assert( ! child_indices[i].empty() );
                child_elements[i] = Bitvector( num_groups );
                for( auto cigi : child_indices[i] ) {
                    assert( cigi < child_elements[i].size() );
                    child_elements[i].set( cigi );
                }
            }

            // We add the power set of all immediate child groups of this taxon as colors.
            // For this, we set up a counter (mask) that we go through, and add a color for each
            // value, activating the elements in the color corresponding to all group children.
            auto const max_mask = static_cast<size_t>( 1 ) << child_indices.size();
            for( size_t mask = 0; mask < max_mask; ++mask ) {
                auto elements = Bitvector( num_groups );

                // Check each bit of the mask, and if it is set, that means that in this iteration,
                // we set all elements in the bitvector corresponding to all indices of this group.
                size_t activated_groups = 0;
                for( size_t a = 0; a < child_indices.size(); ++a ) {
                    assert( ! child_indices[a].empty() );
                    if(( mask & (static_cast<size_t>( 1 ) << a )) == 0 ) {
                        continue;
                    }
                    ++activated_groups;

                    // We have a set bit in the mask, meaning that this group of inidices needs
                    // to be added for this color in the power set.
                    elements |= child_elements[a];
                }

                // Only add the color if it only contains more than one group. The bitvector
                // for that one group has already been produced in a previous iteration here,
                // with the all-one mask for this particular child group.
                if( activated_groups > 1 ) {
                    colors.push_back( std::move( elements ));
                }
            }
        } else {
            // Here we have a taxon which has more than the limit many immediate child groups.
            // We do not want to build a power set for all of them (that would give too many colors),
            // and instead just include it as a single all-set bitvector of the groups.
            auto elements = Bitvector( num_groups );
            for( auto const& ci : child_indices ) {
                assert( ! ci.empty() );
                for( auto cigi : ci ) {
                    assert( cigi < elements.size() );
                    // assert( elements.get( cigi ) == false );
                    elements.set( cigi );
                }
            }
            colors.push_back( std::move( elements ));
        }

        // Lastly, we also need to add all child indices that we found to our returned
        // group index list, so that the higher-up call of the recursion can use those.
        for( auto const& ci : child_indices ) {
            assert( ! ci.empty() );
            for( auto cigi : ci ) {
                assert( cigi < num_groups );
                group_indices.push_back( cigi );
            }
        }

        // We now sort the group indices, mainly just so that our downstream bitvectors are sorted.
        std::sort( group_indices.begin(), group_indices.end() );
        if( type == 1 ) {
            // For grouping, also check that all indices are unique, which they need to be,
            // as we excluded duplicate sibling indices. If we find duplicates here still,
            // that means that duplicate group indices are distributed across non-silbing taxa.
            if( contains_duplicates( group_indices )) {
                throw std::runtime_error( "Duplicate group indices that are not siblings" );
            }
        } else if( type == -1 ) {
            // For partitioning, we expect duplicate indices in the children.
            // Let's remove them for downstream though.
            group_indices.erase(
                std::unique( group_indices.begin(), group_indices.end() ), group_indices.end()
            );
        } else {
            // We checked for empty children above already. But still, let's check here again.
            throw std::runtime_error( "Empty taxon" );
        }
    };
    std::vector<size_t> group_indices;
    recursion_( tax, group_indices );
    assert( group_indices.size()  == num_groups );
    assert( group_indices.front() == 0 );
    assert( group_indices.back()  == num_groups - 1 );
    assert( std::is_sorted( group_indices.begin(), group_indices.end() ));
    assert( ! contains_duplicates( group_indices ));

    // Lastly, we need to check that the all-set color is part of the color set, in order for
    // the kmer color gamut to work properly. This is the case if there is one single highest
    // taxon such as "root" that contains all others. But if the taxonomy starts at, say, the
    // domain level, then we'd have three top level taxa, but none that contained all others.
    if( ! colors.empty() && ! all_set( colors.back() )) {
        colors.push_back( Bitvector( num_groups, true ));
    }

    return colors;
}

// -------------------------------------------------------------------------
//     make_secondary_colors_from_taxonomy_top_down
// -------------------------------------------------------------------------

// -------------------------------------------------------
//     collect_taxa_bitvectors_top_down_experiment_
// -------------------------------------------------------

std::vector<utils::Bitvector> collect_taxa_bitvectors_top_down_experiment_(
    taxonomy::Taxonomy const& tax,
    size_t power_set_taxa,
    bool use_num_sequences,
    size_t num_groups
) {
    using namespace utils;
    using namespace taxonomy;

    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    //     EXPERIMENTAL. UNFINISHED. BROKEN.
    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

    // We always want to split the largest taxon group, until we have reached our power set number
    // of items. For that, we keep a priority queue sorted by the size of the clade (either in sum
    // of seq length, or number of sequences in the taxon), pointing to the respective clade.
    assert( power_set_taxa > 0 );
    assert( num_groups >= power_set_taxa );

    // Define a struct to hold the size to sort by, and a list of pointers to the Taxa.
    // That list at first only contains the single taxon that we are interested in.
    // However, in the end of the taxonomy splitting, we might have a taxon that contains
    // many children, and whose splitting would overshoot our power limit. If that happens,
    // we instead graft all its small children together, as an ad-hoc solution to this issue.
    struct Clade {
        size_t clade_size;
        std::vector<Taxon const*> clade_taxa;
    };
    struct CompareClade {
        bool operator()( Clade const& lhs, Clade const& rhs ) const
        {
            // Ensure uniqueness or consistent ordering by using
            // the taxon pointer as an arbitrary but constant tie breaker.
            // Not really needed with a priority queue, but kept here for reference.
            // if( lhs.clade_size == rhs.clade_size ) {
            //     assert( !lhs.clade_taxa.empty() );
            //     assert( !rhs.clade_taxa.empty() );
            //     return lhs.clade_taxa.front() < rhs.clade_taxa.front();
            // }
            return lhs.clade_size < rhs.clade_size;
        }
    };

    // Instanciate the queue, and a helper to add items to it. We always add all children of a
    // given taxon. However, if that means that we overshoot our power limit, we collect all
    // the smallest ones and group them together, so that we end up with exaclty the right number.
    std::priority_queue<Clade, std::vector<Clade>, CompareClade> clades;
    auto add_children_to_queue_ = [&]( Taxonomy const& tax )
    {
        // LOG_DBG1 << "add_children_to_queue_ " << ( dynamic_cast<Taxon const*>( &tax )  ? dynamic_cast<Taxon const*>( &tax )->name() : " root" );
        // We first build a temporary vector of the taxa and their data.
        std::vector<Clade> temp;
        for( auto const& child : tax ) {
            // LOG_DBG2 << "loop " << dynamic_cast<Taxon const*>( &child )->name();
            auto const& data = child.data<KmerTaxonData>();
            auto const clade_size = (
                use_num_sequences ? data.clade_num_sequences : data.clade_sum_seq_lengths
            );
            // temp.push_back({ clade_size, { &child }});

            size_t group_pos = temp.size();
            for( size_t i = 0; i < temp.size(); ++i ) {
                if( temp[i].clade_taxa.front()->data<KmerTaxonData>().index == data.index ) {
                    assert( group_pos == temp.size() );
                    group_pos = i;
                }
            }
            if( group_pos < temp.size() ) {
                temp[group_pos].clade_size += clade_size;
                temp[group_pos].clade_taxa.push_back( &child );
            } else {
                temp.push_back({ clade_size, { &child }});
            }
        }

        // Sort backwards, largest one first
        std::sort(
            temp.begin(), temp.end(),
            []( Clade const& lhs, Clade const& rhs ){
                return lhs.clade_size > rhs.clade_size;
                // return ! CompareClade()( lhs, rhs );
            }
        );

        // Now, if adding all elements of the vector would overshot the power limit,
        // we need to remove stuff first again. We do this by merging the small elements
        // at the back of the vector. This could be a bit more optimized by doing this
        // not just backwards, as at some point, the accumulated element might not be
        // the smallest any more. But well, good enough for now. This all can only happen
        // if there are two or more elements to being with, as otherwise, we'd not be here.
        while( clades.size() + temp.size() > power_set_taxa - 1 ) {
            // LOG_DBG2 << "merge " << clades.size() << " + " << temp.size() << " > " << power_set_taxa;
            assert( temp.size() >= 2 );
            auto const last = temp.back();
            temp.pop_back();
            temp.back().clade_size += last.clade_size;
            temp.back().clade_taxa.push_back( last.clade_taxa.front() );
        }

        // Now we can add all elements to the queue.
        assert( clades.size() + temp.size() <= power_set_taxa );
        for( auto& elem : temp ) {
            assert( !elem.clade_taxa.empty() );
            if(
                // elem.clade_taxa.size() > 1 ||
                count_taxon_groups_or_partitions( *elem.clade_taxa.front(), true ) > 1
            ) {
                // LOG_DBG2 << "push!";
                clades.push( std::move( elem ));
            } else {
                // LOG_DBG2 << "push not...";
            }
        }
        assert( clades.size() <= power_set_taxa );
    };

    // Initialize the priority queue with the root taxa.
    // LOG_DBG << "add_children_to_queue_( tax );";
    add_children_to_queue_( tax );
    if( !clades.empty() && clades.top().clade_size == 0 ) {
        throw std::invalid_argument( "Cannot make colors from taxonomy without any group sizes" );
    }

    // Now loop until we have reached enough elements in the queue. This might overshoot
    // if there is a clade with many immediate children, but we take care of that later.
    // LOG_DBG << "while( clades.size() < power_set_taxa )";
    assert( !clades.empty() );
    while( clades.size() < power_set_taxa ) {
        // LOG_DBG1 << "while " << clades.size() << " < " << power_set_taxa;
        // We repeatedly pop the top element (largest), and insert its children instead.
        // The taxon vector always contains exactly one element, as we only add multiple
        // at the very end of this loop.
        // auto const top = clades.top();
        // clades.pop();
        // assert( top.clade_taxa.size() == 1 );
        // add_children_to_queue_( *top.clade_taxa.front() );
        // for( auto const& taxon : *top.clade_taxa.front() ) {
        //     add_children_to_queue_( taxon );
        // }

        // New approach that can handle multiple taxa in a clade
        auto top = clades.top();
        clades.pop();
        assert( top.clade_taxa.size() >= 1 );
        while( top.clade_taxa.size() > 0 && clades.size() < power_set_taxa - 1 ) {
            auto back = top.clade_taxa.back();
            top.clade_taxa.pop_back();
            add_children_to_queue_( *back );
        }
        if( !top.clade_taxa.empty() ) {
            clades.push(top);
        }
        // for( auto const& ct : top.clade_taxa ) {
        //     add_children_to_queue_( *ct );
        // }
        assert( !clades.empty() );
    }
    // LOG_DBG << "finished loop at " << clades.size();
    if( clades.empty() ) {
        // This should not happen, as we check for an empty taxonomy first.
        // But if this triggers for some other reason, it's better to check for stability.
        throw std::invalid_argument( "Cannot make colors from taxonomy without any taxa" );
    }
    assert( clades.size() == power_set_taxa );

    // Helper function to set the bit corresponding to a taxon group in a bitvector.
    auto set_taxon_group_bit_ = [num_groups]( Taxon const& taxon, Bitvector& bv )
    {
        assert( bv.size() == num_groups );
        auto const& data = taxon.data<KmerTaxonData>();
        if( data.status == KmerTaxonData::Status::kGroupExpanded ) {
            return;
        }
        if( data.index > num_groups ) {
            throw std::invalid_argument(
                "Invalid KmerTaxonData::GroupStatus, invalid group index > num groups"
            );
        }
        // LOG_DBG2 << "set_taxon_group_bit_ " << data.index << " " << dynamic_cast<Taxon const*>( &taxon )->name();
        bv.set( data.index );
    };

    // We now add the elements, top to bottom, to our result vector. We stop early here
    // as the above loop might have overdone it in case that a clade contains too many children.
    // We add a bitvector for every taxon, containing all bits set of the groups that are children
    // of this taxon. This allows us later to efficiently construct our wanted colors, instead
    // of having to repeatedly traverse the taxonomy there - we only need to do it once here.
    std::vector<Bitvector> colors;
    colors.reserve( power_set_taxa );
    auto control = Bitvector( num_groups );
    // LOG_DBG << "while( clades.size() > 0 )";
    while( clades.size() > 0 ) {
        // LOG_DBG1 << "clades.size() == " << clades.size();
        // Add all groups that are part of the taxon, including itself. That is important
        // if the taxon is a leaf itself, which could happen if it contains a lot of sequences,
        // and hence got its own entry in the queue.
        auto group_elements = Bitvector( num_groups );
        assert( ! clades.top().clade_taxa.empty() );
        // LOG_DBG << "clade top size " << clades.top().clade_size;
        for( auto const& taxon : clades.top().clade_taxa ) {
            // LOG_DBG2 << "set_taxon_group_bit_ " << taxon->data<KmerTaxonData>().index << " " << taxon->name() << " and children";
            set_taxon_group_bit_( *taxon, group_elements );
            for( auto const& child : taxonomy::preorder( *taxon )) {
                set_taxon_group_bit_( child, group_elements );
            }
        }
        clades.pop();

        // LOG_DBG1 << "ctr " << to_bit_string( control, false, '-' );
        LOG_DBG1 << "grp " << to_bit_string( group_elements, false, '-' );

        // We also check that there is no overlap with any previous bitvector
        // which would mean that a group is in multiple non-sibling taxa, which is wrong.
        // if( any_set( control & group_elements )) {
        //     throw std::runtime_error( "Invalid groups with same group number in non-silbing clades" );
        // }
        control |= group_elements;

        // Finally, add the bitvector to our colors list.
        colors.push_back( std::move( group_elements ));
    }
    assert( colors.size() == power_set_taxa );

    // Finally, we must have set all bits in the control, as we have added all groups.
    if( ! all_set( control )) {
        // LOG_DBG1 << "ctr " << to_bit_string( control, false, '-' );
        // throw std::runtime_error( "Missing or non-consecutive group numbers in taxonomy" );
        LOG_WARN << "Missing or non-consecutive group numbers in taxonomy";
        // colors.push_back( Bitvector( num_groups, true ));
    }
    return colors;
}

// -------------------------------------------------------
//     collect_taxa_bitvectors_top_down_partition_sizes_
// -------------------------------------------------------

struct TopDownClade {
    size_t clade_size;
    std::vector<taxonomy::Taxon const*> clade_taxa;
};

std::vector<TopDownClade> collect_taxa_bitvectors_top_down_partition_sizes_(
    taxonomy::Taxonomy const& tax,
    size_t power_set_taxa,
    bool use_num_sequences,
    size_t num_groups
) {
    using namespace utils;
    using namespace taxonomy;

    // Ad hoc solution similar to the partitioning algorithm: split taxa up until their size is below
    // the required average clade size that we want, then merge back the smallest pair of neighbors
    // until we have our desired number of groups. We kind of reimplement the same algorithm here,
    // but with the difference that we need to work within the constraints of the already existing
    // partionining, and hence cannot completely re-partition things here, and need to keep
    // monophyletic partiotions as a whole etc... maybe there is a better way to avoid the
    // duplication, but I don't think so, because the algorithms are similar in principle,
    // but different enough in their details that we cannot reuse much here.
    std::vector<TopDownClade> clades;

    // Get the total size of the taxonomy, by summing up the clade sums of the top level taxa.
    size_t total_size = 0;
    for( auto const& taxon : tax ) {
        auto const& data = taxon.data<KmerTaxonData>();
        total_size += (
            use_num_sequences ? data.clade_num_sequences : data.clade_sum_seq_lengths
        );
    }

    // Fill the clades list will all taxa that are small enough
    std::function<void(Taxon const&)> fill_clades_ = [&]( Taxon const& taxon )
    {
        for( auto const& child : taxon ) {
            auto const& child_data = child.data<KmerTaxonData>();
            auto const child_size = (
                use_num_sequences ? child_data.clade_num_sequences : child_data.clade_sum_seq_lengths
            );
            if(
                child_data.status == KmerTaxonData::Status::kGroupAssigned ||
                child_data.status == KmerTaxonData::Status::kPartitionMonophyletic ||
                child_size < total_size / num_groups
            ) {
                clades.push_back({ child_size, { &child }});
            } else if( child_data.status == KmerTaxonData::Status::kPartitionParaphyletic ) {
                clades.push_back({ child_size, { &child }});
                fill_clades_( child );
            } else {
                fill_clades_( child );
            }
        }
    };
    for( auto const& taxon : tax ) {
        fill_clades_( taxon );
    }
    assert( clades.size() > 0 );

    // Now merge the smallest pair of neighbors until we have the required num of groups.
    while( clades.size() > power_set_taxa ) {
        // Find the smallest neighboring pair of entries.
        size_t first_index = std::numeric_limits<size_t>::max();
        size_t combined_size = std::numeric_limits<size_t>::max();
        for( size_t i = 0; i < clades.size() - 1; ++i ) {
            if( clades[i].clade_size + clades[i+1].clade_size < combined_size ) {
                first_index = i;
                combined_size = clades[i].clade_size + clades[i+1].clade_size;
            }
        }
        assert( combined_size != std::numeric_limits<size_t>::max() );
        assert( first_index < clades.size() - 1 );

        // Combine the pair into the first, and erase the second.
        clades[first_index].clade_size += clades[first_index+1].clade_size;
        clades[first_index].clade_taxa.insert(
            clades[first_index].clade_taxa.end(),
            clades[first_index+1].clade_taxa.begin(),
            clades[first_index+1].clade_taxa.end()
        );
        clades.erase( clades.begin() + first_index + 1 );
    }
    assert( clades.size() == power_set_taxa );

    for( size_t i = 0; i < clades.size(); ++i ) {
        std::vector<size_t> indices;
        for( auto const& taxon : clades[i].clade_taxa ) {
            indices.push_back( taxon->data<KmerTaxonData>().index );
        }
        std::sort( indices.begin(), indices.end() );
        indices.erase( std::unique( indices.begin(), indices.end() ), indices.end() );
        LOG_DBG << i << " (" << clades[i].clade_size << "): " << join( indices );
    }
    return clades;
}

// -------------------------------------------------------
//     collect_taxa_bitvectors_top_down_partitioning_
// -------------------------------------------------------

std::vector<utils::Bitvector> collect_taxa_bitvectors_top_down_partitioning_(
    taxonomy::Taxonomy const& tax,
    size_t power_set_taxa,
    bool use_num_sequences,
    size_t num_groups
) {
    using namespace utils;
    using namespace taxonomy;
    assert( power_set_taxa > 0 );
    assert( num_groups >= power_set_taxa );

    // Get clades according to their sizes.
    auto clades = collect_taxa_bitvectors_top_down_partition_sizes_(
        tax, power_set_taxa, use_num_sequences, num_groups
    );

    // Helper function to set the bit corresponding to a taxon group in a bitvector.
    auto set_taxon_group_bit_ = [num_groups]( Taxon const& taxon, Bitvector& bv )
    {
        assert( bv.size() == num_groups );
        auto const& data = taxon.data<KmerTaxonData>();

        // Do not set bits for expanded groups. In all other cases, we set the bits.
        if( data.status == KmerTaxonData::Status::kGroupExpanded ) {
            return;
        }
        assert(
            data.status == KmerTaxonData::Status::kGroupAssigned ||
            data.status == KmerTaxonData::Status::kPartitionMonophyletic ||
            data.status == KmerTaxonData::Status::kPartitionParaphyletic
        );
        if( data.index > num_groups ) {
            throw std::invalid_argument(
                "Invalid KmerTaxonData::GroupStatus, invalid group index > num groups"
            );
        }
        bv.set( data.index );
    };

    // We create bitvectors for each clade, containing all bits set of the groups that are children
    // of the taxa in that clade. This allows us later to efficiently construct our wanted colors,
    // instead of having to repeatedly traverse the taxonomy there - we only need to do it once here.
    std::vector<Bitvector> colors;
    colors.reserve( power_set_taxa );
    auto control = Bitvector( num_groups );
    for( auto const& clade : clades ) {
        // Add all groups that are part of the taxon, including itself. That is important
        // if the taxon is a leaf itself, which could happen if it contains a lot of sequences,
        // or is a paraphyletic partition, and hence got its own entry in the queue.
        auto group_elements = Bitvector( num_groups );
        for( auto const& taxon : clade.clade_taxa ) {
            set_taxon_group_bit_( *taxon, group_elements );
            for( auto const& child : taxonomy::preorder( *taxon )) {
                set_taxon_group_bit_( child, group_elements );
            }
        }

        // We also check that there is no overlap with any previous bitvector
        // which would mean that a group is in multiple non-sibling taxa, which is wrong.
        // Nope, in the above algorithm, we can have neighboring clades that got assigned to
        // different groups here, so there can be overlap. Doesn't matter for this purpose though,
        // as that just means the color is a bit less specific than it could be, but not by much.
        // Maybe revisit later to check if this can be improved.
        // if( any_set( control & group_elements )) {
        //     throw std::runtime_error( "Invalid groups with same group number in non-silbing clades" );
        // }
        control |= group_elements;

        // Finally, add the bitvector to our colors list.
        LOG_DBG1 << "grp " << to_bit_string( group_elements, false, '-' );
        colors.push_back( std::move( group_elements ));
    }
    assert( colors.size() == power_set_taxa );

    // Finally, we must have set all bits in the control, as we have added all groups.
    if( ! all_set( control )) {
        throw std::runtime_error( "Missing or non-consecutive group/partition numbers in taxonomy" );
    }
    return colors;
}

// -------------------------------------------------------
//     collect_taxa_bitvectors_top_down_intervals_
// -------------------------------------------------------

std::vector<utils::Bitvector> collect_taxa_bitvectors_top_down_intervals_(
    taxonomy::Taxonomy const& tax,
    size_t power_set_taxa,
    bool use_num_sequences,
    size_t num_groups
) {
    using namespace utils;
    using namespace taxonomy;

    // New approach that just builds intervals of partitions of roughly equal size.
    // Simple and straight forward, and should give good results. This assumes that
    // partitions are not only consecutive in their numbers, but also along the taxonomy,
    // i.e., that they come from the partitioning via preorder traversal, where close by
    // partition indices are also close by taxonomically.

    // Collect the total sizes of each partition.
    auto partition_sizes = std::vector<size_t>( num_groups, 0 );
    size_t total_size = 0;
    for( auto const& taxon : preorder( tax )) {
        auto const& data = taxon.data<KmerTaxonData>();
        if( data.index != std::numeric_limits<size_t>::max() ) {
            auto const part_size = (
                use_num_sequences ? data.num_sequences : data.sum_seq_lengths
            );
            partition_sizes[data.index] += part_size;
            total_size += part_size;
        }
    }
    double target = static_cast<double>(total_size) / power_set_taxa;

    std::vector<std::vector<size_t>> intervals;
    do {
        intervals.clear();

        // Data structure to store partitions.
        std::vector<size_t> current_interval;
        long long current_sum = 0;
        size_t intervals_formed = 0;
        LOG_DBG << "target " << target;

        // Traverse the list and form intervals greedily.
        for( size_t i = 0; i < partition_sizes.size(); i++ ) {
            current_sum += partition_sizes[i];
            current_interval.push_back( i );

            // If we've reached or exceeded the target and have not
            // formed the last partition, then cut the partition here.
            if( current_sum >= target && intervals_formed < power_set_taxa - 1 ) {
                LOG_DBG << "interval sum " << current_sum;
                intervals.push_back(current_interval);
                current_interval.clear();
                current_sum = 0;
                ++intervals_formed;
            }
        }

        // Add any remaining elements to the final partition.
        if( !current_interval.empty() ) {
            LOG_DBG << "interval sum " << current_sum;
            intervals.push_back( current_interval );
        }
        LOG_DBG << "intervals.size() == power_set_taxa " << intervals.size() << " vs " << power_set_taxa;

        // If we did not succeed this round, decrement the target by a bit.
        // Super ad hoc, but let's get something to work quickly...
        target *= 0.95;
    } while( intervals.size() < power_set_taxa );
    assert( intervals.size() == power_set_taxa );

    // Turn them into colors.
    std::vector<Bitvector> colors;
    colors.reserve( power_set_taxa );
    for( auto const& interval : intervals ) {
        // LOG_DBG << "interval " <<  join( interval );
        auto group_elements = Bitvector( num_groups );

        // Very inefficient - we could instead just set the range...
        // But too lazy and under too much time pressure to think about this cleanly.
        for( auto e : interval ) {
            group_elements.set(e);
        }
        LOG_DBG1 << "int " << to_bit_string( group_elements, false, '-' );
        colors.push_back( std::move( group_elements ));
    }
    return colors;
}

// -------------------------------------------------------
//     make_secondary_colors_from_taxonomy_top_down
// -------------------------------------------------------

std::vector<utils::Bitvector> make_secondary_colors_from_taxonomy_top_down(
    taxonomy::Taxonomy const& tax,
    char method,
    size_t power_set_taxa,
    bool use_num_sequences
) {
    using namespace utils;
    using namespace taxonomy;

    // Similar to above: Cautionary check of computational limits within this function.
    if( power_set_taxa == 0 || power_set_taxa > 63 ) {
        throw std::invalid_argument( "Invalid power_set_taxa" );
    }

    // Prepare the result and get the number of bits per bitvector, i.e., the number of groups.
    std::vector<utils::Bitvector> colors;
    auto const num_groups = count_taxon_groups_or_partitions( tax );
    if( num_groups < power_set_taxa ) {
        throw std::invalid_argument(
            "Cannot make colors from taxonomy if number of groups "
            "is smaller than requested power set size"
        );
    }

    // Get the list of taxa that we want to run the power set on. It could be smaller than what
    // we want if the taxonomy is small, but never bigger. Update the limit to the actual size.
    std::vector<utils::Bitvector> top_taxa;
    if( method == 'e' ) {
        top_taxa = collect_taxa_bitvectors_top_down_experiment_(
            tax, power_set_taxa, use_num_sequences, num_groups
        );
    } else if( method == 'i' ) {
        top_taxa = collect_taxa_bitvectors_top_down_intervals_(
            tax, power_set_taxa, use_num_sequences, num_groups
        );
    } else if( method == 'p' ) {
        top_taxa = collect_taxa_bitvectors_top_down_partitioning_(
            tax, power_set_taxa, use_num_sequences, num_groups
        );
    } else {
        throw std::runtime_error( "Invalid top down method" );
    }
    assert( top_taxa.size() > 0 );
    assert( top_taxa.size() <= power_set_taxa );
    power_set_taxa = top_taxa.size();
    assert( power_set_taxa > 0 );

    // We now run a similar algorithm to iterate the power set as in the above bottom-up approach.
    // For this, we set up a counter (mask) that we go through, and add a color for each
    // value, adding _all_ groups in the active taxa.
    auto const max_mask = static_cast<size_t>( 1 ) << top_taxa.size();
    for( size_t mask = 0; mask < max_mask; ++mask ) {
        auto group_elements = Bitvector( num_groups );

        // Check each bit of the mask, and if it is set, that means that in this iteration,
        // we set all elements in the bitvector corresponding to all indices of this group.
        size_t activated_groups = 0;
        for( size_t a = 0; a < top_taxa.size(); ++a ) {
            if(( mask & (static_cast<size_t>( 1 ) << a )) == 0 ) {
                continue;
            }
            ++activated_groups;

            // We have a set bit in the mask, meaning that this group of inidices (all groups
            // below the index taxon) needs to be added for this color in the power set.
            // As we have computed all groups of the taxa as bitvectors already, this is a
            // simple OR operation.
            group_elements |= top_taxa[a];
        }
        (void) activated_groups;
        colors.push_back( std::move( group_elements ));
    }

    // Lastly, add the all-set color, just in case it is needed.
    if( ! colors.empty() && ! all_set( colors.back() )) {
        colors.push_back( Bitvector( num_groups, true ));
    }

    return colors;
}

// =================================================================================================
//     Color Gamut Functions
// =================================================================================================

size_t count_unique_lookup_keys( KmerColorGamut const& gamut )
{
    // Loop over the entire multimap, but "jump" over groups of the same key.
    auto const& lookup = gamut.get_color_lookup();
    size_t cnt = 0;
    for (auto it = lookup.begin(); it != lookup.end(); ) {
        ++cnt;
        it = lookup.equal_range( it->first ).second;
    }
    return cnt;
}

void verify_unique_colors( KmerColorGamut const& gamut )
{
    // We copy to a set for this. Using a pairwise comparison between all colors of the same hash
    // would be far more efficient, but this function is mostly for debugging only anyway.
    assert( gamut.get_color_list().size() == gamut.get_color_lookup().size() );
    std::unordered_set<utils::Bitvector> elements_bvs;
    for( auto const& color : gamut.get_color_list() ) {
        elements_bvs.insert( color.elements );
    }
    if( gamut.get_color_list().size() != elements_bvs.size() ) {
        throw std::runtime_error( "Kmer Color Gamut contains duplicate colors" );
    }
}

// =================================================================================================
//     Printing
// =================================================================================================

std::string print_kmer_color_list( KmerColorGamut const& gamut )
{
    assert( gamut.get_color_list().size() == gamut.get_color_lookup().size() );

    size_t const int_width = std::ceil( std::log10( gamut.get_color_list().size() ));
    std::stringstream ss;
    ss << "Colors: " << gamut.get_color_list().size() << "\n";
    for( size_t i = 0; i < gamut.get_color_list().size(); ++i ) {
        auto const& color = gamut.get_color_at(i);
        ss << std::setw( int_width ) << i;
        ss << " " << utils::to_bit_string( color.elements );
        ss << "\n";
    }
    return ss.str();
}

std::string print_kmer_color_lookup( KmerColorGamut const& gamut )
{
    assert( gamut.get_color_list().size() == gamut.get_color_lookup().size() );

    std::stringstream ss;
    ss << "Colors: " << gamut.get_color_list().size();
    ss << ", unique keys: " << count_unique_lookup_keys( gamut );
    ss << "\n";

    // Outer loop: iterate over the hashes while "jumping" over groups of duplicate keys.
    auto const& lookup = gamut.get_color_lookup();
    for( auto it = lookup.begin(); it != lookup.end(); ) {
        auto range = lookup.equal_range(it->first);

        ss << "0x" << std::setw(2 * sizeof(size_t)) << std::setfill('0') << std::hex << it->first;
        ss << " --> { ";

        // Inner loop: All values that have that hash key.
        bool first = true;
        for( auto inner = range.first; inner != range.second; ++inner ) {
            if( ! first ) {
                ss << ", ";
            }
            first = false;
            ss << std::dec << inner->second;
        }
        ss << " }\n";
        it = range.second;
    }
    return ss.str();
}

std::string print_kmer_color_gamut( KmerColorGamut const& gamut )
{
    auto const gamut_matrix = gamut.get_gamut_matrix();
    if( gamut_matrix.empty() ) {
        return "";
    }

    // Proper alignment for nicer output
    auto const max_gamut = *std::max_element( gamut_matrix.begin(), gamut_matrix.end() );
    size_t const first_width = std::ceil( std::log10( gamut_matrix.rows() ));
    size_t const col_width = std::ceil( std::log10( std::max( max_gamut, gamut_matrix.cols() )));

    // Write header line with the element indices
    std::stringstream ss;
    ss << std::string( first_width, ' ' );
    for( size_t c = 0; c < gamut_matrix.cols(); ++c ) {
        ss << " " << std::setw( col_width ) << c;
    }
    ss << "\n";

    // Write the content of the gamut, with an extra first column for the color indices.
    for( size_t r = 0; r < gamut_matrix.rows(); ++r ) {
        ss << std::setw( first_width ) << r;
        for( size_t c = 0; c < gamut_matrix.cols(); ++c ) {
            ss << " " << std::setw( col_width ) << gamut_matrix( r, c );
        }
        ss << "\n";
    }

    return ss.str();
}

std::string print_kmer_color_gamut_summary( KmerColorGamut const& gamut )
{
    assert( gamut.get_color_list().size() == gamut.get_color_lookup().size() );

    // Report how many of the gamut are real and how many are imaginar numbers
    auto const& gamut_matrix = gamut.get_gamut_matrix();
    auto const& stats = gamut.get_gamut_statistics();
    auto const gamut_size = gamut_matrix.rows() * gamut_matrix.cols();
    auto const gamut_empty = gamut_size - ( stats.real_color_count + stats.imag_color_count);
    auto const real_per = gamut_size == 0 ? 0.0 : 100.0 * stats.real_color_count / gamut_size;
    auto const imag_per = gamut_size == 0 ? 0.0 : 100.0 * stats.imag_color_count / gamut_size;
    auto const empt_per = gamut_size == 0 ? 0.0 : 100.0 * gamut_empty / gamut_size;

    // Proper alignment for nicer output
    size_t const gamut_width = (
        gamut_matrix.empty()
        ? 1
        : std::ceil( std::log10( std::max({
            stats.real_color_count, stats.imag_color_count, gamut_empty
        })))
    );
    size_t const percent_width = (
        gamut_matrix.empty()
        ? 3
        : std::ceil( std::log10( std::max({
            std::ceil( real_per ), std::ceil( imag_per ), std::ceil( empt_per  )
        }))) + 2
    );

    // Count all colors
    std::stringstream ss;
    ss << "Elements:    " << gamut.get_element_count() << "\n";
    ss << "Colors:      " << gamut.get_color_list().size() << "\n";
    ss << "Max colors:  " << gamut.get_max_color_count() << "\n";
    ss << "Unique keys: " << count_unique_lookup_keys( gamut ) << "\n";
    ss << "Gamut size:  " << gamut_matrix.rows() << " x " << gamut_matrix.cols() << "\n";
    ss << std::fixed << std::setprecision(1);
    ss << "Gamut real:  " << std::setw(gamut_width) << stats.real_color_count;
    ss << " (" << std::setw(percent_width) << real_per << "%)\n";
    ss << "Gamut imag:  " << std::setw(gamut_width) << stats.imag_color_count;
    ss << " (" << std::setw(percent_width) << imag_per << "%)\n";
    ss << "Gamut empty: " << std::setw(gamut_width) << gamut_empty;
    ss << " (" << std::setw(percent_width) << empt_per << "%)\n";
    return ss.str();
}

// =================================================================================================
//     Serialization
// =================================================================================================

void serialize_kmer_color_gamut_colors(
    KmerColorGamut const& gamut,
    std::shared_ptr<utils::BaseOutputTarget> output_target
) {
    // We need to iterate through the color vector manually here,
    // as the Bitvectors are wrapped in a struct for future compatibility.
    auto ser = utils::Serializer( output_target );
    auto const& color_list = gamut.get_color_list();
    ser << color_list.size();
    for( auto const& color : color_list ) {
        ser << color.elements;
    }
}

void serialize_kmer_color_gamut_matrix(
    KmerColorGamut const& gamut,
    std::shared_ptr<utils::BaseOutputTarget> output_target
) {
    // Functionality for std::vector and Matrix is already
    // implemented in the respective classes and functions.
    auto ser = utils::Serializer( output_target );
    ser << gamut.get_gamut_matrix();
}

std::vector<utils::Bitvector> deserialize_kmer_color_gamut_colors(
    std::shared_ptr<utils::BaseInputSource> input_source
) {
    // Functionality for std::vector and Bitvector is already
    // implemented in the respective classes and functions.
    std::vector<utils::Bitvector> result;
    auto deser = utils::Deserializer( input_source );
    deser >> result;
    return result;
}

utils::Matrix<size_t> deserialize_kmer_color_gamut_matrix(
    std::shared_ptr<utils::BaseInputSource> input_source
) {
    // Functionality for std::vector and Matrix is already
    // implemented in the respective classes and functions.
    utils::Matrix<size_t> result;
    auto deser = utils::Deserializer( input_source );
    deser >> result;
    return result;
}

} // namespace sequence
} // namespace genesis

#endif // GENESIS_CPP_STD >= GENESIS_CPP_STD_17

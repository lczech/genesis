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

#include "genesis/sequence/kmer/color_set.hpp"
#include "genesis/utils/core/logging.hpp"

#include <algorithm>
#include <cassert>
#include <stdexcept>

namespace genesis {
namespace sequence {

// ================================================================================================
//     Init
// ================================================================================================

// -------------------------------------------------------------------------
//     init_primary_colors
// -------------------------------------------------------------------------

void KmerColorSet::init_primary_colors_()
{
    // Sanity checks.
    if( ! colors_.empty() ) {
        throw std::runtime_error( "Primary colors already initialized" );
    }
    assert( colors_.size() == 0 );
    assert( primary_color_count_ != 0 );

    // Add the empty color as the first element. This is so that a color index of 0,
    // which is what we get by default in an uninitialized lookup, does not trick us
    // into believing it's a real color. Hence, internally, we need to offset all
    // primary colors by 1.
    Color empty;
    empty.elements = Bitvector( primary_color_count_ );
    empty.type = Color::Type::kEmpty;
    push_back_color( std::move( empty ));
    assert( colors_.size() == 1 );

    // Now add entries for all primary colors, with bitvectors that have
    // the bit set that corresponds to the color's index.
    for( size_t i = 0; i < primary_color_count_; ++i ) {
        Color color;
        color.elements = Bitvector( primary_color_count_ );
        color.elements.set( i );
        color.type = Color::Type::kPrimary;
        push_back_color( std::move( color ));
    }
    assert( colors_.size() == 1 + primary_color_count_ );
}

// -------------------------------------------------------------------------
//     init_secondary_colors_with_binary_reduction
// -------------------------------------------------------------------------

void KmerColorSet::init_secondary_colors_with_binary_reduction()
{
    // In the absence of a phylogeny or taxonomy to group our primary colors by,
    // we use a binary reduction, essentially creating a complete binary tree
    // through recursive pairwise grouping of the elements.
    check_uninitialized_secondary_colors_();

    // Local shorthand for the lambda capture, needed for C++11.
    auto& colors = colors_;

    // Define a recursive lambda that takes the first index and one-past-the-last index
    // for the current block of colors that we want to merge and create secondary colors from.
    size_t leftover_index = 0;
    std::function<void(size_t, size_t)> recursive_merge_ = [&]( size_t first, size_t last )
    {
        // Base case: no merge needed if no more elements are left.
        assert( first <= last );
        assert( last == colors.size() );
        if( last - first == 0 ) {
            return;
        }

        // Record the starting index of the new block that will be appended,
        // and process elements pairwise in the current block [first, last).
        size_t new_start = colors.size();
        for( size_t i = first; i < last; i += 2 ) {

            // Default case: two groups to merge into a new color.
            if( i + 1 < last ) {
                merge_colors_and_push_back( i, i+1 );
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
                merge_colors_and_push_back( i, leftover_index );
                leftover_index = 0;
            }
        }

        // Recursively merge the newly created block.
        recursive_merge_( new_start, colors.size() );
    };

    // Start the recursive merging using the entire initial range of primary colors.
    recursive_merge_( 1, colors_.size() );

    // At the end, there should be no leftover color; all should be in some secondary color.
    // Due to the exist condition of the recursive function, the leftover is set to the last
    // element that has all colors set (which is hence not really a leftover).
    assert( leftover_index == colors_.size() - 1 );

    // A binary tree has one fewer inner nodes than tips. We have added those as colors,
    // and have the empty color, so our total is now double the number of primary colors.
    // Also, the last secondary color should contain _all_ primary colors.
    assert( colors_.size() == 2 * primary_color_count_ );
    assert( all_set( colors_.back().elements ));
    check_max_real_color_count_();
}

// -------------------------------------------------------------------------
//     init_secondary_colors_from_bitvectors
// -------------------------------------------------------------------------

void KmerColorSet::init_secondary_colors_from_bitvectors(
    std::vector<Bitvector> const& bitvecs
) {
    check_uninitialized_secondary_colors_();

    // We need at least one fully set bitvector, to guarantee that our imaginary colors
    // work as expected, i.e, such that they can always find at least that one as a match.
    // Nope: Deactivated again - we leave this up to the user, and fail later instead.
    // bool found_all_set = false;

    // Add all bitvectors as secondary colors
    for( auto const& bv : bitvecs ) {
        // Validity checks
        if( bv.size() != primary_color_count_ ) {
            throw std::invalid_argument(
                "Cannot initialize Kmer Color Set with Bitvectors of size "
                "that does not match the primary color count"
            );
        }
        if( utils::pop_count( bv ) < 2 ) {
            throw std::invalid_argument(
                "Cannot initialize Kmer Color Set with Bitvectors representing "
                "the empty color or primary colors (i.e., zero or single bit set)"
            );
        }
        // if( all_set( bv )) {
        //     found_all_set = true;
        // }

        // Create a color for the bitvector, and check that it is not a duplicate.
        Color color;
        color.elements = bv;
        color.type = Color::Type::kSecondary;
        if( find_color( color.elements ) > 0 ) {
            throw std::invalid_argument(
                "Cannot initialize Kmer Color Set with Bitvectors containing duplicates"
            );
        }

        // All good; add the color to our list.
        push_back_color( std::move( color ));
    }

    // Final checks
    // if( ! found_all_set ) {
    //     throw std::invalid_argument(
    //         "Cannot initialize Kmer Color Set with Bitvectors "
    //         "that do not contain an all-set bitvector"
    //     );
    // }
    check_max_real_color_count_();
}

// -------------------------------------------------------------------------
//     init_secondary_colors_from_indices
// -------------------------------------------------------------------------

void KmerColorSet::init_secondary_colors_from_indices(
    std::vector<std::vector<size_t>> const& indices
) {
    // Quick and dirty: create bitvectors from the indices, then forward. We are currently
    // not expecting more than a few hundred or thousand colors, so this should be fine
    // for initialization. If extended to more colors, might need optimization.
    std::vector<Bitvector> bitvecs;
    for( auto const& index_list : indices ) {
        bitvecs.emplace_back( Bitvector( primary_color_count_, index_list ));
    }
    init_secondary_colors_from_bitvectors( bitvecs );
}

// -------------------------------------------------------------------------
//     check_uninitialized_secondary_colors_
// -------------------------------------------------------------------------

void KmerColorSet::check_uninitialized_secondary_colors_() const
{
    // We currently only allow initializing the secondary colors once.
    // Might change in the future to allow to set multiple types of starting colors
    // for the secondaries, but for now, we do not need/want that.
    if( colors_.size() != 1 + primary_color_count_ ) {
        throw std::runtime_error(
            "Primary color count is off or secondary colors have already been initialized"
        );
    }
}

// -------------------------------------------------------------------------
//     check_max_real_color_count_
// -------------------------------------------------------------------------

void KmerColorSet::check_max_real_color_count_() const
{
    // If we have a max real color count set, but it is already exhausted after initialization,
    // we have reached an invalid state, as the that means we have valid secondary colors
    // that are beyond the max, and so they are unusuable. Guard against that.
    if( max_real_color_count_ == 0 ) {
        return;
    }
    if( colors_.size() > max_real_color_count_ ) {
        throw std::invalid_argument(
            "Cannot initialize Kmer Color Set with more secondary colors than the max real color count"
        );
    }
}

// ================================================================================================
//     Lookup
// ================================================================================================

// -------------------------------------------------------------------------
//     lookup_and_update
// -------------------------------------------------------------------------

size_t KmerColorSet::lookup_and_update(
    size_t existing_color_index,
    size_t target_element_index
) {
    // Sanity checks.
    if( existing_color_index >= colors_.size() ) {
        throw std::invalid_argument(
            "Invalid color index " + std::to_string( existing_color_index )
        );
    }
    if( target_element_index >= primary_color_count_ ) {
        throw std::invalid_argument(
            "Invalid primary index " + std::to_string( target_element_index )
        );
    }
    assert( 1 + target_element_index <= colors_.size() );
    assert( 1 + primary_color_count_ <= colors_.size() );
    assert( colors_[existing_color_index].elements.size() == primary_color_count_ );

    // Usually, we only want to look up real colors (primary and secondary, but not imaginary),
    // as those are the only ones being stored by the caller anyway. So here we could test
    // if the given color is real. However, the below code also works when requesting an existing
    // imaginary color. Not sure if that is usueful or good, but let's allow this for now.
    // if( max_real_color_count_ > 0 && existing_color_index >= max_real_color_count_ ) {
    //     throw std::invalid_argument(
    //         "Color at index " + std::to_string( existing_color_index ) + " is an imaginary color"
    //     );
    // }

    // Special case for speed: If the existing color is the empty color,
    // then we are dealing with a default empty entry that does not yet have any color.
    // In that case, we can just return the index where the added primary color resides.
    // The below code would also work in this case, but take more time to get there.
    if( existing_color_index == 0 ) {
        // Assert that the element bitvector is indeed set for the target index.
        assert( colors_[ 1 + target_element_index ].elements.get( target_element_index ));
        ++lookup_stats_.existing_is_empty;
        return 1 + target_element_index;
    }

    // Check if the given color already comprises the primary color element.
    // If so, we do not need to do anything here, and the color index is the same as given.
    auto const& existing_color = colors_[existing_color_index];
    if( existing_color.elements.get( target_element_index )) {
        // We do usually not expect that this function is called with the index of an imaginary
        // color, but it also should not be an issue really to do that. We only need to return
        // the index stored in the color here, instead of existing_color_index. Those are the
        // same for primary and secondary colors, but not for imaginary ones, where they point
        // to a secondary color intead.
        ++lookup_stats_.existing_contains_target;
        return existing_color.index;
    }

    // Here, we are in the case where the existing color does not already contain
    // the newly added primary color, so we need to make a new set of their union.
    Color target_color;
    target_color.elements = existing_color.elements;
    target_color.elements.set( target_element_index );
    // target_color.occurrence = 1;

    // Check if an entry with that combination already exists. If so, this is our result.
    auto const target_index = find_color( target_color.elements );
    if( target_index > 0 ) {
        // We return the internal index here, as this might be an imaginary color.
        assert( target_index < colors_.size() );
        assert( max_real_color_count_ == 0 || colors_[target_index].index < max_real_color_count_ );
        ++lookup_stats_.target_color_exists;
        return colors_[target_index].index;
    }

    // If the new color is not in our list yet, this is a secondary color
    // that we have not seen before. We need to add it to our list,
    // either as a new secondary color, or, if we are out of space for those,
    // as an imaginary color.

    // TODO Decommissioning unused colors.
    // Below, whenever we have found the index we are looking for, we might not need
    // the existing color any more, as we are updating it with a new color. If its occurrence
    // goes down to zero, we can instead replace it with the new one in the list, remove
    // from the lookup, and add a new lookup entry instead.
    // if( existing_color.type == Color::Type::kSecondary && existing_color.occurrence == 1 ) {
    //     // replace the entry in the list, remove the hash, add the new hash
    // } else {
    //     // decrement the occurrence
    // }

    // Check if we have already exhausted our supply of secondary colors.
    // If not, we add the new target color as a secondary color.
    if( max_real_color_count_ == 0 || colors_.size() < max_real_color_count_ ) {
        // Add the color and get its index in the list.
        target_color.type = Color::Type::kSecondary;
        auto const added_index = push_back_color( std::move( target_color ));

        // We are adding it as a secondary color, so its index is idempotent. Assert that.
        assert( colors_.size() == added_index + 1 );
        assert( colors_.size() <= max_real_color_count_ );
        assert( colors_[added_index].index == added_index );
        assert( max_real_color_count_ == 0 || added_index < max_real_color_count_ );
        ++lookup_stats_.real_color_added;
        return added_index;
    }

    // Here we are in the case that we want to add the new color, but do not have space
    // for secondary colors any more, so we add it as an imaginary one instead.
    // First, we find the minimal superset in the secondary colors that includes our target color.
    auto const real_index = find_minimal_superset( target_color.elements );

    // If we have not found any matching superset, that means that our secondary colors
    // are missing a proper candidate, and should have been initialized differently,
    // for instance by including some reduction or clustering of the elements.
    if( real_index == 0 ) {
        throw std::runtime_error(
            "Could not find minimal superset of the target color in the secondary colors. "
            "This likely means the secondary colors were not initialized with a proper "
            "reduction or clustering of all elements, and hence do not contain a match "
            "for all needed colors. Consider initializing the secondary colors differently."
        );
    }

    // If we know we have found a valid secondary color, assert its propertiers.
    assert( max_real_color_count_ > 0 );
    assert( real_index > 0 );
    assert( real_index < max_real_color_count_ );
    assert( real_index < colors_.size() );

    // Now we add the color as an imaginary one that points to our secondary color.
    target_color.type = Color::Type::kImaginary;
    target_color.index = real_index;
    auto const added_index = push_back_color( std::move( target_color ));

    // We are adding it as an imaginary color, so its index is not idempotent. Assert that.
    // We then return the pointed-to secondary color.
    assert( colors_.size() == added_index + 1 );
    assert( colors_[added_index].index <  added_index );
    assert( colors_[added_index].index == real_index );
    assert( colors_[added_index].index < max_real_color_count_ );
    ++lookup_stats_.imaginary_color_added;
    return real_index;
}

// -------------------------------------------------------------------------
//     find_color
// -------------------------------------------------------------------------

size_t KmerColorSet::find_color( utils::Bitvector const& target ) const
{
    // Sanity check. Internally not needed, but added for external usage.
    if( target.size() != primary_color_count_ ) {
        throw std::invalid_argument(
            "Invalid target bitvecor with different number of bits than the number of primary colors"
        );
    }

    // There might be more than one color with the same hash, hence we need to
    // loop over the range to access all elements with the given hash key.
    assert( colors_.size() == lookup_.size() );
    auto const hash = utils::bitvector_hash( target );
    auto const range = lookup_.equal_range( hash );
    for( auto it = range.first; it != range.second; ++it ) {
        assert( it->first == hash );

        // Check if the given bitvector matches the one pointed to by this hash.
        // If so, we have found our match, and return its color index.
        if( colors_[it->second].elements == target ) {
            return it->second;
        }
    }

    // We did not find the entry, meaning there is no color with this hash.
    // Return the empty color index instead.
    return 0;
}

// -------------------------------------------------------------------------
//     find_minimal_superset
// -------------------------------------------------------------------------

size_t KmerColorSet::find_minimal_superset( utils::Bitvector const& target ) const
{
    // Sanity check. Internally not needed, but added for external usage.
    if( target.size() != primary_color_count_ ) {
        throw std::invalid_argument(
            "Invalid target bitvecor with different number of bits than the number of primary colors"
        );
    }

    // This function is internally only called if we exhausted the max number of secondary colors.
    // Thus, we only need to look up the secondary colors, as primary colors cannot be
    // our targets, and the imaginary ones are not minimal by definition
    // (as they are strict subsets of existing secondary colors).
    // We do not assert this here though, as we might want to use this function in other contexts
    // by outside callers. In that case, we just scan the range of secondary colors.
    // assert( max_real_color_count_ > 0 );
    // assert( max_real_color_count_ < colors_.size() );

    // Init our trackers. We have at most all bits of all primary colors set,
    // meaning that we start the search with one more, so that the min can find it.
    size_t min_index = 0;
    size_t min_pop_count = primary_color_count_ + 1;

    // We are searching for a strict superset that has minimal pop count.
    // We cannot be better than having exactly one element more set than the target,
    // so we can use that as an early stop criterion.
    size_t const target_pop_count = utils::pop_count( target );

    // Upper bound of the search in the typical use case is the end of the secondary colors.
    // We however allow this function to be called before all slots for secondary colors
    // are used up, in which case we need to stop whenever we reach the end of the colors.
    size_t const max_index = std::min( colors_.size(), max_real_color_count_ );

    // TODO Add an optional early stop condition here that only searches through the initial set
    // of secondary colors created with the init functions, in case that this here is too slow.
    // This would give suboptimal results, but speed up the color set construction if needed.

    // Find the superset of our target in the secondary colors that has the smallest pop count,
    // i.e., that comprises the smallest number of primary colors.
    // Here, we unfortunately need to iterate all colors, which is slow and might need
    // optimization or a different approach in the future. Let's see if the imaginary colors
    // saturate fast enough so that we do not need to search  for a new one all too often.
    for( size_t i = 1 + primary_color_count_; i < max_index; ++i ) {
        // The color needs to be different, as we would have found an identical match earlier.
        // We could use is_strict_superset() here as well, but it's slightly slower.
        assert( colors_[i].elements != target );
        if( ! utils::is_superset( colors_[i].elements, target )) {
            continue;
        }

        // See if this is a new minimal hit.
        auto const popcnt = utils::pop_count( colors_[i].elements );
        if( popcnt < min_pop_count ) {
            min_index = i;
            min_pop_count = popcnt;
        }

        // If it is as minimal as we can get, we can stop here already.
        if( popcnt == target_pop_count + 1 ) {
            break;
        }
    }

    // If we have not found any matching superset, that means that our secondary colors
    // are missing a proper candidate, and should have been initialized differently,
    // for instance by including some reduction or clustering of the elements.
    // Otherwise, we have found a secondary color that is a superset of our target.
    // Furthermore, it is guaranteed to be a strict superset, as otherwise, it would be
    // identical to the target, and so we would have found it in the lookup already.
    assert( min_index == 0 || min_pop_count <= primary_color_count_ );
    assert( min_index == 0 || min_pop_count > utils::pop_count( target ));
    assert( min_index == 0 || colors_[min_index].elements != target );
    assert( min_index == 0 || is_strict_superset( colors_[min_index].elements, target ));
    return min_index;
}

// ================================================================================================
//     Modification
// ================================================================================================

// -------------------------------------------------------------------------
//     push_back_color
// -------------------------------------------------------------------------

size_t KmerColorSet::push_back_color( KmerColorSet::Color&& color )
{
    // Check our assumptions.
    // In the init, we explicitly check for duplicates. During our internal executing however,
    // we assume that our algorithm never wants to add a color that already exists. Assert that.
    assert( find_color( color.elements ) == 0 );
    assert( color.elements.size() == primary_color_count_ );

    // Check usage.
    if( color.type == Color::Type::kEmpty && ! colors_.empty() ) {
        throw std::invalid_argument( "Cannot add empty color unless it is the first one" );
    }

    // Get the index at which the color will be placed in the list,
    // which is simply the current size of the list.
    size_t const index = colors_.size();

    // For imaginary colors, we assume that the caller already set the index
    // of the color it points to. For all others, we set it here for convenience.
    if( color.index == 0 ) {
        color.index = index;
    }

    // Add the color to the list and the lookup.
    auto const hash = utils::bitvector_hash( color.elements );
    colors_.emplace_back( std::move( color ));
    lookup_.insert({ hash, index });
    assert( colors_.size() == lookup_.size() );
    assert( colors_.size() == index + 1 );

    // We here return the real index in the list, not the one that the color points to,
    // and leave the decision about that to the caller.
    return index;
}

size_t KmerColorSet::merge_colors_and_push_back( size_t index_1, size_t index_2 )
{
    // Helper function that takes two colors, merges them, and adds them to the list.
    // First check that the colors are valid entries.
    if( index_1 == 0 || index_1 >= colors_.size() || index_2 == 0 || index_2 >= colors_.size() ) {
        throw std::runtime_error( "Invalid color indices for merging" );
    }

    // Merge a pair using bitwise OR on the color set.
    Color color;
    color.elements = colors_[ index_1 ].elements | colors_[ index_2 ].elements;
    color.type = Color::Type::kSecondary;
    return push_back_color( std::move( color ));
}

// size_t replace_color( size_t index, Color&& color )
// {
//     assert( index < colors_.size() );
//     assert( color.elements.size() == primary_color_count_ );
//
//     // Add the color to the list and the lookup
//     auto const hash = utils::bitvector_hash( color.elements );
//     colors_.emplace_back( std::move( color ));
//     lookup_.insert({ hash, colors_.size() - 1 });
//     return index;
// }

} // namespace sequence
} // namespace genesis

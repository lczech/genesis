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
    assert( element_count_ != 0 );

    // Add the empty color as the first element. This is so that a color index of 0,
    // which is what we get by default in an uninitialized setting, does not trick us
    // into believing it's a real color.
    // Hence, internally, we need to offset all actual primary colors by 1.
    Color empty;
    empty.elements = Bitvector( element_count_ );
    add_real_color( std::move( empty ));
    assert( colors_.size() == 1 );

    // Now add entries for all primary colors, with bitvectors that have
    // the bit set that corresponds to the color's index.
    for( size_t i = 0; i < element_count_; ++i ) {
        Color color;
        color.elements = Bitvector( element_count_ );
        color.elements.set( i );
        add_real_color( std::move( color ));
    }
    assert( colors_.size() == 1 + element_count_ );
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
                merge_and_add_real_colors( i, i+1 );
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
                merge_and_add_real_colors( i, leftover_index );
                leftover_index = 0;
            }
        }

        // Recursively merge the newly created block.
        recursive_merge_( new_start, colors.size() );
    };

    // Start the recursive merging using the entire initial range of primary colors.
    recursive_merge_( 1, colors_.size() );

    // At the end, there should be no leftover color; all should be in some secondary color.
    // Due to the exit condition of the recursive function, the leftover is set to the last
    // element that has all colors set (which is hence not really a leftover).
    assert( leftover_index == colors_.size() - 1 );

    // A binary tree has one fewer inner nodes than tips. We have added those as colors,
    // and have the empty color, so our total is now double the number of primary colors.
    // Also, the last secondary color should contain _all_ primary colors.
    assert( colors_.size() == 2 * element_count_ );
    assert( all_set( colors_.back().elements ));
    check_max_color_count_();
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
        if( bv.size() != element_count_ ) {
            throw std::invalid_argument(
                "Cannot initialize Kmer Color Set with Bitvectors of size "
                "that does not match the element count"
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
        if( find_real_color( color.elements ) > 0 ) {
            throw std::invalid_argument(
                "Cannot initialize Kmer Color Set with Bitvectors containing duplicates"
            );
        }

        // All good; add the color to our list.
        add_real_color( std::move( color ));
    }

    // Final checks
    // if( ! found_all_set ) {
    //     throw std::invalid_argument(
    //         "Cannot initialize Kmer Color Set with Bitvectors "
    //         "that do not contain an all-set bitvector"
    //     );
    // }
    check_max_color_count_();
}

// -------------------------------------------------------------------------
//     init_secondary_colors_from_groups
// -------------------------------------------------------------------------

void KmerColorSet::init_secondary_colors_from_groups(
    std::vector<std::vector<size_t>> const& groups
) {
    // Quick and dirty: create bitvectors from the indices, then forward to the bitvector function.
    // We are currently not expecting more than a few hundred or thousand initial colors, so this
    // should be fine for initialization. If extended to more colors, might need optimization.
    std::vector<Bitvector> bitvecs;
    for( auto const& group_indices : groups ) {
        bitvecs.emplace_back( Bitvector( element_count_, group_indices ));
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
    if( colors_.size() != 1 + element_count_ ) {
        throw std::runtime_error(
            "Primary color count is off or secondary colors have already been initialized"
        );
    }
}

// -------------------------------------------------------------------------
//     check_max_color_count_
// -------------------------------------------------------------------------

void KmerColorSet::check_max_color_count_() const
{
    // If we have a max color count set, but it is already exhausted after initialization,
    // we have reached an invalid state, as the that means we have valid secondary colors
    // that are beyond the max, and so they are unusuable. Guard against that.
    if( max_color_count_ == 0 ) {
        return;
    }
    if( colors_.size() > max_color_count_ ) {
        throw std::invalid_argument(
            "Cannot initialize Kmer Color Set with more secondary colors than the max color count"
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
    // Below, we unfortunately need to allocate a temporary bitvector for lookup up
    // if that one already exists in our colors, even if we do not update anything.
    // But at least we can avoid re-allocation and re-creating of this throughout
    // the calls of lookup_ and update_ below.
    Bitvector target_elements;
    size_t target_hash;

    // First see if we can find a fitting color. Either the existing one indexed here
    // already contains the target element, or there is another color already that is
    // the exact match of the union of the existing one and the new target index.
    auto const retrieved_index = lookup_(
        existing_color_index, target_element_index, target_elements, target_hash
    );
    if( retrieved_index > 0 ) {
        assert( retrieved_index < colors_.size() );
        return retrieved_index;
    }

    // If there is no existing fitting color, we have to add a new one, or, if we are out
    // of colors, find a minimally fitting superset, and make an imaginary one for it.
    return update_(
        existing_color_index, target_element_index, target_elements, target_hash
    );
}

// -------------------------------------------------------------------------
//     lookup_
// -------------------------------------------------------------------------

size_t KmerColorSet::lookup_(
    size_t existing_color_index,
    size_t target_element_index,
    Bitvector& target_elements,
    size_t& target_hash
) const {
    // Sanity checks.
    if( existing_color_index >= colors_.size() ) {
        throw std::invalid_argument(
            "Invalid color index " + std::to_string( existing_color_index )
        );
    }
    if( target_element_index >= element_count_ ) {
        throw std::invalid_argument(
            "Invalid element index " + std::to_string( target_element_index )
        );
    }
    assert( 1 + element_count_ <= colors_.size() );
    assert( 1 + target_element_index <= colors_.size() );
    assert( colors_[existing_color_index].elements.size() == element_count_ );

    // Special case for speed: If the existing color is the empty color, that means that
    // the existing entry is a default empty entry that does not yet have any color.
    // In that case, we can just return the index where the added primary color resides.
    // The other code would also work in this case, but take more time to get there.
    if( existing_color_index == 0 ) {
        // Assert that the element bitvector is indeed set for the target index.
        assert( colors_[ 1 + target_element_index ].elements.get( target_element_index ));
        ++stats_.existing_color_is_empty;
        return 1 + target_element_index;
    }

    // Check if the given color already comprises the primary color element.
    // If so, we do not need to do anything here, and the color index is the same as given.
    auto const& existing_color = colors_[existing_color_index];
    if( existing_color.elements.get( target_element_index )) {
        ++stats_.existing_color_contains_target;
        return existing_color;
    }

    // Here, we are in the case where the existing color does not already contain
    // the newly added element, so we need to make a new bitvector of their union.
    // We use a buffer at call site to avoid re-computing the bitvector and its hash.
    if( target_elements.empty() ) {
        target_elements = existing_color.elements;
        target_elements.set( target_element_index );
        target_hash = utils::bitvector_hash( target_elements );
    } else {
        // Expensive asserts...
        assert( target_elements.size() == element_count_ );
        assert( target_elements.get( target_element_index ));
        assert( target_elements | existing_color.elements == target_elements );
        assert( target_hash == utils::bitvector_hash( target_elements ));
    }

    // Check if an entry with those elements already exists. If so, this is our result.
    auto const real_index = find_real_color_( target_elements, target_hash );
    assert( real_index < colors_.size() );
    if( real_index > 0 ) {
        ++stats_.matching_real_color_exists;
        return real_index;
    }

    // If not in the real colors, check the imaginary ones as well.
    auto const imaginary_index = find_imaginary_color_( target_elements, target_hash );
    assert( imaginary_index < colors_.size() );
    if( imaginary_index > 0 ) {
        ++stats_.imaginary_color_exists;
        return imaginary_index;
    }

    // Otherwise, indicate that we did not find a match in the existing colors by returning
    // the empty color index 0. Also, some bookkeeping for the stats.
    return 0;
}

// -------------------------------------------------------------------------
//     update_
// -------------------------------------------------------------------------

size_t KmerColorSet::update_(
    size_t existing_color_index,
    size_t target_element_index,
    Bitvector& target_elements,
    size_t& target_hash
) {
    // If the new color is not in our list yet, this is a yet unseen secondary color.
    // We need to add it to our color set, either as a new secondary color, or,
    // if we are out of space for those, as an imaginary color.

    // TODO Decommissioning unused colors.
    // Below, whenever we have found the index we are looking for, we might not need
    // the existing color any more, as we are updating it with a new color. If its occurrence
    // goes down to zero, we can instead replace it with the new one in the list, remove
    // from the lookup, and add a new lookup entry instead. Need to make sure that we are not
    // removing primary entries (single bit set), but initializing them with a count of 1
    // should do the trick, as that way, it will never go down to zero.
    // if( existing_color.occurrence == 1 ) {
    //     // replace the entry in the list, remove the hash, add the new hash
    // } else {
    //     // decrement the occurrence
    // }

    // Check if we have already exhausted our supply of secondary colors.
    // If not, we add the new target color as a secondary color.
    if( max_color_count_ == 0 || colors_.size() < max_color_count_ ) {
        // Add the color and get its index in the list.
        auto const added_index = add_real_color( std::move( target_elements ));

        // We are adding it as a secondary color, so its index is idempotent. Assert that.
        assert( colors_.size() == added_index + 1 );
        assert( colors_.size() <= max_color_count_ );
        assert( colors_[added_index].index == added_index );
        assert( max_color_count_ == 0 || added_index < max_color_count_ );
        ++stats_.real_color_added;
        return added_index;
    }

    // Here we are in the case that we want to add the new color, but do not have space
    // for secondary colors any more, so we add it as an imaginary one instead.
    // First, we find the minimal superset in the secondary colors that includes our target color.
    auto const real_index = find_minimal_superset( target_elements.elements );

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
    assert( max_color_count_ > 0 );
    assert( real_index > 0 );
    assert( real_index < max_color_count_ );
    assert( real_index < colors_.size() );

    // Now we add the color as an imaginary one that points to our secondary color.
    target_elements.type = Color::Type::kImaginary;
    target_elements.index = real_index;
    auto const added_index = add_real_color( std::move( target_elements ));

    // We are adding it as an imaginary color, so its index is not idempotent. Assert that.
    // We then return the pointed-to secondary color.
    assert( colors_.size() == added_index + 1 );
    assert( colors_[added_index].index <  added_index );
    assert( colors_[added_index].index == real_index );
    assert( colors_[added_index].index < max_color_count_ );
    ++stats_.imaginary_color_added;
    return real_index;
}

// -------------------------------------------------------------------------
//     find_real_color
// -------------------------------------------------------------------------

size_t KmerColorSet::find_real_color( utils::Bitvector const& target ) const
{
    // Sanity check. Internally not needed, but added for external usage.
    if( target.size() != element_count_ ) {
        throw std::invalid_argument(
            "Invalid target bitvecor with different number of bits than the number of primary colors"
        );
    }

    // Same with the hash, only need to compute this for the external interface here.
    auto const hash = utils::bitvector_hash( target );
    return find_real_color_( target, hash );
}

// -------------------------------------------------------------------------
//     find_imaginary_color
// -------------------------------------------------------------------------

size_t KmerColorSet::find_imaginary_color( utils::Bitvector const& target ) const
{
    // Same as above for the real colors.
    if( target.size() != element_count_ ) {
        throw std::invalid_argument(
            "Invalid target bitvecor with different number of bits than the number of primary colors"
        );
    }
    auto const hash = utils::bitvector_hash( target );
    return find_imaginary_color_( target, hash );
}

// -------------------------------------------------------------------------
//     find_real_color_
// -------------------------------------------------------------------------

size_t KmerColorSet::find_real_color_( utils::Bitvector const& target, size_t hash ) const
{
    // Sanity checks.
    assert( target.size() == element_count_ );
    assert( hash == utils::bitvector_hash( target ));
    assert( real_colors_.size() == real_lookup_.size() );

    // There might be more than one color with the same hash, hence we need to
    // loop over the range to access all elements with the given hash key.
    auto const real_range = real_lookup_.equal_range( hash );
    for( auto it = real_range.first; it != real_range.second; ++it ) {
        assert( it->first == hash );

        // Check if the given bitvector matches the one pointed to by this hash.
        // If so, we have found our match, and return its color index.
        if( real_colors_[it->second].elements == target ) {
            return it->second;
        }
    }

    // We did not find the entry, meaning there is no color with this hash.
    // Return the empty color index instead.
    return 0;
}

// -------------------------------------------------------------------------
//     find_imaginary_color_
// -------------------------------------------------------------------------

size_t KmerColorSet::find_imaginary_color_( utils::Bitvector const& target, size_t hash ) const
{
    // Sanity checks.
    assert( target.size() == element_count_ );
    assert( hash == utils::bitvector_hash( target ));
    assert( real_colors_.size() == real_lookup_.size() );

    // This is only reasonable if the color list is full (reached max_color_count_), as otherwise,
    // the target color would not have been added as an imaginary one anyway yet.
    if( max_color_count_ == 0 || real_colors_.size() < max_color_count_ ) {
        assert( imaginary_lookup_.empty() );
        return 0;
    }

    // Similar principle to the above find_real_color, but with the imaginary color hash map.
    // The map is structured a bit differently for efficiency, but otherwise the same.
    auto const imaginary_range = imaginary_lookup_.equal_range( hash );
    for( auto it = imaginary_range.first; it != imaginary_range.second; ++it ) {
        assert( it->first == hash );
        if( it->second.elements == target ) {
            return it->second.index;
        }
    }

    // We did not find the entry, meaning there is no color with this hash. Return the empty color.
    return 0;
}

// -------------------------------------------------------------------------
//     find_minimal_superset
// -------------------------------------------------------------------------

size_t KmerColorSet::find_minimal_superset( utils::Bitvector const& target ) const
{
    // Sanity check. Internally not needed, but added for external usage.
    if( target.size() != element_count_ ) {
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
    // assert( max_color_count_ > 0 );
    // assert( max_color_count_ < colors_.size() );

    // Init our trackers. We have at most all bits of all primary colors set,
    // meaning that we start the search with one more, so that the min can find it.
    size_t min_index = 0;
    size_t min_pop_count = element_count_ + 1;

    // We are searching for a strict superset that has minimal pop count.
    // We cannot be better than having exactly one element more set than the target,
    // so we can use that as an early stop criterion.
    size_t const target_pop_count = utils::pop_count( target );

    // Upper bound of the search in the typical use case is the end of the secondary colors.
    // We however allow this function to be called before all slots for secondary colors
    // are used up, in which case we need to stop whenever we reach the end of the colors.
    size_t const max_index = std::min( colors_.size(), max_color_count_ );

    // TODO Add an optional early stop condition here that only searches through the initial set
    // of secondary colors created with the init functions, in case that this here is too slow.
    // This would give suboptimal results, but speed up the color set construction if needed.

    // Find the superset of our target in the secondary colors that has the smallest pop count,
    // i.e., that comprises the smallest number of primary colors.
    // Here, we unfortunately need to iterate all colors, which is slow and might need
    // optimization or a different approach in the future. Let's see if the imaginary colors
    // saturate fast enough so that we do not need to search  for a new one all too often.
    for( size_t i = 1 + element_count_; i < max_index; ++i ) {
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
    assert( min_index == 0 || min_pop_count <= element_count_ );
    assert( min_index == 0 || min_pop_count > utils::pop_count( target ));
    assert( min_index == 0 || colors_[min_index].elements != target );
    assert( min_index == 0 || is_strict_superset( colors_[min_index].elements, target ));
    return min_index;
}

// ================================================================================================
//     Modification
// ================================================================================================

// -------------------------------------------------------------------------
//     merge_and_add_real_colors
// -------------------------------------------------------------------------

size_t KmerColorSet::merge_and_add_real_colors( size_t index_1, size_t index_2 )
{
    // Helper function that takes two colors, merges them, and adds them to the list.
    // First check that the colors are valid entries.
    if( index_1 == 0 || index_1 >= colors_.size() || index_2 == 0 || index_2 >= colors_.size() ) {
        throw std::runtime_error( "Invalid color indices for merging" );
    }

    // Merge a pair using bitwise OR on the color set.
    Color color;
    color.elements = colors_[ index_1 ].elements | colors_[ index_2 ].elements;
    return add_real_color( std::move( color ));
}

// -------------------------------------------------------------------------
//     add_real_color
// -------------------------------------------------------------------------

size_t KmerColorSet::add_real_color( KmerColorSet::Color&& color )
{
    // External call, so check the inputs.
    if( color.elements.size() != element_count_ ) {
        throw std::invalid_argument(
            "Cannot add color with bitvector of size that does not match the element count"
        );
    }
    if( utils::pop_count( color.elements ) < 2 ) {
        throw std::invalid_argument(
            "Cannot add color with bitvector representing the empty color or primary colors "
            "(i.e., zero or single bit set)"
        );
    }

    // Use the interal function to perform the actual work.
    auto const hash = utils::bitvector_hash( color.elements );
    return add_real_color_( std::move( color ), hash );
}

// -------------------------------------------------------------------------
//     add_real_color_
// -------------------------------------------------------------------------

size_t KmerColorSet::add_real_color_( Color&& color, size_t hash )
{
    // Sanity checks.
    assert( real_colors_.size() == real_lookup_.size() );
    if( max_color_count_ > 0 && real_colors_.size() >= max_color_count_ ) {
        throw std::runtime_error(
            "Cannot add additional real color, as max color count is already reached"
        );
    }
    assert( max_color_count_ <= real_colors_.size() );

    // Check our assumptions about the color.
    // In the init, we explicitly check for duplicates. During our internal executing however,
    // we assume that our algorithm never wants to add a color that already exists. Assert that.
    assert( color.elements.size() == element_count_ );
    assert( hash == utils::bitvector_hash( color.elements ));
    assert( find_real_color_( color.elements, hash ) == 0 );
    assert( find_imaginary_color_( color.elements, hash ) == 0 );

    // Get the index at which the color will be placed in the list,
    // which is simply the current size of the list.
    size_t const index = real_colors_.size();

    // Add the color to the list and the lookup.
    real_colors_.emplace_back( std::move( color ));
    real_lookup_.insert({ hash, index });
    assert( real_colors_.size() == real_lookup_.size() );
    assert( real_colors_.size() == index + 1 );

    // We here return the real index in the list, not the one that the color points to,
    // and leave the decision about that to the caller.
    return index;
}

// -------------------------------------------------------------------------
//     add_imaginary_color_
// -------------------------------------------------------------------------

size_t KmerColorSet::add_imaginary_color_( Color&& color, size_t hash, size_t real_color_index )
{
    if( max_color_count_ == 0 || real_colors_.size() < max_color_count_ ) {
        throw std::runtime_error(
            "Cannot add imaginary color, as max color count is not yet reached"
        );
    }

    // Check our assumptions about the color.
    assert( color.elements.size() == element_count_ );
    assert( hash == utils::bitvector_hash( color.elements ));
    assert( find_real_color_( color.elements, hash ) == 0 );
    assert( find_imaginary_color_( color.elements, hash ) == 0 );

    // ...
}

} // namespace sequence
} // namespace genesis

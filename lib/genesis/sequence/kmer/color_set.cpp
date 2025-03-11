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
//     Lookup & Modification
// ================================================================================================

// -------------------------------------------------------------------------
//     add_color
// -------------------------------------------------------------------------

size_t KmerColorSet::add_color( utils::Bitvector&& elements )
{
    // Only add new colors if we have not yet saturated the amount of colors.
    if( ! gamut_.empty() ) {
        assert( max_color_count_ > 0 );
        assert( colors_.size() == max_color_count_ );
        throw std::invalid_argument(
            "Cannot add color once the max number of colors is reached"
        );
    }
    assert( max_color_count_ == 0 || colors_.size() < max_color_count_ );

    // External call, so check the input validity.
    if( elements.size() != element_count_ ) {
        throw std::invalid_argument(
            "Cannot add color with bitvector of size that does not match the element count"
        );
    }
    if( utils::pop_count( elements ) < 2 ) {
        throw std::invalid_argument(
            "Cannot add color with bitvector representing the empty color or primary colors "
            "(i.e., zero or single bit set)"
        );
    }

    // Check if the color already exists.
    auto const hash = utils::bitvector_hash( elements );
    if( find_existing_color_( elements, hash ) > 0 ) {
        throw std::invalid_argument( "Cannot add duplicate color" );
    }

    // Use the internal function to perform the actual work.
    return add_color_( std::move( elements ), hash );
}

// -------------------------------------------------------------------------
//     add_merged_color
// -------------------------------------------------------------------------

size_t KmerColorSet::add_merged_color( size_t index_1, size_t index_2 )
{
    // Helper function that takes two colors, merges them, and adds them to the list.
    // First check that the colors are valid entries.
    if( index_1 == 0 || index_1 >= colors_.size() || index_2 == 0 || index_2 >= colors_.size() ) {
        throw std::runtime_error( "Invalid color indices for merging" );
    }

    // Merge a pair using bitwise OR on the color set.
    // We call the above user-facing function, as that does some additional checks for us.
    // We are not expecting this function here to be called after initialization anyway,
    // so being a bit more thorough here is better, and doesn't hurt performance much.
    return add_color( colors_[ index_1 ].elements | colors_[ index_2 ].elements );
}

// -------------------------------------------------------------------------
//     find_matching_color
// -------------------------------------------------------------------------

size_t KmerColorSet::find_matching_color(
    size_t existing_color_index,
    size_t target_element_index
) const {
    // The fully parameterized overload of this function takes a buffer,
    // which we here provide locally for callers that do not need a buffer,
    // i.e., in cases where the target bitvector and hash are not re-used anyway.
    Bitvector target_elements;
    size_t    target_hash;
    return find_matching_color_(
        existing_color_index, target_element_index, target_elements, target_hash
    );
}

// -------------------------------------------------------------------------
//     find_existing_color
// -------------------------------------------------------------------------

size_t KmerColorSet::find_existing_color( utils::Bitvector const& target ) const
{
    auto const hash = utils::bitvector_hash( target );
    return find_existing_color_( target, hash );
}

// -------------------------------------------------------------------------
//     get_joined_color_index
// -------------------------------------------------------------------------

size_t KmerColorSet::get_joined_color_index(
    size_t existing_color_index,
    size_t target_element_index
) {
    // Sanity checks of the user input.
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

    // First check if we have saturated our colors already.
    // If so, we can just  return the entry from the gamut.
    if( ! gamut_.empty() ) {
        assert( colors_.size() == max_color_count_ );
        assert( gamut_.rows() == colors_.size() );
        assert( gamut_.cols() == element_count_ );
        return get_gamut_entry_( existing_color_index, target_element_index );
    }

    // If not, we are still in the phase of building up our colors.
    // We need to perform a lookup, return that, or if not found, add a new color.

    // Below, we need to allocate a temporary bitvector for looking up
    // if that one already exists in our colors, even if we do not update anything.
    // But at least we can avoid re-allocation and re-creating of this throughout here.
    Bitvector target_elements;
    size_t    target_hash;

    // First see if we can find a fitting color. Either the existing one indexed here
    // already contains the target element, or there is another color already that is
    // the exact match of the union of the existing one and the new target index.
    auto const matching_index = find_matching_color_(
        existing_color_index, target_element_index, target_elements, target_hash
    );
    if( matching_index > 0 ) {
        assert( matching_index < colors_.size() );
        return matching_index;
    }

    // If the new color is not in our list yet, this is a yet unseen secondary color.
    // We need to add it to our color set, either as a new secondary color, or,
    // if we are out of space for those, start the gamut, and add it as an imaginary color.

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

    // Check if we have already saturated our supply of secondary colors.
    // If not, we add the new target color as a secondary color.
    if( max_color_count_ == 0 || colors_.size() < max_color_count_ ) {
        // Add the color and return its index in the list.
        auto const added_index = add_color_( std::move( target_elements ), target_hash );
        assert( colors_.size() == added_index + 1 );
        assert( colors_.size() <= max_color_count_ );
        assert( max_color_count_ == 0 || added_index < max_color_count_ );
        return added_index;
    }

    // Otherwise, if we have saturated the colors, we instead switch to the gamut
    // of minimally fitting supersets, and use imaginary colors going forward.
    init_gamut_();
    return get_gamut_entry_( existing_color_index, target_element_index );
}

// ================================================================================================
//     Internal Members
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
    auto empty = Bitvector( element_count_ );
    auto const empty_hash = utils::bitvector_hash( empty );
    add_color_( std::move( empty ), empty_hash );
    assert( colors_.size() == 1 );

    // Now add entries for all primary colors, with bitvectors that have
    // the bit set that corresponds to the color's index.
    for( size_t i = 0; i < element_count_; ++i ) {
        auto elements = Bitvector( element_count_ );
        elements.set( i );
        auto const hash = utils::bitvector_hash( elements );
        add_color_( std::move( elements ), hash );
    }
    assert( colors_.size() == 1 + element_count_ );
}

// -------------------------------------------------------------------------
//     find_matching_color_
// -------------------------------------------------------------------------

size_t KmerColorSet::find_matching_color_(
    size_t     existing_color_index,
    size_t     target_element_index,
    Bitvector& target_elements,
    size_t&    target_hash
) const {
    assert( existing_color_index < colors_.size() );
    assert( target_element_index < element_count_ );

    // Special case for speed: If the existing color is the empty color, that means that
    // the existing entry is a default empty entry that does not yet have any color.
    // In that case, we can just return the index where the added primary color resides.
    // The other code would also work in this case, but take more time to get there.
    if( existing_color_index == 0 ) {
        // Assert that the element bitvector is indeed set for the target index.
        assert( colors_[ 1 + target_element_index ].elements.get( target_element_index ));
        return 1 + target_element_index;
    }

    // Check if the given color already comprises the primary color target element.
    // If so, the color is idempotent for the target element; we do not need to do anything here.
    // The color index is the same as given. We could also skip this whole check here, and just use
    // the below code that sets the element bit and checks if that results in an existing color.
    // But here, checking one individual bit is faster than creating a whole new bitvector.
    auto const& existing_color = colors_[existing_color_index];
    if( existing_color.elements.get( target_element_index )) {
        return existing_color_index;
    }

    // Here, we are in the case where the existing color does not already contain
    // the newly added element, so we need to make a new bitvector of their union.
    // We use a buffer at call site to avoid re-computing the bitvector and its hash.
    if( target_elements.empty() ) {
        target_elements = existing_color.elements;
        target_elements.set( target_element_index );
        target_hash = utils::bitvector_hash( target_elements );
    } else {
        // We have already computed the target bitvector and its hash.
        // Let's assert that they are correct (the last two should be deactivated later).
        assert( target_elements.size() == element_count_ );
        assert(( target_elements | existing_color.elements ) == target_elements );
        assert( target_elements.get( target_element_index ));
        assert( target_hash == utils::bitvector_hash( target_elements ));
    }

    // Check if an entry with those elements already exists, and return its index. If there is
    // no such color, the locate function returns 0, which is then also our result here.
    size_t const target_index = find_existing_color_( target_elements, target_hash );
    assert( target_index < colors_.size() );
    return target_index;
}

// -------------------------------------------------------------------------
//     find_existing_color_
// -------------------------------------------------------------------------

size_t KmerColorSet::find_existing_color_( utils::Bitvector const& target, size_t hash ) const
{
    // Sanity checks.
    assert( target.size() == element_count_ );
    assert( hash == utils::bitvector_hash( target ));
    assert( colors_.size() == lookup_.size() );

    // There might be more than one color with the same hash, hence we need to
    // loop over the range to access all elements with the given hash key.
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
//     add_color_
// -------------------------------------------------------------------------

size_t KmerColorSet::add_color_( utils::Bitvector&& elements, size_t hash )
{
    // Sanity checks.
    assert( colors_.size() == lookup_.size() );
    if( max_color_count_ > 0 && colors_.size() >= max_color_count_ ) {
        throw std::runtime_error(
            "Cannot add additional real color, as max color count is already reached"
        );
    }
    assert( max_color_count_ == 0 || colors_.size() < max_color_count_ );
    assert( gamut_.empty() );

    // Check our assumptions about the color.
    // In the init, we explicitly check for duplicates. During our internal executing however,
    // we assume that our algorithm never wants to add a color that already exists. Assert that.
    assert( elements.size() == element_count_ );
    assert( hash == utils::bitvector_hash( elements ));
    assert( find_existing_color_( elements, hash ) == 0 );

    // Get the index at which the color will be placed in the list,
    // which is simply the current size of the list.
    size_t const index = colors_.size();

    // Add the color to the list and the lookup.
    Color color;
    color.elements = std::move( elements );
    colors_.emplace_back( std::move( color ));
    lookup_.insert({ hash, index });
    assert( colors_.size() == lookup_.size() );
    assert( colors_.size() == index + 1 );

    // We here return the real index in the list, not the one that the color points to,
    // and leave the decision about that to the caller.
    return index;
}

// -------------------------------------------------------------------------
//     init_gamut_
// -------------------------------------------------------------------------

void KmerColorSet::init_gamut_()
{
    // Only call this if we have saturated our color list.
    if( max_color_count_ == 0 || colors_.size() != max_color_count_ ) {
        throw std::runtime_error( "Invalid call of init_gamut_() before colors are saturated" );
    }
    if( ! gamut_.empty() ) {
        throw std::runtime_error( "Invalid call of init_gamut_() with already set up matrix" );
    }

    // Report to the user that we have saturated the list of colors, and are creating the gamut.
    if( on_saturation_callback_ ) {
        on_saturation_callback_();
    }

    // For each color, we create a row where the columns correspond to each of the elements being set.
    // Wherever the original color (of the row) already has the bit set anyway, the color is
    // idempotent. Otherwise, it either points to another existing color that has exactly that bit
    // set in addition to our original color, or, if that does not exist in the color list,
    // we instead use the minimal (w.r.t. pop count) superset color instead as an imaginary color.
    // We only compute these on-demand, as likely not all of them will be needed.
    gamut_ = utils::Matrix<size_t>( colors_.size(), element_count_ );

    // Potential parallel implementation to pre-compute the gamut matrix.
    // However, in the current design, this will likely deadlock in our intended use case,
    // as the outside kmer iteration will already occupy all threads in the pool, and our
    // internal locking here will not let any of them make progress. Hence, the table would
    // only be computed by the single thread that was the unlucky one that called this function.
    // Not efficient, and right now, no easy solution in sight without yet another re-design.
    // But lucky for us, the on-demand lookup might be the better choice anyway.
    // utils::parallel_for(
    //     0, colors_.size(),
    //     [this]( size_t color_index )
    //     {
    //         for( size_t elem_index = 0; elem_index < element_count_; ++elem_index ) {
    //             gamut_( color_index, elem_index ) = get_gamut_entry_( color_index, elem_index );
    //         }
    //     }
    // ).get();
}

// -------------------------------------------------------------------------
//     get_gamut_entry_
// -------------------------------------------------------------------------

size_t KmerColorSet::get_gamut_entry_(
    size_t existing_color_index,
    size_t target_element_index
) {
    // The usual sanity checks.
    assert( existing_color_index < colors_.size() );
    assert( target_element_index < element_count_ );
    assert( ! gamut_.empty() );

    // If the entry is already in the gamut, we just return that.
    auto const gamut_entry = gamut_( existing_color_index, target_element_index );
    assert( gamut_entry < colors_.size() );
    if( gamut_entry > 0 ) {
        return gamut_entry;
    }

    // Otherwise, we need to compute the entry first, which means we try to find a matching color...
    Bitvector target_elements;
    size_t    target_hash;
    auto const matching_index = find_matching_color_(
        existing_color_index,
        target_element_index,
        target_elements,
        target_hash
    );
    assert( matching_index < colors_.size() );
    if( matching_index > 0 ) {
        gamut_( existing_color_index, target_element_index ) = matching_index;
        ++gamut_stats_.real_color_count;
        return matching_index;
    }

    // ...or, if none exists, find the imaginary color representing the minimal fitting superset.
    // The target_elements has been set to our desired bitvector by the above call already.
    assert( ! target_elements.empty() );
    auto const superset_index = find_minimal_superset_( target_elements );

    // If we have not found any matching superset, that means that our secondary colors
    // are missing a proper candidate, and should have been initialized differently,
    // for instance by including some reduction or clustering of the elements.
    if( superset_index == 0 ) {
        throw std::runtime_error(
            "Could not find minimal superset of the target color in the secondary colors. "
            "This likely means the secondary colors were not initialized with a proper "
            "reduction or clustering of all elements, and hence do not contain a match "
            "for all needed colors. Consider initializing the secondary colors differently."
        );
    }

    // Finally, update the gamut with the new imaginary color entry
    gamut_( existing_color_index, target_element_index ) = superset_index;
    ++gamut_stats_.imag_color_count;
    return superset_index;
}

// -------------------------------------------------------------------------
//     find_minimal_superset_
// -------------------------------------------------------------------------

size_t KmerColorSet::find_minimal_superset_( utils::Bitvector const& target ) const
{
    // Sanity check. Internally not needed, but added for external usage.
    if( target.size() != element_count_ ) {
        throw std::invalid_argument(
            "Invalid target bitvecor with different number of bits than the number of primary colors"
        );
    }

    // Init our trackers. We have at most all bits of all primary colors set,
    // meaning that we start the search with one more, so that the min can find it.
    size_t min_index = 0;
    size_t min_pop_count = element_count_ + 1;

    // We are searching for a strict superset that has minimal pop count.
    // We cannot be better than having exactly one element more set than the target,
    // so we can use that as an early stop criterion.
    size_t const target_pop_count = utils::pop_count( target );

    // TODO Add an optional early stop condition here that only searches through the initial set
    // of secondary colors created with the add functions, in case that this here is too slow.
    // This would give suboptimal results, but speed up the gamut construction if needed.

    // Find the superset of our target in the secondary colors that has the smallest pop count,
    // i.e., that comprises the smallest number of primary colors. Here, we unfortunately need
    // to iterate all colors, which is slow - hence, the frozen table caches these for re-use.
    for( size_t i = 1 + element_count_; i < colors_.size(); ++i ) {
        // If the color is not a superset of what we need, we skip it.
        if( ! utils::is_superset( colors_[i].elements, target )) {
            continue;
        }

        // See if this is a new minimal hit; if so, update it.
        auto const popcnt = utils::pop_count( colors_[i].elements );
        if( popcnt < min_pop_count ) {
            min_index = i;
            min_pop_count = popcnt;
        }

        // If it is as minimal as we can get, we can stop here already.
        if( popcnt <= target_pop_count + 1 ) {
            break;
        }
    }

    // If we have not found any matching superset, that means that our secondary colors
    // are missing a proper candidate, and should have been initialized differently,
    // for instance by including some reduction or clustering of the elements.
    // Otherwise, we have found a secondary color that is a superset of our target.
    // We leave it up to the caller though to decide in case of no fitting color.
    assert( min_index == 0 || min_pop_count <= element_count_ );
    assert( min_index == 0 || min_pop_count >= utils::pop_count( target ));
    assert( min_index == 0 || colors_[min_index].elements != target );
    assert( min_index == 0 || is_strict_superset( colors_[min_index].elements, target ));
    return min_index;
}

} // namespace sequence
} // namespace genesis

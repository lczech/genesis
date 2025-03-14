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

#include "genesis/sequence/kmer/color_gamut.hpp"
#include "genesis/utils/core/logging.hpp"

// The KmerColorGamut class is only available from C++17 onwards.
#if GENESIS_CPP_STD >= GENESIS_CPP_STD_17

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <chrono>
#include <thread>

namespace genesis {
namespace sequence {

// ================================================================================================
//     Lookup & Modification
// ================================================================================================

// -------------------------------------------------------------------------
//     add_color
// -------------------------------------------------------------------------

size_t KmerColorGamut::add_color( utils::Bitvector&& elements )
{
    // Obtain write lock. Usually not needed here, as this function is meant to be called
    // before starting any concurrent access, but maybe there is a use case where the caller
    // has multiple threads filling in colors, and it does not hurt to have this here.
    std::unique_lock write_lock( color_mutex_ );

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

size_t KmerColorGamut::add_merged_color( size_t index_1, size_t index_2 )
{
    // Helper function that takes two colors, merges them, and adds them to the list.
    // First check that the colors are valid entries.
    if(
        index_1 == 0 || index_1 >= colors_.size() ||
        index_2 == 0 || index_2 >= colors_.size()
    ) {
        throw std::runtime_error( "Invalid color indices for merging" );
    }

    // Merge a pair using bitwise OR on the color gamut.
    // We call the above user-facing function, as that does some additional checks for us.
    // We are not expecting this function here to be called after initialization anyway,
    // so being a bit more thorough here is better, and doesn't hurt performance much.
    return add_color( colors_[ index_1 ].elements | colors_[ index_2 ].elements );
}

// -------------------------------------------------------------------------
//     find_existing_color
// -------------------------------------------------------------------------

size_t KmerColorGamut::find_existing_color( utils::Bitvector const& target_elements ) const
{
    std::shared_lock read_lock( color_mutex_ );
    if( target_elements.size() != element_count_ ) {
        throw std::invalid_argument(
            "Cannot find bitvector of size " + std::to_string( target_elements.size() ) +
            " in Color Gamut that has " + std::to_string( element_count_ ) + " elements"
        );
    }
    auto const target_hash = utils::bitvector_hash( target_elements );
    return find_existing_color_( target_elements, target_hash );
}

// -------------------------------------------------------------------------
//     find_minimal_superset
// -------------------------------------------------------------------------

size_t KmerColorGamut::find_minimal_superset( utils::Bitvector const& target_elements ) const
{
    std::shared_lock read_lock( color_mutex_ );
    if( target_elements.size() != element_count_ ) {
        throw std::invalid_argument(
            "Cannot find bitvector of size " + std::to_string( target_elements.size() ) +
            " in Color Gamut that has " + std::to_string( element_count_ ) + " elements"
        );
    }
    return find_minimal_superset_( target_elements );
}

// -------------------------------------------------------------------------
//     get_joined_color_index
// -------------------------------------------------------------------------

size_t KmerColorGamut::get_joined_color_index(
    size_t existing_color_index,
    size_t additive_element_index
) {
    {
        // Sanity checks of the user input, might need shared locking.
        std::shared_lock read_lock( color_mutex_, std::defer_lock );
        if( ! gamut_started_.load( std::memory_order_acquire )) {
            read_lock.lock();
        }
        if( existing_color_index >= colors_.size() ) {
            throw std::invalid_argument(
                "Invalid color index " + std::to_string( existing_color_index )
            );
        }
        if( additive_element_index >= element_count_ ) {
            throw std::invalid_argument(
                "Invalid element index " + std::to_string( additive_element_index )
            );
        }
    }

    // The below code is thread-safe (as far as I can tell...), but despite using read/write
    // shared/exclusive access locks, still has too much contention on the mutex.
    // Hence, we here signal that we might not need to take that path at all:
    // If we have already set up the gamut matrix, we can circumvent the locking entirely,
    // and just get our values from the gamut directly.
    if( gamut_started_.load( std::memory_order_acquire )) {
        assert( max_color_count_ > 0 );
        assert( colors_.size() == max_color_count_ );
        assert( gamut_.rows() == colors_.size() );
        assert( gamut_.cols() == element_count_ );
        return get_gamut_entry_( existing_color_index, additive_element_index );
    }

    // Below, we need to allocate a temporary bitvector for looking up
    // if that one already exists in our colors, even if we do not update anything.
    // But at least we can avoid re-allocation and re-creating of this throughout here.
    Bitvector target_elements;
    size_t    target_hash;

    // We here have a couple of read operations, potentially followed by write operations.
    // First, we check if the requested color already exists, either in the gamut,
    // or, if we are still in the phase of collecting colors, in there.
    // For that part, we hence need the shared lock, and afterwards, the unique lock.
    // We do this in a loop, because otherwise, the writer threads might starve,
    // if multiple of them arrive at the point where they want to add a color,
    // but while waiting for the exclusive lock, some other thread has already started
    // the gamut phase. In that case, all running threads will be so fast that they will
    // starve the "old" threads that still think we are in the color collecting phase
    // before the gamut. So, we give them a timeout, and let them recheck every now and then,
    // so that they can detect the beginning of the gamut phase, in which case they will
    // not need to write to the color list any more anyway.
    // There is probably a better design to do this, either for the mutexes here, or
    // a complete re-design of how colors are stored. For instance, instead of a vector,
    // they could be stored in a parallel hash map from color indices to bitvectors,
    // which could then operate concurrently. But right now, this here is good enough.
    size_t attempt = 1;
    while( true ) {
        // First, under shared locking, check if we have a matching entry already,
        // either in the color list, or in the gamut, if we have started with that yet.
        // If so, we are done already and can return, without needed exclusive locking.
        {
            std::shared_lock read_lock( color_mutex_ );
            auto const matching_index = get_joined_color_index_read_(
                existing_color_index, additive_element_index, target_elements, target_hash
            );
            if( matching_index > 0 ) {
                return matching_index;
            }
        }

        // If the new color is not in our list yet, this is a yet unseen secondary color.
        // We need to add it to our color gamut, either as a new secondary color, or,
        // if we are out of space for those, start the gamut, and add it as an imaginary color.
        // This has to happen with the exclusive write lock being held.
        // In order to avoid starvation here, we use a timed lock with an increasing time
        // depending on the attempts at locking. Starvation can happen if multiple threads
        // want to write the very last color before the list is full; in that case, other
        // threads might start going into read-only mode (above), which is fast enough such
        // that the writers starve here, despite all of them being obsolete anyway one the
        // colors are saturated... hence, we loop and check again if we even need to write.
        std::unique_lock write_lock( color_mutex_, std::defer_lock );
        if( write_lock.try_lock_for( std::chrono::milliseconds( attempt ))) {
            // We have a gap in locking between the above and this block. Maybe in the future,
            // we can use an upgradable lock here, but that on the other hand might be too slow.
            // But with the current design, we unfortunately need to re-do the above checks,
            // as the conditions might have changed in between the shared and the exclusive lock.
            auto const matching_index = get_joined_color_index_read_(
                existing_color_index, additive_element_index, target_elements, target_hash
            );
            if( matching_index > 0 ) {
                return matching_index;
            }

            // We hold the exclusive lock, and hence can write the new color, and return its index.
            return get_joined_color_index_write_(
                existing_color_index, additive_element_index, target_elements, target_hash
            );
        }

        // If we could not get the lock in time, yield and loop to check again.
        ++attempt;
        std::this_thread::yield();
    }
}

// -------------------------------------------------------------------------
//     precompute_gamut
// -------------------------------------------------------------------------

void KmerColorGamut::precompute_gamut( std::shared_ptr<utils::ThreadPool> thread_pool )
{
    // We can only init the gamut once there will be no more colors added.
    if( max_color_count_ == 0 || colors_.size() < max_color_count_ ) {
        throw std::invalid_argument(
            "Cannot precompute the gamut before the colors have been saturated"
        );
    }

    // Precompute the gamut - multi-threaded if possible. We can deactivate the internal locking
    // during the gamut computation here, as this loop is only accessing each cell once.
    init_gamut_();
    if( thread_pool ) {
        utils::parallel_for(
            0, colors_.size(),
            [this]( size_t color_index )
            {
                // Parallelize over colors, i.e., rows of the matrix, then compute the cells
                // along the columns in one thread. Should minimize false sharing issues.
                for( size_t element_index = 0; element_index < element_count_; ++element_index ) {
                    get_gamut_entry_( color_index, element_index, false );
                }
            },
            thread_pool
        ).get();
    } else {
        // Serial version of the above.
        for( size_t color_index = 0; color_index < colors_.size(); ++color_index ) {
            for( size_t element_index = 0; element_index < element_count_; ++element_index ) {
                get_gamut_entry_( color_index, element_index, false );
            }
        }
    }

    assert( ! gamut_.empty() );
    assert( gamut_filled_.load() );
}

// ================================================================================================
//     Internal Members
// ================================================================================================

// -------------------------------------------------------------------------
//     init_primary_colors
// -------------------------------------------------------------------------

void KmerColorGamut::init_primary_colors_()
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
//     get_joined_color_index_read_
// -------------------------------------------------------------------------

size_t KmerColorGamut::get_joined_color_index_read_(
    size_t     existing_color_index,
    size_t     additive_element_index,
    Bitvector& target_elements,
    size_t&    target_hash
) {
    // Check if we have saturated our colors already and have a gamut.
    // If so, we can just return the entry from there.
    if( ! gamut_.empty() ) {
        assert( max_color_count_ > 0 );
        assert( colors_.size() == max_color_count_ );
        assert( gamut_.rows() == colors_.size() );
        assert( gamut_.cols() == element_count_ );
        return get_gamut_entry_( existing_color_index, additive_element_index );
    }

    // If not, we are still in the phase of building up our colors.
    // We need to perform a lookup, return that, or if not found, add a new color.

    // First see if we can find a fitting color. Either the existing one indexed here
    // already contains the target element, or there is another color already that is
    // the exact match of the union of the existing one and the new target index.
    // If the search is unsuccessful, it returns 0, which is then also our return value here.
    auto const matching_index = find_matching_color_(
        existing_color_index, additive_element_index, target_elements, target_hash
    );
    assert( matching_index < colors_.size() );
    return matching_index;
}

// -------------------------------------------------------------------------
//     find_matching_color_write_
// -------------------------------------------------------------------------

size_t KmerColorGamut::get_joined_color_index_write_(
    size_t     existing_color_index,
    size_t     additive_element_index,
    Bitvector& target_elements,
    size_t&    target_hash
) {
    // TODO Decommissioning unused colors.
    // Below, whenever we have found the index we are looking for, we might not need
    // the existing color any more, as we are updating it with a new color. If its occurrence
    // goes down to zero, we can instead replace it with the new one in the list, remove
    // from the lookup, and add a new lookup entry instead. Need to make sure that we are not
    // removing primary entries (single bit set), but initializing them with a count of 1
    // should do the trick, as that way, it will never go down to zero.
    // Maybe a better stragegy is to consolidate colors every now and then (for instance,
    // via a callback whenever we are about to switch to gamut mode): go through a user-provided
    // iterable of data, and see which colors are actually in use. Then, remove all unused,
    // and provide a new update array for the user to replace all the colors with their new indices.
    // if( existing_color.occurrence == 1 ) {
    //     // replace the entry in the list, remove the hash, add the new hash
    // } else {
    //     // decrement the occurrence
    // }

    // Check if we have already saturated our supply of secondary colors.
    // If not, we add the new target color as a secondary color.
    if( max_color_count_ == 0 || colors_.size() < max_color_count_ ) {
        // Add the color and return its index in the list. The target_elements and their hash
        // has been populated above by the find_matching_color_ function. Still, for clarity,
        // we call the populate function here; it does nothing. Stupid? Or more clarity?
        populate_target_color_(
            existing_color_index, additive_element_index, target_elements, target_hash
        );
        auto const added_index = add_color_( std::move( target_elements ), target_hash );
        assert( colors_.size() == added_index + 1 );
        assert( colors_.size() <= max_color_count_ );
        assert( max_color_count_ == 0 || added_index < max_color_count_ );
        return added_index;
    }

    // Otherwise, if we have saturated the colors, we instead switch to the gamut
    // of minimally fitting supersets, and use imaginary colors going forward.
    init_gamut_();

    // If we are here, we have initialized the gamut, and now only need to return the entry.
    // We could do this outside of the lock, as the function does its own locking on top
    // of the read/write shared/unique locking used above. This is because the gamut is a matrix
    // of independent values, with no larger data strucuture or re-allocations needed once set up.
    // So we can do this more fine grained than blocking everything for a single cell write.
    // But in the current design, we do not do that, and just compute this first entry
    // while holding the lock. Should be fine.
    return get_gamut_entry_( existing_color_index, additive_element_index );
}

// -------------------------------------------------------------------------
//     find_matching_color_
// -------------------------------------------------------------------------

size_t KmerColorGamut::find_matching_color_(
    size_t     existing_color_index,
    size_t     additive_element_index,
    Bitvector& target_elements,
    size_t&    target_hash
) const {
    assert( existing_color_index < colors_.size() );
    assert( additive_element_index < element_count_ );

    // Special case for speed: If the existing color is the empty color, that means that
    // the existing entry is a default empty entry that does not yet have any color.
    // In that case, we can just return the index where the added primary color resides.
    // The other code would also work in this case, but take more time to get there.
    if( existing_color_index == 0 ) {
        // Assert that the element bitvector is indeed set for the target index.
        assert( colors_[ 1 + additive_element_index ].elements.get( additive_element_index ));
        return 1 + additive_element_index;
    }

    // Check if the given color already comprises the primary color target element.
    // If so, the color is idempotent for the target element; we do not need to do anything here.
    // The color index is the same as given. We could also skip this whole check here, and just use
    // the below code that sets the element bit and checks if that results in an existing color.
    // But here, checking one individual bit is faster than creating a whole new bitvector.
    auto const& existing_color = colors_[existing_color_index];
    if( existing_color.elements.get( additive_element_index )) {
        return existing_color_index;
    }

    // Here, we are in the case where the existing color does not already contain
    // the newly added element, so we need to make a new bitvector of their union.
    // We use a buffer at call site to avoid re-computing the bitvector and its hash.
    populate_target_color_(
        existing_color_index, additive_element_index, target_elements, target_hash
    );

    // Check if an entry with those elements already exists, and return its index. If there is
    // no such color, the find function returns 0, which is then also our result here.
    size_t const target_index = find_existing_color_( target_elements, target_hash );
    assert( target_index < colors_.size() );
    return target_index;
}

// -------------------------------------------------------------------------
//     populate_target_color_
// -------------------------------------------------------------------------

void KmerColorGamut::populate_target_color_(
    size_t     existing_color_index,
    size_t     additive_element_index,
    Bitvector& target_elements,
    size_t&    target_hash
) const {
    // Populate the target elements and their hash, but only if they are not already set.
    // This is mostly meant as a performance improvement, as we might not always need those,
    // but if we do, we need them multiple times, and can avoid recomputation.
    auto const& existing_color = colors_[existing_color_index];
    if( target_elements.empty() ) {
        target_elements = existing_color.elements;
        target_elements.set( additive_element_index );
        target_hash = utils::bitvector_hash( target_elements );
    } else {
        // We have already computed the target bitvector and its hash.
        // Let's assert that they are correct (the last two can be deactivated later).
        assert( target_elements.size() == element_count_ );
        assert(( target_elements | existing_color.elements ) == target_elements );
        assert( target_elements.get( additive_element_index ));
        assert( target_hash == utils::bitvector_hash( target_elements ));
    }
}

// -------------------------------------------------------------------------
//     find_existing_color_
// -------------------------------------------------------------------------

size_t KmerColorGamut::find_existing_color_( utils::Bitvector const& target, size_t hash ) const
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

size_t KmerColorGamut::add_color_( utils::Bitvector&& elements, size_t hash )
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
    return index;
}

// -------------------------------------------------------------------------
//     init_gamut_
// -------------------------------------------------------------------------

void KmerColorGamut::init_gamut_()
{
    // Only call this if we have saturated our color list.
    if( max_color_count_ == 0 || colors_.size() != max_color_count_ ) {
        throw std::runtime_error( "Invalid call of init_gamut_() before colors are saturated" );
    }
    if( ! gamut_.empty() ) {
        throw std::runtime_error( "Invalid call of init_gamut_() with already set up matrix" );
    }

    // Report to the user that we have saturated the list of colors, and are creating the gamut.
    if( on_gamut_start_callback_ ) {
        on_gamut_start_callback_();
    }

    // Set up the vector guard for accessing the gamut. We use the square root of the number
    // of total entries in the gamut matrix to get a large enough number of buckets for the
    // guards to avoid collision. Probably overkill, and super ad-hoc, but let's see if it works.
    gamut_guard_ = utils::ConcurrentVectorGuard( std::sqrt( colors_.size() * element_count_ ));

    // For each color, we create a row where the columns correspond to each of the elements being set.
    // Wherever the original color (of the row) already has the bit set anyway, the color is
    // idempotent. Otherwise, it either points to another existing color that has exactly that bit
    // set in addition to our original color, or, if that does not exist in the color list,
    // we instead use the minimal (w.r.t. pop count) superset color instead as an imaginary color.
    // We only compute these on-demand, as likely not all of them will be needed.
    gamut_ = utils::Matrix<size_t>( colors_.size(), element_count_, 0 );

    // Now we have set up the gamut, so we can activate the fast path.
    // This function here is only called while holding the write mutex,
    // and only called once, which we check for safety here.
    if( gamut_started_.load( std::memory_order_acquire )) {
        throw std::runtime_error( "Gamut initialization flag has already been set" );
    }
    gamut_started_.store( true, std::memory_order_release );
}

// -------------------------------------------------------------------------
//     get_gamut_entry_
// -------------------------------------------------------------------------

size_t KmerColorGamut::get_gamut_entry_(
    size_t existing_color_index,
    size_t additive_element_index,
    bool needs_locking
) {
    // The usual sanity checks. Can be done without a lock, as they all only depend on conditions
    // that we consider const within the context of this function. If there is a concurrency issue
    // here due to any of these changing, then things are seriously broken elsewhere as well.
    assert( existing_color_index < colors_.size() );
    assert( additive_element_index < element_count_ );
    assert( ! gamut_.empty() );

    // Another optimization to avoid even more locking: If the gamut is completely filled,
    // we can skip even more locking here, as then we do not need to compute anything any more.
    if( gamut_filled_.load( std::memory_order_acquire )) {
        auto const gamut_entry = gamut_( existing_color_index, additive_element_index );
        assert( gamut_entry > 0 );
        return gamut_entry;
    }

    // Below, we need read and write acces to the cells in the gamut matrix. Protect against
    // concurrent calling for the same entry in the gamut matrix. We use the index in the
    // (linearized) matrix to obtain a guard that is unique for a bucket of entries.
    auto const cell_index = gamut_.index( existing_color_index, additive_element_index );

    // If the entry is already in the gamut, we just return that.
    {
        utils::ConcurrentVectorGuard::LockGuard lock;
        if( needs_locking ) {
            lock = gamut_guard_.get_lock_guard( cell_index );
        }
        auto const gamut_entry = gamut_( existing_color_index, additive_element_index );
        assert( gamut_entry < colors_.size() );
        if( gamut_entry > 0 ) {
            return gamut_entry;
        }
    }

    // Otherwise, we need to compute the entry first, which means we try to find a matching color...
    // Outside of the lock, so that the cell guard is not blocking other threads unnecessarily.
    Bitvector target_elements;
    size_t    target_hash;
    auto const matching_index = find_matching_color_(
        existing_color_index,
        additive_element_index,
        target_elements,
        target_hash
    );
    assert( matching_index < colors_.size() );
    if( matching_index > 0 ) {
        return set_gamut_entry_(
            existing_color_index, additive_element_index, matching_index,
            gamut_stats_.real_color_count, needs_locking
        );
    }

    // ...or, if none exists, find the imaginary color representing the minimal fitting superset.
    // The target_elements has been set to our desired bitvector by the above call already.
    // Again, outside of the lock, to allow for more concurrency even within the lock guards
    // that are shared between certain cells. The minimal subset is expensive!
    assert( ! target_elements.empty() );
    populate_target_color_(
        existing_color_index, additive_element_index, target_elements, target_hash
    );
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

    // Finally, update the gamut with the new imaginary color entry, and return its index.
    return set_gamut_entry_(
        existing_color_index, additive_element_index, superset_index,
        gamut_stats_.imag_color_count, needs_locking
    );
}

// -------------------------------------------------------------------------
//     set_gamut_entry_
// -------------------------------------------------------------------------

size_t KmerColorGamut::set_gamut_entry_(
    size_t existing_color_index,
    size_t additive_element_index,
    size_t target_color_index,
    std::atomic<size_t>& stat_counter,
    bool needs_locking
) {
    // We re-aquire the lock for the current gamut cell.
    auto const cell_index = gamut_.index( existing_color_index, additive_element_index );
    utils::ConcurrentVectorGuard::LockGuard lock;
    if( needs_locking ) {
        lock = gamut_guard_.get_lock_guard( cell_index );
    }

    // In the meantime, the entry can have changed from some other thread.
    // If everything is working correctly, that should have given the same result as we found
    // here, so all good, no need to save it again. For safety, we check and throw otherwise.
    auto const gamut_entry = gamut_( existing_color_index, additive_element_index );
    if( gamut_entry != 0 ){
        if( gamut_entry == target_color_index ) {
            return target_color_index;
        } else {
            throw std::runtime_error( "Inconsistent state of the gamut matrix" );
        }
    }

    // Set the entry in the gamut, and increment the real/imag counter.
    assert( gamut_entry == 0 );
    assert( target_color_index != 0 );
    gamut_( existing_color_index, additive_element_index ) = target_color_index;
    ++stat_counter;

    // Check if it is full now, so that we can switch to an even faster retrieve-only mode.
    auto const total_count = gamut_stats_.real_color_count + gamut_stats_.imag_color_count;
    if( total_count == gamut_.size() ) {
        if( gamut_filled_.load( std::memory_order_acquire )) {
            throw std::runtime_error( "Gamut initialization flag already set" );
        }
        if( on_gamut_filled_callback_ ) {
            on_gamut_filled_callback_();
        }
        gamut_filled_.store( true, std::memory_order_release );
    } else if( total_count > gamut_.size() ) {
        throw std::runtime_error( "Gamut overflow" );
    }
    return target_color_index;
}

// -------------------------------------------------------------------------
//     find_minimal_superset_
// -------------------------------------------------------------------------

size_t KmerColorGamut::find_minimal_superset_( utils::Bitvector const& target_elements ) const
{
    // Sanity check.
    assert( target_elements.size() == element_count_ );

    // Init our trackers. We have at most all bits of all primary colors set,
    // meaning that we start the search with one more, so that the min can find it.
    size_t min_index = 0;
    size_t min_pop_count = element_count_ + 1;

    // We are searching for a strict superset that has minimal pop count.
    // We cannot be better than having exactly one element more set than the target,
    // so we can use that as an early stop criterion.
    size_t const target_pop_count = utils::pop_count( target_elements );

    // TODO Add an optional early stop condition here that only searches through the initial set
    // of secondary colors created with the add functions, in case that this here is too slow.
    // This would give suboptimal results, but speed up the gamut construction if needed.

    // Find the superset of our target in the secondary colors that has the smallest pop count,
    // i.e., that comprises the smallest number of primary colors. Here, we unfortunately need
    // to iterate all colors, which is slow - hence, the frozen table caches these for re-use.
    for( size_t i = 1 + element_count_; i < colors_.size(); ++i ) {
        // If the color is not a superset of what we need, we skip it.
        if( ! utils::is_superset( colors_[i].elements, target_elements )) {
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
    assert( min_index == 0 || min_pop_count >= utils::pop_count( target_elements ));
    assert( min_index == 0 || colors_[min_index].elements != target_elements );
    assert( min_index == 0 || is_strict_superset( colors_[min_index].elements, target_elements ));
    return min_index;
}

} // namespace sequence
} // namespace genesis

#endif // GENESIS_CPP_STD >= GENESIS_CPP_STD_17

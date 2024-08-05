#ifndef GENESIS_UTILS_THREADING_SEQUENTIAL_OUTPUT_BUFFER_H_
#define GENESIS_UTILS_THREADING_SEQUENTIAL_OUTPUT_BUFFER_H_

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
 * @ingroup utils
 */

#include "genesis/utils/core/std.hpp"
#include "genesis/utils/threading/threadsafe_queue.hpp"

#include <array>
#include <cassert>
#include <climits>
#include <cstdint>
#include <deque>
#include <functional>
#include <limits>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Sequential Output Buffer
// =================================================================================================

/**
 * @brief Buffer structure for output to be produced in a well-defined sequential order.
 *
 * In multi-threaded processing, we might have cases where elements are computed in some semi-random
 * order, depending on the speed of processing of each element in the compute threads. However,
 * for ordered output, we might need the elements to be processed in their original sequential order.
 * This class helps to achive this, by buffering elements along with their sequence id. Once the
 * buffer is filled to some degree with a consecutive sequence of elements (we use blocks internally),
 * the elements can be processed in the correct order by an output function.
 *
 * Elements are emplaced with a sequence id indicating their desired position in the output.
 * Each id can only be used once, and after all elements have been processed, there shall be no
 * gaps in the ids up until the highest id that was emplaced. After that, calling close() or
 * destructing the buffer will then flush the remaining set of elements if their block was not
 * yet completely filled.
 *
 * Example:
 *
 *     // Create a buffer that prints lines in the correct order.
 *     SequentialOutputBuffer<std::string> seq_out_buff{
 *         [&]( std::string&& line ){
 *             std::cout << line << "\n";
 *         }
 *     };
 *
 *     // Emplace elements in the buffer. This loop can be run in a thread pool,
 *     // where each thread processed a subset of lines in some order. In that case,
 *     // make sure to wait for all threads to finish before calling close().
 *     for( size_t i = 0; i < num_lines; ++i ) {
 *         seq_out_buff.emplace( i, lines[i] );
 *     }
 *     seq_out_buff.close();
 *
 * As we buffer all elements until they are ready, this class is meant for data that is roughly in
 * the correct order already. If elements come in completely random order, we need to allocate
 * the full range of memory spanning up to the differenence in the current id and the max id that
 * was emplaced. It is hence best used in a multi-threaded setting, where some threads process
 * elements in their order, so that the sequence of elements is only locally random, but overall
 * monotonic.
 */
template<typename T>
class SequentialOutputBuffer
{
    // -------------------------------------------------------------------------
    //     Private Member Types
    // -------------------------------------------------------------------------

private:

    // We use the slots as indicators which elements in the slots of a block have been set already.
    // Using 64 slots fixed for now, for efficiency. Might parameterize as template param,
    // so that the buffer can be made smaller if needed for large elements instead.
    using BlockSlotBits = uint64_t;
    static const BlockSlotBits ALL_SLOTS = std::numeric_limits<BlockSlotBits>::max();
    static const size_t BLOCK_SLOT_SIZE = CHAR_BIT * sizeof( BlockSlotBits );
    static_assert( CHAR_BIT == 8, "CHAR_BIT != 8" );

    // For sequentially capturing output, we need to know which elements have already been set.
    // This could be done with one bool per element, but that seems wasteful, and might be slightly
    // inefficient due to the interleaved memory alignment of inserting single bools between
    // elements. So instead, we store an array of elements at once, with a bit vector indicating
    // which ones have already been set. This also reduces the amount of allocations of the deque,
    // at the cost of higher upfront memory for whole blocks. This currently allocates blocks
    // of 64 elements each time. For very large elements, we might want to reduce that. But then
    // again, 64 is an okay number to buffer anyway in a highly multi-threaded environment.
    struct Block
    {
        BlockSlotBits occupied_slots = 0;
        std::array<T, BLOCK_SLOT_SIZE> slots;
    };

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

public:

    /**
     * @brief Initialize a sequential output buffer with the function that is to be called
     * for each element in the correct sequence order.
     *
     * The @p output_function is called exactly once for each emplaced element, in their order.
     * By default, the sequence order starts at 0, unless @p first_sequence_id is set to some
     * other value, and has to be dense, i.e., in the end, all sequence ids from the first one
     * to the last have to be emplaced at some point, with no gaps.
     */
    SequentialOutputBuffer( std::function<void(T&&)> output_function, size_t first_sequence_id = 0 )
        : head_sequence_id_( first_sequence_id )
        , output_function_( output_function )
    {
        if( !output_function ) {
            throw std::invalid_argument(
                "Cannot initialize SequentialOutputBuffer with empty output function"
            );
        }
    }

    ~SequentialOutputBuffer()
    {
        close();
    }

    SequentialOutputBuffer( SequentialOutputBuffer const& ) = delete;
    SequentialOutputBuffer( SequentialOutputBuffer&& )      = delete;

    SequentialOutputBuffer& operator= ( SequentialOutputBuffer const& ) = delete;
    SequentialOutputBuffer& operator= ( SequentialOutputBuffer&& )      = delete;

    // -------------------------------------------------------------------------
    //     Element Access
    // -------------------------------------------------------------------------

    /**
     * @brief Emplace an @p element in the buffer, at the given @p sequence_id.
     *
     * This overload places a copy of the element in the buffer.
     */
    void emplace( size_t sequence_id, T const& element )
    {
        T copy = element;
        emplace( sequence_id, std::move( copy ));
    }

    /**
     * @brief Emplace an @p element in the buffer, at the given @p sequence_id.
     *
     * This overload places the element in the buffer by moving. Preferred if possible.
     */
    void emplace( size_t sequence_id, T&& element )
    {
        // Lock the mutex, as everything from here on needs exclusive access.
        std::unique_lock<std::mutex> block_lock( block_mutex_ );

        // Fundamental checks.
        if( !output_function_ ) {
            throw std::runtime_error(
                "Cannot emplace element in SequentialOutputBuffer after it has been closed"
            );
        }
        if( sequence_id < head_sequence_id_ ) {
            throw std::runtime_error(
                "Invalid sequence in Sequential Output Buffer, emplacing element " +
                std::to_string( sequence_id ) + " when head is already at " +
                std::to_string( head_sequence_id_ )
            );
        }

        // Emplace the element
        emplace_element_( sequence_id, std::move( element ));

        // Check if the first blocks are ready for output, i.e., if all their slots are full.
        // If not, we are done here, at which point the above lock on the mutex gets released.
        if( block_deque_.empty() || block_deque_.front()->occupied_slots != ALL_SLOTS ) {
            return;
        }

        // Here, we know that at least the first block is ready for output, as all its slots are full.
        // Transfer all full blocks at the front to the output queue, after which we can release
        // the block_mutex_ again. At this point, other threads can keep adding to the buffer,
        // while we process the output, while then holding the output_mutex_. This way,
        // we avoid having other threads to wait adding to the buffer while processing.
        transfer_full_blocks_to_output_queue_();

        // We have transferred all blocks that are ready, and can release the lock on block_mutex_,
        // so that other threads can continue emplacing elements. Then, we can process the blocks
        // we have just transferred. This is as close as we can get to lock-free behaviour without
        // massive over-engineering (there is probably a way to avoid the lock in this function here,
        // by processing the slots in some atomic way), while still maintaining the correct output
        // order (which has no way around a lock on the actual output processing, I think).
        block_lock.unlock();
        process_output_queue_();
    }

    /**
     * @brief Close the buffer, i.e., process all remaining elements.
     *
     * After this, no new elements can be emplaced any more.
     */
    void close()
    {
        // Lock both mutexes, just in case this is used in a weird way. We never do this anywhere
        // else, so there should be no deadlock possible here. Also, this function is not meant
        // to be called in a concurrent way anyway. If multiple places call this at once,
        // the second call will wait here, and then fail, as we can only call close() once.
        std::lock_guard<std::mutex> block_lock( block_mutex_ );
        std::lock_guard<std::mutex> output_lock( output_mutex_ );

        // Make sure that we are in a valid state. This function might have been called already,
        // and then again in the destructor. In that case, do nothing. Otherwise, it's an error.
        if( !output_function_ ) {
            if( block_deque_.empty() ) {
                return;
            }
            throw std::runtime_error(
                "Invalid state of SequentialOutputBuffer after it has been closed"
            );
        }
        assert( output_function_ );

        // Process the last blocks, and indicate that we are closed already.
        // No more emplacing possible after this.
        process_last_blocks_();
        output_function_ = nullptr;
    }

    // -------------------------------------------------------------------------
    //     Private Members
    // -------------------------------------------------------------------------

private:

    void emplace_element_( size_t sequence_id, T&& element )
    {
        // Assumes that caller holds a lock on block_mutex_,
        // and assumes the following two properties.
        assert( output_function_ );
        assert( sequence_id >= head_sequence_id_ );

        // Get the indices into the block chain that we need.
        auto const index = sequence_id - head_sequence_id_;
        auto const block_index = index / BLOCK_SLOT_SIZE;
        auto const slot_index  = index % BLOCK_SLOT_SIZE;
        auto const slot_bit = static_cast<BlockSlotBits>( 1 ) << slot_index;
        assert( slot_bit != 0 );

        // Get the block we need, creating it if not present.
        while( block_deque_.size() < block_index + 1 ) {
            block_deque_.emplace_back(
                genesis::utils::make_unique<Block>()
            );
        }
        assert( block_deque_.size() > block_index );
        auto& block = *block_deque_[block_index];

        // Check that the element has not been set already.
        if( block.occupied_slots & slot_bit ) {
            throw std::runtime_error(
                "Invalid sequence in Sequential Output Buffer, emplacing element " +
                std::to_string( sequence_id ) + ", which has already been emplaced before"
            );
        }

        // Insert the element, and set its slot bit to mark that this is set now.
        block.slots[slot_index] = std::move( element );
        block.occupied_slots |= slot_bit;
    }

    void transfer_full_blocks_to_output_queue_()
    {
        // Assumes that caller holds a lock on block_mutex_,
        // and assumes the following property, meaning there is a full block at the front.
        assert( block_deque_.size() > 0 && block_deque_.front()->occupied_slots == ALL_SLOTS );

        // Transfer all full blocks from the front to the output queue.
        // While this is being transferred, there might be another thread that is currently
        // in process_output_queue_(), but that's okay, because we are using a thread safe queue
        // for the transfer. In that case, the other thread will also process the elements
        // pushed from here. Then, once our thread here gets to the process_output_queue_() as well,
        // there might be nothing to do any more, and that's okay.
        while( block_deque_.size() > 0 && block_deque_.front()->occupied_slots == ALL_SLOTS ) {
            output_queue_.push( std::move( block_deque_.front() ));
            block_deque_.pop_front();
            head_sequence_id_ += BLOCK_SLOT_SIZE;
        }
    }

    void process_output_queue_()
    {
        // We obtain a lock on the output_mutex_, to make sure that everything is in the correct
        // order. If any other thread is currently writing output, we will have to wait here.
        std::lock_guard<std::mutex> output_lock( output_mutex_ );
        assert( output_function_ );
        while( !output_queue_.empty() ) {

            // Get the front element of the queue. This always succeeds, as we have a lock
            // on this function and it is the only place where elements are removed.
            // If any other thread adds elements in the meantime, that's okay as well.
            std::unique_ptr<Block> block;
            if( ! output_queue_.try_pop( block )) {
                throw std::runtime_error( "Internal error: broken lock in SequentialOutputBuffer" );
            }
            if( block->occupied_slots != ALL_SLOTS ) {
                throw std::runtime_error( "Internal error: broken block in SequentialOutputBuffer" );
            }

            // Now process the block.
            for( size_t slot = 0; slot < BLOCK_SLOT_SIZE; ++slot ) {
                output_function_( std::move( block->slots[slot] ));
            }
        }
    }

    void process_last_blocks_()
    {
        // Here, we know we need to wrap up. We process all elements, checking each time that
        // the sequence is complete, and remove them until nothing is left.
        // We cannot have more than the last unfinished block here, as all previous ones
        // are already processed if the order is correct. Still, we process this in a loop,
        // so that any errors can be elegantly handled and reported.
        size_t closed_blocks = 0;
        while( ! block_deque_.empty() ) {
            auto& first_block = *block_deque_.front();

            // Process the currently first block in the deque, stopping once we find an unused slot.
            size_t slot = 0;
            for( ; slot < BLOCK_SLOT_SIZE; ++slot ) {
                auto const slot_bit = static_cast<BlockSlotBits>( 1 ) << slot;
                assert( slot_bit != 0 );

                // If the slot bit is set, we can process the element.
                // If not, we are done - in that case, for the buffer to be valid, there must be
                // no further blocks or slots after this one, which we check below.
                if( !( first_block.occupied_slots & slot_bit )) {
                    break;
                }

                // Process the element, and unset its slot bit.
                output_function_( std::move( first_block.slots[slot] ));
                assert( first_block.occupied_slots & slot_bit );
                first_block.occupied_slots ^= slot_bit;
                assert( !( first_block.occupied_slots & slot_bit ));
            }

            // We processed the first block. It now needs to be empty for the buffer to be valid.
            if( first_block.occupied_slots || block_deque_.size() > 1 ) {
                throw std::runtime_error(
                    "Invalid sequence in Sequential Output Buffer, closing the buffer with gaps "
                    "at sequence id " + std::to_string( head_sequence_id_ + slot )
                );
            }

            // We can now remove the block.
            assert( slot == BLOCK_SLOT_SIZE || first_block.occupied_slots == 0 );
            block_deque_.pop_front();
            head_sequence_id_ += BLOCK_SLOT_SIZE;
            ++closed_blocks;
        }

        // Here, we have finished the above successfully, without throwing.
        // So indeed, as stated above, this must have been a single block.
        (void) closed_blocks;
        assert( closed_blocks <= 1 );
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // This is meant for multithreaded access, so we need to lock access to the main block deque,
    // as well as the output processing deque that we keep internally.
    std::mutex block_mutex_;
    std::mutex output_mutex_;

    // A sequence of blocks, each containing slots for the elements.
    // We store them via pointers, so that we can move them around quickly.
    // We use a deque here, as we need index-based access to the elements.
    std::deque<std::unique_ptr<Block>> block_deque_;

    // Another sequence of blocks, but only those that are ready for output.
    // We keep this separate, so that output processing in the thread that finishes a block
    // does not make the other threads wait unnecessarily.
    // We are using a simple blocking threadsafe queue here, instead of the concurrent queue,
    // as the latter does not guarantee the order of elements that have been enqueued from
    // several threads, which would make it harder to reason about the order of blocks here.
    ThreadsafeQueue<std::unique_ptr<Block>> output_queue_;

    // We only ever need to keep the id (sequence order) of the first element of the first block
    // here, as everything else can easily be computed from that one.
    size_t head_sequence_id_ = 0;

    // The function to process each element in the correct order with.
    std::function<void( T&& )> output_function_;
};

} // namespace utils
} // namespace genesis

#endif // include guard

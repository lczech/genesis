#ifndef GENESIS_POPULATION_WINDOW_SLIDING_ENTRIES_WINDOW_STREAM_H_
#define GENESIS_POPULATION_WINDOW_SLIDING_ENTRIES_WINDOW_STREAM_H_

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

#include "genesis/population/window/base_window_stream.hpp"
#include "genesis/population/window/window_view_stream.hpp"
#include "genesis/population/window/window_view.hpp"
#include "genesis/population/window/window.hpp"

#include <cassert>
#include <functional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace genesis {
namespace population {

// =================================================================================================
//     Sliding Entries Window Stream
// =================================================================================================

/**
 * @brief Stream for sliding Window%s of a fixed number of (variant) positions in a genome.
 *
 * With each step of the iteration, a window consisting of count() many entries of the underlying
 * input stream is yielded. Then, when incrementing, we move forward stride() many entries,
 * which can correspond to an arbitrary number of positions in the genome, depending on how far
 * the entries are spread out. Hence, any filterin on positions in the genome should be done
 * beforehand on the inputs, so that only those entries are used by this stream that are
 * meant to be considered.
 *
 * The three functors
 *
 *  * #entry_input_function,
 *  * #chromosome_function, and
 *  * #position_function
 *
 * have to be set in the class prior to starting the iteration, as well as the count() of how many
 * entries shall be used in each iteration.
 * All other settings are optional and/or defaulted to reasonable values.
 * See make_sliding_entries_window_stream() and make_default_sliding_entries_window_stream()
 * for helper functions that take care of this for most of our data types.
 *
 * See BaseWindowStream for more details on the three functors, the template parameters,
 * and general usage examples of the class.
 *
 * @see make_sliding_entries_window_stream()
 * @see make_default_sliding_entries_window_stream()
 */
template<class InputStreamIterator, class DataType = typename InputStreamIterator::value_type>
class SlidingEntriesWindowStream final : public BaseWindowStream<InputStreamIterator, DataType>
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using self_type = SlidingEntriesWindowStream<InputStreamIterator, DataType>;
    using base_type = BaseWindowStream<InputStreamIterator, DataType>;

    using Window            = ::genesis::population::Window<DataType>;
    using Entry             = typename Window::Entry;
    using InputType         = typename InputStreamIterator::value_type;

    using iterator_category = std::input_iterator_tag;
    using value_type        = Window;
    using pointer           = value_type*;
    using reference         = value_type&;
    using const_reference   = value_type const&;

    // ======================================================================================
    //      Internal Iterator
    // ======================================================================================

    /**
     * @brief Internal iterator that produces Window%s.
     */
    class DerivedIterator final : public BaseWindowStream<InputStreamIterator, DataType>::BaseIterator
    {
    public:

        // -------------------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------------------

        using self_type = typename SlidingEntriesWindowStream<
            InputStreamIterator, DataType
        >::DerivedIterator;

        using base_iterator_type = typename BaseWindowStream<
            InputStreamIterator, DataType
        >::BaseIterator;

        using Window            = ::genesis::population::Window<DataType>;
        using Entry             = typename Window::Entry;
        using InputType         = typename InputStreamIterator::value_type;

        using iterator_category = std::input_iterator_tag;
        using value_type        = Window;
        using pointer           = value_type*;
        using reference         = value_type&;
        using const_reference   = value_type const&;

    private:

        DerivedIterator() = default;

        DerivedIterator(
            SlidingEntriesWindowStream const* parent
        )
            : base_iterator_type( parent )
            , parent_( parent )
        {
            // Edge case check. See Base for details.
            if( ! parent_ ) {
                return;
            }

            // Call the base class initializer.
            // base_iterator_type::init_( parent_ );

            // Check our own the settings.
            if( parent_->count_ == 0 ) {
                throw std::runtime_error(
                    "Cannot use SlidingEntriesWindowStream with count == 0."
                );
            }
            if( parent_->stride_ == 0 ) {
                parent_->stride_ = parent_->count_;
            }
            if( parent_->stride_ > parent_->count_ ) {
                throw std::runtime_error(
                    "Cannot use SlidingEntriesWindowStream with stride > count."
                );
            }

            // Let's get going.
            init_chromosome_();

            // If the input is empty (no data at all), we might already be done.
            // If not, fill the window with data.
            if( parent_ ) {
                update_();
            }
        }

    public:

        virtual ~DerivedIterator() = default;

        DerivedIterator( self_type const& ) = default;
        DerivedIterator( self_type&& )      = default;

        DerivedIterator& operator= ( self_type const& ) = default;
        DerivedIterator& operator= ( self_type&& )      = default;

        friend SlidingEntriesWindowStream;

        // -------------------------------------------------------------------------
        //     Internal and Virtual Members
        // -------------------------------------------------------------------------

    private:

        void init_chromosome_()
        {
            // Check that we are still good. If not, this function being called is likely a user
            // error by trying to increment a past-the-end iterator.
            assert( parent_ );

            // Saveguard. This might be called on an empty range, in which case we just do nothing.
            if( base_iterator_type::current_ == base_iterator_type::end_ ) {
                parent_ = nullptr;
                return;
            }

            // Clear the window and prepare for new chromosome.
            window_.clear();
            window_.chromosome( parent_->chromosome_function( *base_iterator_type::current_ ));
            base_iterator_type::is_first_window_ = true;
            base_iterator_type::is_last_window_ = false;
            next_index_ = 0;
        }

        void increment_() override final
        {
            // Basic check again.
            assert( parent_ );

            // Special case: If we have no more underlying data, the iterator still needs to stop
            // at the last window(s), so that they can be processed. After that, when this
            // increment_() function is called again by the user, we then set parent_ = nullptr
            // to indicate that now we are done for good.
            if( base_iterator_type::current_ == base_iterator_type::end_ ) {
                // If current_ == end_, we have definitely reached the end of the input, so we need
                // to have set is_last_window_ previously. If not set, that means it was already
                // reset, so that this is an iteration past the end.
                if( ! base_iterator_type::is_last_window_ ) {
                    throw std::runtime_error(
                        "SlidingIntervalWindowIterator: Incrementing past the end"
                    );
                }

                // Indicate that we are done now.
                parent_ = nullptr;
                return;
            }

            // Check if this call moves to the next chromosome.
            auto const cur_chr = parent_->chromosome_function( *base_iterator_type::current_ );
            if( cur_chr != window_.chromosome() ) {
                init_chromosome_();
            } else {
                base_iterator_type::is_first_window_ = false;
            }

            // Fill window with data.
            update_();
        }

        void update_()
        {
            // Dequeue everything that we do not want to keep. If stride == count (default case),
            // we can simply remove everything at once, for speed.
            if( parent_->stride_ == parent_->count_ ) {
                window_.entries().clear();
            } else {
                for( size_t i = 0; i < parent_->stride_; ++i ) {
                    if( window_.empty() ) {
                        // Edge case when we start with a new empty window.
                        break;
                    }
                    window_.entries().pop_front();
                }
            }

            // Now enqueue new entries.
            size_t add_cnt = 0;
            (void) add_cnt;
            while( window_.size() < parent_->count_ ) {
                if( base_iterator_type::current_ == base_iterator_type::end_ ) {
                    break;
                }
                auto const cur_chr = parent_->chromosome_function( *base_iterator_type::current_ );
                auto const cur_pos = parent_->position_function( *base_iterator_type::current_ );

                // If we are at the next chromosome, we are done with this window.
                if( cur_chr != window_.chromosome() ) {
                    break;
                }
                assert( cur_chr == window_.chromosome() );

                // Check that we are not going backwards in the chromosome,
                // i.e., if we got unsorted data. That would lead to unwanted behaviour.
                if( window_.size() > 0 && window_.entries().back().position >= cur_pos ) {
                    throw std::runtime_error(
                        "Invalid entry in sliding window that not in sequence with other entries. "
                        "Previous entry is " + window_.chromosome() + ":" +
                        std::to_string( window_.entries().back().position ) +
                        ", current (invalid) entry is " + window_.chromosome() + ":" +
                        std::to_string( cur_pos )
                    );
                }

                // Now enqueue the entry, and move to the next.
                window_.entries().emplace_back(
                    next_index_,
                    cur_pos,
                    parent_->entry_input_function( *base_iterator_type::current_ )
                );
                ++add_cnt;
                ++next_index_;
                ++base_iterator_type::current_;
            }

            // Either we have added as many new entries as the stride tells us, or, if this
            // was a new empty window, we have added a full count of entries,
            // or we reached the end of the data or the end of the chromosome.
            // Also, we can never have _more_ entries in the window, and we cannot have an empty
            // window, as in that case this update function should not have been called at all.
            assert(
                add_cnt == parent_->stride_ ||
                add_cnt == parent_->count_ ||
                base_iterator_type::current_ == base_iterator_type::end_ ||
                parent_->chromosome_function( *base_iterator_type::current_ ) != window_.chromosome()
            );
            assert(
                window_.size() == parent_->count_ ||
                base_iterator_type::current_ == base_iterator_type::end_ ||
                parent_->chromosome_function( *base_iterator_type::current_ ) != window_.chromosome()
            );
            assert( window_.size() <= parent_->count_ );
            assert( window_.size() > 0 );

            // Cases in which we are at the last window: Either we reached the end of the input,
            // or the end of the current chromosome.
            if(
                base_iterator_type::current_ == base_iterator_type::end_ ||
                parent_->chromosome_function( *base_iterator_type::current_ ) != window_.chromosome()
            ) {
                base_iterator_type::is_last_window_ = true;
            }

            // Update the window positions.
            window_.first_position( window_.entries().front().position );
            window_.last_position( window_.entries().back().position );
        }

        value_type& get_current_window_() const override final
        {
            return const_cast<value_type&>( window_ );
        }

        BaseWindowStream<InputStreamIterator, DataType> const* get_parent_() const override final
        {
            return parent_;
        }

    private:

        // Parent. Needs to live here to have the correct derived type.
        SlidingEntriesWindowStream const* parent_ = nullptr;

        // Current window and its position
        Window window_;
        size_t next_index_ = 0;

    };

    // ======================================================================================
    //      Main Class
    // ======================================================================================

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    SlidingEntriesWindowStream(
        InputStreamIterator begin, InputStreamIterator end
    )
        : base_type( begin, end )
    {}

    virtual ~SlidingEntriesWindowStream() = default;

    SlidingEntriesWindowStream( SlidingEntriesWindowStream const& ) = default;
    SlidingEntriesWindowStream( SlidingEntriesWindowStream&& )      = default;

    SlidingEntriesWindowStream& operator= ( SlidingEntriesWindowStream const& ) = default;
    SlidingEntriesWindowStream& operator= ( SlidingEntriesWindowStream&& )      = default;

    friend DerivedIterator;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    /**
     * @brief Number of entries in each Window.
     *
     * The count has to be `> 0`. This is the number of entries that are put into each Window.
     * Typically, we process, e.g., variant positions only - in that case, the count() is the number
     * of Variant%s in each Window.
     */
    self_type& count( size_t value )
    {
        count_ = value;
        return *this;
    }

    size_t count() const
    {
        return count_;
    }

    /**
     * @brief Stride of the Window, that is, how many entries to move forward with each iteration.
     *
     * The stride his is the shift towards the next interval, determining how many entries are
     * dropped from the beginning and added to the end of each Window when iterating.
     * It has to be `<= count`. If stride is set to 0 (default), it is set automatically to the
     * count() when starting the iteration, meaning that each Window contains the next count()
     * many new entries from the underlying input iterator.
     */
    self_type& stride( size_t value )
    {
        stride_ = value;
        return *this;
    }

    size_t stride() const
    {
        return stride_;
    }

    // -------------------------------------------------------------------------
    //     Virtual Members
    // -------------------------------------------------------------------------

protected:

    std::unique_ptr<typename BaseWindowStream<InputStreamIterator, DataType>::BaseIterator>
    get_begin_iterator_() override final
    {
        // Cannot use make_unique here, as the Iterator constructor is private,
        // and trying to make make_unique a friend does not seem to be working...
        return std::unique_ptr<DerivedIterator>( new DerivedIterator( this ));
        // return utils::make_unique<DerivedIterator>( this );
    }

    std::unique_ptr<typename BaseWindowStream<InputStreamIterator, DataType>::BaseIterator>
    get_end_iterator_() override final
    {
        return std::unique_ptr<DerivedIterator>( new DerivedIterator( nullptr ));
        // return utils::make_unique<DerivedIterator>( nullptr );
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // Settings. We make stride_ mutable so that the iterator can set it to the count.
    size_t count_ = 0;
    mutable size_t stride_ = 0;

};

// =================================================================================================
//     Make Sliding Window Stream
// =================================================================================================

/**
 * @brief Helper function to instantiate a SlidingEntriesWindowStream
 * without the need to specify the template parameters manually.
 */
template<class InputStreamIterator, class DataType = typename InputStreamIterator::value_type>
SlidingEntriesWindowStream<InputStreamIterator, DataType>
make_sliding_entries_window_stream(
    InputStreamIterator begin, InputStreamIterator end, size_t count = 0, size_t stride = 0
) {
    auto it = SlidingEntriesWindowStream<InputStreamIterator, DataType>( begin, end );
    it.count( count );
    it.stride( stride );
    return it;
}

/**
 * @brief Helper function to instantiate a SlidingEntriesWindowStream for a default use case.
 *
 * This helper assumes that the underlying type of the input data stream and of the Window%s
 * that we are sliding over are of the same type, that is, we do no conversion in the
 * `entry_input_function` functor of the SlidingEntriesWindowStream. It further assumes that this
 * data type has public member variables `chromosome` and `position` that are accessed by the
 * `chromosome_function` and `position_function` functors of the SlidingEntriesWindowStream.
 * For example, a data type that this works for is Variant data.
 */
template<class InputStreamIterator>
SlidingEntriesWindowStream<InputStreamIterator>
make_default_sliding_entries_window_stream(
    InputStreamIterator begin, InputStreamIterator end, size_t count = 0, size_t stride = 0
) {
    using DataType = typename InputStreamIterator::value_type;

    // Set functors.
    auto it = SlidingEntriesWindowStream<InputStreamIterator>( begin, end );
    it.entry_input_function = []( DataType const& variant ) {
        return variant;
    };
    it.chromosome_function = []( DataType const& variant ) {
        return variant.chromosome;
    };
    it.position_function = []( DataType const& variant ) {
        return variant.position;
    };

    // Set properties.
    it.count( count );
    it.stride( stride );
    return it;
}

/**
 * @brief Helper class that creates a SlidingEntriesWindowStream
 * and wraps it in a WindowViewStream.
 *
 * See make_default_sliding_entries_window_stream() for the base functionality,
 * and see make_window_view_stream() for the wrapping behaviour.
 *
 * Note that because this is a simple wrapper around the constructor of SlidingEntriesWindowStream,
 * we lose access to that class itself, so that its more specialized member functions cannot be called
 * any more. If this is needed, use the two aforementioned `make_...()` functions individually.
 */
template<class InputStreamIterator>
WindowViewStream<InputStreamIterator>
make_default_sliding_entries_window_view_stream(
    InputStreamIterator begin, InputStreamIterator end, size_t count = 0, size_t stride = 0
) {
    return make_window_view_stream(
        make_default_sliding_entries_window_stream( begin, end, count, stride )
    );
}

} // namespace population
} // namespace genesis

#endif // include guard

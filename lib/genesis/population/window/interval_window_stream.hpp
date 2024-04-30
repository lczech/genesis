#ifndef GENESIS_POPULATION_WINDOW_INTERVAL_WINDOW_STREAM_H_
#define GENESIS_POPULATION_WINDOW_INTERVAL_WINDOW_STREAM_H_

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
//     Sliding Interval Window Stream
// =================================================================================================

/**
 * @brief Stream for sliding Window%s of fixed sized intervals over the chromosomes of a genome.
 *
 * The three functors
 *
 *  * #entry_input_function,
 *  * #chromosome_function, and
 *  * #position_function
 *
 * have to be set in the class prior to starting the iteration, as well as the width().
 * All other settings are optional and/or defaulted to reasonable values.
 * See make_interval_window_stream() and make_default_interval_window_stream()
 * for helper functions that take care of this for most of our data types.
 *
 * See BaseWindowStream for more details on the three functors, the template parameters,
 * and general usage examples of the class.
 *
 * @see make_interval_window_stream()
 * @see make_default_interval_window_stream()
 */
template<class InputStreamIterator, class DataType = typename InputStreamIterator::value_type>
class IntervalWindowStream final : public BaseWindowStream<InputStreamIterator, DataType>
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using self_type = IntervalWindowStream<InputStreamIterator, DataType>;
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

        using self_type = typename IntervalWindowStream<
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
            IntervalWindowStream const* parent
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
            if( parent_->width_ == 0 ) {
                throw std::runtime_error( "Cannot use IntervalWindowStream of width 0." );
            }
            if( parent_->stride_ == 0 ) {
                parent_->stride_ = parent_->width_;
            }
            if( parent_->stride_ > parent_->width_ ) {
                throw std::runtime_error(
                    "Cannot use IntervalWindowStream with stride > width."
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

        virtual ~DerivedIterator() override = default;

        DerivedIterator( self_type const& ) = default;
        DerivedIterator( self_type&& )      = default;

        DerivedIterator& operator= ( self_type const& ) = default;
        DerivedIterator& operator= ( self_type&& )      = default;

        friend IntervalWindowStream;

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

            if( parent_->emit_leading_empty_windows_ ) {
                current_start_ = 1;
            } else {
                // Set the start to the window position that we would get after going through all
                // the previous windows if they were emitted.
                auto const pos = parent_->position_function( *base_iterator_type::current_ );
                current_start_ = pos - (( pos - 1 ) % parent_->stride_ );
            }
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
                        "IntervalWindowStream: Incrementing past the end"
                    );
                }

                // Indicate that we are done now.
                parent_ = nullptr;
                return;
            }

            // Check if this call moves to the next chromosome.
            if(
                parent_->chromosome_function( *base_iterator_type::current_ ) != window_.chromosome()
            ) {
                init_chromosome_();
            } else {
                // Update positions.
                current_start_ += parent_->stride_;
                base_iterator_type::is_first_window_ = false;
            }

            // Fill window with data.
            update_();
        }

        void update_()
        {
            // Basic check again.
            assert( parent_ );

            // Dequeue everything that is not part of the current interval any more.
            // We can speed up by clearing the whole window if its last entry is before the current
            // start, as in that case, all its entries are, so we want to pop them all anyway.
            // That is the default case when moving with stride = width, so that's nice.
            if(
                window_.entries().size() > 0 &&
                window_.entries().back().position < current_start_
            ) {
                window_.entries().clear();
            } else {
                while(
                    window_.entries().size() > 0 &&
                    window_.entries().front().position < current_start_
                ) {
                    window_.entries().pop_front();
                }
            }

            // Now enqueue new entries.
            while( base_iterator_type::current_ != base_iterator_type::end_ ) {
                auto const cur_pos = parent_->position_function( *base_iterator_type::current_ );

                // Get the chromosome and position of the current entry, and see if it belongs
                // into the current window. If not, we are done here with this window.
                if(
                    parent_->chromosome_function(
                        *base_iterator_type::current_
                    ) != window_.chromosome() ||
                    cur_pos >= current_start_ + parent_->width_
                ) {
                    break;
                }
                assert(
                    parent_->chromosome_function( *base_iterator_type::current_ ) ==
                    window_.chromosome()
                );
                assert( cur_pos >= current_start_ );
                assert( cur_pos <  current_start_ + parent_->width_ );

                // Check that we are not going backwards in the chromosome,
                // i.e., if we got unsorted data. That would lead to unwanted behaviour.
                if(
                    window_.size() > 0 &&
                    window_.entries().back().position >= cur_pos
                ) {
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
                ++next_index_;
                ++base_iterator_type::current_;
            }

            // Cases in which we are at the last window: Either we reached the end of the input,
            // or the end of the current chromosome.
            if(
                base_iterator_type::current_ == base_iterator_type::end_ ||
                parent_->chromosome_function( *base_iterator_type::current_ ) != window_.chromosome()
            ) {
                base_iterator_type::is_last_window_ = true;
            }

            // Update the window positions.
            window_.first_position( current_start_ );
            window_.last_position( current_start_ + parent_->width_ - 1 );
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
        IntervalWindowStream const* parent_ = nullptr;

        // Current window and its position
        Window window_;
        size_t current_start_ = 1;
        size_t next_index_ = 0;

    };

    // ======================================================================================
    //      Main Class
    // ======================================================================================

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    IntervalWindowStream(
        InputStreamIterator begin, InputStreamIterator end,
        size_t width = 0, size_t stride = 0
    )
        : base_type( begin, end )
        , width_( width )
        , stride_( stride )
    {}

    virtual ~IntervalWindowStream() override = default;

    IntervalWindowStream( IntervalWindowStream const& ) = default;
    IntervalWindowStream( IntervalWindowStream&& )      = default;

    IntervalWindowStream& operator= ( IntervalWindowStream const& ) = default;
    IntervalWindowStream& operator= ( IntervalWindowStream&& )      = default;

    friend DerivedIterator;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    /**
     * @brief Width of the Window, that is, the fixed length along the chromosome.
     *
     * The width has to be `> 0`. This is the length of the interval, determining the first and
     * last position in each Window.
     */
    self_type& width( size_t value )
    {
        width_ = value;
        return *this;
    }

    size_t width() const
    {
        return width_;
    }

    /**
     * @brief Stride of the Window, that is, how many positions to move forward with each iteration.
     *
     * The stride his is the shift towards the next interval, determining how the first and last
     * position in each Window change. It has to be `<= width`. If stride is set to 0 (default),
     * it is set automatically to the width when starting the iteration.
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

    /**
     * @brief Select whether the iterator produces empty windows in the beginning of each
     * chromosome, before the first actual position that is reported by the underlying data.
     *
     * Say the underlying iterator has the first Variant (or whatever datatype it iterates over)
     * at position 1020 for a chromosome, and we use a window size of 100. If this setting is set
     * to `true`, the iterator will emit 10 empty windows before reaching this position.
     * If set to `false`, it will skip these, and start at position 1001, which is the first one
     * that would have been reached by striding along the chromosome.
     */
    self_type& emit_leading_empty_windows( bool value )
    {
        emit_leading_empty_windows_ = value;
        return *this;
    }

    bool emit_leading_empty_windows() const
    {
        return emit_leading_empty_windows_;
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
        return std::unique_ptr<DerivedIterator>( new DerivedIterator() );
        // return utils::make_unique<DerivedIterator>( nullptr );
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // Settings. We make stride_ mutable so that the iterator can set it to the width.
    size_t width_ = 0;
    mutable size_t stride_ = 0;

    bool emit_leading_empty_windows_ = false;

    // bool emit empty_windows = true;
    // bool emit_unfinished_trailing_window = false;

};

// =================================================================================================
//     Make Sliding Window Stream
// =================================================================================================

/**
 * @brief Helper function to instantiate a IntervalWindowStream
 * without the need to specify the template parameters manually.
 *
 * The three functors `entry_input_function`, `chromosome_function`, and `position_function`
 * of the IntervalWindowStream have to be set in the returned stream before using it.
 * See make_default_interval_window_stream() for an alternative make function
 * that sets these three functors to reasonable defaults that work for the Variant data type.
 */
template<class InputStreamIterator, class DataType = typename InputStreamIterator::value_type>
IntervalWindowStream<InputStreamIterator, DataType>
make_interval_window_stream(
    InputStreamIterator begin, InputStreamIterator end, size_t width = 0, size_t stride = 0
) {
    auto it = IntervalWindowStream<InputStreamIterator, DataType>( begin, end );
    it.width( width );
    it.stride( stride );
    return it;
}

/**
 * @brief Helper function to instantiate a IntervalWindowStream for a default use case.
 *
 * This helper assumes that the underlying type of the input data stream and of the Window%s
 * that we are sliding over are of the same type, that is, we do no conversion in the
 * `entry_input_function` functor of the IntervalWindowStream. It further assumes that this
 * data type has public member variables `chromosome` and `position` that are accessed by the
 * `chromosome_function` and `position_function` functors of the IntervalWindowStream.
 * For example, a data type that this works for is Variant data.
 */
template<class InputStreamIterator>
IntervalWindowStream<InputStreamIterator>
make_default_interval_window_stream(
    InputStreamIterator begin, InputStreamIterator end, size_t width = 0, size_t stride = 0
) {
    using DataType = typename InputStreamIterator::value_type;

    // Set functors.
    auto it = IntervalWindowStream<InputStreamIterator>( begin, end );
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
    it.width( width );
    it.stride( stride );
    return it;
}

/**
 * @brief Helper class that creates a IntervalWindowStream
 * and wraps it in a WindowViewStream.
 *
 * See make_default_interval_window_stream() for the base functionality,
 * and see make_window_view_stream() for the wrapping behaviour.
 *
 * Note that because this is a simple wrapper around the constructor of IntervalWindowStream,
 * we lose access to that class itself, so that its more specialized member functions cannot be called
 * any more. If this is needed, use the two aforementioned `make_...()` functions individually.
 */
template<class InputStreamIterator>
WindowViewStream<InputStreamIterator>
make_default_sliding_interval_window_view_stream(
    InputStreamIterator begin, InputStreamIterator end, size_t width = 0, size_t stride = 0
) {
    return make_window_view_stream(
        make_default_interval_window_stream( begin, end, width, stride )
    );
}

} // namespace population
} // namespace genesis

#endif // include guard

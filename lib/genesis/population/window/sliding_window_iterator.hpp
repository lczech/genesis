#ifndef GENESIS_POPULATION_WINDOW_SLIDING_WINDOW_ITERATOR_H_
#define GENESIS_POPULATION_WINDOW_SLIDING_WINDOW_ITERATOR_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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
 * @ingroup population
 */

#include "genesis/population/window/window.hpp"
#include "genesis/utils/containers/range.hpp"

#include <cassert>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Genomic Sliding Window Iterator
// =================================================================================================

/**
 * @brief Iterator for sliding Window%s over the chromosomes of a genome.
 */
template<
    class Data, class ForwardIterator,
    typename EntryInputFunctor, typename ChromosomeFunctor, typename PositionFunctor
>
class SlidingWindowIterator
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    // using Data              = typename ForwardIterator::value_type;
    // using Data              = typename std::result_of<decltype(EntryInputFunctor)()>::type;

    using Window            = ::genesis::population::Window<Data>;
    using Entry             = typename Window::Entry;

    using self_type         = SlidingWindowIterator<
        Data, ForwardIterator, EntryInputFunctor, ChromosomeFunctor, PositionFunctor
    >;

    using value_type        = Window;
    using reference         = value_type&;
    using const_reference   = value_type const&;

    // using difference_type   = typename Window::container::difference_type;
    // using size_type         = typename Window::container::size_type;
    using iterator_category = std::input_iterator_tag;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Construct a SlidingWindowIterator, given the WindowType and width, and potentially stride.
     *
     * The @p width has to be `> 0`, and the @p stride has to be `<= width`.
     * If stride is not given (or set to `0`), it is set automatically to the width,
     * which means, we create windows that do not overlap.
     */
    SlidingWindowIterator(
        ForwardIterator begin, ForwardIterator end,
        EntryInputFunctor const& entry_function,
        ChromosomeFunctor const& chromosome_function,
        PositionFunctor const& position_function,
        WindowType type, size_t width, size_t stride = 0, bool emit_leading_empty_windows = false
    )
        : window_type_(type)
        , width_(width)
        , stride_(stride)
        , emit_leading_empty_windows_(emit_leading_empty_windows)
        , begin_(begin)
        , current_(begin)
        , end_(end)
        , entry_input_function_(entry_function)
        , chromosome_function_(chromosome_function)
        , position_function_(position_function)
    {
        // Some boundary checks.
        if( width == 0 ) {
            throw std::runtime_error( "Cannot use SlidingWindowIterator of width 0." );
        }
        if( stride == 0 ) {
            stride_ = width;
        }
        if( stride_ > width_ ) {
            throw std::runtime_error( "Cannot use SlidingWindowIterator with stride > width." );
        }

        // Let's get going.
        init_chromosome_();
        update_();
    }

    ~SlidingWindowIterator() = default;

    SlidingWindowIterator( SlidingWindowIterator const& ) = default;
    SlidingWindowIterator( SlidingWindowIterator&& )      = default;

    SlidingWindowIterator& operator= ( SlidingWindowIterator const& ) = default;
    SlidingWindowIterator& operator= ( SlidingWindowIterator&& )      = default;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    /**
     * @brief Get the non-mutable WindowType of this SlidingWindowIterator.
     */
    WindowType window_type() const
    {
        return window_type_;
    }

    /**
     * @brief Get the non-mutable width of this SlidingWindowIterator.
     *
     * With WindowType::kInterval, this is the length of the interval, determining the first and
     * last position in each Window. With WindowType::kVariants instead, this is the number of
     * variants (SNPs or VCF records/lines) per Window.
     */
    size_t width() const
    {
        return width_;
    }

    /**
     * @brief Get the non-mutable stride of this SlidingWindowIterator.
     *
     * With WindowType::kInterval, this is the shift towards the next interval, determining how the
     * first and last position in each Window change. With WindowType::kVariants instead, this is
     * the number of variants (SNPs or VCF records/lines) per Window that we dequeue and enqueue.
     */
    size_t stride() const
    {
        return stride_;
    }

    /**
     * @brief Get the WindowAnchorType that we use for the emitted Window%s.
     */
    WindowAnchorType anchor_type() const
    {
        return window_.anchor_type();
    }

    /**
     * @brief Set the WindowAnchorType that we use for the emitted Window%s.
     */
    void anchor_type( WindowAnchorType value )
    {
        window_.anchor_type( value );
    }

    // -------------------------------------------------------------------------
    //     Accessors & Modifiers
    // -------------------------------------------------------------------------

    bool is_first_window() const
    {
        return is_first_window_;
    }

    bool is_last_window() const
    {
        return is_last_window_;
    }

    // /**
    //  * @brief Get the chromosome name that we are currently processing.
    //  *
    //  * Initially, this is empty. After enqueuing data, it contains the chromosome name of the last
    //  * Data entry that was enqueued.
    //  */
    // std::string const& chromosome() const
    // {
    //     // We could keep our own chromosome here, but Window already as a member for this,
    //     // so we just re-use.
    //     return window_.chromosome();
    // }

    // -------------------------------------------------------------------------
    //     Basic Iterator Operators
    // -------------------------------------------------------------------------

    value_type const& operator*() const
    {
        return window_;
    }

    value_type const* operator->() const
    {
        return &window_;
    }

    // -------------------------------------------------------------------------
    //     Iteration
    // -------------------------------------------------------------------------

    self_type& operator ++ ()
    {
        increment_();
        return *this;
    }

    self_type operator ++(int)
    {
        self_type it(*this);
        ++(*this);
        return it;
    }

    // -------------------------------------------------------------------------
    //     Iterator Comparison
    // -------------------------------------------------------------------------

    // bool operator==( self_type const& it ) const
    // {
    //     return current_ == it.current_;
    // }
    //
    // bool operator!=( self_type const& it ) const
    // {
    //     return !(*this == it);
    // }

    explicit operator bool() const
    {
        return current_ != end_ || is_last_window_;
    }

    bool good() const
    {
        return current_ != end_ || is_last_window_;
    }

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    void increment_()
    {
        // Special case: If we have no more data, the iterator still needs to stop at the last
        // window, so that it can be processed. Hence, the operator bool() checks for this condition
        // by testing for is_last_window_. After that, when this increment_() function is called
        // again by the user, we then set is_last_window_ to false, indicating that now we are
        // done for good.
        if( current_ == end_ ) {
            // If current_ == end_, we have definitely reached the end of the input, so we need
            // to have set is_last_window_ previously. If not set, that means it was already reset,
            // so that this is an iteration past the end.
            if( ! is_last_window_ ) {
                throw std::runtime_error( "SlidingWindowIterator: Incrementing past the end" );
            }

            // Now reset, so that operator bool() returns false, indicating that we are done.
            is_last_window_ = false;
            return;
        }

        // Check if this call moves to the next chromosome.
        if( chromosome_function_( *current_ ) != window_.chromosome() ) {
            init_chromosome_();
        } else {
            // Update positions.
            current_start_ += stride_;
            is_first_window_ = false;
        }

        update_();
    }

    void init_chromosome_()
    {
        // Saveguard. This might be called on an empty range, in which case we just do nothing.
        if( current_ == end_ ) {
            return;
        }

        // Clear the window and prepare for new chromsome.
        window_.clear();
        window_.chromosome( chromosome_function_( *current_ ) );
        is_first_window_ = true;
        is_last_window_ = false;
        next_index_ = 0;

        if( emit_leading_empty_windows_ ) {
            current_start_ = 0;
        } else {
            current_start_ = position_function_( *current_ );
        }
    }

    void update_()
    {
        // Do the correct type of enqueuing.
        if( window_type_ == WindowType::kInterval ) {
            update_interval_();
        } else if( window_type_ == WindowType::kVariants ) {
            throw std::runtime_error( "Not yet implemented" );
        } else {
            throw std::runtime_error( "Invalid WindowType" );
        }
    }

    void update_interval_()
    {
        // Dequeue everything that is not part of the current interval any more.
        while(
            ! window_.entries().empty() && window_.entries().front().position < current_start_
        ) {
            window_.entries().pop_front();
        }

        // Now enqueue new entries.
        while( current_ != end_ ) {
            // Get the chromosome and position of the current entry, and see if it belongs
            // into the current window. If not, we are done here with this window.
            if(
                chromosome_function_( *current_ ) != window_.chromosome() ||
                position_function_( *current_ ) >= current_start_ + width_
            ) {
                is_last_window_ = true;
                break;
            }

            // Now enqueue the entry, and move to the next.
            window_.entries().emplace_back(
                next_index_, position_function_( *current_ ), entry_input_function_( *current_ )
            );
            ++next_index_;
            ++current_;
        }

        // Another case in which we are at the last window.
        if( current_ == end_ ) {
            is_last_window_ = true;
        }

        // Update the window positions.
        window_.first_position( current_start_ );
        window_.last_position( current_start_ + width_ );
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // Fixed settings
    WindowType window_type_;
    size_t width_;
    size_t stride_;

    // Other settings
    bool emit_leading_empty_windows_ = false;

    // Current window and its position
    size_t current_start_ = 0;
    size_t next_index_ = 0;
    Window window_;

    // Need to manually keep track of those...
    bool is_first_window_ = true;
    bool is_last_window_ = false;

    // Underlying iterator
    ForwardIterator begin_;
    ForwardIterator current_;
    ForwardIterator end_;

    // Functionals to go from iterator to our needed data
    EntryInputFunctor entry_input_function_;
    ChromosomeFunctor chromosome_function_;
    PositionFunctor   position_function_;

};

// =================================================================================================
//     Make Genomic Sliding Window Iterator
// =================================================================================================

template<class Data, class ForwardIterator, typename EntryInputFunctor, typename ChromosomeFunctor, typename PositionFunctor>
SlidingWindowIterator<Data, ForwardIterator, EntryInputFunctor, ChromosomeFunctor, PositionFunctor>
make_sliding_window_iterator(
    ForwardIterator begin, ForwardIterator end,
    EntryInputFunctor const& entry_function,
    ChromosomeFunctor const& chromosome_function,
    PositionFunctor const& position_function,
    WindowType type, size_t width, size_t stride = 0, bool emit_leading_empty_windows = false
) {
    // using D = typename std::result_of<decltype(entry_function)()>::type;
    using SWI = SlidingWindowIterator<
        Data, ForwardIterator, EntryInputFunctor, ChromosomeFunctor, PositionFunctor
    >;
    return SWI(
        begin, end, entry_function, chromosome_function, position_function,
        type, width, stride, emit_leading_empty_windows
    );
}

// template<class ForwardIterator, typename EntryInputFunctor, typename PositionFunctor>
// utils::Range<SlidingWindowIterator<ForwardIterator, EntryInputFunctor, PositionFunctor>>
// make_sliding_window_range(
//     ForwardIterator begin, ForwardIterator end,
//     EntryInputFunctor const& entry_function, PositionFunctor const& position_function,
//     WindowType type, size_t width, size_t stride = 0, bool emit_leading_empty_windows = false
// ) {
//     using T = SlidingWindowIterator<ForwardIterator, EntryInputFunctor, PositionFunctor>;
//     return utils::Range<T>(
//         T(
//             begin, end, entry_function, position_function,
//             type, width, stride, emit_leading_empty_windows
//         ),
//         T(
//             end, end, entry_function, position_function,
//             type, width, stride, emit_leading_empty_windows
//         )
//     );
// }

} // namespace population
} // namespace genesis

#endif // include guard

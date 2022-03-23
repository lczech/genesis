#ifndef GENESIS_POPULATION_WINDOW_SLIDING_WINDOW_ITERATOR_H_
#define GENESIS_POPULATION_WINDOW_SLIDING_WINDOW_ITERATOR_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2022 Lucas Czech

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

#include "genesis/population/window/window.hpp"
#include "genesis/utils/containers/range.hpp"

#include <cassert>
#include <functional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Sliding Window Type
// =================================================================================================

/**
 * @brief SlidingWindowType of a Window, that is, whether we slide along a fixed size interval of
 * the genome, along a fixed number of variants, or represents a whole chromosome.
 */
enum class SlidingWindowType
{
    /**
     * @brief Windows of this type are defined by a fixed start and end position on a chromosome.
     *
     * The amount of data contained in between these two loci can differ, depending on the number of
     * variant positions found in the underlying data iterator.
     */
    kInterval,

    /**
     * @brief Windows of this type are defined as containing a fixed number of entries (usually,
     * Variant%s or other data that), and hence can span window widths of differing sizes.
     */
    kVariants,

    /**
     * @brief Windows of this type contain positions across a whole chromosome.
     *
     * The window contains all data from a whole chromosome. Moving to the next window then is
     * equivalent to moving to the next chromosome. Note that this might need a lot of memory
     * to keep all the data at once.
     */
    kChromosome
};

// =================================================================================================
//     Sliding Window Iterator
// =================================================================================================

/**
 * @brief Iterator for sliding Window%s over the chromosomes of a genome.
 *
 * The template parameters are:
 *  * `ForwardIterator`: The type of the underlying iterator over the genome data (that is, the input
 *     iterator from which the sliding window takes its data). Needs to have a member type
 *     `value_type` that specifies the actual input type that the iterator produces, which we here
 *     call the `InputType` (and typedef it as that).
 *  * `DataType`: The data type of the Window::Data that is stored in Window::Entry. The functor
 *    `entry_input_function` needs to be provided to convert from `InputType` to this `DataType`.
 *    By default, we take this to be the same as the `InputType`, meaning that the Window contains
 *    the same data type as the underlying iterator that we slide over.
 *
 * The three functors
 *
 *  * entry_input_function,
 *  * chromosome_function, and
 *  * position_function
 *
 * have to be set in the class prior to starting the iteration. Furthermore, except when using
 * #SlidingWindowType::kChromosome, the width() has to be set as well.
 * All other settings are optional and/or defaulted to reasonable values.
 */
template<class ForwardIterator, class DataType = typename ForwardIterator::value_type>
class SlidingWindowIterator
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    // using Data              = typename ForwardIterator::value_type;
    // using Data              = typename std::result_of<decltype(EntryInputFunctor)()>::type;

    using Window            = ::genesis::population::Window<DataType>;
    using Entry             = typename Window::Entry;
    using InputType         = typename ForwardIterator::value_type;

    using self_type         = SlidingWindowIterator<ForwardIterator, DataType>;
    using iterator_category = std::input_iterator_tag;

    using value_type        = Window;
    using pointer           = value_type*;
    using reference         = value_type&;
    using const_reference   = value_type const&;

    // -------------------------------------------------------------------------
    //     Public Functors
    // -------------------------------------------------------------------------

    /**
     * @brief Functor to convert from the underlying input iterator that provides the data
     * for the sliding window to the data that is stored per window.
     */
    std::function<DataType( InputType const& )> entry_input_function;

    /**
     * @brief Functor that yields the current chromosome, given the input iterator data.
     */
    std::function<std::string( InputType const& )> chromosome_function;

    /**
     * @brief Functor that yields the current position on the chromosome,
     * given the input iterator data.
     */
    std::function<size_t( InputType const& )> position_function;

    // ======================================================================================
    //      Internal Iterator
    // ======================================================================================

    /**
     * @brief Internal iterator that produces Window%s.
     */
    class Iterator
    {
    public:

        // -------------------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------------------

        using Window            = ::genesis::population::Window<DataType>;
        using Entry             = typename Window::Entry;

        using self_type         = SlidingWindowIterator<ForwardIterator, DataType>::Iterator;
        using iterator_category = std::input_iterator_tag;

        using value_type        = Window;
        using pointer           = value_type*;
        using reference         = value_type&;
        using const_reference   = value_type const&;

    private:

        Iterator() = default;

        Iterator(
            SlidingWindowIterator const* parent
        )
            : parent_(  parent )
        {
            // We use the parent as a check if this Iterator is intended to be a begin()
            // or end() iterator. If its the former, init. If the latter, we are done here.
            // After we are done iterating the input (for which we do need _its_ begin and end
            // iterators), we then set the parent_ to nullptr, as a sign that we are done.
            // This allows us also to know if we reached end() (of the sliding window iteration;
            // not of the underlying data iterator) without having to store the end() iterator
            // when using this class.
            if( ! parent_ ) {
                return;
            }

            // Check that the functors are set up.
            if( ! parent_->entry_input_function ) {
                throw std::runtime_error(
                    "Need to set SlidingWindowIterator::entry_input_function before iterating "
                    "over Windows with the SlidingWindowIterator."
                );
            }
            if( ! parent_->chromosome_function ) {
                throw std::runtime_error(
                    "Need to set SlidingWindowIterator::chromosome_function before iterating "
                    "over Windows with the SlidingWindowIterator."
                );
            }
            if( ! parent_->position_function ) {
                throw std::runtime_error(
                    "Need to set SlidingWindowIterator::position_function before iterating "
                    "over Windows with the SlidingWindowIterator."
                );
            }

            // Check the settings.
            if( parent_->width_ == 0 ) {
                throw std::runtime_error( "Cannot use SlidingWindowIterator of width 0." );
            }
            if( parent_->stride_ == 0 ) {
                parent_->stride_ = parent_->width_;
            }
            if( parent_->stride_ > parent_->width_ ) {
                throw std::runtime_error( "Cannot use SlidingWindowIterator with stride > width." );
            }

            // Copy over the underlying data iterator.
            current_ = parent_->begin_;
            end_     = parent_->end_;

            // Let's get going.
            init_chromosome_();
            update_();
        }

    public:

        ~Iterator() = default;

        Iterator( self_type const& ) = default;
        Iterator( self_type&& )      = default;

        Iterator& operator= ( self_type const& ) = default;
        Iterator& operator= ( self_type&& )      = default;

        friend SlidingWindowIterator;

        // -------------------------------------------------------------------------
        //     Properties
        // -------------------------------------------------------------------------

        /**
         * @brief Return whether the current iteration is the first of the current chromosome.
         *
         * When iterating over (e.g.) a VCF file with multiple chromosomes, this function is useful
         * to run some initialization per chromosome in the user code, such as preparing some output.
         *
         * See is_last_window() for the respective end-of-chromosome indicator, that can be used
         * to wrap up after a chromosome, such as writing the output that was producing during the
         * iterator.
         */
        bool is_first_window() const
        {
            return is_first_window_;
        }

        /**
         * @brief Return whether the current iteration is the last of the current chromosome.
         *
         * When iterating over (e.g.) a VCF file with multiple chromosomes, this function is useful
         * to wrap up after a chromosome in the user code, such as writing the output that was
         * producing during the iterator.
         *
         * See is_first_window() for the respective beginning-of-chromosome indicator, that can be
         * used to run some initialization per chromosome, such as preparing some output.
         */
        bool is_last_window() const
        {
            return is_last_window_;
        }

        // -------------------------------------------------------------------------
        //     Accessors
        // -------------------------------------------------------------------------

        value_type const & operator*() const
        {
            return window_;
        }

        value_type & operator*()
        {
            return window_;
        }

        value_type const * operator->() const
        {
            return &window_;
        }

        value_type * operator->()
        {
            return &window_;
        }

        // -------------------------------------------------------------------------
        //     Iteration
        // -------------------------------------------------------------------------

        self_type& operator ++()
        {
            increment_();
            return *this;
        }

        // self_type operator ++(int)
        // {
        //     auto cpy = *this;
        //     increment_();
        //     return cpy;
        // }

        /**
         * @brief Compare two iterators for equality.
         *
         * Any two iterators that are copies of each other or started from the same parent
         * will compare equal, as long as neither of them is past-the-end.
         * A valid (not past-the-end) iterator and an end() iterator will not compare equal,
         * no matter from which SlidingWindowIterator they were created.
         * Two past-the-end iterators compare equal.
         */
        bool operator==( self_type const& other ) const
        {
            // We compare the parents as a baseline - two past-the-end iterator shall
            // always compare equal. If only one of them is past-the-end, they will compare false.
            return parent_ == other.parent_;
        }

        bool operator!=( self_type const& other ) const
        {
            return !(*this == other);
        }

        // -------------------------------------------------------------------------
        //     Internal Members
        // -------------------------------------------------------------------------

    private:

        void init_chromosome_()
        {
            // Saveguard. This might be called on an empty range, in which case we just do nothing.
            if( current_ == end_ ) {
                return;
            }

            // Clear the window and prepare for new chromosome.
            window_.clear();
            window_.chromosome( parent_->chromosome_function( *current_ ));
            is_first_window_ = true;
            is_last_window_ = false;
            next_index_ = 0;

            if( parent_->emit_leading_empty_windows_ ) {
                current_start_ = 1;
            } else {
                // Set the start to the window position that we would get after going through all
                // the previous windows if they were emitted.
                auto const pos = parent_->position_function( *current_ );
                current_start_ = pos - (( pos - 1 ) % parent_->stride_ );
            }
        }

        void increment_()
        {
            // Special case: If we have no more underlying data, the iterator still needs to stop
            // at the last window(s), so that they can be processed. After that, when this
            // increment_() function is called again by the user, we then set parent_ = nullptr
            // to indicate that now we are done for good.
            if( current_ == end_ ) {
                // If current_ == end_, we have definitely reached the end of the input, so we need
                // to have set is_last_window_ previously. If not set, that means it was already reset,
                // so that this is an iteration past the end.
                if( ! is_last_window_ ) {
                    throw std::runtime_error( "SlidingWindowIterator: Incrementing past the end" );
                }

                // Indicate that we are done now.
                parent_ = nullptr;
                return;
            }

            // Check if this call moves to the next chromosome.
            if( parent_->chromosome_function( *current_ ) != window_.chromosome() ) {
                init_chromosome_();
            } else {
                // Update positions.
                current_start_ += parent_->stride_;
                is_first_window_ = false;
            }

            update_();
        }

        void update_()
        {
            // Do the correct type of enqueuing.
            if( parent_->window_type_ == SlidingWindowType::kInterval ) {
                update_interval_();
            } else if( parent_->window_type_ == SlidingWindowType::kVariants ) {
                update_variants_();
            } else if( parent_->window_type_ == SlidingWindowType::kChromosome ) {
                update_chromosome_();
            } else {
                throw std::runtime_error( "Invalid SlidingWindowType" );
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
                auto const cur_pos = parent_->position_function( *current_ );

                // Get the chromosome and position of the current entry, and see if it belongs
                // into the current window. If not, we are done here with this window.
                if(
                    parent_->chromosome_function( *current_ ) != window_.chromosome() ||
                    cur_pos >= current_start_ + parent_->width_
                ) {
                    break;
                }
                assert( parent_->chromosome_function( *current_ ) == window_.chromosome() );
                assert( cur_pos >= current_start_ );
                assert( cur_pos <  current_start_ + parent_->width_ );

                // Check that we are not going backwards in the chromosome,
                // i.e., if we got unsorted data. That would lead to unwanted behaviour.
                if(
                    ! window_.empty() &&
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
                    parent_->entry_input_function( *current_ )
                );
                ++next_index_;
                ++current_;
            }

            // Cases in which we are at the last window: Either we reached the end of the input,
            // or the end of the current chromosome.
            if(
                current_ == end_ ||
                parent_->chromosome_function( *current_ ) != window_.chromosome()
            ) {
                is_last_window_ = true;
            }

            // Update the window positions.
            window_.first_position( current_start_ );
            window_.last_position( current_start_ + parent_->width_ - 1 );
        }

        void update_variants_()
        {
            throw std::runtime_error( "Not yet implemented" );
        }

        void update_chromosome_()
        {
            throw std::runtime_error( "Not yet implemented" );
        }

    private:

        // Parent.
        SlidingWindowIterator const* parent_ = nullptr;

        // Current window and its position
        Window window_;
        size_t current_start_ = 1;
        size_t next_index_ = 0;

        // Need to manually keep track of those...
        bool is_first_window_ = true;
        bool is_last_window_ = false;

        // Underlying iterator
        ForwardIterator current_;
        ForwardIterator end_;

    };

    // ======================================================================================
    //      Main Class
    // ======================================================================================

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    SlidingWindowIterator(
        ForwardIterator begin, ForwardIterator end
    )
        : begin_(begin)
        , end_(end)
    {}

    ~SlidingWindowIterator() = default;

    SlidingWindowIterator( SlidingWindowIterator const& ) = default;
    SlidingWindowIterator( SlidingWindowIterator&& )      = default;

    SlidingWindowIterator& operator= ( SlidingWindowIterator const& ) = default;
    SlidingWindowIterator& operator= ( SlidingWindowIterator&& )      = default;

    friend Iterator;

    // -------------------------------------------------------------------------
    //     Iteration
    // -------------------------------------------------------------------------

    Iterator begin()
    {
        return Iterator( this );
    }

    Iterator end()
    {
        return Iterator( nullptr );
    }

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    /**
     * @brief Type of the Window, that is, whether to iterate over intervals of fixed length,
     * over a certain number of variants/entries per Window, or over whole chromosomes.
     */
    self_type& window_type( SlidingWindowType value )
    {
        window_type_ = value;
        return *this;
    }

    SlidingWindowType window_type() const
    {
        return window_type_;
    }

    /**
     * @brief Width of the Window, either in fixed length along the chromosome, or in number
     * of variants/entries per Window, depending on the setting for SlidingWindowType.
     *
     * The width has to be `> 0`. With SlidingWindowType::kInterval, this is the length of the
     * interval, determining the first and last position in each Window.
     * With SlidingWindowType::kVariants instead, this is the number of variants (SNPs or VCF
     * records/lines) per Window.
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
     * @brief Stride of the Window, that is, how many positions or entries
     * (depending on SlidingWindowType) to move forward with each iteration step.
     *
     * The stride has to be `<= width`.
     * If stride is set to 0 (default here), it is set automatically to the width when constructing
     * a SlidingWindowIterator with this settings object, which means, we create windows that do
     * not overlap.
     *
     * With SlidingWindowType::kInterval, this is the shift towards the next interval, determining
     * how the first and last position in each Window change. With SlidingWindowType::kVariants
     * instead, this is the number of variants (SNPs or VCF records/lines) per Window that we
     * dequeue and enqueue. Not used with SlidingWindowType::kChromosome.
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
     * @brief Select whether the iterator produces empty windows in the beginning of each chromosome,
     * before the first actual position that is reported by the underlying data.
     *
     * Only used with SlidingWindowType::kInterval.
     *
     * Say the underlying iterator has the first Variant (or whatever datatype it iterates over)
     * at position 1020 for a chromosome, and we use a window size of 100. If this setting is set
     * to `true`, the SlidingWindowIterator will emit 10 empty windows before reaching this position.
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
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // Underlying iterator
    ForwardIterator begin_;
    ForwardIterator end_;

    // Settings. We make stride_ mutable so that the iterator can set it to the width.
    SlidingWindowType window_type_ = SlidingWindowType::kInterval;
    size_t width_ = 0;
    mutable size_t stride_ = 0;

    bool emit_leading_empty_windows_ = true;

    // bool emit empty_windows = true;
    // bool emit_unfinished_trailing_window = false;

};

// =================================================================================================
//     Make Sliding Window Iterator
// =================================================================================================

/**
 * @brief Helper function to instantiate a SlidingWindowIterator
 * without the need to specify the template parameters manually.
 */
template<class ForwardIterator, class DataType = typename ForwardIterator::value_type>
SlidingWindowIterator<ForwardIterator, DataType>
make_sliding_window_iterator(
    ForwardIterator begin, ForwardIterator end,
    size_t width = 0, size_t stride = 0, SlidingWindowType window_type = SlidingWindowType::kInterval
) {
    auto it = SlidingWindowIterator<ForwardIterator, DataType>( begin, end );
    it.width( width );
    it.stride( stride );
    it.window_type( window_type );
    return it;
}

/**
 * @brief Helper function to instantiate a SlidingWindowIterator for a default use case.
 *
 * This helper assumes that the underlying type of the input data stream and of the Window%s
 * that we are sliding over are of the same type, that is, we do no conversion in the
 * `entry_input_function` functor of the SlidingWindowIterator. It further assumes that this data
 * type has public member variables `chromosome` and `position` that are accessed by the
 * `chromosome_function` and `position_function` functors of the SlidingWindowIterator.
 * A data type that is works for is Variant data.
 */
template<class ForwardIterator>
SlidingWindowIterator<ForwardIterator>
make_default_sliding_window_iterator(
    ForwardIterator begin, ForwardIterator end,
    size_t width = 0, size_t stride = 0, SlidingWindowType window_type = SlidingWindowType::kInterval
) {
    using DataType = typename ForwardIterator::value_type;

    auto it = SlidingWindowIterator<ForwardIterator>( begin, end );
    it.entry_input_function = []( DataType const& variant ) {
        return variant;
    };
    it.chromosome_function = []( DataType const& variant ) {
        return variant.chromosome;
    };
    it.position_function = []( DataType const& variant ) {
        return variant.position;
    };
    it.width( width );
    it.stride( stride );
    it.window_type( window_type );
    return it;
}

} // namespace population
} // namespace genesis

#endif // include guard

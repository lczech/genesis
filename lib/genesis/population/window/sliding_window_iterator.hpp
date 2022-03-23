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
//     Sliding Window Iterator Settings
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
     * The amount of data contain in between these two loci can differ, for example, the number of
     * variant positions
     */
    kInterval,

    /**
     * @brief Windows of this type are defined as containing a fixed number of entries (usually,
     * Variant%s or other data that )
     */
    kVariants,

    /**
     * @brief Windows of this type contain positions across a whole chromosome.
     *
     * The window contains all relevant data from a whole chromosome. Relevant can be either all
     * positions, only variant ones, or some other filter can be applied when filling the window,
     * as needed. Moving to the next window then is equivalent to moving to the next chromosome.
     */
    kChromosome
};

/**
 * @brief Settings for running a sliding window iteration.
 *
 * The SlidingWindowIterator takes a lot of settings, and providing all of them in its constructor
 * is cumbersome and error prone, so instead we offer this convenient settings class to all
 * prepare them beforehand. This allows to set all values by their name, instead of just by their
 * position in the constructor.
 *
 * The template parameters are:
 *  * `InputType`: The input data type that the basis iterator provided (that is, the input iterator
 *    from which the sliding window takes its data),
 *  * `DataType`: The data type of the Window::Data that is stored in Window::Entry. The functor
 *    `entry_input_function` needs to be provided to convert from `InputType` to this `DataType`.
 *
 * The `width`, as well as the three functors
 *
 *  * entry_input_function,
 *  * chromosome_function, and
 *  * position_function
 *
 * have to be set in this settings class prior to using it to initialize a SlidingWindowIterator.
 * All other settings are option and/or defaulted to reasonable values.
 */
template<class InputType, class DataType = InputType>
struct SlidingWindowIteratorSettings
{
    // -------------------------------------------------------------------------
    //     Functors
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
     * @brief Functor that yields the current position on the chromosome, given the input iterator
     * data.
     */
    std::function<size_t( InputType const& )> position_function;

    // -------------------------------------------------------------------------
    //     Basic Settings
    // -------------------------------------------------------------------------

    /**
     * @brief Type of the Window, that is, whether to iterate over intervals of fixed length,
     * over a certain number of variants/entries per Window, or over whole chromosomes.
     */
    SlidingWindowType window_type = SlidingWindowType::kInterval;

    /**
     * @brief Width of the Window, either in fixed length along the chromosome, or in number
     * of variants/entries per Window, depending on the setting for SlidingWindowType.
     *
     * The width has to be `> 0`. With SlidingWindowType::kInterval, this is the length of the
     * interval, determining the first and last position in each Window.
     * With SlidingWindowType::kVariants instead, this is the number of variants (SNPs or VCF
     * records/lines) per Window.
     */
    size_t width = 0;

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
    size_t stride = 0;

    // -------------------------------------------------------------------------
    //     Extended and Detail Settings
    // -------------------------------------------------------------------------

    /**
     * @brief Select whether the iterator produces empty windows in the beginning of each chromosome,
     * before the first actual position that is reported by the underlying data.
     *
     * Only used with SlidingWindowType::kInterval.
     *
     * Say the underlying iterator has the first Variant (or whatever datatype it iterates over)
     * at position 1020 for a chromosome, and we use a window size of 100. If this setting is set
     * to `true`, the SlidingWindowIterator will emit 10 empty windows before reaching this position.
     * If set to `false`, it will skip these, and start at position 1000, which is the first one
     * that would have been reached by striding along the chromosome.
     */
    bool emit_leading_empty_windows = true;

    // bool emit empty_windows = true;
    // bool emit_unfinished_trailing_window = false;
    //
    // all three of the above could also be an int enum where differnt settings
    // can be combined as binary flags. kEmitAllEmptyWindows would then be 111
    //
    // in the doc, write a note that all of them are only used for interval windows.
};

// =================================================================================================
//     Sliding Window Iterator
// =================================================================================================

/**
 * @brief Iterator for sliding Window%s over the chromosomes of a genome.
 */
template<class ForwardIterator, class InputType, class DataType = InputType>
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

    using settings_type     = SlidingWindowIteratorSettings<InputType, DataType>;
    using self_type         = SlidingWindowIterator<ForwardIterator, InputType, DataType>;
    using value_type        = Window;
    using reference         = value_type&;
    using const_reference   = value_type const&;

    // using difference_type   = typename Window::container::difference_type;
    // using size_type         = typename Window::container::size_type;
    using iterator_category = std::input_iterator_tag;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    SlidingWindowIterator(
        settings_type const& settings,
        ForwardIterator begin, ForwardIterator end
    )
        : settings_(settings)
        , current_(begin)
        , end_(end)
    {
        // Some boundary checks.
        if( settings_.width == 0 ) {
            throw std::runtime_error( "Cannot use SlidingWindowIterator of width 0." );
        }
        if( settings_.stride == 0 ) {
            settings_.stride = settings_.width;
        }
        if( settings_.stride > settings_.width ) {
            throw std::runtime_error( "Cannot use SlidingWindowIterator with stride > width." );
        }

        // Error checking.
        if( ! settings_.entry_input_function ) {
            throw std::runtime_error(
                "Need to set SlidingWindowIteratorSettings::entry_input_function before using it "
                "to construct a SlidingWindowIterator"
            );
        }
        if( ! settings_.chromosome_function ) {
            throw std::runtime_error(
                "Need to set SlidingWindowIteratorSettings::chromosome_function before using it "
                "to construct a SlidingWindowIterator"
            );
        }
        if( ! settings_.position_function ) {
            throw std::runtime_error(
                "Need to set SlidingWindowIteratorSettings::position_function before using it "
                "to construct a SlidingWindowIterator"
            );
        }

        // Let's get going.
        init_chromosome_();
        update_();
    }

    // SlidingWindowIterator(
    //     settings_type const& settings,
    //     ForwardIterator begin, ForwardIterator end,
    //     std::string const& chromosome,
    //     size_t start_position, size_t end_position
    // )
    //     : settings_(settings)
    //     , current_(begin)
    //     , end_(end)
    // {
    //     // Some boundary checks.
    //     if( settings_.width == 0 ) {
    //         throw std::runtime_error( "Cannot use SlidingWindowIterator of width 0." );
    //     }
    //     if( settings_.stride == 0 ) {
    //         settings_.stride = settings_.width;
    //     }
    //     if( settings_.stride > settings_.width ) {
    //         throw std::runtime_error( "Cannot use SlidingWindowIterator with stride > width." );
    //     }
    //
    //     // Let's get going.
    //     init_chromosome_();
    //     update_();
    // }

    ~SlidingWindowIterator() = default;

    SlidingWindowIterator( SlidingWindowIterator const& ) = default;
    SlidingWindowIterator( SlidingWindowIterator&& )      = default;

    SlidingWindowIterator& operator= ( SlidingWindowIterator const& ) = default;
    SlidingWindowIterator& operator= ( SlidingWindowIterator&& )      = default;

    // -------------------------------------------------------------------------
    //     Accessors & Modifiers
    // -------------------------------------------------------------------------

    /**
     * @brief Return whether the current iteration is the first of the current chromosome.
     *
     * When iterating over (e.g.) a VCF file with multiple chromosomes, this function is useful
     * to run some initialization per chromosome, such as preparing some output.
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
     * to wrap up after a chromosome, such as writing the output that was producing during the
     * iterator.
     *
     * See is_first_window() for the respective beginning-of-chromosome indicator, that can be used
     * to run some initialization per chromosome, such as preparing some output.
     */
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

    void init_chromosome_()
    {
        // Saveguard. This might be called on an empty range, in which case we just do nothing.
        if( current_ == end_ ) {
            return;
        }

        // Clear the window and prepare for new chromosome.
        window_.clear();
        window_.chromosome( settings_.chromosome_function( *current_ ));
        is_first_window_ = true;
        is_last_window_ = false;
        next_index_ = 0;

        if( settings_.emit_leading_empty_windows ) {
            current_start_ = 1;
        } else {
            // Set the start to the window position that we would get after going through all
            // the previous windows if they were emitted.
            auto const pos = settings_.position_function( *current_ );
            current_start_ = pos - (( pos - 1 ) % settings_.stride );
        }
    }

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
        if( settings_.chromosome_function( *current_ ) != window_.chromosome() ) {
            init_chromosome_();
        } else {
            // Update positions.
            current_start_ += settings_.stride;
            is_first_window_ = false;
        }

        update_();
    }

    void update_()
    {
        // Do the correct type of enqueuing.
        if( settings_.window_type == SlidingWindowType::kInterval ) {
            update_interval_();
        } else if( settings_.window_type == SlidingWindowType::kVariants ) {
            update_variants_();
        } else if( settings_.window_type == SlidingWindowType::kChromosome ) {
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
            auto const cur_pos = settings_.position_function( *current_ );

            // Get the chromosome and position of the current entry, and see if it belongs
            // into the current window. If not, we are done here with this window.
            if(
                settings_.chromosome_function( *current_ ) != window_.chromosome() ||
                cur_pos >= current_start_ + settings_.width
            ) {
                break;
            }
            assert( settings_.chromosome_function( *current_ ) == window_.chromosome() );

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
                settings_.entry_input_function( *current_ )
            );
            ++next_index_;
            ++current_;
        }

        // Cases in which we are at the last window: Either we reached the end of the input,
        // or the end of the current chromosome.
        if( current_ == end_ || settings_.chromosome_function( *current_ ) != window_.chromosome() ) {
            is_last_window_ = true;
        }

        // Update the window positions.
        window_.first_position( current_start_ );
        window_.last_position( current_start_ + settings_.width - 1 );
    }

    void update_variants_()
    {
        throw std::runtime_error( "Not yet implemented" );
    }

    void update_chromosome_()
    {
        throw std::runtime_error( "Not yet implemented" );
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // All combined settings for the window
    settings_type settings_;

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

// =================================================================================================
//     Make Genomic Sliding Window Iterator
// =================================================================================================

/**
 * @brief Helper function to instanciate a SlidingWindowIterator without the need to specify
 * all template parameters manually.
 */
template<class ForwardIterator, class InputType, class DataType = InputType>
SlidingWindowIterator<ForwardIterator, InputType, DataType>
make_sliding_window_iterator(
    SlidingWindowIteratorSettings<InputType, DataType> const& settings,
    ForwardIterator begin, ForwardIterator end
) {
    return SlidingWindowIterator<ForwardIterator, InputType, DataType>(
        settings, begin, end
    );
}

} // namespace population
} // namespace genesis

#endif // include guard

#ifndef GENESIS_POPULATION_WINDOW_SLIDING_WINDOW_GENERATOR_H_
#define GENESIS_POPULATION_WINDOW_SLIDING_WINDOW_GENERATOR_H_

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

#include <cassert>
#include <deque>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

#include "genesis/population/window/window.hpp"

namespace genesis {
namespace population {

// =================================================================================================
//     Genomic Sliding Window Generator
// =================================================================================================

/**
 * @brief Generator for sliding Window%s over the chromosomes of a genome.
 *
 * The class allows to accumulate and compute arbitrary data within a sliding window over
 * a genome. The basic setup is to provide a set of plugin functions that do the actual computation,
 * and then feed the data in via the enqueue() functions. The SlidingWindowGenerator class then
 * takes care of calling the respective plugin functions to compute values and emit results once a
 * Window is finished.
 *
 * To this end, the SlidingWindowGenerator takes care of collecting the data (whose type is given
 * via the template parameter `D`/`Data`) in a list of @link Window::Entry Entry@endlink instances
 * per Window. For each finished window, the on_emission plugin functions are called, which
 * typically are set by the user code to compute and store/print/visualize a per-window summary of
 * the `Data`. Use the add_emission_plugin() function to add such plugin functions.
 *
 * A typical use case for this class is a window over the variants that are present in a set
 * of (pooled) individuals, for example, the records/lines of a VCF file. Each record would then
 * form a Data @link Window::Entry Entry@endlink, and some summary of a window along the positions
 * in the VCF file would be computed per Window. As those files can potentially contain multiple
 * chromosomes, we also support that. In this case, the Window is "restarted" at the beginning of
 * a new chromosome.
 *
 * This however necessitates to finish each chromosome properly when sliding over intervals.
 * This is because the Window cannot know how long a chromosome is from just the variants in the VCF
 * file (there might just not be any variants at the end of a chromosome, but we still want our
 * interval to cover these positions). Instead, we need the total chromosome length from somewhere
 * else - typically this is provided in the VCF header. Use the convenience function run_vcf_window()
 * to automatically take care of this - or see there for an example of how to do this in your
 * own code. See also below in this description for some further details.
 *
 * In some cases (in particular, if a stride is chosen that is less than the window size), it might
 * be advantageous to not compute the summary per window from scratch each time, but instead
 * hold a rolling record while sliding - that is, to add incrementally the values when they are
 * enqueued, and to remove them once the window moves past their position in the genome.
 * To this end, the second template parameter `A`/`Accumulator` can be used, which can store
 * the necessary intermediate data. For example, to compute some average of values over a window,
 * the Accumulator would need to store a sum of the values and a count of the number of values.
 * In order to update the Accumulator for each Data @link Window::Entry Entry@endlink that is added
 * or removed from the window while sliding, the plugin functions on_enqueue and on_dequeue need to
 * be set accordingly via add_enqueue_plugin() and add_dequeue_plugin().
 *
 * There are two Type%s of sliding window that this class can be used for:
 *
 *  1. For windows of a fixed size along the genome, that is, an interval of a certain number of
 *     basepairs/nucleotides. There may be a varying number of variants (Data Entries) in each such
 *     fixed interval (or none at all).
 *  2. For a fixed number of variants/polymorphisms. Some statistics are not computed over a fixed
 *     size window, but instead for n consecutive variants that can span an interval of varying
 *     size along the genome.
 *
 * Both types are possible here, and have to be determined at construction, along with the width
 * of the Window (either in number of basepairs or in number of variants).
 *
 * Once all data has been processed, finish_chromosome() should be called to emit the last remaining
 * window(s). See the following note for details. Furthermore, in some cases, it might be desirable
 * to emit a window for an incomplete interval or an incomplete numer of variants at the end of a
 * chromosome, while in other cases, these incomplete last entries might need to be skipped.
 * See emit_incomplete_windows() for details.
 *
 * Note:
 * The plugin functions are typically lambdas that might make use of other data from the calling code.
 * However, as this SlidingWindowGenerator class works conceptually similar to a stream, where new
 * data is enqueued in some form of loop or iterative process from the outside by the user, the
 * class cannot know when the process is finished, that is, when the end of the genome is reached.
 * Hence, either finish_chromosome() has to be called once all data has been processed, or it has
 * to be otherwise ensured that the class instance is destructed before the other data that the
 * plugin lambda funtions depend on. This is because the destructor also calls finish_chromosome(),
 * in order to ensure that the last intervals are processed properly. Hence, if any of the functions
 * called from within the plugin functions use data outside of this instance, that data has
 * still to be alive (unless finish_chromosome() was called explicitly before, in which case the
 * destructor does not call it again) - in other words, the instance has to be destroyed after these
 * data.
 */
template<class D, class A = EmptyAccumulator>
class SlidingWindowGenerator
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using Data = D;
    using Accumulator = A;
    using Window = ::genesis::population::Window<D, A>;
    using Entry = typename Window::Entry;

    using self_type = SlidingWindowGenerator<Data, Accumulator>;

    /**
     * @brief Plugin functions that are called on the first enqueue() of a newly started chromosome.
     *
     * Use add_chromosome_start_plugin() to add plugin functions.
     *
     * The purpose of this plugin is to allow to prepare the Window accumulator as needed.
     * Note that it is not immediately called when calling start_chromosome() (which is mostly
     * a decorative function anyway), but instead is called once actual data is added via
     * enqueue(). This is because calling start_chromosome() is optional. Furthermore, we would
     * then also need to call this plugin function from the constructor, but during construction,
     * we do not know the chromosome name yet - hence, the calling is delayed until actual data is
     * seen.
     */
    using on_chromosome_start = std::function<void(
        std::string const& chromosome,
        typename Window::Accumulator& accumulator
    )>;

    /**
     * @brief Plugin functions that are called when finishing a chromosome.
     *
     * Use add_chromosome_finish_plugin() to add plugin functions.
     *
     * The purpose of this plugin is to allow to clean up the accumulator as needed.
     * The function is called when enqueue() is called with a new chromosome name, or when calling
     * finish_chromosome() explictly, or on destruction of the whole class.
     */
    using on_chromosome_finish = std::function<void(
        std::string const& chromosome,
        typename Window::Accumulator& accumulator
    )>;

    /**
     * @brief Plugin functions to update the Accumulator when new Data is enqueued.
     *
     * Use add_enqueue_plugin() to add plugin functions.
     *
     * The purpose of this plugin function is to avoid re-computation of values in a Window if
     * that computation can be done incrementally instead. This is of course mostly helpful if the
     * stride is significantly smaller than the window width. Otherwise (if the stride is equal to
     * the window width, or only a bit smaller), each window will contain more new/different data
     * than re-used data, so incrementally prossing data when it is enqueued AND dequeued again
     * might actually be more computationally expensive.
     *
     * If used, this function is meant to update the Accumulator in a way that adds/incorporates
     * the new data Entry.
     */
    using on_enqueue = std::function<void(
        typename Window::Entry const& entry,
        typename Window::Accumulator& accumulator
    )>;

    /**
     * @brief Plugin functions to update the Accumulator when Data is removed due to the window
     * moving away from it.
     *
     * Use add_dequeue_plugin() to add plugin functions.
     *
     * This is the counterpart for on_enqueue to remove data from the Accumulator once it is no
     * longer part of the current window. See on_enqueue for details.
     */
    using on_dequeue = std::function<void(
        typename Window::Entry const& entry,
        typename Window::Accumulator& accumulator
    )>;

    /**
     * @brief Main plugin functions that are called for every window.
     *
     * Use add_emission_plugin() to add plugin functions.
     *
     * This is the plugin that typically is the most important to set for the user.
     * This is where the data from the Window is processed, that is, where the summary of the window
     * is computed and stored/visualized/plotted. This can either be done by using the Accumulator,
     * or by computing the value based on all the Data Entry values in the window,
     * or a mixture thereof.
     */
    using on_emission = std::function<void(
        Window const& window
    )>;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    // /**
    //  * @brief Create a default (empty) instance.
    //  *
    //  * Not really important for usage, as this cannot do much. Typically, one nees a window width
    //  * and stride for doing any work. But having a default constructor comes in handy at times.
    //  */
    // SlidingWindowGenerator() = default;

    /**
     * @brief Construct a SlidingWindowGenerator, given the WindowType and width, and potentially
     * stride.
     *
     * The @p width has to be `> 0`, and the @p stride has to be `<= width`.
     * If stride is not given (or set to `0`), it is set automatically to the width,
     * which means, we create windows that do not overlap.
     */
    SlidingWindowGenerator( WindowType type, size_t width, size_t stride = 0 )
        : window_type_(type)
        , width_(width)
        , stride_(stride)
    {
        if( width == 0 ) {
            throw std::runtime_error( "Cannot use SlidingWindowGenerator of width 0." );
        }
        if( stride == 0 ) {
            stride_ = width;
        }
        if( stride_ > width_ ) {
            throw std::runtime_error( "Cannot use SlidingWindowGenerator with stride > width." );
        }
    }

    /**
     * @brief Destruct the instance.
     *
     * This typically has to be called before other data storage instances on the user side go out
     * of scope. See the SlidingWindowGenerator class description note for details on why that is
     * the case.
     */
    ~SlidingWindowGenerator()
    {
        finish_chromosome();
    }

    SlidingWindowGenerator( SlidingWindowGenerator const& ) = default;
    SlidingWindowGenerator( SlidingWindowGenerator&& )      = default;

    SlidingWindowGenerator& operator= ( SlidingWindowGenerator const& ) = default;
    SlidingWindowGenerator& operator= ( SlidingWindowGenerator&& )      = default;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    /**
     * @brief Get the non-mutable WindowType of this SlidingWindowGenerator.
     */
    WindowType window_type() const
    {
        return window_type_;
    }

    /**
     * @brief Get the non-mutable width of this SlidingWindowGenerator.
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
     * @brief Get the non-mutable stride of this SlidingWindowGenerator.
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
     * @brief Get whether the last (incomplete) window is also emitted.
     *
     * See emit_incomplete_windows( bool ) for details.
     */
    bool emit_incomplete_windows() const
    {
        return emit_incomplete_windows_;
    }

    /**
     * @brief Set whether the last (incomplete) window is also emitted.
     *
     * For some computations that normalize by window width, this might be  desirable,
     * while in other cases where e.g. absolute per-window numbers are computed, it might
     * not be. Hence, we offer this setting.
     */
    void emit_incomplete_windows( bool value )
    {
        emit_incomplete_windows_ = value;
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

    /**
     * @brief Get the chromosome name that we are currently processing.
     *
     * Initially, this is empty. After enqueuing data, it contains the chromosome name of the last
     * Data entry that was enqueued.
     */
    std::string const& chromosome() const
    {
        // We could keep our own chromosome here, but Window already as a member for this,
        // so we just re-use.
        return window_.chromosome();
    }

    /**
     * @brief Return whether the instance is empty.
     *
     * The Window and SlidingWindowGenerator are empty if no Data has been enqueued for the current
     * chromosome yet.
     */
    bool empty() const
    {
        return next_index_ == 0;
    }

    /**
     * @brief Clear all data of the Window.
     *
     * This can be used to completely forget about the current chromosome, and start afresh.
     * It just clears the data, while keeping all plugins and other settins as they are.
     */
    void clear()
    {
        current_start_ = 0;
        next_index_ = 0;
        window_.clear();
    }

    // -------------------------------------------------------------------------
    //     Event Plugin Functionals
    // -------------------------------------------------------------------------

    /**
     * @brief Add an on_chromosome_start plugin function, typically a lambda.
     */
    self_type& add_chromosome_start_plugin( on_chromosome_start const& plugin )
    {
        chromosome_start_plugins_.push_back(plugin);
        return *this;
    }

    /**
     * @brief Add an on_chromosome_finish plugin function, typically a lambda.
     */
    self_type& add_chromosome_finish_plugin( on_chromosome_finish const& plugin )
    {
        chromosome_finish_plugins_.push_back(plugin);
        return *this;
    }

    /**
     * @brief Add an on_enqueue plugin function, typically a lambda.
     */
    self_type& add_enqueue_plugin( on_enqueue const& plugin )
    {
        enqueue_plugins_.push_back(plugin);
        return *this;
    }

    /**
     * @brief Add an on_dequeue plugin function, typically a lambda.
     */
    self_type& add_dequeue_plugin( on_dequeue const& plugin )
    {
        dequeue_plugins_.push_back(plugin);
        return *this;
    }

    /**
     * @brief Add an on_emission plugin function, typically a lambda.
     */
    self_type& add_emission_plugin( on_emission const& plugin )
    {
        emission_plugins_.push_back(plugin);
        return *this;
    }

    /**
     * @brief Clear all plugin functions.
     *
     * Not sure why this would be needed. But doesn't hurt to have it.
     */
    void clear_plugins()
    {
        chromosome_start_plugins_.clear();
        chromosome_finish_plugins_.clear();
        enqueue_plugins_.clear();
        dequeue_plugins_.clear();
        emission_plugins_.clear();
    }

    // -------------------------------------------------------------------------
    //     Enqueue and Generate Windows
    // -------------------------------------------------------------------------

    /**
     * @brief Signal the start of a new chromosome, given its name.
     *
     * This function is typically not needed to be called manually, but mostly here for symmetry
     * reasons. See finish_chromosome() for details.
     */
    void start_chromosome( std::string const& chromosome )
    {
        if( chromosome != window_.chromosome() ) {
            finish_chromosome();
            window_.chromosome( chromosome );
        }
    }

    /**
     * @brief Enqueue a new Data value.
     *
     * This is the main function to be called when processing data. It takes care of filling
     * the Window, calling all necessary plugin functions, and in particular, calling the
     * on_emission plugins once a Window is finished.
     *
     * The function also takes the @p chromosome that this Data entry belongs to. This allows
     * to automatically determine when a new chromosome starts, so that the positions and all
     * other data (and potentially accumulators) can be reset accordingly.
     *
     * However, we cannot determine when the last chromosome ends automatically.
     * Hence, see also finish_chromosome() for details on wrapping up the input of a chromosome.
     */
    void enqueue( std::string const& chromosome, size_t position, Data const& data )
    {
        start_chromosome( chromosome );
        enqueue_( position, Data{ data });
    }

    /**
     * @brief Enqueue a new Data value, by moving it.
     *
     * See the non-move overload of this function for details.
     */
    void enqueue( std::string const& chromosome, size_t position, Data&& data )
    {
        start_chromosome( chromosome );
        enqueue_( position, std::move( data ));
    }

    /**
     * @brief Enqueue a new Data value, without considering its chromosome.
     *
     * This alternative overload does not use the chromosome, and hence should only be used if
     * we are sure that we are always on the same chromosome (or are not using chromosome information
     * at all), and hence, that @p position always increases between calls of this function.
     *
     * This is mostly meant as a simplification in cases where the data does not come with chromosome
     * information. Typically however, when using VCF data, the `CHROM` column is present and
     * should be used; that is, typically, the other overload of this function should be used.
     */
    void enqueue( size_t position, Data const& data )
    {
        enqueue_( position, Data{ data });
    }

    /**
     * @brief Enqueue a new Data value by moving it, without considering its chromosome.
     *
     * See the non-moving overload of this function for details.
     */
    void enqueue( size_t position, Data&& data )
    {
        enqueue_( position, std::move( data ));
    }

    /**
     * @brief Explicitly finish a chromosome, and emit all remaining Window%s.
     *
     * When sliding along a genome, we can typically use the provided chromosome name in enqueue()
     * to determine the chromosome we are currently on (typically, the input for this is the `CHROM`
     * information of a VCF file, or the first column of a pileup file), and switch to a new
     * chromosome if needed. In that case, all remaining data in the last window needs to be
     * emitted, so that it is not forgotten. Only after that, we can start a new window for the new
     * chromosome.
     *
     * However, we cannot automatically tell when the last chromosome of the genome is finished
     * from within this class here (as there will simply be no more enqueue() calls, but how would
     * we know that?!). Hence, there might be windows with data at the end that are not yet emitted.
     * In order to also process their data, we need to explicitly call this function here.
     *
     * It makes sure that the remaining data is processed. If provided with a @p last_position,
     * all Window%s up to that position are emitted (which is only relevant for interval windows) -
     * that is, if the full genome length is known, there might be (potentially empty) windows at
     * the end that do not contain any data, but which still need to be emitted for a thorough
     * and complete output. In that case, call this function with the respective genome length,
     * and it will take care of emitting all the windows.
     *
     * Additionally, if emit_incomplete_windows() is set to `true`, the last window that contains
     * the @p last_position is also emitted, which might be incomplete (it might be shorter than
     * the window width). For some computations that normalize by window width, this might be
     * desirable, while in other cases where e.g. absolute per-window numbers are computed, it might
     * not be. Hence, we offer this setting.
     *
     * NB: This function is also called from the destructor, to ensure that all data is processed
     * properly. This also means that any calling code needs to make sure that all data that is
     * needed for emitting window data is still available when the window is destructed without
     * having called this function first. See the SlidingWindowGenerator class description for
     * details.
     */
    void finish_chromosome( size_t last_position = 0 )
    {
        // If nothing was enqueued yet, there is nothing to finish.
        // This also makes sure that calling this function multiple times in a row does not
        // have any side effects.
        if( next_index_ == 0 ) {
            return;
        }

        // If we did not get a specific last position, we just finish the current interval.
        if( last_position == 0 ) {
            last_position = current_start_ + width_;
        }

        // Boundary check. We make sure that the given position is neither in front of the current
        // window, or, if there are entries in the list, also not in front of those.
        // See enqueue_() for details. Same here.
        size_t cur_end = window_.entries().empty() ? 0 : window_.entries().back().position;
        cur_end = current_start_ > cur_end ? current_start_ - 1 : cur_end;
        if( last_position <= cur_end ) {
            throw std::runtime_error(
                "Cannot call finish_chromosome() with position " + std::to_string(last_position) +
                ", as the current window/chromosome is already advanced up to position " +
                std::to_string( cur_end ) + "."
            );
        }

        // Emit the remaining data entries.
        if( window_type_ == WindowType::kInterval ) {
            synchronize_interval_( last_position );
            assert( current_start_ <= last_position );
            assert( last_position < current_start_ + width_ );

            // Special case for the emit_incomplete_windows_ setting. We have synchronized so that
            // the given last_position is within the current interval. Now we need to emit that
            // particular (incomplete) window and clean it up. The last p
            if( emit_incomplete_windows_ ) {
                emit_window_( current_start_, last_position + 1, last_position + 1 );
            }

        } else if( window_type_ == WindowType::kVariants ) {
            throw std::runtime_error( "Not yet implemented." );
        } else {
            assert( false );
        }

        // Wrap up the chromosome, and clear, so that we can start a new chromosome cleanly.
        for( auto const& chromosome_finish_plugin : chromosome_finish_plugins_ ) {
            if( chromosome_finish_plugin ) {
                chromosome_finish_plugin( window_.chromosome(), window_.accumulator() );
            }
        }
        clear();
    }

    // -------------------------------------------------------------------------
    //     General Internal Members
    // -------------------------------------------------------------------------

private:

    void enqueue_( size_t position, Data&& data )
    {
        // If this is the first enqueuing of the window or the chromosome,
        // we need to call the start plugin.
        if( next_index_ == 0 ) {
            for( auto const& chromosome_start_plugin : chromosome_start_plugins_ ) {
                if( chromosome_start_plugin ) {
                    chromosome_start_plugin( window_.chromosome(), window_.accumulator() );
                }
            }
        }

        // Boundary check. We make sure that the given position is neither in front of the current
        // window, or, if there are entries in the list, also not in front of those.
        // (There might be cases were we are already in the middle of the chromosome, but the
        // entries list is empty. Not entirely sure when this can occurr, but it feels like it can,
        // and just checking this doesn't cost us much. If anyone wants to think this through,
        // feel free.)
        size_t cur_end = window_.entries().empty() ? 0 : window_.entries().back().position;
        cur_end = current_start_ > cur_end ? current_start_ - 1 : cur_end;
        if( position <= cur_end ) {
            throw std::invalid_argument(
                "Cannot enqueue at position " + std::to_string( position ) +
                ", as the current window/chromosome is already advanced up to position " +
                std::to_string( cur_end ) +
                ". Either start a new window or a new chromosome within the window. "
                "Note that this error might be caused by a VCF file that is not sorted by "
                "chromosome and position."
            );
        }
        assert( position >= current_start_ );
        assert( window_.entries().empty() || position > window_.entries().back().position );

        // Do the correct type of enqueuing.
        if( window_type_ == WindowType::kInterval ) {
            enqueue_interval_( position, std::move( data ));
        } else if( window_type_ == WindowType::kVariants ) {
            throw std::runtime_error( "Not yet implemented." );
        } else {
            assert( false );
        }
    }

    // -------------------------------------------------------------------------
    //     Interval Internal Members
    // -------------------------------------------------------------------------

private:

    /**
     * @brief Enqueue new data in an inveral, and call the respective plugin function.
     */
    void enqueue_interval_( size_t position, Data&& data )
    {
        assert( window_type_ == WindowType::kInterval );

        // Make sure that we move to the interval where our position needs to be added to.
        synchronize_interval_( position );
        assert( current_start_ <= position );
        assert( position < current_start_ + width_ );

        // Add the new data to our entry queue.
        window_.entries().emplace_back( next_index_, position, std::move( data ));
        ++next_index_;

        // Run the enqueue event plugins. We do not emit anything here. That will be done once
        // the interval is finished, that is, above, when a new position outside of the interval
        // is added (or we finish the whole iteration).
        for( auto const& enqueue_plugin : enqueue_plugins_ ) {
            if( enqueue_plugin ) {
                assert( window_.entries().size() > 0 );
                enqueue_plugin( window_.entries().back(), window_.accumulator() );
            }
        }

        // Make sure that all entries in the queue are within our current bounds,
        // and are in the correct order. We use a lambda that we immediately call.
        assert( [&](){
            size_t cur_pos = 0;
            for( auto it : window_.entries() ) {
                if( it.position < current_start_ || it.position >= current_start_ + width_ ) {
                    return false;
                }
                if( it.position < cur_pos ) {
                    return false;
                }
                cur_pos = it.position;
            }
            return true;
        }() );
    }

    /**
     * @brief Move the window up to a given position, potentially emitting all windows in between.
     */
    void synchronize_interval_( size_t position )
    {
        assert( window_type_ == WindowType::kInterval );

        // This function is only called internally, and only if we are sure that the position is
        // valid. Let's assert this.
        assert( position >= current_start_ );
        assert( window_.entries().empty() || window_.entries().back().position < position );

        // Either there are no entries, or they are all within the current interval.
        // That has to be the case, because we emit if we finish an interval, and remove the data.
        // So, there should never be data that is from an old interval at this point here.
        assert(
            window_.entries().empty() || window_.entries().front().position >= current_start_
        );
        assert(
            window_.entries().empty() || window_.entries().front().position < current_start_ + width_
        );
        assert(
            window_.entries().empty() || window_.entries().back().position >= current_start_
        );
        assert(
            window_.entries().empty() || window_.entries().back().position < current_start_ + width_
        );

        // Emit the windows up to the position where we want to enqueue the new data entry.
        // As we slide over intervals of fixed size along the genome, this can mean that we
        // have to emit multiple (potentially empty) windows along the way, until we are at the
        // interval that contains our new position.
        while( current_start_ + width_ <= position ) {

            // Emit and move to next interval.
            emit_window_( current_start_, current_start_ + width_, current_start_ + stride_ );
            current_start_ += stride_;
        }

        // We are now within the exact interval where we need to be.
        assert( current_start_ <= position );
        assert( position < current_start_ + width_ );
    }

    /**
     * @brief Emit a Window, that is, set all its properties, and call the plugins.
     */
    void emit_window_( size_t first_position, size_t last_position, size_t dequeue_until )
    {
        assert( window_type_ == WindowType::kInterval );

        // Make sure that all entries in the queue are within our current bounds,
        // and are in the correct order. We use a lambda that we immediately call.
        assert( [&](){
            size_t cur_pos = 0;
            for( auto const& it : window_.entries() ) {
                if( it.position < first_position || it.position >= last_position ) {
                    return false;
                }
                if( it.position < cur_pos ) {
                    return false;
                }
                cur_pos = it.position;
            }
            return true;
        }() );

        // Prepare the window properties.
        assert( last_position > first_position );
        window_.first_position( first_position );
        window_.last_position( last_position );

        // Now emit all plugin functions.
        for( auto const& emission_plugin : emission_plugins_ ) {
            if( emission_plugin ) {
                emission_plugin( window_ );
            }
        }

        // Dequeue everything that just moved out of the current interval.
        while(
            window_.entries().size() > 0 && window_.entries().front().position < dequeue_until
        ) {
            for( auto const& dequeue_plugin : dequeue_plugins_ ) {
                if( dequeue_plugin ) {
                    dequeue_plugin( window_.entries().front(), window_.accumulator() );
                }
            }
            window_.entries().pop_front();
        }
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
    bool emit_incomplete_windows_ = false;

    // Current window and its position
    size_t current_start_ = 0;
    size_t next_index_ = 0;
    Window window_;

    // Plugin functions
    std::vector<on_chromosome_start>  chromosome_start_plugins_;
    std::vector<on_chromosome_finish> chromosome_finish_plugins_;
    std::vector<on_enqueue>           enqueue_plugins_;
    std::vector<on_dequeue>           dequeue_plugins_;
    std::vector<on_emission>          emission_plugins_;

};

} // namespace population
} // namespace genesis

#endif // include guard

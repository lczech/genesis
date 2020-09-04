#ifndef GENESIS_POPULATION_TOOLS_WINDOW_H_
#define GENESIS_POPULATION_TOOLS_WINDOW_H_

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

namespace genesis {
namespace population {

// =================================================================================================
//     Accumulator
// =================================================================================================

/**
 * @brief Empty helper data struct to serve as a dummy for Window.
 *
 * By default, the Window::Accumulator type does not do anything, because most of the time, we
 * do not need it. This class here serves as that empty class, so that the user does not have to
 * provide one when using Window without an accumulator.
 */
struct EmptyAccumulator
{};

// =================================================================================================
//     Genomic Window
// =================================================================================================

/**
 * @brief Sliding window over the chromosomes of a genome.
 *
 * The class allows to accumulate and compute arbitrary data within a sliding window over
 * a genome. The basic setup is to provide a set of plugin functions that do the actual computation,
 * and then feed the data in via the enqueue() functions. The Window class then takes care of
 * calling the respective plugin functions to compute values and emit results once a window is
 * finished.
 *
 * To this end, the Window takes care of collecting the data (whose type is given via
 * the template parameter `D`/`Data`) in a list of Entry instances per window. For each finished
 * window, the on_emission() plugin function is called, which typically is set by the user code
 * to compute and store/print/visualize a per-window summary of the `Data`.
 *
 * A typical use case for this class is a window over the variants that are present in a set
 * of (pooled) individuals, for example, the records/lines of a VCF file. Each record would then
 * form a Data Entry, and some summary of a window along the positions in the VCF file would be
 * computed per window. As those files can potentially contain multiple chromosomes, we also support
 * that. In this case, the window is "restarted" at the beginning of a new chromosome.
 *
 * In some cases (in particular, if a stride is chosen that is less than the window size), it might
 * be advantageous to not compute the summary per window from scratch each time, but instead
 * hold a rolling record while sliding - that is, to add in values when they are enqueued, and
 * to remove them once the window moves past their position in the genome.
 * To this end, the second template parameter `A`/`Accumulator` can be used, which can store
 * the necessary intermediate date. For example, to compute some average of values over a window,
 * the Accumulator would need to store a sum of the values and a count of the number of values.
 * In order to update the Accumulator for each Data Entry that is added or removed from the window
 * while sliding, the plugin functions on_enqueue() and on_dequeue() need to be set accordingly.
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
 * of the window (either in number of basepairs or in number of variants).
 *
 * Once all data has been processed, finish_chromosome() should be called to emit the last remaining
 * window(s). See the following note for details.
 *
 * Note:
 * The plugin functions are typically lambdas that might make use of other data from the calling code.
 * However, as this Window class works conceptually similar to a stream, where new data is enqueued
 * in some form of loop or iterative process, the class cannot know when the process is finished,
 * that is, when the end of the genome is reached.
 * Hence, either finish_chromosome() has to be called once all data has been processed, or it has
 * to be otherwise ensured that the Window instance is destructed before the other data that the
 * plugin lambda funtions depend on. This is because the destructor also calls finish_chromosome(),
 * in order to ensure that the last interval is processed properly. Hence, if any of the functions
 * called from within the plugin functions use data outside of the Window instance, that data has
 * still to be alive (unless finish_chromosome() was called explicitly before, in which case the
 * destructor does not call it again) - in other words, the Window has to be destroyed after these
 * data.
 */
template<class D, class A = EmptyAccumulator>
class Window
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using Data = D;
    using Accumulator = A;

    /**
     * @brief Type of Window, that is, whether we slide along a fixed size interval along the genome,
     * or along a fixed number of variants.
     */
    enum class Type
    {
        kInterval,
        kVariants
    };

    /**
     * @brief Position in the genome that is used for reporting when emitting a window.
     *
     * When a window is finished, the on_emission() plugin function is called, which reports the
     * position in the genome at which the window is. There are several ways that this position
     * is computed. Typically, just the first position of the window is used (that is, for an
     * interval, the beginning of the interval, and for variants, the position of the first variant).
     *
     * However, it might be desirable to report a different position, for example when plotting the
     * results. When using Type::kVariants for example, one might want to plot the values
     * computed per window at the midpoint genome position of the variants in that window.
     */
    enum class ReportedPosition
    {
        kBegin,
        kEnd,
        kMidpoint,
        kMedian,
        kMean
    };

    /**
     * @brief Data that is stored per entry that was enqueued in a window.
     *
     * This is the data that the per-window computation is based on. We store the actual user-provided
     * `D`/`Data` type, as well as its position in the genome (as for example given by the `POS`
     * column in a VCF file), and the index within the current chromosome - that is, the how many'th
     * Entry this data point is in the list of enqueued data (starting from zero for each chromosome).
     */
    struct Entry
    {
        Entry( size_t index, size_t position, Data const& data )
            : index(index)
            , position(position)
            , data(data)
        {}

        size_t index;
        size_t position;
        Data   data;
    };

    using self_type       = Window<Data, Accumulator>;
    using container       = std::deque<Entry>;

    using value_type      = Entry;
    using reference       = value_type&;
    using const_reference = value_type const&;

    using iterator               = typename container::iterator;
    using const_iterator         = typename container::const_iterator;
    using reverse_iterator       = typename container::reverse_iterator;
    using const_reverse_iterator = typename container::const_reverse_iterator;

    using difference_type = typename container::difference_type;
    using size_type       = typename container::size_type;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Create a default (empty) instance.
     *
     * Not really important for usage, as this cannot do much.
     */
    Window() = default;

    /**
     * @brief Construct a Window, given the Type and width, and potentially stride.
     *
     * If stride is not given (or set to `0`), it is set automatically to the window size,
     * so that windows do not overlap.
     */
    Window( Type type, size_t width, size_t stride = 0 )
        : type_(type)
        , width_(width)
        , stride_(stride)
    {
        if( width == 0 ) {
            throw std::runtime_error( "Cannot use Window of width 0." );
        }
        if( stride == 0 ) {
            stride_ = width;
        }
    }

    /**
     * @brief Destruct the instance.
     *
     * This typically has to be called before other data storage instances on the user side
     * go out of scope. See the Window class description note for details on why that is the case.
     */
    ~Window()
    {
        finish_chromosome();
    }

    Window( Window const& ) = default;
    Window( Window&& )      = default;

    Window& operator= ( Window const& ) = default;
    Window& operator= ( Window&& )      = default;

    // -------------------------------------------------------------------------
    //     Event Plugin Functionals
    // -------------------------------------------------------------------------

    /**
     * @brief Plugin function that is called on the first enqueue() of a newly started chromosome.
     *
     * The purpose of this plugin is to allow to prepare the accumulator as needed.
     * Note that it is not immediately called when calling start_chromosome() (which is mostly
     * a decorative function anyway), but instead is called once actual data is added via
     * enqueue(). This is because calling start_chromosome() is optional. Furthermore, we would
     * then also need to call this plugin function from the constructor, but during construction,
     * we do not know the chromosome name yet - hence, the calling is delayed until actual data is
     * seen.
     */
    std::function<void( std::string const& chromosome, Accumulator& accu )> on_chromosome_start;

    /**
     * @brief Plugin function that is called when finishing a chromosome.
     *
     * The purpose of this plugin is to allow to clean up the accumulator as needed.
     * The function is called when enqueue() is called with a new chromosome name, or when calling
     * finish_chromosome() explictly, or on destruction of the whole class.
     */
    std::function<void( std::string const& chromosome, Accumulator& accu )> on_chromosome_end;

    /**
     * @brief Plugin function to update the Accumulator when new Data is enqueued.
     *
     * The purpose of this plugin function is to avoid re-computation of values in a window if
     * that computation can be done partially instead. This is of course mostly helpful if the stride
     * is smaller than the window width. Otherwise (if the stride is greater/equal to the window
     * width), each window will contain completely new/different data, so there is nothing
     * to re-use between windows.
     *
     * If used, this function is meant to update the Accumulator in a way that adds the new data
     * Entry.
     */
    std::function<void( Entry const& entry, Accumulator& accu )> on_enqueue;

    /**
     * @brief Plugin function to update the Accumulator when Data is removed due to the window
     * moving away from it.
     *
     * This is the counterpart for on_enqueue to remove data from the Accumulator once it is no longer
     * part of the current window. See on_enqueue for details.
     */
    std::function<void( Entry const& entry, Accumulator& accu )> on_dequeue;

    /**
     * @brief Main plugin function that is called for every window.
     *
     * This is the plugin that typically is the most important to set for the user.
     * This is where the data from the window is processed, that is, where the summary of the window
     * is computed and stored/visualized/plotted. This can either be done by using the Accumulator,
     * or by computing the value based on all the Data Entry values in the window (using @p begin
     * and @p end to iterate them).
     *
     * The plugin function also allows to access the first and last position of the window;
     * for interval windows, these are the interval boundarys, and for variant windows, these
     * are simply the positions of the first and last variant (that is, `begin->position`, and
     * `end->position`).
     *
     * Lastly, the plugin also sets the @p reported_position according to the setting of
     * reported_position( ReportedPosition ). This is the position in the genome that should be used
     * by the user to store the value or as the axis value at which the resulting values of the
     * window should be plotted.
     */
    std::function<void(
        size_t first_position, size_t last_position, size_t reported_position,
        const_iterator begin, const_iterator end, Accumulator& accu
    )> on_emission;

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    size_t width() const
    {
        return width_;
    }

    size_t stride() const
    {
        return stride_;
    }

    size_t current_start() const
    {
        return current_start_;
    }

    std::string const& chromosome() const
    {
        return chromosome_;
    }

    ReportedPosition reported_position() const
    {
        return reported_position_;
    }

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    self_type& reported_position( ReportedPosition value ) const
    {
        reported_position_ = value;
        return *this;
    }

    void clear()
    {
        chromosome_ = "";
        current_start_ = 0;
        next_index_ = 0;
        accumulator_ = Accumulator{};
        entries_ = container{};
    }

    // -------------------------------------------------------------------------
    //     Enqueue
    // -------------------------------------------------------------------------

    /**
     * @brief Signal the start of a new chromosome, given its name.
     *
     * This function is typically not needed to be called manyally, but mostly here for symmetry
     * reasons. See finish_chromosome() for details.
     */
    void start_chromosome( std::string const& chromosome )
    {
        if( chromosome != chromosome_ ) {
            finish_chromosome();
            chromosome_ = chromosome;
        }
    }

    /**
     * @brief Enqueue a new Data value.
     *
     * This is the main function to be called when processing data. It takes care of filling
     * the window, calling all necessary plugin functions, and in particular, calling the
     * on_emission() plugin once a window is finished.
     *
     * The function also takes the @p chromosome that this Data entry belongs to. This allows
     * to automatically determine when a new chromosome starts, so that the positions and all
     * other data (and potentially accumulators) can be reset accordingly.
     *
     * However, we cannot determine when the last chromosome ends automatically.
     * Hence, see also finish_chromosome() for details.
     */
    void enqueue( std::string const& chromosome, size_t position, Data const& data )
    {
        start_chromosome( chromosome );
        enqueue( position, data );
    }

    /**
     * @brief Enqueue a new Data value, without considering its chromosome.
     *
     * This alternative version does not use the chromosome, and hence should only be used if
     * we are sure that we are always on the same chromosome, and hence, that @p position always
     * increases between calls of this function.
     *
     * This is mostly meant as a simplification in cases where the data does not come with chromsome
     * information. Typically however, when using VCF data, the `CHROM` column is present and
     * should be used; that is, typically, the other version of this function should be used.
     */
    void enqueue( size_t position, Data const& data )
    {
        // If this is the first enqueuing of the window or the chromosome,
        // we need to call the start plugin.
        if( next_index_ == 0 ) {
            if( on_chromosome_start ) {
                on_chromosome_start( chromosome_, accumulator_ );
            }
        }

        // Boundary check.
        if( ! entries_.empty() && position <= entries_.back().position ) {
            assert( ! entries_.empty() );
            throw std::invalid_argument(
                "Cannot enqueue at position " + std::to_string( position ) +
                ", as the current window/chromosome is already filled up to position " +
                std::to_string( entries_.back().position ) +
                ". Either start a new window or a new chromosome within the window."
            );
        }
        assert( position >= current_start_ );
        assert( entries_.empty() || position > entries_.back().position );

        // Do the correct type of enqueuing.
        if( type_ == Type::kInterval ) {
            enqueue_interval_( position, data );
        } else if( type_ == Type::kVariants ) {
            throw std::runtime_error( "Not yet implemented." );
        } else {
            assert( false );
        }
    }

    /**
     * @brief Explicitly finish a chromosome, and emit all remaining windows.
     *
     * When sliding along a genome, we can typically use the `CHROM` information of a VCF file to
     * determine the chromosome we are currently on, and switch to a new chromosome if needed.
     * In that case, all remaining data in the last window needs to be emitted, so that it is not
     * forgotten. Only after that, we can start a new window for the new chromosome.
     *
     * However, we cannot automatically tell when the last chromosome of the genome is finished
     * from within this window class (as there will simply be no more enqueue() calls, but how
     * would we know that?!). Hence, there might be windows with data at the end that are not yet
     * emitted. In order to also process their data, we need to explicitly call this function here.
     *
     * It makes sure that the remaining data is processed. If provided with a genome position,
     * all windows up to that position are emitted (which is only relevant for interval windows) -
     * that is, if the full genome length is known, there might be (potentially empty) windows at
     * the end that do not contain any data, but which still need to be emitted for a thorough
     * and complete output. In that case, call this function with the respective genome length,
     * and it will take care of emitting all the windows.
     *
     * NB: This function is also called from the destructor, to ensure that all data is processed
     * properly. This also means that any calling code needs to make sure that all data that is
     * needed for emitting window data is still available when the window is destructed without
     * having called this function first. See the Window class description for details.
     */
    void finish_chromosome( size_t last_position = 0 )
    {
        // If nothing was enqueued yet, there is nothing to finish.
        // This also makes sure that calling this function multiple times in a row does not
        // have any side effects.
        if( next_index_ == 0 ) {
            return;
        }

        // Argument check.
        if( ! entries_.empty() && last_position <= entries_.back().position ) {
            assert( ! entries_.empty() );
            throw std::runtime_error(
                "Cannot call finish_chromosome() with position " + std::to_string(last_position) +
                ", as the current window/chromosome is already filled up to position " +
                std::to_string( entries_.back().position ) + "."
            );
        }

        // If we did not get a useful last position, we just finish the whole interval.
        if( last_position == 0 ) {
            last_position = current_start_ + width_;
        }

        // Emit the remaining data entries.
        if( type_ == Type::kInterval ) {
            synchronize_interval_( last_position );
        } else if( type_ == Type::kVariants ) {
            throw std::runtime_error( "Not yet implemented." );
        } else {
            assert( false );
        }

        // Wrap up the chromosome, and clear, so that we can start a new chromosome cleanly.
        if( on_chromosome_end ) {
            on_chromosome_end( chromosome_, accumulator_ );
        }
        clear();
    }

    // -------------------------------------------------------------------------
    //     Interval Internal Members
    // -------------------------------------------------------------------------

private:

    /**
     * @brief Enqueue new data in an inveral, and call the respective plugin function.
     */
    void enqueue_interval_( size_t position, Data const& data )
    {
        assert( type_ == Type::kInterval );

        // LOG_DBG << "enter " << position;

        // Make sure that we move to the interval where our position needs to be added to.
        synchronize_interval_( position );
        assert( current_start_ <= position );
        assert( position < current_start_ + width_ );

        // Add the new data to our entry queue.
        entries_.emplace_back( next_index_, position, data );
        ++next_index_;

        // Run the enqueue event plugin. We do not emit anything here. That will be done once
        // the interval is finished, that is, above, when a new position outside of the interval
        // is added (or we finish the whole iteration).
        if( on_enqueue ) {
            assert( entries_.size() > 0 );
            on_enqueue( entries_.back(), accumulator_ );
        }

        // Make sure that all entries in the queue are within our current bounds,
        // and are in the correct order. We use a lambda that we immediately call.
        assert( [&](){
            size_t cur_pos = 0;
            for( auto it : entries_ ) {
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

        // LOG_DBG << "test " << position << " in " << current_start_ << " - " << (current_start_+width_);
        // for( auto it : entries_ ) {
        //     LOG_DBG1 << "e " << it.position;
        // }
    }

    /**
     * @brief Move the window up to a given position, potentially emitting all windows in between.
     */
    void synchronize_interval_( size_t position )
    {
        assert( type_ == Type::kInterval );

        // This function is only called internally, and only if we are sure that the position is
        // valid. Let's assert this.
        assert( position >= current_start_ );
        assert( entries_.empty() || entries_.back().position < position );

        // Either there are no entries, or they are all within the current interval.
        // That has to be the case, because we emit if we finish an interval, and remove the data.
        // So, there should never be data that is from an old interval at this point here.
        assert( entries_.empty() || entries_.front().position >= current_start_ );
        assert( entries_.empty() || entries_.front().position < current_start_ + width_ );
        assert( entries_.empty() || entries_.back().position >= current_start_ );
        assert( entries_.empty() || entries_.back().position < current_start_ + width_ );

        // Emit the windows up to the position where we want to enqueue the new data entry.
        // As we slide over intervals of fixed size along the genome, this can mean that we
        // have to emit multiple (potentially empty) windows along the way, until we are at the
        // interval that contains our new position.
        while( current_start_ + width_ <= position ) {

            // Make sure that all entries in the queue are within our current bounds,
            // and are in the correct order. We use a lambda that we immediately call.
            assert( [&](){
                size_t cur_pos = 0;
                for( auto it : entries_ ) {
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

            // Now emit and move to next interval.
            if( on_emission ) {

                // Get the reported position and assert that it is within our bounds.
                size_t const rep_pos = get_reported_position_interval_( current_start_ );
                assert( current_start_ <= rep_pos );
                assert( rep_pos < current_start_ + width_ );

                // Run the emission function.
                on_emission(
                    current_start_, current_start_ + width_, rep_pos,
                    entries_.begin(), entries_.end(), accumulator_
                );
            }
            current_start_ += stride_;

            // Dequeue everything that just moved out of the current interval.
            while(
                entries_.size() > 0 && entries_.front().position < current_start_
            ) {
                if( on_dequeue ) {
                    on_dequeue( entries_.front(), accumulator_ );
                }
                entries_.pop_front();
            }
        }

        // We are now within the exact interval where we need to be.
        assert( current_start_ <= position );
        assert( position < current_start_ + width_ );
    }

    /**
     * @brief Get the reported position for an interval window.
     */
    size_t get_reported_position_interval_( size_t start_position ) const
    {
        assert( type_ == Type::kInterval );

        // Calculate the SNP position that we want to output when emitting a window.
        // Some use integer division, which is intended. We don't want the hassle of floating
        // point genomic positions, so we have to do these roundings... But given a large window
        // size, that should probably not matter much.
        switch( reported_position_ ) {
            case ReportedPosition::kBegin: {
                return start_position;
                break;
            }
            case ReportedPosition::kEnd: {
                return start_position + width_ - 1;
                break;
            }
            case ReportedPosition::kMidpoint: {
                return start_position + width_ / 2;
                break;
            }
            case ReportedPosition::kMedian: {
                return start_position + width_ / 2;
                break;
            }
            case ReportedPosition::kMean: {
                return start_position + width_ / 2;
                break;
            }
            default: {
                assert( false );
                return 0;
            }
        }
        assert( false );
        return 0;
    }

    // -------------------------------------------------------------------------
    //     Variants Internal Members
    // -------------------------------------------------------------------------

private:

    /**
     * @brief Get the reported position for a variants window.
     */
    size_t get_reported_position_variants_() const
    {
        // Calculate the SNP position that we want to output when emitting a window.
        // Some use integer division, which is intended.
        assert( ! entries_.empty() );
        switch( reported_position_ ) {
            case ReportedPosition::kBegin: {
                return entries_.front().position;
                break;
            }
            case ReportedPosition::kEnd: {
                return entries_.back().position;
                break;
            }
            case ReportedPosition::kMidpoint: {
                return (entries_.front().position + entries_.back().position) / 2;
                break;
            }
            case ReportedPosition::kMedian: {
                return entries_[ entries_.size() / 2 ].position;
                break;
            }
            case ReportedPosition::kMean: {
                size_t sum = 0;
                for( auto const& e : entries_ ) {
                    sum += e.position;
                }
                return sum / entries_.size();
                break;
            }
            default: {
                assert( false );
                return 0;
            }
        }
        assert( false );
        return 0;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    Type type_;
    size_t width_;
    size_t stride_;
    ReportedPosition reported_position_ = ReportedPosition::kBegin;

    std::string chromosome_;
    size_t current_start_ = 0;
    size_t next_index_ = 0;

    Accumulator accumulator_;
    container entries_;

};

} // namespace population
} // namespace genesis

#endif // include guard

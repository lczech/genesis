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

#include "genesis/utils/core/logging.hpp"

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
 * @brief
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

    enum class Type
    {
        kInterval,
        kVariants
    };

    enum class ReportedPosition
    {
        kBegin,
        kEnd,
        kMidpoint,
        kMedian,
        kMean
    };

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
     */
    Window() = default;

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

    std::function<void( Entry const& entry, Accumulator& accu )> on_enqueue;
    std::function<void( Entry const& entry, Accumulator& accu )> on_dequeue;
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
        current_start_ = 0;
        next_index_ = 0;
        accumulator_ = Accumulator{};
        entries_ = container{};
    }

    // -------------------------------------------------------------------------
    //     Enqueue
    // -------------------------------------------------------------------------

    void start_chromosome( std::string const& chromosome )
    {
        if( chromosome != chromosome_ ) {
            finish_chromosome();
            chromosome_ = chromosome;
        }
    }

    void enqueue( std::string const& chromosome, size_t position, Data const& data )
    {
        start_chromosome( chromosome );
        enqueue( position, data );
    }

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

    void finish_chromosome()
    {
        // If nothing was enqueued yet, there is nothing to finish.
        // This also makes sure that calling this function multiple times in a row does not
        // have any side effects.
        if( next_index_ == 0 ) {
            return;
        }

        // Emit the remaining data entries.
        if( type_ == Type::kInterval ) {
            synchronize_interval_( current_start_ + width_ );
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

    void synchronize_interval_( size_t position )
    {
        assert( type_ == Type::kInterval );

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

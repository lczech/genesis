#ifndef GENESIS_POPULATION_WINDOW_WINDOW_H_
#define GENESIS_POPULATION_WINDOW_WINDOW_H_

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

#include <cassert>
#include <deque>
#include <stdexcept>
#include <string>
#include <vector>

#include "genesis/population/window/base_window.hpp"
#include "genesis/population/genome_region.hpp"

namespace genesis {
namespace population {

// =================================================================================================
//     Auxiliary Structures
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
 * @brief %Window over the chromosomes of a genome.
 *
 * This class is a container for the Window::Data (of template type `D`) that is produced when
 * sliding over the chromosomes of a genome in windows, or filling genomic region windows.
 * It is for example produced by the SlidingIntervalWindowIterator class (and the deprecated
 * SlidingWindowGenerator).
 *
 * We here define a window to have a first_position() and a last_position() that can be set
 * independently of the content of the window. For example, for a sliding window, they would
 * be determined by the width and stride of the slider. Within that window, entries can be added,
 * as for example found in a variant call file, or other data type along the genome.
 *
 * For example, we might have a Window between two positions `|` (e.g., determined by a sliding
 * window), filled with several entries `x` coming from the underlying data source like this:
 *
 *     -----|--------x-----x-xx---xxxx---xxxx-xxx----|-----
 *
 * The width() of the window then is the number of bases in the genome between the first_position()
 * and last_position(), that is, the distance between the two `|` (plus one, due to using inclusive
 * intervals), On the other hand, the span() of the window is the distance between the first and
 * last entry `x` in the window (again plus one).
 * Furthermore, the entry_count() or size() is the number of entries in that window, that is,
 * the total number of `x` in the window.
 *
 * **Remark:** We use 1-based inclusive intervals to denote genome regions. That means that the
 * numeric values returned from both first_position() and last_position() are both positions that
 * are part of the window.
 *
 * The class is mostly meant to be used to be read/iterated over, where the data is filled
 * in beforehand (e.g., via the SlidingIntervalWindowIterator), and can then be processed to compute
 * some values for the Window. That is, from the user side, the const access functions are mostly
 * important, while the non-const modification functions are chiefly meant for the code that fills
 * the Window in the first place.
 *
 * This means that data from an input file has to be copied into the Window first, which is a bit
 * of a waste, but, given that we might want to do things such as sliding windows with stride
 * smaller than window with, probably a necessary cost to have in order to keep the design simple
 * and easy to use.
 */
template<class D, class A = EmptyAccumulator>
class Window final : public BaseWindow<D>
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using Data = D;
    using Accumulator = A;

    /**
     * @brief Data that is stored per entry that was enqueued in a window.
     *
     * This is the data that the per-window computation is based on. We store the actual
     * user-provided `D`/`Data` type, as well as its position in the genome (as for example given
     * by the `POS` column in a VCF file), and the index within the current chromosome - that is,
     * the how many'th Entry this data point is in the list of enqueued data (starting from zero
     * for each chromosome).
     */
    struct Entry
    {
        /**
         * @brief Contructor that takes @p data by reference.
         */
        Entry( size_t index, size_t position, Data const& data )
            : index(index)
            , position(position)
            , data(data)
        {}

        /**
         * @brief Contructor that takes @p data by r-value reference (i.e., moved data);
         * preferred if possible to use, for speed.
         */
        Entry( size_t index, size_t position, Data&& data )
            : index(index)
            , position(position)
            , data(std::move( data ))
        {}

        /**
         * @brief Implicit conversion to `Data&` type.
         *
         * Useful to plug in a Window into some function expecting a range of Data objects.
         */
        operator Data&()
        {
            return data;
        }

        /**
         * @brief Implicit conversion to `Data const&` type.
         *
         * Useful to plug in a Window into some function expecting a range of Data objects.
         */
        operator Data const&() const
        {
            return data;
        }

        /**
         * @brief Index of the entry, that is, how many other entries have there been in total
         * in the underlying data for the current chromosome.
         *
         * This is useful for example when working with SlidingWindowType::kVariants, to know the
         * how many-th variant in the chromosome the entry is. Gets reset to 0 for each chromosome.
         */
        size_t index;

        /**
         * @brief Genomic position (1-based) of the entry along a chromosome.
         *
         * We here only store the position; for the name of the chromosome, call
         * Window::chromosome(), because for storage and speed reasons, we do not copy and store the
         * chromosome name with every entry.
         */
        size_t position;

        /**
         * @brief Data stored in the Window for this entry.
         */
        Data data;
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

    Window() = default;
    virtual ~Window() override = default;

    Window( Window const& ) = default;
    Window( Window&& )      = default;

    Window& operator= ( Window const& ) = default;
    Window& operator= ( Window&& )      = default;

    // -------------------------------------------------------------------------
    //     General Properties
    // -------------------------------------------------------------------------

    /**
     * @brief Get the number of `D`/Data @link Entry Entries@endlink that are stored in the Window.
     *
     * @see size() for an alias.
     */
    size_t entry_count() const
    {
        return entries_.size();
    }

    /**
     * @brief Get the number of `D`/Data @link Entry Entries@endlink that are stored in the Window.
     *
     * @see entry_count() for an alias.
     */
    size_t size() const
    {
        return entries_.size();
    }

    /**
     * @brief Return whether the Window is empty, that is, if it does not contain any
     * @link Entry Entries@endlink.
     */
    bool empty() const
    {
        return entries_.empty();
    }

    /**
     * @brief Get the fraction of entries to window width.
     */
    double saturation() const
    {
        double const es = static_cast<double>( entries_.size() );
        double const wd = static_cast<double>( this->width() );
        double const frac = es / wd;

        assert( this->width() > 0 );
        assert( frac >= 0.0 );
        assert( frac <= 1.0 );
        return frac;
    }

    /**
     * @brief Get the distance that is spanned by the first and the last variant (entry) in the
     * Window, that is, the number of bases between them (including both).
     *
     * This is the distance between the positions of the first and the last variant (entry) in
     * the Window, plus one as we are working with closed intervals where both positions are
     * included. It differs from width(), which instead gives the distance between the first
     * and last position as set for the Window (plus one again).
     *
     * See the class documentation for an example of the difference between the two functions.
     */
    size_t span() const
    {
        if( entries_.empty() ) {
            return 0;
        }
        return entries_.back().position - entries_.front().position + 1;
    }

    // -------------------------------------------------------------------------
    //     Data Accessors
    // -------------------------------------------------------------------------

    /**
     * @brief Return a reference to the element at specified location @p index.
     *
     * No bounds checking is performed.
     */
    reference operator[]( size_t index )
    {
        assert( index < entries_.size() );
        return entries_[ index ];
    }

    /**
     * @brief Return a reference to the element at specified location @p index.
     *
     * No bounds checking is performed.
     */
    const_reference operator[]( size_t index ) const
    {
        assert( index < entries_.size() );
        return entries_[ index ];
    }

    /**
     * @brief Return a reference to the element at specified location pos, with bounds checking.
     */
    reference at( size_t index )
    {
        return entries_.at( index );
    }

    /**
     * @brief Return a reference to the element at specified location pos, with bounds checking.
     */
    const_reference at( size_t index ) const
    {
        return entries_.at( index );
    }

    /**
     * @brief Iterator to the begin of the Data @link Entry Entries@endlink.
     */
    iterator begin()
    {
        return entries_.begin();
    }

    /**
     * @brief Const iterator to the begin of the Data @link Entry Entries@endlink.
     */
    const_iterator begin() const
    {
        return entries_.begin();
    }

    /**
     * @brief Iterator to the end of the Data @link Entry Entries@endlink.
     */
    iterator end()
    {
        return entries_.end();
    }

    /**
     * @brief Const iterator to the end of the Data @link Entry Entries@endlink.
     */
    const_iterator end() const
    {
        return entries_.end();
    }

    /**
     * @brief Immediate container access to the Data @link Entry Entries@endlink.
     */
    container const& entries() const
    {
        return entries_;
    }

    /**
     * @brief Immediate container access to the Data @link Entry Entries@endlink.
     */
    container& entries()
    {
        return entries_;
    }

    /**
     * @brief Get the Accumulator data that can be used for speeding up certain window computations.
     */
    Accumulator& accumulator()
    {
        return accumulator_;
    }


    /**
     * @brief Get the Accumulator data that can be used for speeding up certain window computations.
     */
    Accumulator const& accumulator() const
    {
        return accumulator_;
    }

    // -------------------------------------------------------------------------
    //     Modifiers and Helpers
    // -------------------------------------------------------------------------

    /**
     * @brief Validate the window data.
     *
     * The function checks that 0 < first_position() <= last_position(),
     * and that all entries in the Window are within first_position() and
     * last_position().
     */
    void validate() const
    {
        if( this->first_position() == 0 ) {
            throw std::runtime_error( "Invalid Window with first_position() == 0." );
        }
        if( this->last_position() < this->first_position() ) {
            throw std::runtime_error( "Invalid Window with last_position() < first_position()." );
        }
        for( auto const& entry : entries_ ) {
            if( entry.position < this->first_position() || entry.position > this->last_position() ) {
                throw std::runtime_error(
                    "Invalid Window::Entry in chromosome " + this->chromosome() + " at position " +
                    std::to_string( entry.position ) +
                    ", which is not between the window boundaries [" +
                    std::to_string( this->first_position() ) + "," +
                    std::to_string( this->last_position() ) + "]."
                );
            }
        }
    }

    // -------------------------------------------------------------------------
    //     Virtual Members
    // -------------------------------------------------------------------------

private:

    virtual void clear_() override
    {
        accumulator_   = Accumulator{};
        entries_       = container{};
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    Accumulator accumulator_;
    container entries_;

};

} // namespace population
} // namespace genesis

#endif // include guard

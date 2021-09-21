#ifndef GENESIS_POPULATION_WINDOW_WINDOW_H_
#define GENESIS_POPULATION_WINDOW_WINDOW_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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

namespace genesis {
namespace population {

// =================================================================================================
//     Auxiliary Structures
// =================================================================================================

/**
 * @brief WindowType of a Window, that is, whether we slide along a fixed size interval of the
 * genome, or along a fixed number of variants.
 */
enum class WindowType
{
    kInterval,
    kVariants
};

/**
 * @brief Position in the genome that is used for reporting when emitting or using a window.
 *
 * When a window is finished, the on_emission() plugin function is called, which reports the
 * position in the genome at which the window is. There are several ways that this position
 * is computed. Typically, just the first position of the window is used (that is, for an
 * interval, the beginning of the interval, and for variants, the position of the first variant).
 *
 * However, it might be desirable to report a different position, for example when plotting the
 * results. When using WindowType::kVariants for example, one might want to plot the values
 * computed per window at the midpoint genome position of the variants in that window.
 */
enum class WindowAnchorType
{
    kIntervalBegin,
    kIntervalEnd,
    kIntervalMidpoint,
    kVariantFirst,
    kVariantLast,
    kVariantMedian,
    kVariantMean,
    kVariantMidpoint
};

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
 * This class is a container for the Data that is produced when sliding over the chromosomes of a
 * genome in windows. It is for example emitted by the WindowGenerator class.
 *
 * The class is mostly meant to be used to be read/iterated over, where the data is filled
 * in beforehand (e.g., via the WindowGenerator), and can then be processed to compute some values
 * for the Window. That is, from the user side, the const access functions are mostly important,
 * while the non-const modification functions are chiefly meant for the code that fills the
 * Window in the first place.
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
     * @brief Data that is stored per entry that was enqueued in a window.
     *
     * This is the data that the per-window computation is based on. We store the actual user-provided
     * `D`/`Data` type, as well as its position in the genome (as for example given by the `POS`
     * column in a VCF file), and the index within the current chromosome - that is, the how many'th
     * Entry this data point is in the list of enqueued data (starting from zero for each chromosome).
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
         * @brief Contructor that takes @p data by r-value reference; preferred if possible to use.
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
         * @brief Index of the entry, that is, how many other entries have there been in total.
         */
        size_t index;

        /**
         * @brief Genomic position of the entry along a chromosome.
         *
         * For the actual chromosome, we need to call Window::chromosome(), because for storage and
         * speed reasons, we do not store the chromosome name with every entry.
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
    ~Window() = default;

    Window( Window const& ) = default;
    Window( Window&& )      = default;

    Window& operator= ( Window const& ) = default;
    Window& operator= ( Window&& )      = default;

    // -------------------------------------------------------------------------
    //     General Properties
    // -------------------------------------------------------------------------

    /**
     * @brief Get the chromosome name that this Window belongs to.
     */
    std::string const& chromosome() const
    {
        return chromosome_;
    }

    /**
     * @brief Set the chromosome name that this Window belongs to.
     */
    void chromosome( std::string const& value )
    {
        chromosome_ = value;
    }

    /**
     * @brief Get the width of the Window.
     *
     * This is the distance between first_position() and last_position(). That is, for Window%s
     * of type WindowType::kInterval, this is the distance between the start of the Window
     * and its end. For WindowType::kVariants however, this is the distance between the positions of
     * the first and the last variant (entry) in the Window.
     *
     * See span() for a function that computes that latter distance for WindowType::kInterval
     * windows as well.
     */
    size_t width() const
    {
        // We need to do the check here, when this is used.
        if( first_position_ >= last_position_ ) {
            throw std::runtime_error(
                "Invalidly set first and last position in the Window, with " +
                std::to_string( first_position_ ) + " >= " + std::to_string( last_position_ )
            );
        }
        return last_position_ - first_position_;
    }

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
        double const frac = static_cast<double>( entries_.size() ) / static_cast<double>( width() );

        assert( width() > 0 );
        assert( frac >= 0.0 );
        assert( frac <= 1.0 );
        return frac;
    }

    /**
     * @brief Get the distance between the first and the last variant (entry) in the Window.
     *
     * The width() function returns a different distance depending on the #WindowType used for the
     * Window. However, sometimes it is useful to know the distance between the first and the last
     * variant (entry) in a window, independently from whether that Window runs across intervals
     * or variatns. This is what we here compute and call the span of the window.
     */
    size_t span() const
    {
        if( entries_.empty() ) {
            return 0;
        }
        return entries_.back().position - entries_.front().position;
    }

    // -------------------------------------------------------------------------
    //     Position
    // -------------------------------------------------------------------------

    /**
     * @brief Get the first position in the chromosome of the Window, that is, where the Window starts.
     *
     * We use half-open intervals; the first position is part of the Window, the last is not.
     */
    size_t first_position() const
    {
        return first_position_;
    }

    /**
     * @brief Set the first position in the chromosome of the Window, that is, where the Window starts.
     *
     * We use half-open intervals; the first position is part of the Window, the last is not.
     */
    void first_position( size_t value )
    {
        first_position_ = value;
    }

    /**
     * @brief Get the last (past-the-end) position in the chromosome of the Window, that is,
     * where the Window ends.
     *
     * We use half-open intervals; the first position is part of the Window, the last is not.
     */
    size_t last_position() const
    {
        return last_position_;
    }

    /**
     * @brief Set the last (past-the-end) position in the chromosome of the Window, that is,
     * where the Window ends.
     *
     * We use half-open intervals; the first position is part of the Window, the last is not.
     */
    void last_position( size_t value )
    {
        last_position_ = value;
    }

    /**
     * @brief Get the position in the chromosome reported according to the currently set
     * #WindowAnchorType.
     *
     * See anchor_type( #WindowAnchorType ) to change the type of position that is reported here,
     * and see anchor_position( #WindowAnchorType ) for an alternative that allows to freely pick
     * the #WindowAnchorType instead.
     */
    size_t anchor_position() const
    {
        return anchor_position( anchor_type_ );
    }

    /**
     * @brief Get the position in the chromosome reported according to a specific #WindowAnchorType.
     */
    size_t anchor_position( WindowAnchorType anchor_type ) const
    {
        auto check_entries = [&](){
            if( entries_.empty() ) {
                throw std::runtime_error(
                    "Cannot use empty Window (with no variants/entries) for variant-based anchor "
                    "positions. Typically these anchor positions are used with WindowType::kVariants."
                );
            }
        };

        // Calculate the SNP position that we want to output when emitting a window.
        // Some use integer division, which is intended. We don't want the hassle of floating
        // point genomic positions, so we have to do these roundings... But given a large window
        // size, that should probably not matter much.
        switch( anchor_type ) {
            case WindowAnchorType::kIntervalBegin: {
                return first_position_;
            }
            case WindowAnchorType::kIntervalEnd: {
                return last_position_;
            }
            case WindowAnchorType::kIntervalMidpoint: {
                return ( first_position_ + last_position_ ) / 2;
            }
            case WindowAnchorType::kVariantFirst: {
                check_entries();
                assert( ! entries_.empty() );
                return entries_.front().position;
            }
            case WindowAnchorType::kVariantLast: {
                check_entries();
                assert( ! entries_.empty() );
                return entries_.back().position;
            }
            case WindowAnchorType::kVariantMedian: {
                check_entries();
                assert( ! entries_.empty() );
                return entries_[ entries_.size() / 2 ].position;
            }
            case WindowAnchorType::kVariantMean: {
                check_entries();
                assert( ! entries_.empty() );

                size_t sum = 0;
                for( auto const& e : entries_ ) {
                    sum += e.position;
                }
                return sum / entries_.size();
            }
            case WindowAnchorType::kVariantMidpoint: {
                check_entries();
                assert( ! entries_.empty() );
                return (entries_.front().position + entries_.back().position) / 2;
            }
            default: {
                throw std::runtime_error( "Invalid WindowAnchorType." );
            }
        }
        assert( false );
        return 0;
    }

    /**
     * @brief Get the #WindowAnchorType that is currently set for using anchor_position().
     */
    WindowAnchorType anchor_type() const
    {
        return anchor_type_;
    }

    /**
     * @brief Set the #WindowAnchorType that is currently set for using anchor_position().
     *
     * This function is mainly useful to set the #WindowAnchorType once, and then use the variant of
     * anchor_position() without any arguments to get the reported position.
     */
    void anchor_type( WindowAnchorType value )
    {
        anchor_type_ = value;
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
    //     Modifiers
    // -------------------------------------------------------------------------

    /**
     * @brief Clear all data from the Window.
     */
    void clear()
    {
        chromosome_    = "";
        first_position_ = 0;
        last_position_ = 0;
        accumulator_   = Accumulator{};
        entries_       = container{};
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    WindowAnchorType anchor_type_ = WindowAnchorType::kIntervalBegin;

    std::string chromosome_;
    size_t first_position_ = 0;
    size_t last_position_ = 0;

    Accumulator accumulator_;
    container entries_;

};

} // namespace population
} // namespace genesis

#endif // include guard

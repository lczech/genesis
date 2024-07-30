#ifndef GENESIS_POPULATION_WINDOW_BASE_WINDOW_H_
#define GENESIS_POPULATION_WINDOW_BASE_WINDOW_H_

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

#include <stdexcept>
#include <string>

#include "genesis/population/genome_region.hpp"

namespace genesis {
namespace population {

// =================================================================================================
//     Genomic Window
// =================================================================================================

/**
 * @brief Base class for Window and WindowView, to share common functionality.
 *
 * See Window for usage and details on the functions offered here.
 */
template<class D>
class BaseWindow
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using Data = D;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    BaseWindow() = default;
    virtual ~BaseWindow() = default;

    BaseWindow( BaseWindow const& ) = default;
    BaseWindow( BaseWindow&& )      = default;

    BaseWindow& operator= ( BaseWindow const& ) = default;
    BaseWindow& operator= ( BaseWindow&& )      = default;

    // -------------------------------------------------------------------------
    //     Chromosome and Positions
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
     * @brief Get the first position in the chromosome of the Window, that is, where the Window starts.
     *
     * The first and last position in the Window are determined for example by the sliding window
     * width and stride, or by a genomic region that the window represents. They are hence
     * independent of the actual data entries stored in the window, but obviously should be covering
     * their positions. See the Window class documentation for an example.
     *
     * We use 1-based coordinates and closed intervals, where both the first and the last position
     * are inclusive.
     */
    size_t first_position() const
    {
        return first_position_;
    }

    /**
     * @brief Set the first position in the chromosome of the Window, that is, where the Window starts.
     *
     * @copydetails first_position() const
     */
    void first_position( size_t value )
    {
        first_position_ = value;
    }

    /**
     * @brief Get the last position in the chromosome of the Window, that is, where the Window ends.
     *
     * @copydetails first_position() const
     */
    size_t last_position() const
    {
        return last_position_;
    }

    /**
     * @brief Set the last position in the chromosome of the Window, that is, where the Window ends.
     *
     * @copydetails first_position() const
     */
    void last_position( size_t value )
    {
        last_position_ = value;
    }

    /**
     * @brief Return the genome region that this Windows is defined over.
     *
     * This is a convenience function that gives the chromosome(), as well as first_position() and
     * last_position(), combined into a GenomeRegion object, for ease of use.
     */
    GenomeRegion genome_region() const
    {
        return { chromosome_, first_position_, last_position_ };
    }

    /**
     * @brief Get the width of the Window.
     *
     * This is the distance between first_position() and last_position(), i.e., the distance
     * between the start of the Window and its end as denoted by these positions, plus one,
     * as we are using closed intervals where both positions are included.
     *
     * See Window::span() for a function that computes the distance between the positions of the
     * first and last _entry_ in the window instead, which might be smaller than the width,
     * if there are no entries in the beginning or end of the window.
     *
     * See the Window class documentation for an example of the difference between the two functions.
     */
    size_t width() const
    {
        // We need to do the check here, when this is used.
        if( first_position_ > last_position_ ) {
            throw std::runtime_error(
                "Invalidly set first and last position in the Window, with " +
                std::to_string( first_position_ ) + " > " + std::to_string( last_position_ )
            );
        }
        return last_position_ - first_position_ + 1;
    }

    // -------------------------------------------------------------------------
    //     Modifiers and Helpers
    // -------------------------------------------------------------------------

    /**
     * @brief Clear all data from the Window.
     */
    void clear()
    {
        chromosome_     = "";
        first_position_ = 0;
        last_position_  = 0;
        clear_();
    }

    // -------------------------------------------------------------------------
    //     Virtual Members
    // -------------------------------------------------------------------------

protected:

    /**
     * @brief Virtual clear function for derived classes to clear their data.
     */
    virtual void clear_()
    {
        // Do nothing. Override by derived classes to clear their data.
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::string chromosome_;
    size_t first_position_ = 0;
    size_t last_position_  = 0;

};

} // namespace population
} // namespace genesis

#endif // include guard

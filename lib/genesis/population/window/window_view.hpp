#ifndef GENESIS_POPULATION_WINDOW_WINDOW_VIEW_H_
#define GENESIS_POPULATION_WINDOW_WINDOW_VIEW_H_

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

#include "genesis/population/window/base_window.hpp"
#include "genesis/population/window/window.hpp"

#include <cassert>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Genomic Window View
// =================================================================================================

/**
 * @brief Proxy view over window-like regions of a genome.
 *
 * As opposed to the Window class, this WindowView merely is a proxy for iterating some underlying
 * data, but without copying the entries into the window. It serves as an abstraction for algorithms
 * that need to stream through a file in a window-like manner in a single pass.
 *
 * It expexts the `get_element` function to be set, returning a pointer to the current underlying
 * data entry, or a `nullptr` to signal the end of the iteration. The provided function needs to
 * distinguish the inital call (get the first element of the underlying input iterator),
 * from later calls, in which case the underlying iterator needs to be advanced first.
 *
 * That is, this provided `get_element` function is also responsible for advancing the underlying
 * stream. This abstraction allows the class to be used as an iterator pointing to some other data,
 * without using the being() and end() functions of that data (which might not even need to have
 * those). For example, when iteraing individual whole chromosomes as windows
 * (see ChromosomeIterator for this), we want to stop the iteration of each window after a
 * chromosome is done, which might be before the end of the data itself (if there are multiple
 * chromosomes in the input). The `get_element` function of this WindowView class allows to
 * define such conditions, and stops the iteration.
 *
 * Because of its streaming approach, its memory footprint is smaller than that of a Window,
 * and hence allows to iterator whole chromosomes or genomes. On the flipside, its a single pass
 * iterator with no random access to the data in the window.
 */
template<class D>
class WindowView final : public BaseWindow<D>
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using Data = D;
    using self_type = WindowView<Data>;

    using iterator_category      = std::input_iterator_tag;

    using value_type             = Data;
    using reference              = value_type&;
    using const_reference        = value_type const&;

    class Iterator;
    friend Iterator;

    // =========================================================================
    //     Genomic Window View
    // =========================================================================

    class Iterator
    {
    public:

        // -------------------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------------------

        using Data = D;
        using self_type         = WindowView<Data>::Iterator;

        using value_type        = Data;
        using pointer           = value_type const*;
        using reference         = value_type&;
        using const_reference   = value_type const&;
        using difference_type   = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;

        /**
         * @brief Default constructor for empty (past-the-end) data.
         */
        Iterator() = default;

        /**
         * @brief Constructor for data iteration.
         */
        Iterator( WindowView const* parent )
            : parent_( parent )
        {
            // Either we have no parent, or a valid one. This is checked in the begin() function.
            assert( ! parent_ || parent_->get_element );

            // If there is a parent, this is not an end iterator.
            // Then, we need to read the first element, and check if there is any.
            if( parent_ ) {
                assert( parent_->get_element );
                current_element_ = parent_->get_element();
                if( ! current_element_ ) {
                    parent_ = nullptr;
                }
            }
        }

        ~Iterator() = default;

        Iterator( self_type const& ) = default;
        Iterator( self_type&& )      = default;

        Iterator& operator= ( self_type const& ) = default;
        Iterator& operator= ( self_type&& )      = default;

        // -------------------------------------------------------------------------
        //     Accessors
        // -------------------------------------------------------------------------

        value_type const * operator->() const
        {
            assert( parent_ );
            assert( current_element_ );
            return current_element_;
        }

        value_type * operator->()
        {
            assert( parent_ );
            assert( current_element_ );
            return current_element_;
        }

        value_type const & operator*() const
        {
            assert( parent_ );
            assert( current_element_ );
            return *current_element_;
        }

        value_type & operator*()
        {
            assert( parent_ );
            assert( current_element_ );
            return *current_element_;
        }

        // -------------------------------------------------------------------------
        //     Iteration
        // -------------------------------------------------------------------------

        self_type& operator ++()
        {
            assert( parent_ );
            assert( parent_->get_element );

            current_element_ = parent_->get_element();
            if( ! current_element_ ) {
                parent_ = nullptr;
            }
            return *this;
        }

        // No post increment, to keep it simple, fast, and consistent.
        // self_type operator ++(int)
        // {}

        /**
         * @brief Compare two iterators for equality.
         *
         * Any two iterators that are copies of each other without having moved will compare equal,
         * as long as neither of them is past-the-end. A valid (not past-the-end) iterator and an end()
         * iterator will not compare equal. Two past-the-end iterators compare equal.
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

        Data* current_element_ = nullptr;
        WindowView const* parent_ = nullptr;

    };

    // =========================================================================
    //     Main Class
    // =========================================================================

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    WindowView() = default;

    /**
     * @brief Constructor that takes a Window @p window and creates a view into it.
     *
     * This is just a simple way of "converting" a Window to a WindowView, by having the view
     * access all the data of the Window. This automatically sets the get_element function.
     *
     * It is required and the users responsibility
     * that the scope of the @p window outlives this WindowView.
     */
    WindowView( Window<Data> const& window )
        // Set the chromosome and positions
        : BaseWindow<Data>( static_cast<BaseWindow<Data> const&>( window ))
    {
        size_t index = 0;
        get_element = [ index, &window ]() mutable -> Data const* {
            if( index >= window.size() ) {
                return nullptr;
            }
            return &window[index++].data;
        };
    }

    /**
     * @copydoc WindowView( Window<Data> const& )
     */
    WindowView( Window<Data>& window )
        // Set the chromosome and positions
        : BaseWindow<Data>( static_cast<BaseWindow<Data> const&>( window ))
    {
        size_t index = 0;
        get_element = [ index, &window ]() mutable -> Data* {
            if( index >= window.size() ) {
                return nullptr;
            }
            return &window[index++].data;
        };
    }

    virtual ~WindowView() override = default;

    WindowView( WindowView const& ) = default;
    WindowView( WindowView&& )      = default;

    WindowView& operator= ( WindowView const& ) = default;
    WindowView& operator= ( WindowView&& )      = default;

    // -------------------------------------------------------------------------
    //     Data Accessors
    // -------------------------------------------------------------------------

    Iterator begin() const
    {
        if( ! get_element ) {
            throw std::runtime_error(
                "WindowView begin() has been called without setting "
                "the get_element function first."
            );
        }
        if( started_ ) {
            throw std::runtime_error(
                "WindowView is an input iterator (single pass), "
                "but begin() has been called multiple times."
            );
        }
        started_ = true;
        return Iterator( this );
    }

    Iterator end() const
    {
        return Iterator( nullptr );
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

public:

    /**
     * @brief Function to read the next element from some input source.
     *
     * The return value of the function is expected to be a pointer to the current element in the
     * underlying input source. When called multiple times, the function is expected to advance
     * the underlying iterator first. That also means it usually needs to distinguish between
     * its first call and subsequent calls, which would require incrementing.
     */
    std::function<Data*()> get_element;

private:

    mutable bool started_ = false;

};

} // namespace population
} // namespace genesis

#endif // include guard

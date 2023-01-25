#ifndef GENESIS_POPULATION_WINDOW_WINDOW_VIEW_H_
#define GENESIS_POPULATION_WINDOW_WINDOW_VIEW_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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
 * It expexts the `get_next_element` function to be set, returning a pointer to the current underlying
 * data entry, or a `nullptr` to signal the end of the iteration. That function takes a bool
 * indicating whether this is the inital call (get the first element of the underlying input iterator),
 * or not, in which case the iterator is advanced first. This is a bit cumbersome... but allows us
 * to only take one lambda to process both cases.
 *
 * This provided `get_next_element` function is hence also responsible for advancing the underlying
 * stream. This abstraction allows the class to be used as an iterator pointing to some other data,
 * without using the being() and end() functions of that data (which might not even need to have
 * those). For example, when iteraing individual whole chromosomes as windows
 * (see ChromosomeIterator for this), we want to stop the iteration of each window after a
 * chromosome is done, which might be before the end of the data itself (if there are multiple
 * chromosomes in the input). The `get_next_element` function of this WindowView class allows to
 * define such conditions, and stops the iteration.
 *
 * Because of its streaming approach, its memory footprint is smaller than that of a Window,
 * and hence allows to iterator whole chromosomes or genomes. On the flipside, its a single pass
 * iterator with no random access to the data in the window.
 */
template<class D>
class WindowView
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
            assert( ! parent_ || parent_->get_next_element );

            // If there is a parent, this is not an end iterator.
            // Then, we need to read the first element, and check if there is any.
            if( parent_ ) {
                assert( parent_->get_next_element );
                current_ = parent_->get_next_element( true );
                if( ! current_ ) {
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
            assert( current_ );
            return current_;
        }

        value_type * operator->()
        {
            assert( parent_ );
            assert( current_ );
            return current_;
        }

        value_type const & operator*() const
        {
            assert( parent_ );
            assert( current_ );
            return *current_;
        }

        value_type & operator*()
        {
            assert( parent_ );
            assert( current_ );
            return *current_;
        }

        // -------------------------------------------------------------------------
        //     Iteration
        // -------------------------------------------------------------------------

        self_type& operator ++()
        {
            assert( parent_ );
            assert( parent_->get_next_element );

            current_ = parent_->get_next_element( false );
            if( ! current_ ) {
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

        Data* current_ = nullptr;
        WindowView const* parent_ = nullptr;

    };

    // =========================================================================
    //     Main Class
    // =========================================================================

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    WindowView() = default;

    WindowView( std::string const& chromosome )
        : chromosome_(chromosome)
    {}

    ~WindowView() = default;

    WindowView( WindowView const& ) = default;
    WindowView( WindowView&& )      = default;

    WindowView& operator= ( WindowView const& ) = default;
    WindowView& operator= ( WindowView&& )      = default;

    // -------------------------------------------------------------------------
    //     General Properties
    // -------------------------------------------------------------------------

    /**
     * @brief Get the chromosome name that this WindowView belongs to.
     */
    std::string const& chromosome() const
    {
        return chromosome_;
    }

    /**
     * @brief Set the chromosome name that this WindowView belongs to.
     *
     * Can be left empty if the window does not belong to any chromosome in particular, e.g.,
     * when iterating a whole genome. Downstream processes should hence not rely on this being
     * set to a name; it can be empty.
     */
    void chromosome( std::string const& value )
    {
        chromosome_ = value;
    }

    // -------------------------------------------------------------------------
    //     Data Accessors
    // -------------------------------------------------------------------------

    Iterator begin() const
    {
        if( ! get_next_element ) {
            throw std::runtime_error(
                "WindowView begin() has been called without setting "
                "the get_next_element function first."
            );
        }
        if( started ) {
            throw std::runtime_error(
                "WindowView is an input iterator (single pass), "
                "but begin() has been called multiple times."
            );
        }
        started = true;
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
     * The function takes a bool indicating whether this is its first call for the chromosome or not.
     * In the latter, the function is expected to advance in the input first. Then, in both bases,
     * the return value of the function is expected to be a pointer to the current element in the
     * underlying input source.
     *
     * We chose this design, which admittedly is a bit cumbersome, in order to only have to set one
     * function for both purposes, advancing and dereferencing. We also cannot do the advance after
     * getting the current element, as this might fail with iterators that delete their current
     * element after incrementing. Hence, the pointed to data (the return value of this function)
     * should always be the data that the underlying input source is at.
     */
    std::function<Data*(bool)> get_next_element;

private:

    mutable bool started = false;
    std::string chromosome_;

};

} // namespace population
} // namespace genesis

#endif // include guard

#ifndef GENESIS_POPULATION_WINDOW_WINDOW_VIEW_ITERATOR_H_
#define GENESIS_POPULATION_WINDOW_WINDOW_VIEW_ITERATOR_H_

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

#include "genesis/population/window/base_window_iterator.hpp"
#include "genesis/population/window/window.hpp"
#include "genesis/population/window/window_view.hpp"

#include "genesis/utils/core/std.hpp"

#include <cassert>
#include <functional>
#include <memory>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Window View Iterator
// =================================================================================================

/**
 * @brief Iterator wrapper that turns a BaseWindowIterator over Window into a BaseWindowIterator
 * over WindowView.
 *
 * This serves as an abstraction to be able to use WindowViewIterator everywhere, instead
 * of having to switch between WindowViewIterator and WindowIterator depending on the type
 * of windowing that is being done. For example, SlidingIntervalWindowIterator,
 * SlidingEntriesWindowIterator, and RegionWindowIterator yield iterators over Window%s,
 * while the ChromosomeIterator (and the whole genome iterator that can be created with it as well)
 * yield iterators over WindowView%s instead. This makes it cumbersome to switch between the two
 * types downstream. Hence, yet another abstraction.
 *
 * In more technical terms, some of our window iterators are inheriting from
 * `BaseWindowIterator<InputIterator, Data, Window>`, while others are inheriting from
 * `BaseWindowIterator<InputIterator, Data, WindowView>`, making their base clases incompatible,
 * so that they cannot be assigned to the same pointer variable. This class here solves this,
 * by wrapping the former into a class derived from the latter, and hence using the latter type
 * as the one that we can use for uniform access to window iterators.
 *
 * The class takes the BaseWindowIterator to be iterated over as input, and iteraters its windows,
 * and then simply wraps them into a WindowView whose elements point to these windows.
 */
template<class InputIterator, class Data = typename InputIterator::value_type>
class WindowViewIterator final : public BaseWindowIterator<
    InputIterator, Data, ::genesis::population::WindowView<Data>
>
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using InputIteratorType = InputIterator;
    using DataType          = Data;
    using WindowType        = ::genesis::population::WindowView<DataType>;
    using WindowViewType    = WindowType;

    using base_type = BaseWindowIterator<InputIteratorType, DataType, WindowViewType>;
    using self_type = WindowViewIterator<InputIteratorType, DataType>;

    using WrappedWindowIterator = BaseWindowIterator<InputIteratorType, DataType>;

    static_assert(
        std::is_same<WindowViewIterator, self_type>::value, "WindowViewIterator != self_type"
    );

    // The input types that we take from the underlying iterator over genome positions.
    using InputType         = typename InputIterator::value_type;
    // using Entry             = typename Window::Entry;
    using WrappedWindowIteratorIterator = typename WrappedWindowIterator::Iterator;

    // This class produces an iterator of type WindowView.
    // That WindowView then iterates over the actual values of the input.
    using iterator_category = std::input_iterator_tag;
    using value_type        = WindowViewType;
    using pointer           = value_type*;
    using reference         = value_type&;
    using const_reference   = value_type const&;

    // ======================================================================================
    //      Internal Iterator
    // ======================================================================================

    /**
     * @brief Internal iterator that produces WindowView%s.
     */
    class DerivedIterator final : public BaseWindowIterator<
        InputIterator, DataType, WindowViewType
    >::BaseIterator
    {
    public:

        // -------------------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------------------

        using self_type = typename WindowViewIterator<
            InputIterator, DataType
        >::DerivedIterator;

        // using base_iterator_type = typename base_type::BaseIterator;
        using base_iterator_type = typename BaseWindowIterator<
            InputIterator, DataType, WindowViewType
        >::BaseIterator;

        // using WindowViewType    = WindowViewType;
        // using Window            = ::genesis::population::Window<DataType>;
        // using Entry             = typename Window::Entry;
        using InputType         = typename InputIterator::value_type;

        using iterator_category = std::input_iterator_tag;
        using value_type        = WindowViewType;
        using pointer           = value_type*;
        using reference         = value_type&;
        using const_reference   = value_type const&;

        static_assert(
            std::is_same<DerivedIterator, self_type>::value, "DerivedIterator != self_type"
        );

    private:

        DerivedIterator() = default;

        DerivedIterator(
            WindowViewIterator const* parent
        )
            // We here call the base class constructor without arguments,
            // in order to not trigger the full setup of the base class, which we don't want here.
            // Hella hacky, but works.
            : base_iterator_type()
            , parent_( parent )
        {
            // Edge case check. See Base for details.
            if( ! parent_ ) {
                return;
            }

            // Store the underlying window iterators.
            current_ = parent_->window_iterator_->begin();
            end_     = parent_->window_iterator_->end();

            // Start a view into the first window. This creates a view that mirrors the underlying
            // window, and iteratres through it, using the WindowView constructor that takes a Window.
            window_view_ = WindowViewType{ *current_ };
            execute_visitors_( window_view_ );
        }

    public:

        virtual ~DerivedIterator() = default;

        DerivedIterator( self_type const& ) = default;
        DerivedIterator( self_type&& )      = default;

        DerivedIterator& operator= ( self_type const& ) = default;
        DerivedIterator& operator= ( self_type&& )      = default;

        friend WindowViewIterator;

        // -------------------------------------------------------------------------
        //     Internal and Virtual Members
        // -------------------------------------------------------------------------

    private:

        void increment_() override final
        {
            // Check that we are still good. If not, this function being called is likely a user
            // error by trying to increment a past-the-end iterator.
            assert( parent_ );

            // Increment the window
            ++current_;
            if( current_ == end_ ) {
                parent_ = nullptr;
                return;
            }

            // Start a view into the new window. This creates a view that mirrors the underlying
            // window, and iteratres through it, using the WindowView constructor that takes a Window.
            window_view_ = WindowViewType{ *current_ };
            execute_visitors_( window_view_ );
        }

        value_type& get_current_window_() const override final
        {
            return const_cast<value_type&>( window_view_ );
        }

        base_type const* get_parent_() const override final
        {
            return parent_;
        }

        void execute_visitors_( WindowViewType const& element )
        {
            assert( parent_ );
            for( auto const& visitor : parent_->visitors_ ) {
                visitor( element );
            }
        }

    private:

        // Parent. Needs to live here to have the correct derived type.
        WindowViewIterator const* parent_ = nullptr;

        // Base window iterators
        WrappedWindowIteratorIterator current_;
        WrappedWindowIteratorIterator end_;

        // Store the iterator for the window.
        WindowViewType window_view_;

    };

    // ======================================================================================
    //      Main Class
    // ======================================================================================

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    WindowViewIterator(
        std::unique_ptr<WrappedWindowIterator> window_iterator
    )
        : window_iterator_( std::move( window_iterator ))
    {}

    virtual ~WindowViewIterator() = default;

    WindowViewIterator( WindowViewIterator const& ) = default;
    WindowViewIterator( WindowViewIterator&& )      = default;

    WindowViewIterator& operator= ( WindowViewIterator const& ) = default;
    WindowViewIterator& operator= ( WindowViewIterator&& )      = default;

    friend DerivedIterator;

    // -------------------------------------------------------------------------
    //     Visitors
    // -------------------------------------------------------------------------

    /**
     * @brief Add a visitor function that is executed when the iterator moves to a new Window
     * during the iteration.
     *
     * These functions are executed when starting and incrementing the iterator, once for each
     * Window, in the order in which they are added here. They take the WindowView that the iterator
     * just moved to as their argument, so that user code can react to the new Window properties.
     *
     * They are a way of adding behaviour to the iteration loop that could also simply be placed
     * in the beginning of the loop body of the user code. Still, offering this here can reduce
     * redundant code, such as logging Windows during the iteration.
     */
    self_type& add_visitor( std::function<void(WindowViewType const&)> const& visitor )
    {
        visitors_.push_back( visitor );
        return *this;
    }

    /**
     * @brief Clear all functions that are executed on incrementing to the next element.
     */
    self_type& clear_visitors()
    {
        visitors_.clear();
    }

    // -------------------------------------------------------------------------
    //     Virtual Members
    // -------------------------------------------------------------------------

protected:

    std::unique_ptr<typename base_type::BaseIterator>
    get_begin_iterator_() override final
    {
        // Cannot use make_unique here, as the Iterator constructor is private,
        // and trying to make make_unique a friend does not seem to be working...
        return std::unique_ptr<DerivedIterator>( new DerivedIterator( this ));
        // return utils::make_unique<DerivedIterator>( this );
    }

    std::unique_ptr<typename base_type::BaseIterator>
    get_end_iterator_() override final
    {
        return std::unique_ptr<DerivedIterator>( new DerivedIterator( nullptr ));
        // return utils::make_unique<DerivedIterator>( nullptr );
    }

    // -------------------------------------------------------------------------
    //     Members
    // -------------------------------------------------------------------------

private:

    // Need a pointer here, in order to allow to take derived classes.
    std::unique_ptr<WrappedWindowIterator> window_iterator_;

    // Keep the visitors for each window view.
    std::vector<std::function<void(WindowViewType const&)>> visitors_;

};

// =================================================================================================
//     Make Window View Iterator
// =================================================================================================

/**
 * @brief Create a WindowViewIterator that iterates some underlying BaseWindowIterator.
 *
 * This serves as an abstraction to be able to use WindowViewIterator everywhere, instead
 * of having to switch between WindowViewIterator and WindowIterator depending on the type
 * of windowing that is being done. See WindowViewIterator for details.
 */
template<class T>
WindowViewIterator<typename T::InputIteratorType, typename T::DataType>
make_window_view_iterator(
    T const& window_iterator
) {
    using InputIteratorType = typename T::InputIteratorType;
    using DataType          = typename T::DataType;
    using BaseWindowIteratorType = BaseWindowIterator<InputIteratorType, DataType>;

    return WindowViewIterator<InputIteratorType, DataType>(
        std::unique_ptr<BaseWindowIteratorType>( new T{ window_iterator })
    );
}

/**
 * @copydoc make_window_view_iterator( T const& )
 *
 * This overload of the function takes the underlying iterator by r-value ref,
 * so that it can be provided directly without copy.
 */
template<class T>
WindowViewIterator<typename T::InputIteratorType, typename T::DataType>
make_window_view_iterator(
    T&& window_iterator
) {
    using InputIteratorType = typename T::InputIteratorType;
    using DataType          = typename T::DataType;
    using BaseWindowIteratorType = BaseWindowIterator<InputIteratorType, DataType>;

    return WindowViewIterator<InputIteratorType, DataType>(
        std::unique_ptr<BaseWindowIteratorType>( new T{ std::move( window_iterator )})
    );
}

} // namespace population
} // namespace genesis

#endif // include guard

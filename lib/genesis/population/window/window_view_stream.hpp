#ifndef GENESIS_POPULATION_WINDOW_WINDOW_VIEW_STREAM_H_
#define GENESIS_POPULATION_WINDOW_WINDOW_VIEW_STREAM_H_

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

#include "genesis/population/window/base_window_stream.hpp"
#include "genesis/population/window/window.hpp"
#include "genesis/population/window/window_view.hpp"

#include "genesis/utils/core/std.hpp"

#include <cassert>
#include <memory>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Window View Stream
// =================================================================================================

/**
 * @brief Stream wrapper that turns a BaseWindowStream over Window into a BaseWindowStream
 * over WindowView.
 *
 * This serves as an abstraction to be able to use WindowViewStream everywhere, instead
 * of having to switch between WindowViewStream and WindowStream depending on the type
 * of windowing that is being done. For example, SlidingIntervalWindowStream,
 * SlidingEntriesWindowStream, and RegionWindowStream yield streams over Window%s,
 * while the ChromosomeStream (and the whole genome stream that can be created with it as well)
 * yield streams over WindowView%s instead. This makes it cumbersome to switch between the two
 * types downstream. Hence, yet another abstraction.
 *
 * In more technical terms, some of our window streams are inheriting from
 * `BaseWindowStream<InputStreamIterator, Data, Window>`, while others are inheriting from
 * `BaseWindowStream<InputStreamIterator, Data, WindowView>`, making their base classes incompatible,
 * so that they cannot be assigned to the same pointer variable. This class here solves this,
 * by wrapping the former into a class derived from the latter, and hence using the latter type
 * as the one that we can use for uniform access to window streams.
 *
 * The class takes the BaseWindowStream to be iterated over as input, and iteraters its windows,
 * and then simply wraps them into a WindowView whose elements point to these windows.
 */
template<class InputStreamIterator, class Data = typename InputStreamIterator::value_type>
class WindowViewStream final : public BaseWindowStream<
    InputStreamIterator, Data, ::genesis::population::WindowView<Data>
>
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using InputStreamType = InputStreamIterator;
    using DataType          = Data;
    using WindowType        = ::genesis::population::WindowView<DataType>;
    using WindowViewType    = WindowType;

    using base_type = BaseWindowStream<InputStreamType, DataType, WindowViewType>;
    using self_type = WindowViewStream<InputStreamType, DataType>;

    using WrappedWindowStream = BaseWindowStream<InputStreamType, DataType>;

    static_assert(
        std::is_same<WindowViewStream, self_type>::value, "WindowViewStream != self_type"
    );

    // The input types that we take from the underlying iterator over genome positions.
    using InputType         = typename InputStreamIterator::value_type;
    // using Entry             = typename Window::Entry;
    using WrappedWindowStreamIterator = typename WrappedWindowStream::Iterator;

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
    class DerivedIterator final : public BaseWindowStream<
        InputStreamIterator, DataType, WindowViewType
    >::BaseIterator
    {
    public:

        // -------------------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------------------

        using self_type = typename WindowViewStream<
            InputStreamIterator, DataType
        >::DerivedIterator;

        // using base_iterator_type = typename base_type::BaseIterator;
        using base_iterator_type = typename BaseWindowStream<
            InputStreamIterator, DataType, WindowViewType
        >::BaseIterator;

        // using WindowViewType    = WindowViewType;
        // using Window            = ::genesis::population::Window<DataType>;
        // using Entry             = typename Window::Entry;
        using InputType         = typename InputStreamIterator::value_type;

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
            WindowViewStream const* parent
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
            current_ = parent_->window_stream_->begin();
            end_     = parent_->window_stream_->end();

            // Start a view into the first window. This creates a view that mirrors the underlying
            // window, and iteratres through it, using the WindowView constructor that takes a Window.
            window_view_ = WindowViewType{ *current_ };
        }

    public:

        virtual ~DerivedIterator() = default;

        DerivedIterator( self_type const& ) = default;
        DerivedIterator( self_type&& )      = default;

        DerivedIterator& operator= ( self_type const& ) = default;
        DerivedIterator& operator= ( self_type&& )      = default;

        friend WindowViewStream;

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
        }

        value_type& get_current_window_() const override final
        {
            return const_cast<value_type&>( window_view_ );
        }

        base_type const* get_parent_() const override final
        {
            return parent_;
        }

    private:

        // Parent. Needs to live here to have the correct derived type.
        WindowViewStream const* parent_ = nullptr;

        // Base window iterators
        WrappedWindowStreamIterator current_;
        WrappedWindowStreamIterator end_;

        // Store the iterator for the window.
        WindowViewType window_view_;

    };

    // ======================================================================================
    //      Main Class
    // ======================================================================================

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    WindowViewStream(
        std::unique_ptr<WrappedWindowStream> window_iterator
    )
        : window_stream_( std::move( window_iterator ))
    {}

    virtual ~WindowViewStream() = default;

    WindowViewStream( WindowViewStream const& ) = default;
    WindowViewStream( WindowViewStream&& )      = default;

    WindowViewStream& operator= ( WindowViewStream const& ) = default;
    WindowViewStream& operator= ( WindowViewStream&& )      = default;

    friend DerivedIterator;

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
    std::unique_ptr<WrappedWindowStream> window_stream_;

};

// =================================================================================================
//     Make Window View Iterator
// =================================================================================================

/**
 * @brief Create a WindowViewStream that iterates some underlying BaseWindowStream.
 *
 * The template parameter `T` is expected to be a BaseWindowStream.
 *
 * This serves as an abstraction to be able to use WindowViewStream everywhere, instead
 * of having to switch between WindowViewStream and WindowStream depending on the type
 * of windowing that is being done. See WindowViewStream for details.
 */
template<class T>
WindowViewStream<typename T::InputStreamType, typename T::DataType>
make_window_view_stream(
    T const& window_iterator
) {
    using InputStreamType = typename T::InputStreamType;
    using DataType          = typename T::DataType;
    using BaseWindowStreamType = BaseWindowStream<InputStreamType, DataType>;

    return WindowViewStream<InputStreamType, DataType>(
        std::unique_ptr<BaseWindowStreamType>( new T{ window_iterator })
    );
}

/**
 * @copydoc make_window_view_stream( T const& )
 *
 * This overload of the function takes the underlying iterator by r-value ref,
 * so that it can be provided directly without copy.
 */
template<class T>
WindowViewStream<typename T::InputStreamType, typename T::DataType>
make_window_view_stream(
    T&& window_iterator
) {
    using InputStreamType = typename T::InputStreamType;
    using DataType          = typename T::DataType;
    using BaseWindowStreamType = BaseWindowStream<InputStreamType, DataType>;

    return WindowViewStream<InputStreamType, DataType>(
        std::unique_ptr<BaseWindowStreamType>( new T{ std::move( window_iterator )})
    );
}

} // namespace population
} // namespace genesis

#endif // include guard

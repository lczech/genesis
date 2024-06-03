#ifndef GENESIS_POPULATION_WINDOW_BASE_WINDOW_STREAM_H_
#define GENESIS_POPULATION_WINDOW_BASE_WINDOW_STREAM_H_

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

#include "genesis/population/window/window.hpp"
#include "genesis/utils/containers/range.hpp"
#include "genesis/utils/core/std.hpp"

#include <cassert>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Base Window Stream
// =================================================================================================

/**
 * @brief Base class for streams of Window%s over the chromosomes of a genome.
 *
 * This base class serves for sliding windows, windows over regions of a genome, etc.
 *
 * The template parameters are:
 *  * `InputStreamIterator`: The type of the underlying stream over the genome data (that is, the input
 *     stream from which the windows take their data). Needs to have a member type
 *     `value_type` that specifies the actual input type that the stream produces, which we here
 *     call the `InputType` (and typedef it as that).
 *  * `Data`: The data type of the Window::Data that is stored in Window::Entry. The functor
 *    #entry_input_function needs to be provided to convert from `InputType` to this `Data`.
 *    By default, we take this to be the same as the `InputType`, meaning that the Window contains
 *    the same data type as the underlying stream that we get our data from.
 *  * `WindowType`: The type of window that is emitted in each step of the iteration. This can
 *    either be Window or WindowView, depending on whether the data needs to be kept in memory,
 *    or can be produced on the fly while iterating each window.
 *
 * The three functors
 *
 *  * #entry_input_function,
 *  * #chromosome_function, and
 *  * #position_function
 *
 * have to be set in the class prior to starting the iteration.
 *
 * The general usage of the derived classes that actually implement this stream is as follows,
 * on the example of the SlidingIntervalWindowStream:
 *
 *     // Make a window stream using some underlying data stream
 *     // that yields data for one position in the genome at a time.
 *     auto win_it = SlidingIntervalWindowStream<InputStreamIterator>( data_begin, data_end );
 *
 *     // Set functors to access the underlying data.
 *     win_it.entry_input_function = []( Data const& variant ) {
 *         return variant;
 *     };
 *     win_it.chromosome_function = []( Data const& variant ) {
 *         return variant.chromosome;
 *     };
 *     win_it.position_function = []( Data const& variant ) {
 *         return variant.position;
 *     };
 *
 *     // Set properties specific to the derived class.
 *     win_it.width( width );
 *     win_it.stride( stride );
 *
 *     // Iterate over Windows.
 *     for( auto it = win_it.begin(); it != win_it.end(); ++it ) {
 *         // Inside here, we typically then want to loop over the entries of each window
 *     }
 *
 *     // Alternative version.
 *     for( auto const& window : win_it ) {
 *         // Same as above, nested loop here to iterate the entries of each window
 *     }
 *
 * Other derived classes work accordingly.
 */
template<
    class InputStreamIterator,
    class Data       = typename InputStreamIterator::value_type,
    class WindowType = typename ::genesis::population::Window<Data>
>
class BaseWindowStream
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using InputStreamType   = InputStreamIterator;
    using DataType          = Data;

    using self_type         = BaseWindowStream<InputStreamIterator, DataType, WindowType>;
    using InputType         = typename InputStreamIterator::value_type;

    using iterator_category = std::input_iterator_tag;
    using value_type        = WindowType;
    using pointer           = value_type*;
    using reference         = value_type&;
    using const_reference   = value_type const&;

    // -------------------------------------------------------------------------
    //     Public Functors
    // -------------------------------------------------------------------------

    /**
     * @brief Functor to convert from the underlying input stream that provides the data
     * to fill the windows to the data that is stored per window.
     */
    std::function<DataType( InputType const& )> entry_input_function;

    /**
     * @brief Functor that yields the current chromosome, given the input stream data.
     */
    std::function<std::string( InputType const& )> chromosome_function;

    /**
     * @brief Functor that yields the current position on the chromosome,
     * given the input stream data.
     */
    std::function<size_t( InputType const& )> position_function;

    // ======================================================================================
    //      Internal Public Iterator
    // ======================================================================================

protected:

    // Forward Declaration.
    class BaseIterator;

public:

    /**
     * @brief Internal public iterator that produces Window%s.
     *
     * This is the iterator that is exposed to the user when calling begin() and end()
     * on the main class. Using this interface ensures that all types of window iterators
     * (sliding window, regions, etc) expose the same type of iterator (at least for the same
     * template parameters), making it easier to re-use code across different types of window
     * iterators.
     *
     * We internally use a PIMPL-like setup to abstract away from this and allow the actual
     * iterator implementations to run their own code. However, we here don't use PIMPL to hide
     * our implementation, but rather to simply abstract from it, so that the public interface
     * stays stable. See BaseIterator below for the class that needs to be derived from to implement
     * the actual window iteration code.
     */
    class Iterator final
    {
    public:

        // -------------------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------------------

        using self_type = typename BaseWindowStream<
            InputStreamType, DataType, WindowType
        >::Iterator;
        using InputType = typename InputStreamType::value_type;

        using iterator_category = std::input_iterator_tag;
        using value_type        = WindowType;
        using pointer           = value_type*;
        using reference         = value_type&;
        using const_reference   = value_type const&;

        static_assert( std::is_same<Iterator, self_type>::value, "Iterator != self_type" );

    protected:

        // Iterator() = delete;

        Iterator(
            BaseWindowStream const* parent,
            std::unique_ptr<BaseIterator> base_iterator
        )
            : base_parent_( parent )
            , pimpl_( std::move( base_iterator ))
        {
            // Observe the first element, if this is an active iterator.
            assert( pimpl_ );

            // If we are here from an end iterator, we do nothing.
            if( !base_parent_ ) {
                return;
            }
            assert( base_parent_ );

            // Before starting to init anything, call the callbacks.
            execute_begin_callbacks_();
            execute_on_enter_observers_();

            // Special case: no data. Need to execute the end callback as well.
            assert( pimpl_ );
            if( !pimpl_->get_parent_() ) {
                execute_end_callbacks_();
            }
        }

    public:

        Iterator() = default;
        ~Iterator() = default;

        Iterator( Iterator const& ) = delete;
        Iterator( Iterator&& other )
        {
            // Copy the base parent pointer, as that instance stays the same.
            // We however need to move the pimpl, as we own it.
            base_parent_ = other.base_parent_;
            pimpl_ = std::move( other.pimpl_ );

            // Now reset the other, so that in case of a bug here, we induce a segfault
            // instead of accidental undefined behaviour when accessing the old data.
            other.base_parent_ = nullptr;
            other.pimpl_ = nullptr;
        }

        Iterator& operator= ( Iterator const& ) = delete;
        Iterator& operator= ( Iterator&& other )
        {
            // Same as above.
            // Probably we should follow the rule of five more closely here though,
            // and implement one in terms of the other...
            base_parent_ = other.base_parent_;
            pimpl_ = std::move( other.pimpl_ );
            other.base_parent_ = nullptr;
            other.pimpl_ = nullptr;
            return *this;
        }

        friend BaseWindowStream;

        // -------------------------------------------------------------------------
        //     Properties
        // -------------------------------------------------------------------------

        /**
         * @brief Return whether the current iteration is the first of the current chromosome.
         *
         * When iterating over (e.g.) a VCF file with multiple chromosomes, this function is useful
         * to run some initialization per chromosome in the user code, such as preparing some output.
         *
         * See is_last_window() for the respective end-of-chromosome indicator, that can be used
         * to wrap up after a chromosome, such as writing the output that was producing during the
         * iterator.
         */
        bool is_first_window() const
        {
            assert( pimpl_ );
            return pimpl_->is_first_window_;
        }

        /**
         * @brief Return whether the current iteration is the last of the current chromosome.
         *
         * When iterating over (e.g.) a VCF file with multiple chromosomes, this function is useful
         * to wrap up after a chromosome in the user code, such as writing the output that was
         * producing during the iterator.
         *
         * See is_first_window() for the respective beginning-of-chromosome indicator, that can be
         * used to run some initialization per chromosome, such as preparing some output.
         */
        bool is_last_window() const
        {
            assert( pimpl_ );
            return pimpl_->is_last_window_;
        }

        // -------------------------------------------------------------------------
        //     Accessors
        // -------------------------------------------------------------------------

        value_type const & operator*() const
        {
            assert( pimpl_ );
            return pimpl_->get_current_window_();
        }

        value_type & operator*()
        {
            assert( pimpl_ );
            return pimpl_->get_current_window_();
        }

        value_type const * operator->() const
        {
            assert( pimpl_ );
            return &( pimpl_->get_current_window_() );
        }

        value_type * operator->()
        {
            assert( pimpl_ );
            return &( pimpl_->get_current_window_() );
        }

        // -------------------------------------------------------------------------
        //     Iteration
        // -------------------------------------------------------------------------

        self_type& operator ++()
        {
            assert( pimpl_ );
            assert( base_parent_ );

            // Advance to the next element, and observe it.
            // This is the only place that we need to call the leaving observers:
            // During normal iteration, this works fine anyway. Then, the last iteration
            // will first call the leaving oberservers, then try to move to the next window,
            // but find that there is none, and finish the iteration. By that time, the
            // leaving oberserver has been called correclty already, so nothing else to do.

            execute_on_leave_observers_();
            pimpl_->increment_();

            // Now that we are at the new element, we execute the enter observers.
            // If we instead reached the end of the input though, that one will do nothing.
            // In that case however, we want to execute the end callbacks instead.
            execute_on_enter_observers_();

            assert( pimpl_ );
            assert( base_parent_ );
            if( !pimpl_->get_parent_() ) {
                execute_end_callbacks_();
            }
            return *this;
        }

        // self_type operator ++(int)
        // {
        //     auto cpy = *this;
        //     increment_();
        //     return cpy;
        // }

        /**
         * @brief Compare two iterators for equality.
         *
         * Any two iterators that are copies of each other or started from the same parent
         * will compare equal, as long as neither of them is past-the-end.
         * A valid (not past-the-end) iterator and an end() iterator will not compare equal,
         * no matter from which BaseWindowStream they were created.
         * Two past-the-end iterators compare equal.
         */
        bool operator==( self_type const& other ) const
        {
            assert( pimpl_ );
            assert( other.pimpl_ );

            // We compare the parents as a baseline - two past-the-end iterator shall
            // always compare equal. If only one of them is past-the-end, they will compare false.
            return pimpl_->get_parent_() == other.pimpl_->get_parent_();
        }

        bool operator!=( self_type const& other ) const
        {
            return !(*this == other);
        }

        // -------------------------------------------------------------------------
        //     Internal and Virtual Members
        // -------------------------------------------------------------------------

    private:

        void execute_on_enter_observers_()
        {
            // If there is still a parent, we are active,
            // and execute all observers for the window.
            assert( pimpl_ );
            if( pimpl_->get_parent_() ) {
                auto& window = pimpl_->get_current_window_();
                for( auto const& observer : pimpl_->get_parent_()->on_enter_observers_ ) {
                    observer( window );
                }
            }
        }

        void execute_on_leave_observers_()
        {
            // If there is still a parent, we are active,
            // and execute all observers for the window.
            assert( pimpl_ );
            if( pimpl_->get_parent_() ) {
                auto& window = pimpl_->get_current_window_();
                for( auto const& observer : pimpl_->get_parent_()->on_leave_observers_ ) {
                    observer( window );
                }
            }
        }

        void execute_begin_callbacks_() const
        {
            assert( base_parent_ );
            for( auto const& cb : base_parent_->begin_callbacks_ ) {
                cb();
            }
        }

        void execute_end_callbacks_() const
        {
            assert( base_parent_ );
            for( auto const& cb : base_parent_->end_callbacks_ ) {
                cb();
            }
        }

        // -------------------------------------------------------------------------
        //     PIMPL-like Implementation Abstraction
        // -------------------------------------------------------------------------

    private:

        // Parent. This is only the base class, and hence cannot be used by the derived classes.
        // We need it here for the begin and end callbacks only, as those need to be run
        // indepentently of the derived classes.
        BaseWindowStream const* base_parent_ = nullptr;

        // Implementation of the derived iterator. This is where all the logic for the actual
        // window iteration lives. We use the pimpl idiom here so that this class here does
        // not need to expose derived interfaces.
        std::unique_ptr<BaseIterator> pimpl_;

    };

    // ======================================================================================
    //      Internal Base Iterator for PIMPL-like Abstraction
    // ======================================================================================

protected:

    /**
     * @brief Internal PIMPL-like implementation of the iterator that produces Window%s.
     *
     * This is the internal iterator class that the actual window iterator needs to derive from.
     * It declares the interface that we expect in the PIMPL-like setup in the above public
     * Iterator.
     */
    class BaseIterator
    {
    public:

        // -------------------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------------------

        using self_type = typename BaseWindowStream<
            InputStreamType, DataType, WindowType
        >::BaseIterator;
        using InputType = typename InputStreamType::value_type;

        using iterator_category = std::input_iterator_tag;
        using value_type        = WindowType;
        using pointer           = value_type*;
        using reference         = value_type&;
        using const_reference   = value_type const&;

        static_assert( std::is_same<BaseIterator, self_type>::value, "BaseIterator != self_type" );

    protected:

        BaseIterator() = default;

        /**
         * @brief Construct the base class, which does initialization checks on its member
         * variables to ensure that the user has set up the functors correctly.
         */
        BaseIterator( BaseWindowStream const* parent )
        {
            init_( parent );
        }

    public:

        virtual ~BaseIterator() = default;

        // Abstract base class. Cannot be defaulted, and there is no need to.
        // We leave that to the implementations.
        //
        // BaseIterator( self_type const& ) = default;
        // BaseIterator( self_type&& )      = default;
        // BaseIterator& operator= ( self_type const& ) = default;
        // BaseIterator& operator= ( self_type&& )      = default;

        friend BaseWindowStream;
        friend Iterator;

        // -------------------------------------------------------------------------
        //     Internal Members
        // -------------------------------------------------------------------------

    protected:

        /**
         * @brief Initialize the base iterator class and check that it is set up correctly.
         */
        void init_( BaseWindowStream const* parent )
        {
            // We use the parent as a check if this Iterator is intended to be a begin()
            // or end() iterator. If its the former, init. If the latter, we are done here.
            // After we are done iterating the input (for which we do need _its_ begin and end
            // iterators), we then set the parent_ to nullptr, as a sign that we are done.
            // This allows us also to know if we reached end() (of the window iteration;
            // not of the underlying data iterator) without having to store the end() iterator
            // when using this class.
            if( ! parent ) {
                return;
            }

            // Check that the functors are set up.
            if( ! parent->entry_input_function ) {
                throw std::runtime_error(
                    "Need to set BaseWindowStream::entry_input_function "
                    "before iterating over Windows with a Window Iterator."
                );
            }
            if( ! parent->chromosome_function ) {
                throw std::runtime_error(
                    "Need to set BaseWindowStream::chromosome_function "
                    "before iterating over Windows with a Window Iterator."
                );
            }
            if( ! parent->position_function ) {
                throw std::runtime_error(
                    "Need to set BaseWindowStream::position_function "
                    "before iterating over Windows with a Window Iterator."
                );
            }

            // Copy over the underlying data iterator.
            current_ = parent->begin_;
            end_     = parent->end_;
        }

        // -------------------------------------------------------------------------
        //     Virtual Members
        // -------------------------------------------------------------------------

    protected:

        /**
         * @brief Advance in the iteration. This function is called from operator++
         */
        virtual void increment_() = 0;

        /**
         * @brief Get the current window that the iterator shall return when dereferenced.
         */
        virtual value_type& get_current_window_() const = 0;

        /**
         * @brief Get a pointer to the base class parent.
         *
         * In the derived class implementation, this should be a pointer to the _derived_ parent
         * class, to make sure that it contains the correct settings etc needed for the iteration.
         */
        virtual BaseWindowStream const* get_parent_() const = 0;

    protected:

        // Need to manually keep track of those...
        bool is_first_window_ = true;
        bool is_last_window_ = false;

        // Underlying data stream
        InputStreamIterator current_;
        InputStreamIterator end_;

    };

    // ======================================================================================
    //      Main Class
    // ======================================================================================

public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    BaseWindowStream( InputStreamIterator begin, InputStreamIterator end )
        : begin_(begin)
        , end_(end)
    {}

    virtual ~BaseWindowStream() = default;

    BaseWindowStream( BaseWindowStream const& ) = default;
    BaseWindowStream( BaseWindowStream&& )      = default;

    BaseWindowStream& operator= ( BaseWindowStream const& ) = default;
    BaseWindowStream& operator= ( BaseWindowStream&& )      = default;

    friend Iterator;

    // -------------------------------------------------------------------------
    //     Observers
    // -------------------------------------------------------------------------

    /**
     * @brief Add a observer function that is executed once for each window during the iteration,
     * when entering the window during the iteration.
     *
     * These functions are executed when starting and incrementing the iterator, once for each
     * window, in the order in which they are added here. They take the window (typically of type
     * Window or WindowView) that the iterator just moved to as their argument, so that user code
     * can react to the new window properties.
     *
     * They are a way of adding behaviour to the iteration loop that could also simply be placed
     * in the beginning of the loop body of the user code. Still, offering this here can reduce
     * redundant code, such as logging window positions during the iteration.
     */
    self_type& add_on_enter_observer( std::function<void(WindowType const&)> const& observer )
    {
        on_enter_observers_.push_back( observer );
        return *this;
    }

    /**
     * @brief Add a observer function that is executed once for each window during the iteration,
     * when leaving the window during the iteration.
     *
     * These functions are executed when incrementing the iterator towards the next window or at the
     * end of the iteration, once for each window, in the order in which they are added here.
     * They take the window (typically of type Window or WindowView) that the iterator is about to
     * move on from as their argument, so that user code can react to the new window properties.
     *
     * They are a way of adding behaviour to the iteration loop that could also simply be placed
     * at the end of the loop body of the user code. Still, offering this here can reduce
     * redundant code, such as logging window positions during the iteration.
     */
    self_type& add_on_leave_observer( std::function<void(WindowType const&)> const& observer )
    {
        on_leave_observers_.push_back( observer );
        return *this;
    }

    /**
     * @brief Clear all functions that are executed on incrementing to the next element.
     *
     * This clears both the on enter and on leave observers.
     */
    self_type& clear_observers()
    {
        on_enter_observers_.clear();
        on_leave_observers_.clear();
        return *this;
    }

    /**
     * @brief Add a callback function that is executed when beginning the iteration.
     *
     * Similar to the functionality offered by the observers, this could also be achieved by
     * executing these functions direclty where needed, but having it as a callback here helps
     * to reduce code duplication.
     *
     * See also add_end_callback().
     */
    self_type& add_begin_callback( std::function<void()> const& callback )
    {
        if( started_ ) {
            throw std::runtime_error(
                "Window Stream: Cannot change callbacks after iteration has started."
            );
        }
        begin_callbacks_.push_back( callback );
        return *this;
    }

    /**
     * @brief Add a callback function that is executed when the end of the iteration is reached.
     *
     * This is similar to the add_begin_callback() functionality, but instead of executing the
     * callback when starting the iteration, it is called when ending it. Again, this is meant
     * as a means to reduce user code duplication, for example for logging needs.
     */
    self_type& add_end_callback( std::function<void()> const& callback )
    {
        if( started_ ) {
            throw std::runtime_error(
                "Window Stream: Cannot change callbacks after iteration has started."
            );
        }
        end_callbacks_.push_back( callback );
        return *this;
    }

    /**
     * @brief Clear all functions that have been added via add_begin_callback() and
     * add_end_callback().
     */
    self_type& clear_callbacks()
    {
        if( started_ ) {
            throw std::runtime_error(
                "Window Stream: Cannot change callbacks after iteration has started."
            );
        }
        begin_callbacks_.clear();
        end_callbacks_.clear();
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Iteration
    // -------------------------------------------------------------------------

    Iterator begin()
    {
        if( started_ ) {
            throw std::runtime_error(
                "Window Stream is an input iterator (single pass), "
                "but begin() has been called multiple times."
            );
        }
        started_ = true;
        return Iterator( this, get_begin_iterator_() );
    }

    Iterator end()
    {
        return Iterator( nullptr, get_end_iterator_() );
    }

    // -------------------------------------------------------------------------
    //     Virtual Members
    // -------------------------------------------------------------------------

protected:

    // Need a default for WindowViewStream.
    BaseWindowStream() = default;

    /**
     * @brief Get the begin iterator.
     *
     * Needs to be implemented by the derived class, to give the correct derived BaseIterator
     * instance.
     */
    virtual std::unique_ptr<BaseIterator> get_begin_iterator_() = 0;

    /**
     * @brief Get the end iterator.
     *
     * Needs to be implemented by the derived class, to give the correct derived BaseIterator
     * instance.
     */
    virtual std::unique_ptr<BaseIterator> get_end_iterator_() = 0;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // Underlying iterator to the data that we want to put in windows.
    InputStreamIterator begin_;
    InputStreamIterator end_;
    mutable bool started_ = false;

    // Keep the observers for each window view.
    std::vector<std::function<void(WindowType const&)>> on_enter_observers_;
    std::vector<std::function<void(WindowType const&)>> on_leave_observers_;

    // We furthermore allow callbacks for the beginning and and of the iteration.
    std::vector<std::function<void()>> begin_callbacks_;
    std::vector<std::function<void()>> end_callbacks_;

};

} // namespace population
} // namespace genesis

#endif // include guard

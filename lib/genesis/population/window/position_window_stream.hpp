#ifndef GENESIS_POPULATION_WINDOW_POSITION_WINDOW_STREAM_H_
#define GENESIS_POPULATION_WINDOW_POSITION_WINDOW_STREAM_H_

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

#include "genesis/population/window/base_window_stream.hpp"
#include "genesis/population/window/window_view_stream.hpp"
#include "genesis/population/window/window_view.hpp"
#include "genesis/population/window/window.hpp"

#include <cassert>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <utility>

namespace genesis {
namespace population {

// =================================================================================================
//     Single Position Window Stream
// =================================================================================================

/**
 * @brief Stream for traversing each position along a genome individually.
 *
 * With each step of the iteration, an inner Window iterator is yielded that contains exactly
 * one position on a chromosome of the underlying input data stream.
 * Then, when incrementing the main iterator, we move forward to the next position.
 * Note that the #entry_selection_function can be used to subset the loci that are used in the
 * iteration. For instance, using make_default_position_window_stream() gives an instance of this
 * class that only traverses the positions that have a Variant::status of passing.
 *
 * This class is merely meant as a simplification over manually iteration, so that it can be used
 * in line with the other window streams using the same interface of the BaseWindowStream.
 * This class contains a quite unfortunate amount of boiler plate, but hopefully makes downstream
 * algorithms easier to write.
 *
 * The four functors
 *
 *  * #entry_input_function,
 *  * #chromosome_function, and
 *  * #position_function
 *  * #entry_selection_function
 *
 * have to be set in the class prior to starting the iteration for the chromosome iterator.
 * See make_position_window_stream() and make_default_position_window_stream()
 * for helper functions that take care of this for most of our data types.
 * See BaseWindowStream for more details on the first three functors, the template parameters.
 *
 * @see make_position_window_stream()
 * @see make_default_position_window_stream()
 */
template<class InputStreamIterator, class DataType = typename InputStreamIterator::value_type>
class PositionWindowStream final : public BaseWindowStream<InputStreamIterator, DataType>
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using self_type = PositionWindowStream<InputStreamIterator, DataType>;
    using base_type = BaseWindowStream<InputStreamIterator, DataType>;

    using Window            = ::genesis::population::Window<DataType>;
    using Entry             = typename Window::Entry;
    using InputType         = typename InputStreamIterator::value_type;

    using iterator_category = std::input_iterator_tag;
    using value_type        = Window;
    using pointer           = value_type*;
    using reference         = value_type&;
    using const_reference   = value_type const&;

    // -------------------------------------------------------------------------
    //     Public Functors
    // -------------------------------------------------------------------------

    /**
     * @brief Functor that takes an entry of the underlying input stream and returns whether
     * that entry should be selected as part of the data stream iterated over here.
     *
     * Any entry that is selected (the function here returning `true`) will be considered here
     * as a single window of size one. Entries for which the function returns `false` are skipped.
     */
    std::function<bool( InputType const& )> entry_selection_function;

    // ======================================================================================
    //      Internal Iterator
    // ======================================================================================

    /**
     * @brief Internal iterator that produces WindowView%s.
     */
    class DerivedIterator final : public BaseWindowStream<InputStreamIterator, DataType>::BaseIterator
    {
    public:

        // -------------------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------------------

        using self_type = typename PositionWindowStream<
            InputStreamIterator, DataType
        >::DerivedIterator;

        using base_iterator_type = typename BaseWindowStream<
            InputStreamIterator, DataType
        >::BaseIterator;

        using Window            = ::genesis::population::Window<DataType>;
        using Entry             = typename Window::Entry;
        using InputType         = typename InputStreamIterator::value_type;

        using iterator_category = std::input_iterator_tag;
        using value_type        = Window;
        using pointer           = value_type*;
        using reference         = value_type&;
        using const_reference   = value_type const&;

    private:

        DerivedIterator() = default;

        DerivedIterator(
            PositionWindowStream const* parent
        )
            : base_iterator_type( parent )
            , parent_( parent )
        {
            // Edge case check. See Base for details.
            if( ! parent_ ) {
                return;
            }

            // Check that our selection functor is set up. The other three are already checked
            // in the base class, which is called from the delegated constructor call above.
            if( ! parent->entry_selection_function ) {
                throw std::runtime_error(
                    "Need to set BaseWindowStream::entry_selection_function "
                    "before iterating over Windows with a PositionWindowStream."
                );
            }

            // Let's get going.
            increment_();
        }

    public:

        virtual ~DerivedIterator() override = default;

        DerivedIterator( self_type const& ) = default;
        DerivedIterator( self_type&& )      = default;

        DerivedIterator& operator= ( self_type const& ) = default;
        DerivedIterator& operator= ( self_type&& )      = default;

        friend PositionWindowStream;

        // -------------------------------------------------------------------------
        //     Internal and Virtual Members
        // -------------------------------------------------------------------------

    private:

        void increment_() override final
        {
            // Check that we are still good. If not, this function being called is likely a user
            // error by trying to increment a past-the-end iterator.
            assert( parent_ );

            // Find the next selected entry that we want to visit.
            // Except for in the first iteration, this loop is never executed,
            // as we move to the next entry below already when wrapping up the increment.
            while(
                base_iterator_type::current_ != base_iterator_type::end_ &&
                ! parent_->entry_selection_function( *base_iterator_type::current_ )
            ) {
                ++base_iterator_type::current_;
            }

            // If that lead us to the end of the input, we are done.
            if( base_iterator_type::current_ == base_iterator_type::end_ ) {
                parent_ = nullptr;
                return;
            }
            assert( base_iterator_type::current_ != base_iterator_type::end_ );
            assert( parent_->entry_selection_function( *base_iterator_type::current_ ));

            // We are now at a position that we want to visit for the iteration. Get the basics.
            auto const cur_chr = parent_->chromosome_function( *base_iterator_type::current_ );
            auto const cur_pos = parent_->position_function( *base_iterator_type::current_ );

            // Set the chromosome for this position.
            // If it is a new one, we also mark this as the first window on the chromosome.
            base_iterator_type::is_first_window_ = ( cur_chr != window_.chromosome() );
            window_.chromosome( cur_chr );

            // Update the window position. The window uses a closed interval,
            // where both first and last position are part of the interval.
            window_.first_position( cur_pos );
            window_.last_position( cur_pos );

            // Now enqueue the single entry, and move to the next.
            window_.entries().clear();
            window_.entries().emplace_back(
                next_index_,
                cur_pos,
                parent_->entry_input_function( *base_iterator_type::current_ )
            );
            ++next_index_;

            // We now already need to move to the next entry that we want to visit.
            // This is because otherwise we would not know if we are already at the end of
            // the chromosome or data, and hence could not set the last window property.
            // So first, we move one entry explicitly, and then move on as before.
            ++base_iterator_type::current_;
            while(
                base_iterator_type::current_ != base_iterator_type::end_ &&
                ! parent_->entry_selection_function( *base_iterator_type::current_ )
            ) {
                ++base_iterator_type::current_;
            }

            // We just moved to the next entry, so we can check if we are at the end of the
            // chromosome or data now, and set the last window property accordingly.
            base_iterator_type::is_last_window_ = (
                base_iterator_type::current_ == base_iterator_type::end_ ||
                parent_->chromosome_function( *base_iterator_type::current_ ) != window_.chromosome()
            );
        }

        value_type& get_current_window_() const override final
        {
            return const_cast<value_type&>( window_ );
        }

        base_type const* get_parent_() const override final
        {
            return parent_;
        }

    private:

        // Parent. Needs to live here to have the correct derived type.
        PositionWindowStream const* parent_ = nullptr;

        // Current window and the index
        Window window_;
        size_t next_index_ = 0;

    };

    // ======================================================================================
    //      Main Class
    // ======================================================================================

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    PositionWindowStream(
        InputStreamIterator begin, InputStreamIterator end
    )
        : base_type( begin, end )
    {}

    virtual ~PositionWindowStream() override = default;

    PositionWindowStream( PositionWindowStream const& ) = default;
    PositionWindowStream( PositionWindowStream&& )      = default;

    PositionWindowStream& operator= ( PositionWindowStream const& ) = default;
    PositionWindowStream& operator= ( PositionWindowStream&& )      = default;

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

};

// =================================================================================================
//     Make Position Window View Iterator
// =================================================================================================

/**
 * @brief Helper function to instantiate a PositionWindowStream for each position as an individual
 * window, without the need to specify the template parameters manually.
 */
template<class InputStreamIterator, class DataType = typename InputStreamIterator::value_type>
PositionWindowStream<InputStreamIterator, DataType>
make_position_window_stream(
    InputStreamIterator begin, InputStreamIterator end
) {
    return PositionWindowStream<InputStreamIterator, DataType>( begin, end );
}

/**
 * @brief Helper function to instantiate a PositionWindowStream for each position as an individual
 * window, for a default use case.
 *
 * This helper assumes that the underlying type of the input data stream and of the data
 * that we are sliding over are of the same type, that is, we do no conversion in the
 * `entry_input_function` functor of the PositionWindowStream. It further assumes that this
 * data type has public member variables `chromosome` and `position` that are accessed by the
 * `chromosome_function` and `position_function` functors of the PositionWindowStream.
 * For example, a data type that this works for is Variant data.
 *
 * The PositionWindowStream::entry_selection_function is set so that _all_ entries are selected to
 * be considered in the iteration. This can be re-set afterwards if a different criterion is needed.
 * See also make_passing_variant_position_window_stream() and
 * make_passing_variant_position_window_view_stream() for specializations of this for data
 * type Variant, which instead only select entries that have Variant::status passing.
 *
 * @see make_default_position_window_view_stream() wraps the returned stream in an additional
 * WindowViewStream.
 */
template<class InputStreamIterator>
PositionWindowStream<InputStreamIterator>
make_default_position_window_stream(
    InputStreamIterator begin, InputStreamIterator end
) {
    using DataType = typename InputStreamIterator::value_type;

    // Set functors.
    auto it = PositionWindowStream<InputStreamIterator>( begin, end );
    it.entry_input_function = []( DataType const& variant ) {
        return variant;
    };
    it.chromosome_function = []( DataType const& variant ) {
        return variant.chromosome;
    };
    it.position_function = []( DataType const& variant ) {
        return variant.position;
    };
    it.entry_selection_function = []( DataType const& variant ) {
        (void) variant;
        return true;
    };

    return it;
}

/**
 * @brief Helper class that creates a PositionWindowStream with default functors
 * and wraps it in a WindowViewStream.
 *
 * See make_default_position_window_stream() for the base functionality,
 * and see make_window_view_stream() for the wrapping behaviour.
 *
 * Note that because this is a simple wrapper around the constructor of PositionWindowStream,
 * we lose access to that class itself, so that its more specialized member functions cannot be called
 * any more. If this is needed, use the two aforementioned `make_...()` functions individually.
 */
template<class InputStreamIterator>
WindowViewStream<InputStreamIterator>
make_default_position_window_view_stream(
    InputStreamIterator begin, InputStreamIterator end
) {
    return make_window_view_stream(
        make_default_position_window_stream( begin, end )
    );
}

/**
 * @brief Helper function to instantiate a PositionWindowStream for a default use case with
 * underlying data of type Variant, where only Variant%s with passing status are selected.
 *
 * This helper assumes that the underlying type of the input data stream and of the Window%s
 * that we are sliding over are of type Variant. It is hence a more specialized version of
 * make_default_position_window_stream(). Here, we check the Variant::status, and only
 * select those Variants%s to yield a window that have a passing FilterStatus. The
 * PositionWindowStream::entry_selection_function is set accordingly.
 *
 * @see make_passing_variant_position_window_view_stream() wraps the returned stream in an
 * additional WindowViewStream.
 */
template<class InputStreamIterator>
PositionWindowStream<InputStreamIterator>
make_passing_variant_position_window_stream(
    InputStreamIterator begin, InputStreamIterator end
) {
    using DataType = typename InputStreamIterator::value_type;

    // Set functors.
    auto it = PositionWindowStream<InputStreamIterator>( begin, end );
    it.entry_input_function = []( DataType const& variant ) {
        return variant;
    };
    it.chromosome_function = []( DataType const& variant ) {
        return variant.chromosome;
    };
    it.position_function = []( DataType const& variant ) {
        return variant.position;
    };
    it.entry_selection_function = []( DataType const& variant ) {
        return variant.status.passing();
    };

    return it;
}

/**
 * @brief Helper class that creates a PositionWindowStream with default functions for Variant
 * data, and wraps it in a WindowViewStream.
 *
 * See make_passing_variant_position_window_stream() for the base functionality,
 * and see make_window_view_stream() for the wrapping behaviour.
 *
 * Note that because this is a simple wrapper around the constructor of PositionWindowStream,
 * we lose access to that class itself, so that its more specialized member functions cannot be called
 * any more. If this is needed, use the two aforementioned `make_...()` functions individually.
 */
template<class InputStreamIterator>
WindowViewStream<InputStreamIterator>
make_passing_variant_position_window_view_stream(
    InputStreamIterator begin, InputStreamIterator end
) {
    return make_window_view_stream(
        make_passing_variant_position_window_stream( begin, end )
    );
}

} // namespace population
} // namespace genesis

#endif // include guard

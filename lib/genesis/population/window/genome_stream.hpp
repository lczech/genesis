#ifndef GENESIS_POPULATION_WINDOW_GENOME_STREAM_H_
#define GENESIS_POPULATION_WINDOW_GENOME_STREAM_H_

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
#include "genesis/population/window/window_view.hpp"

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
//     Genome Stream
// =================================================================================================

/**
 * @brief Stream for traversing the entire genome as a single window,
 * with an inner WindowView iterator over the positions along the chromosomes.
 *
 * The class produces exctly one window, which then traverses all positions of the whole underlying
 * input data stream via an inner WindowView iterator. This class is merely meant as a
 * simplification and wrapper, so that downstream statistics algorithms can just use a window as
 * their input. This class contains a quite unfortunate amount of boiler plate, but hopefully
 * makes downstream algorithms easier to write.
 *
 * The three functors
 *
 *  * #entry_input_function,
 *  * #chromosome_function, and
 *  * #position_function
 *
 * have to be set in the class prior to starting the iteration for the iterator.
 * In fact, only the first of them has to be set, as we internally do not need
 * access to the chromosome and position information of the underlying data iterator.
 * But to be conformant with the other window streams, it is better to be consistent here.
 * See make_genome_stream() and make_default_genome_stream()
 * for helper functions that take care of this for most of our data types.
 *
 * See BaseWindowStream for more details on the three functors, the template parameters.
 * This class here however does not derive from the BaseWindowStream over normal Window%s,
 * but behaves in a similar way - with the exception that it does not produce Window%s in each
 * step of the iteration, as we do not want to keep the positions of a whole genome in memory.
 * Hence, instead, it yields a WindowView iterator, directly streaming over the positions of the
 * chromosome, without keeping all data in memory.
 *
 * @see make_genome_stream()
 * @see make_default_genome_stream()
 */
template<class InputStreamIterator, class DataType = typename InputStreamIterator::value_type>
class GenomeStream final : public BaseWindowStream<
    InputStreamIterator, DataType, ::genesis::population::WindowView<DataType>
>
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using WindowViewType = ::genesis::population::WindowView<DataType>;
    using self_type = GenomeStream<InputStreamIterator, DataType>;
    using base_type = BaseWindowStream<InputStreamIterator, DataType, WindowViewType>;

    // The input types that we take from the underlying stream over genome positions.
    using InputType         = typename InputStreamIterator::value_type;
    // using Entry             = typename Window::Entry;

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

        using self_type = typename GenomeStream<
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

    private:

        DerivedIterator() = default;

        DerivedIterator(
            GenomeStream const* parent
        )
            : base_iterator_type( parent )
            , parent_( parent )
            // , window_( base_iterator_type::current_, base_iterator_type::end_ )
            // , window_( parent )
        {
            // Edge case check. See Base for details.
            if( ! parent_ ) {
                return;
            }

            // For this particular iterator, where we process the whole genome,
            // we are always at the "first" and "last" window of a chromosome, in a sense...
            base_iterator_type::is_first_window_ = true;
            base_iterator_type::is_last_window_ = true;

            // Let's get going. For the whole genome case, we only need to do the init once,
            // and then are done, as the iterator will do the whole thing in one pass, so there
            // never is a second iteration, and hence, increment is never called.
            init_whole_genome_();
        }

    public:

        virtual ~DerivedIterator() override = default;

        DerivedIterator( self_type const& ) = default;
        DerivedIterator( self_type&& )      = default;

        DerivedIterator& operator= ( self_type const& ) = default;
        DerivedIterator& operator= ( self_type&& )      = default;

        friend GenomeStream;

        // -------------------------------------------------------------------------
        //     Internal and Virtual Members
        // -------------------------------------------------------------------------

    private:

        void increment_() override final
        {
            // Check that we are still good. If not, this function being called is likely a user
            // error by trying to increment a past-the-end iterator.
            assert( parent_ );

            // For whole chromosome, we always reach the end after incrementing,
            // and don't need to do anything, except for singalling that end.
            parent_ = nullptr;
        }

        void init_whole_genome_()
        {
            assert( parent_ );

            // Need to check whether there is any data at all. If not, we are done here.
            if( base_iterator_type::current_ == base_iterator_type::end_ ) {
                parent_ = nullptr;
                return;
            }

            // We need pointer variables to the iterators and other elements,
            // so that we can capture them in a C++11 lambda... bit cumbersome.
            bool is_first = true;
            auto& cur = self_type::current_;
            auto& end = self_type::end_;

            // We reset the window view, so that it is a new iterator for the new chromosome,
            // starting from the first position, with a fitting increment function.
            window_ = WindowViewType();
            window_.get_element = [ is_first, &cur, &end ]() mutable -> DataType* {
                assert( cur != end );

                // If this is the first call of the function, we are initializing the WindowView
                // with the current entry of the underlying iterator. If not, we first move to the
                // next position (if there is any), before getting the data.
                if( is_first ) {
                    is_first = false;
                    return &*cur;
                }

                // Now we are in the case that we want to move to the next position first.
                // Move to the next position, and check that it is in the correct order.
                ++cur;

                // Now check whether we are done with the chromosome.
                // If not, we return the current element that we just moved to.
                if( cur == end ) {
                    return nullptr;
                }
                return &*cur;
            };
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
        GenomeStream const* parent_ = nullptr;

        // Store the iterator for the window.
        WindowViewType window_;

    };

    // ======================================================================================
    //      Main Class
    // ======================================================================================

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    GenomeStream(
        InputStreamIterator begin, InputStreamIterator end
    )
        : base_type( begin, end )
    {}

    virtual ~GenomeStream() override = default;

    GenomeStream( GenomeStream const& ) = default;
    GenomeStream( GenomeStream&& )      = default;

    GenomeStream& operator= ( GenomeStream const& ) = default;
    GenomeStream& operator= ( GenomeStream&& )      = default;

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
//     Make Genome Window View Iterator
// =================================================================================================

/**
 * @brief Helper function to instantiate a GenomeStream for the whole genome,
 * without the need to specify the template parameters manually.
 *
 * This helper function creates a GenomeStream from the given pair of iterators, so that the whole
 * genome is traversed without stopping at individual chromosomes in each iteration.
 */
template<class InputStreamIterator, class DataType = typename InputStreamIterator::value_type>
GenomeStream<InputStreamIterator, DataType>
make_genome_stream(
    InputStreamIterator begin, InputStreamIterator end
) {
    return GenomeStream<InputStreamIterator, DataType>( begin, end );
}

/**
 * @brief Helper function to instantiate a GenomeStream for the whole genome,
 * for a default use case.
 *
 * This helper assumes that the underlying type of the input data stream and of the data
 * that we are sliding over are of the same type, that is, we do no conversion in the
 * `entry_input_function` functor of the GenomeStream. It further assumes that this
 * data type has public member variables `chromosome` and `position` that are accessed by the
 * `chromosome_function` and `position_function` functors of the GenomeStream.
 * For example, a data type that this works for is Variant data.
 *
 * This helper function creates a GenomeStream from the given pair of iterators, so that the whole
 * genome is traversed without stopping at individual chromosomes in each iteration.
 */
template<class InputStreamIterator>
GenomeStream<InputStreamIterator>
make_default_genome_stream(
    InputStreamIterator begin, InputStreamIterator end
) {
    using DataType = typename InputStreamIterator::value_type;

    // Set functors.
    auto it = GenomeStream<InputStreamIterator>( begin, end );
    it.entry_input_function = []( DataType const& variant ) {
        return variant;
    };
    it.chromosome_function = []( DataType const& variant ) {
        return variant.chromosome;
    };
    it.position_function = []( DataType const& variant ) {
        return variant.position;
    };

    // Set properties and return.
    return it;
}

} // namespace population
} // namespace genesis

#endif // include guard

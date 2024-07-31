#ifndef GENESIS_POPULATION_WINDOW_GENOME_WINDOW_STREAM_H_
#define GENESIS_POPULATION_WINDOW_GENOME_WINDOW_STREAM_H_

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
//     Genome Window Stream
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
 * See make_genome_window_stream() and make_default_genome_window_stream()
 * for helper functions that take care of this for most of our data types.
 *
 * See BaseWindowStream for more details on the three functors, the template parameters.
 * This class here however does not derive from the BaseWindowStream over normal Window%s,
 * but behaves in a similar way - with the exception that it does not produce Window%s in each
 * step of the iteration, as we do not want to keep the positions of a whole genome in memory.
 * Hence, instead, it yields a WindowView iterator, directly streaming over the positions of the
 * chromosome, without keeping all data in memory.
 *
 * @see make_genome_window_stream()
 * @see make_default_genome_window_stream()
 */
template<class InputStreamIterator, class DataType = typename InputStreamIterator::value_type>
class GenomeWindowStream final : public BaseWindowStream<
    InputStreamIterator, DataType, ::genesis::population::WindowView<DataType>
>
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using WindowViewType = ::genesis::population::WindowView<DataType>;
    using self_type = GenomeWindowStream<InputStreamIterator, DataType>;
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

        using self_type = typename GenomeWindowStream<
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
            GenomeWindowStream const* parent
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

        friend GenomeWindowStream;

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
            // We need to access the underlying iterator through the self type of the class,
            // see here https://stackoverflow.com/a/28623297/4184258
            // (we could do this in all other instances as well, but it only matters here).
            bool is_first = true;
            auto& cur = self_type::current_;
            auto& end = self_type::end_;
            auto const par = parent_;
            auto chr = parent_->chromosome_function( *base_iterator_type::current_ );
            auto const seq_dict = parent_->sequence_dict_;

            // We set the genome window view. We leave the normal properties for chromosome,
            // and start and end position of the view untouched here at their defaults,
            // as this special case instead uses the mechanism of WindowView direclty to report
            // the chromosomes and their lengths as they are encountered here in the stream.
            // This is becase we do not have a window over a single chromosome here, and hence
            // need this special case. See WindowView.
            window_ = WindowViewType();
            window_.is_whole_genome( true );
            auto& window = window_;

            window_.get_element = [
                is_first, &cur, &end, par, chr, seq_dict, &window
            ]() mutable -> DataType* {
                assert( cur != end );

                // If this is the first call of the function, we are initializing the WindowView
                // with the current entry of the underlying iterator. If not, we first move to the
                // next position (if there is any), before getting the data.
                if( is_first ) {
                    is_first = false;
                    return &*cur;
                }

                // Now we are in the case that we want to move to the next position first.
                auto const old_pos = par->position_function( *cur );
                ++cur;

                // Check whether we are done with the chromosome. That's when we want to update
                // the window chromosome lengths, and check everything related to that.
                if( cur == end || par->chromosome_function( *cur ) != chr ) {

                    // We now are finished with a chromsome, so we can add its length to the window.
                    // First we get the length, either from the data or from the dict, if given.
                    size_t chr_len = 0;
                    if( seq_dict ) {
                        auto const dict_entry = seq_dict->find( chr );
                        if( dict_entry == seq_dict->end() ) {
                            throw std::invalid_argument(
                                "In GenomeWindowStream: Cannot iterate chromosome \"" + chr +
                                "\", as the provided sequence dictionary or reference genome "
                                "does not contain the chromosome."
                            );
                        }
                        chr_len = dict_entry->length;

                        if( old_pos > chr_len ) {
                            throw std::invalid_argument(
                                "In GenomeWindowStream: Chromosome \"" + chr + "\" has length " +
                                std::to_string( chr_len ) +
                                " in the provided sequence dictionary or reference genome, "
                                "but the input data contains positions up to " +
                                std::to_string( old_pos ) + " for that chromosome."
                            );
                        }
                    } else {
                        chr_len = old_pos;
                    }

                    // Add the chr and its length to the window.
                    if( window.chromosomes().count( chr ) > 0 ) {
                        throw std::runtime_error(
                            "Chromosome " + chr + " occurs multiple times in the input."
                        );
                    }
                    window.chromosomes()[ chr ] = chr_len;

                    // Now check again whether we are done with the data.
                    // If so, nothing else to do here.
                    if( cur == end ) {
                        return nullptr;
                    }

                    // Here, we are not yet at the end of the data, but at a new chromosome.
                    assert( par->chromosome_function( *cur ) != chr );
                    chr = par->chromosome_function( *cur );

                    return &*cur;
                }
                assert( cur != end );
                assert( par->chromosome_function( *cur ) == chr );

                // Check that it is in the correct order.
                auto const new_pos = par->position_function( *cur );
                if( old_pos >= new_pos ) {
                    throw std::runtime_error(
                        "Invalid order on chromosome " + chr + " with position " +
                        std::to_string( old_pos ) + " followed by position " +
                        std::to_string( new_pos )
                    );
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
        GenomeWindowStream const* parent_ = nullptr;

        // Store the iterator for the window.
        WindowViewType window_;

    };

    // ======================================================================================
    //      Main Class
    // ======================================================================================

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    GenomeWindowStream(
        InputStreamIterator begin, InputStreamIterator end
    )
        : base_type( begin, end )
    {}

    virtual ~GenomeWindowStream() override = default;

    GenomeWindowStream( GenomeWindowStream const& ) = default;
    GenomeWindowStream( GenomeWindowStream&& )      = default;

    GenomeWindowStream& operator= ( GenomeWindowStream const& ) = default;
    GenomeWindowStream& operator= ( GenomeWindowStream&& )      = default;

    friend DerivedIterator;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    /**
     * @brief Get the currently set sequence dictionary used for the chromosome lengths.
     */
    std::shared_ptr<genesis::sequence::SequenceDict> sequence_dict() const
    {
        return sequence_dict_;
    }

    /**
     * @brief Set a sequence dictionary to be used for the chromosome lengths.
     *
     * By default, we use the chromosome positions as given in the data to set the window positions.
     * When setting a @link ::genesis::sequence::SequenceDict SequenceDict@endlink here, we use
     * lengths as provided instead, throwing an exception should the dict not contain a chromosome
     * of the input.
     *
     * To un-set the dictionary, simply call this function with a `nullptr`.
     */
    self_type& sequence_dict( std::shared_ptr<genesis::sequence::SequenceDict> value )
    {
        sequence_dict_ = value;
        return *this;
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
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // When iterating the genome, we might want to look up their lengths,
    // in order to properly set the window start and end. Otherwise we use what's in the data.
    std::shared_ptr<genesis::sequence::SequenceDict> sequence_dict_;

};

// =================================================================================================
//     Make Genome Window View Iterator
// =================================================================================================

/**
 * @brief Helper function to instantiate a GenomeWindowStream for the whole genome,
 * without the need to specify the template parameters manually.
 *
 * This helper function creates a GenomeWindowStream from the given pair of iterators, so that the whole
 * genome is traversed without stopping at individual chromosomes in each iteration.
 */
template<class InputStreamIterator, class DataType = typename InputStreamIterator::value_type>
GenomeWindowStream<InputStreamIterator, DataType>
make_genome_window_stream(
    InputStreamIterator begin, InputStreamIterator end
) {
    return GenomeWindowStream<InputStreamIterator, DataType>( begin, end );
}

/**
 * @brief Helper function to instantiate a GenomeWindowStream for the whole genome,
 * for a default use case.
 *
 * This helper assumes that the underlying type of the input data stream and of the data
 * that we are sliding over are of the same type, that is, we do no conversion in the
 * `entry_input_function` functor of the GenomeWindowStream. It further assumes that this
 * data type has public member variables `chromosome` and `position` that are accessed by the
 * `chromosome_function` and `position_function` functors of the GenomeWindowStream.
 * For example, a data type that this works for is Variant data.
 *
 * This helper function creates a GenomeWindowStream from the given pair of iterators, so that the whole
 * genome is traversed without stopping at individual chromosomes in each iteration.
 */
template<class InputStreamIterator>
GenomeWindowStream<InputStreamIterator>
make_default_genome_window_stream(
    InputStreamIterator begin, InputStreamIterator end
) {
    using DataType = typename InputStreamIterator::value_type;

    // Set functors.
    auto it = GenomeWindowStream<InputStreamIterator>( begin, end );
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

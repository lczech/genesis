#ifndef GENESIS_UTILS_CONTAINERS_LAMBDA_ITERATOR_H_
#define GENESIS_UTILS_CONTAINERS_LAMBDA_ITERATOR_H_

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

#include "genesis/utils/containers/optional.hpp"

#include <cassert>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//      Helpers
// =================================================================================================

/**
 * @brief Empty helper data struct to serve as a dummy for LambdaIterator.
 *
 * By default, the LambdaIterator::Data type does not do anything.
 * This class here serves as the empty dummy class, so that the user does not have to
 * provide one when using LambdaIterator without an extra data type.
 */
struct EmptyLambdaIteratorData
{};

// =================================================================================================
//      Lambda Iterator
// =================================================================================================

/**
 * @brief Type erasure for iterators, using std::function to eliminate the underlying input type.
 *
 * This class offers an abstraction to get a uniform iterator type over a set of underlying iterators
 * of different type. It expects a function (most likely, you want to use a lambda) that converts
 * the underlying data into the desired type T, which is where the type erasure happens.
 * The data that is iterated over is kept in an @link utils::Optional Optional@endlink,
 * which at the end of the iteration (when the underlying iterator is done) simply has to be
 * returned as @link genesis::utils::nullopt nullopt@endlink by the lambda to indicate the end
 * of the iteration.
 *
 * Example:
 *
 * ~~~{.cpp}
 *     // Convert from an iterator over VcfRecord to Variant.
 *     auto beg = vcf_range.begin();
 *     auto end = vcf_range.end();
 *
 *     // Create the conversion with type erasure via the lambda function.
 *     generator = LambdaIterator<Variant>(
 *         [beg, end]() mutable -> genesis::utils::Optional<Variant>{
 *             if( beg != end ) {
 *                 auto res = utils::make_optional<Variant>( convert_to_variant(*beg) );
 *                 ++beg;
 *                 return res;
 *             } else {
 *                 return genesis::utils::nullopt;
 *             }
 *         }
 *     );
 *
 *     // Iterate over generator.begin() and generator.end()
 *     for( auto it : generator ) ...
 * ~~~
 *
 * For other types of iterators, instead of `beg` and `end`, other input can be used:
 *
 * ~~~{.cpp}
 *     // Use a pileup iterator, which does not offer begin and end.
 *     auto it = SimplePileupInputIterator( utils::from_file( pileup_file_.value ), reader );
 *     generator = LambdaIterator<Variant>(
 *         [it]() mutable -> genesis::utils::Optional<Variant>{
 *             if( it ) {
 *                 auto res = utils::make_optional<Variant>( convert_to_variant(*it) );
 *                 ++it;
 *                 return res;
 *             } else {
 *                 return genesis::utils::nullopt;
 *             }
 *         }
 *     );
 * ~~~
 *
 * And accordingly for other underlying iterator types.
 *
 * In addition to the type `T` that we iterate over, for user convenience, we also offer to use
 * a data storage variable of the template type `D` (typedef'd as LambdaIterator::Data).
 * This data is provided at construction of the LambdaIterator, and can be accessed via the
 * @link LambdaIterator::data() data()@endlink functions.
 * it is a generic extra variable to store iterator-specific information.
 * As the LambdaIterator is intended to be initializable with just a lambda function that yields
 * the elements to traverse over, there is otherwise no convenient way to access related information
 * of the underlying iterator. For example, when iterating over a file, one might want to store
 * the file name or other characteristics of the input in the data().
 *
 * @see VariantInputIterator for a use case of this iterator that allows to traverse different
 * input file types that all are convertible to @link genesis::population::Variant Variant@endlink.
 */
template<class T, class D = EmptyLambdaIteratorData>
class LambdaIterator
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type         = LambdaIterator;
    using value_type        = T;
    using pointer           = value_type const*;
    using reference         = value_type const&;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    using Data              = D;

    // ======================================================================================
    //      Internal Iterator
    // ======================================================================================

    class Iterator
    {
    public:

        // -------------------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------------------

        using self_type  = LambdaIterator<T, D>::Iterator;
        using value_type = T;
        using Data       = D;

    private:

        Iterator() = default;

        Iterator(
            LambdaIterator* generator
        )
            : generator_(generator)
        {
            // We use the generator as a check if this Iterator is intended to be a begin()
            // or end() iterator. If its the former, get the first element.
            // We could also just use the default constructor to create end() iterators,
            // this would have the same effect.
            // After we are done iterating the input, we then set the generator_ to nullptr,
            // as a sign that we are done. This allows us also to know if we reached end() without
            // having to store the end() iterator when using this class.
            if( generator_ ) {
                if( ! generator_->get_element_ ) {
                    throw std::invalid_argument(
                        "Cannot use LambdaIterator without a function to get elements."
                    );
                }
                advance_();
            }
        }

    public:

        ~Iterator() = default;

        Iterator( self_type const& ) = default;
        Iterator( self_type&& )      = default;

        Iterator& operator= ( self_type const& ) = default;
        Iterator& operator= ( self_type&& )      = default;

        friend LambdaIterator;

        // -------------------------------------------------------------------------
        //     Accessors
        // -------------------------------------------------------------------------

        value_type const * operator->() const
        {
            assert( current_element_ );
            return &*current_element_;
        }

        value_type * operator->()
        {
            assert( current_element_ );
            return &*current_element_;
        }

        value_type const & operator*() const
        {
            assert( current_element_ );
            return *current_element_;
        }

        value_type & operator*()
        {
            assert( current_element_ );
            return *current_element_;
        }

        /**
         * @brief Optimization function that offers to steal the internal Optional.
         *
         * After this function has been called, the element is in a moved-from state,
         * and cannot be accessed via any other methods. The iterator hence cannot be
         * dereferenced any more.
         * Advancing the iterator to the next element will set the element to a new, valid value.
         */
        utils::Optional<T> take_optional()
        {
            return std::move( current_element_ );
        }

        /**
         * @brief Access the data stored in the iterator.
         */
        Data const& data() const
        {
            if( ! generator_ ) {
                throw std::runtime_error(
                    "Cannot access default constructed or past-the-end LambdaIterator content."
                );
            }
            assert( generator_ );
            return generator_->data_;
        }

        /**
         * @brief Access the data stored in the iterator.
         */
        Data& data()
        {
            if( ! generator_ ) {
                throw std::runtime_error(
                    "Cannot access default constructed or past-the-end LambdaIterator content."
                );
            }
            assert( generator_ );
            return generator_->data_;
        }

        // -------------------------------------------------------------------------
        //     Iteration
        // -------------------------------------------------------------------------

        self_type& operator ++ ()
        {
            advance_();
            return *this;
        }

        self_type operator ++(int)
        {
            auto cpy = *this;
            advance_();
            return cpy;
        }

        /**
         * @brief Return whether the iterator is at a valid position,
         * that is, if its input has yielded an element.
         */
        operator bool() const
        {
            // We are not using the optional status of the element here, as this can be in a
            // moved-from state if take_optional() was called. Instead, we rely on the fact
            // that we set the generator_ to nullptr if this is either the end() iterator or
            // if the iteration has reached its end after the data is done.
            return generator_ != nullptr;

            // assert( static_cast<bool>( current_element_ ) || current_element_ == nullopt );
            // return static_cast<bool>( current_element_ );
        }

        /**
         * @brief Compare two iterators for equality.
         *
         * Any two iterators that are created by calling begin() on the same LambdaIterator
         * instance will compare equal, as long as neither of them is past-the-end.
         * A valid (not past-the-end) iterator and an end() iterator will not compare equal,
         * no matter from which LambdaIterator they were created.
         */
        bool operator==( self_type const& it ) const
        {
            // We need fake iterator comparison here. We do not want to compare the pointed-to
            // optional objects (as this is not what iterator comparison is supposed to do),
            // so instead we just return if we belong to the same generator.
            // That works for iteration purposes.
            // We do not test if both of them have data, as this might have been moved from
            // using the take() function.
            return generator_ == it.generator_;
            // static_cast<bool>( current_element_ ) == static_cast<bool>( it.current_element_ )

            // Version for when the element is stored in a std::shared_ptr
            // return current_element_ == it.current_element_;
        }

        bool operator!=( self_type const& it ) const
        {
            return !(*this == it);
        }

        // -------------------------------------------------------------------------
        //     Internal Members
        // -------------------------------------------------------------------------

    private:

        void advance_()
        {
            assert( generator_ );

            bool got_element = false;
            do {
                // Get the next element from the input source.
                current_element_ = generator_->get_element_();

                if( current_element_ ) {
                    // If this is an element (not yet at the end of the data),
                    // apply all transforms and filters, and get out of here if all of them
                    // return `true`, that is, if none of them wants to filter out the element.
                    // If however one of them returns `false`, we need to find another element.
                    got_element = true;
                    for( auto const& tra_fil : generator_->transforms_and_filters_ ) {
                        got_element = tra_fil( *current_element_ );
                        if( ! got_element ) {
                            // If one of the transforms/filters does not want us to continue,
                            // we do not call the others. Break out of the inner loop.
                            break;
                        }
                    }

                } else {
                    // If this is not a valid element, we reached the end of the input.
                    // We note this by unsetting the generator_. Other functions here use that
                    // as a hint that we are done with the iteration.
                    assert( current_element_ == nullopt );
                    generator_ = nullptr;
                    break;
                }
            } while( ! got_element );
        }

    private:

        LambdaIterator* generator_          = nullptr;
        utils::Optional<T> current_element_ = nullopt;

    };

    // ======================================================================================
    //      Main Class
    // ======================================================================================

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    LambdaIterator() = default;

    /**
     * @brief Create an iterator over some underlying content.
     *
     * The constructor expects the function that returns elements as long as there is underlying
     * data, and returns and empty optional (@link genesis::utils::nullopt nullopt@endlink)
     * once the end is reached.
     */
    LambdaIterator(
        std::function<utils::Optional<value_type>()> get_element
    )
        : get_element_(get_element)
    {}

    /**
     * @copydoc LambdaIterator( std::function<utils::Optional<value_type>()> )
     *
     * Additionally, @p data can be given here, which we simply store and make accessible
     * via data(). This is a convenience so that iterators generated via a `make` function
     * can for example forward their input source name for user output.
     */
    LambdaIterator(
        std::function<utils::Optional<value_type>()> get_element,
        Data const& data
    )
        : get_element_(get_element)
        , data_(data)
    {}

    /**
     * @copydoc LambdaIterator( std::function<utils::Optional<value_type>()>, Data const& )
     *
     * This version of the constructor takes the data by r-value reference, for moving it.
     */
    LambdaIterator(
        std::function<utils::Optional<value_type>()> get_element,
        Data&& data
    )
        : get_element_(get_element)
        , data_( std::move( data ))
    {}

    ~LambdaIterator() = default;

    LambdaIterator( self_type const& ) = default;
    LambdaIterator( self_type&& )      = default;

    self_type& operator= ( self_type const& ) = default;
    self_type& operator= ( self_type&& )      = default;

    friend Iterator;

    // -------------------------------------------------------------------------
    //     Iteration
    // -------------------------------------------------------------------------

    Iterator begin()
    {
        return Iterator( this );
    }

    Iterator end()
    {
        return Iterator( nullptr );
    }

    /**
     * @brief Return whether a function to get elemetns was assigend to this generator,
     * that is, whether it is default constructed (`false`) or not (`true`).
     */
    operator bool() const
    {
        return static_cast<bool>( get_element_ );
    }

    /**
     * @brief Access the data stored in the iterator.
     */
    Data const& data() const
    {
        return data_;
    }

    /**
     * @brief Access the data stored in the iterator.
     */
    Data& data()
    {
        return data_;
    }

    // -------------------------------------------------------------------------
    //     Filters and Transformations
    // -------------------------------------------------------------------------

    /**
     * @brief Add a transformation function that is applied to each element of the iteration.
     *
     * Note that all of add_transform(), add_filter(), and add_transform_filter() are chained
     * in the order in which they are added - meaning that they can be mixed as needed.
     * For example, it makes sense to first filter by some property, and then apply transformations
     * only on those elements that passed the filter to avoid unneeded work.
     */
    self_type& add_transform( std::function<void(T&)> transform )
    {
        transforms_and_filters_.push_back(
            [transform]( T& element ){
                transform( element );
                return true;
            }
        );
        return *this;
    }

    /**
     * @brief Add a filter function that is applied to each element of the iteration.
     *
     * If the function returns `false`, the element is skipped in the iteration.
     *
     * @copydetails add_transform()
     */
    self_type& add_filter( std::function<bool(T const&)> filter )
    {
        transforms_and_filters_.push_back(
            [filter]( T& element ){
                return filter( element );
            }
        );
        return *this;
    }

    /**
     * @brief Add a transformation and filter function that is applied to each element
     * of the iteration.
     *
     * This can be used to transform and filter an alement at the same time,
     * as a shortcut where several steps might be needed at once.
     * If the function returns `false`, the element is skipped in the iteration.
     *
     * @copydetails add_transform()
     */
    self_type& add_transform_filter( std::function<bool(T&)> filter )
    {
        transforms_and_filters_.push_back(
            [filter]( T& element ){
                return filter( element );
            }
        );
        return *this;
    }

    /**
     * @brief Clear all filters and transformations.
     */
    self_type& clear_filters_and_transformations()
    {
        transforms_and_filters_.clear();
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // std::vector<std::function<void(T&)>> transforms_;
    // std::vector<std::function<bool(T const&)>> filters_;
    std::vector<std::function<bool(T&)>> transforms_and_filters_;

    std::function<utils::Optional<value_type>()> get_element_;
    Data data_;

};

} // namespace utils
} // namespace genesis

#endif // include guard

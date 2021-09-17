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

#include <cassert>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//      Lambda Iterator
// =================================================================================================

/**
 * @brief Type erasure for iterators, using std::function to eliminate the underlying input type.
 *
 * This class offers some abstraction to get a uniform iterator type over some underlying iterator.
 * Expects a function (most likely, you want to use a lambda) that converts the underlying data
 * into the desired type T, which is where the type erasure happens. The data that is iterated over
 * is kept in a shared pointer, which at the end of the iteration (when the underlying iterator
 * is done) simply has to be returned as nullptr by the lambda to indicate the end of the iteration.
 *
 * Example:
 *
 *     // Convert from an iterator over VcfRecord to Variant.
 *     auto beg = vcf_range.begin();
 *     auto end = vcf_range.end();
 *
 *     // Create the conversion with type erasure via the lambda function.
 *     generator = LambdaIterator<Variant>(
 *         [beg, end]() mutable -> std::shared_ptr<Variant>{
 *             if( beg != end ) {
 *                 auto res = std::make_shared<Variant>( convert_to_variant(*beg) );
 *                 ++beg;
 *                 return res;
 *             } else {
 *                 return nullptr;
 *             }
 *         }
 *     );
 *
 *     // Iterate over generator.begin() and generator.end()
 *     for( auto it : generator ) ...
 *
 * For other types of iterators, instead of `beg` and `end`, other input can be used:
 *
 *     // Use a pileup iterator, which does not offer begin and end.
 *     auto it = SimplePileupInputIterator( utils::from_file( pileup_file_.value ), reader );
 *     generator = LambdaIterator<Variant>(
 *         [it]() mutable -> std::shared_ptr<Variant>{
 *             if( it ) {
 *                 auto res = std::make_shared<Variant>( convert_to_variant(*it) );
 *                 ++it;
 *                 return res;
 *             } else {
 *                 return nullptr;
 *             }
 *         }
 *     );
 *
 * And accordinly for other underlying iterator types.
 */
template<class T>
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

    // -------------------------------------------------------------------------
    //     Iterator
    // -------------------------------------------------------------------------

    class Iterator
    {
    public:

        // -------------------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------------------

        using self_type = LambdaIterator<T>::Iterator;

        // Iterator() = default;

        Iterator(
            LambdaIterator* generator
        )
            : generator_(generator)
        {
            // We use the generator as a check if this Iterator is intended to be a begin()
            // or end() iterator. If its the former, get the first element.
            if( generator_ ) {
                advance_();
            }
        }

        ~Iterator() = default;

        Iterator( self_type const& ) = default;
        Iterator( self_type&& )      = default;

        Iterator& operator= ( self_type const& ) = default;
        Iterator& operator= ( self_type&& )      = default;

        // friend LambdaIterator;

        // -------------------------------------------------------------------------
        //     Accessors
        // -------------------------------------------------------------------------

        T const& operator*() const
        {
            return *current_element_;
        }

        // T operator*()
        // {
        //     return *current_element_;
        // }

        operator bool() const
        {
            return static_cast<bool>( current_element_ );
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

        bool operator==( self_type const& it ) const
        {
            return current_element_ == it.current_element_;
        }

        bool operator!=( self_type const& it ) const
        {
            return !(*this == it);
        }

        // -------------------------------------------------------------------------
        //     Internal Members
        // -------------------------------------------------------------------------

        void advance_()
        {
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
                    assert( current_element_ == nullptr );
                    break;
                }
            } while( ! got_element );
        }

    private:

        LambdaIterator* generator_;
        std::shared_ptr<T> current_element_;

    };

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    LambdaIterator() = default;

    LambdaIterator(
        std::function<std::shared_ptr<value_type>()> get_element
    )
        : get_element_(get_element)
    {}

    ~LambdaIterator() = default;

    LambdaIterator( self_type const& ) = default;
    LambdaIterator( self_type&& )      = default;

    self_type& operator= ( self_type const& ) = default;
    self_type& operator= ( self_type&& )      = default;

    friend Iterator;

    // -------------------------------------------------------------------------
    //     Data Members
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
     * @brief Return whether a function was assigend to this generator, that is, whether it is
     * default constructed (`false`) or not (`true`).
     */
    operator bool() const
    {
        return static_cast<bool>( get_element_ );
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
            [transform]( T& variant ){
                transform( variant );
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
            [filter]( T& variant ){
                return filter( variant );
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
            [filter]( T& variant ){
                return filter( variant );
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

    std::function<std::shared_ptr<value_type>()> get_element_;

};

} // namespace utils
} // namespace genesis

#endif // include guard

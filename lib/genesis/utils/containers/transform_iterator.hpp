#ifndef GENESIS_UTILS_CONTAINERS_TRANSFORM_ITERATOR_H_
#define GENESIS_UTILS_CONTAINERS_TRANSFORM_ITERATOR_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/containers/range.hpp"

#include <cstddef>
#include <iterator>
#include <type_traits>

namespace genesis {
namespace utils {

// =================================================================================================
//     Transforming Iterator
// =================================================================================================

/**
 * @brief Iterator class that allows to transform an underlying iterator by applying a function
 * to each element before dereferencing.
 *
 * The constructor is used to set the transformation function. See also make_transform_iterator()
 * and make_transform_range() for helper functions to easily create an instance.
 *
 * This iterator takes the argument and return types of the TransformFunctor into account:
 *
 *  * If the TransformFunctor returns by value, we copy that value into an internal cache. This way,
 *    in cases where the iterator is dereferenced multiple times at the same iteration position,
 *    we do not need to apply the TransformFunctor each time.
 *  * However, if the TransformFunctor returns by reference (const or non-const, but see also below),
 *    making a copy would be wasteful.
 *    Hence, in that case, we simply forward that reference here as well. This kind of transforming
 *    iterator is for example useful to efficiently select an entry from the underlying BaseIterator,
 *    e.g., an iterator over a `std::pair<>` could be transformed for selecting the first entry,
 *    without the need to copy that. If the TransformIterator is used that way, the underlying
 *    type does not even need to be copyable.
 *  * Lastly, if the TransformFunctor returns by non-const reference, the underlying element
 *    can also be modified!
 *
 * When using lambda functions for the `TransformFunctor`, a bit of extra care is necessary to
 * handle references correctly. In these cases, the return type has to be specified explicitly.
 * For example, to select a particular entry, the following can be used:
 *
 *     // Some vector of vectors, e.g, a table of sorts.
 *     std::vector<std::vector<LargeClass>> data;
 *
 *     // Fixed index of the element (i.e., the column when `data` is interpreted as a table)
 *     // that we want to get from each of the rows of the above data.
 *     size_t index = 2;
 *
 *     // Iterate the rows of data, and select a particular column index in each step.
 *     // Have the lambda return those values per reference in order to avoid copies.
 *     auto column_range = make_transform_range(
 *         [index]( std::vector<LargeClass> const& values ) -> LargeClass const& {
 *             assert( index < values.size() )
 *             return values[index];
 *         },
 *         data
 *     );
 *
 *     // Print the resulting values.
 *     for( auto const& value : column_range ) {
 *         std::cout << value << "\n";
 *     }
 *
 * Inspired by the Boost Transform Iterator
 * (https://www.boost.org/doc/libs/1_66_0/libs/iterator/doc/html/iterator/specialized/transform.html)
 * and the Intel TBB Transform Iterator
 * (https://software.intel.com/content/www/us/en/develop/documentation/onetbb-documentation/top/intel-174-oneapi-threading-building-blocks-onetbb-developer-reference/iterators/transform-iterator.html).
 * See also https://www.fluentcpp.com/2019/02/12/the-terrible-problem-of-incrementing-a-smart-iterator/
 * for a discussuion on the issue of calling the transform functor multiple times with caching.
 * We went a bit above and beyond that, as explained above.
 */
template< typename TransformFunctor, typename BaseIterator >
class TransformIterator
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    // Get the type that the TransformFunctor returns
    using result_type = typename std::result_of<
        TransformFunctor( typename std::iterator_traits<BaseIterator>::reference )
    >::type;

    // We want to cache the transformed values if the transform functor returns a valy by copy
    // (that is, for example, the result of some computation that is simply returned by the functor).
    // However, when the functor returns a reference instead (for example, it just selects a certain
    // element from an underlying base iterator over vectors of elements), we do not want to cache,
    // as that would make an unnecessary copy of the element (and would require that the element
    // type is copyable). So in that case, we deactivate caching. We could use SFINAE and some class
    // derivation to disable the cache member variable (see https://stackoverflow.com/a/25497790 for
    // example), but we keep it simple here and instead just use a dummy bool variable in that case.
    using cache_type = typename std::conditional<
        std::is_reference<result_type>::value, bool, result_type
    >::type;

    // Furthermore, the operator*(), operator->(), and operator[]() need to return the proper type:
    // If the TransformFunctor returns a reference, we return a reference as well. If not, that is,
    // if the functor returns a value by copy, we still return a reference here, but this time,
    // referencing the cache! So, we need to remove the reference type trait first in case that it
    // is present, and then can have these operators add the correct reference trait again.
    using return_type = typename std::remove_reference<result_type>::type;

    // For the standard iterator types, we also want to use the types of the TransformFunctor,
    // so that whatever that one does is the basis for our types here. We could instead also use the
    // (commented out) types based on the BaseIterator here as well, which in many cases will be
    // identical to what the transform functor returns. But by not doing so, and using a return
    // type based on the transform functor, we allow that the TransformFunctor can perform implicit
    // type conversions from the BaseIterator type to its input argument!
    // Bit more flexibility this way, and helpful at times.
    using value_type        = return_type;
    using pointer           = return_type*;
    using reference         = return_type&;
    // using value_type        = typename std::iterator_traits<BaseIterator>::value_type;
    // using pointer           = typename std::iterator_traits<BaseIterator>::pointer;
    // using reference         = typename std::iterator_traits<BaseIterator>::reference;

    // using reference         = result_type;
    // using iterator_category = typename std::random_access_iterator_tag;
    using iterator_category = typename std::iterator_traits<BaseIterator>::iterator_category;
    using difference_type   = typename std::iterator_traits<BaseIterator>::difference_type;

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Construct a transforming iterator, given the underlying base iterator and the
     * transformation function.
     *
     * This constructor needs to be used for both the begin and end of the range that one wants
     * to iterate, using the same transformation function both times. See also make_transform_iterator()
     * and make_transform_range() for helper functions to easily create an instance.
     */
    TransformIterator( TransformFunctor unary_func, BaseIterator iterator )
        : functor_( unary_func )
        , current_( iterator )
    {
        // We allow also non-random-access-iterators! No need for the following assertion.
        // static_assert(
        //     (
        //         std::is_same<
        //             typename std::iterator_traits<BaseIterator>::iterator_category,
        //             std::random_access_iterator_tag
        //         >::value
        //     ),
        //     "TransformIterator requires underlying BaseIterator to be a random access iterator"
        // );
    }

    ~TransformIterator() = default;

    TransformIterator( TransformIterator const& ) = default;
    TransformIterator( TransformIterator&& )      = default;

    TransformIterator& operator= ( TransformIterator const& ) = default;
    TransformIterator& operator= ( TransformIterator&& )      = default;

    // Explicit copy constructor and assignment. Not needed.
    // TransformIterator( TransformIterator const& input )
    //     : functor_( input.functor_ )
    //     , current_( input.current_ )
    // {}
    //
    // TransformIterator& operator=( TransformIterator const& input )
    // {
    //     current_ = input.current_;
    //     return *this;
    // }

    // -------------------------------------------------------------------------
    //     Basic Iterator Operators
    // -------------------------------------------------------------------------

    return_type& operator*() const
    {
        return get_value_<result_type>();
    }

    return_type* operator->() const
    {
        return &get_value_<result_type>();
    }

    return_type& operator[]( difference_type i ) const
    {
        return *(*this + i);
    }

    BaseIterator base() const
    {
        return current_;
    }

    // -------------------------------------------------------------------------
    //     Iterator Navigation
    // -------------------------------------------------------------------------

    TransformIterator& operator++()
    {
        ++current_;
        cache_valid_ = false;
        return *this;
    }

    TransformIterator& operator--()
    {
        --current_;
        cache_valid_ = false;
        return *this;
    }

    TransformIterator operator++(int)
    {
        TransformIterator it(*this);
        ++(*this);
        return it;
    }

    TransformIterator operator--(int)
    {
        TransformIterator it(*this);
        --(*this);
        return it;
    }

    TransformIterator operator+( difference_type n ) const
    {
        return { current_ + n, functor_ };
    }

    TransformIterator operator-( difference_type n ) const
    {
        return { current_ - n, functor_ };
    }

    TransformIterator& operator+=( difference_type n )
    {
        current_ += n;
        cache_valid_ = false;
        return *this;
    }

    TransformIterator& operator-=( difference_type n )
    {
        current_ -= n;
        cache_valid_ = false;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Iterator Interaction
    // -------------------------------------------------------------------------

    friend TransformIterator operator+( difference_type n, TransformIterator const& it )
    {
        return it + n;
    }

    difference_type operator-( TransformIterator const& it ) const
    {
        return current_ - it.current_;
    }

    // -------------------------------------------------------------------------
    //     Iterator Comparison
    // -------------------------------------------------------------------------

    bool operator==( TransformIterator const& it ) const
    {
        return current_ == it.current_;
    }

    bool operator!=( TransformIterator const& it ) const
    {
        return !(*this == it);
    }

    bool operator<( TransformIterator const& it ) const
    {
        return *this - it < 0;
    }

    bool operator>( TransformIterator const& it ) const
    {
        return it < *this;
    }

    bool operator<=( TransformIterator const& it ) const
    {
        return !(*this > it);
    }

    bool operator>=( TransformIterator const& it ) const
    {
        return !(*this < it);
    }

    // -------------------------------------------------------------------------
    //     Internal Functions
    // -------------------------------------------------------------------------

private:

    /**
     * @brief Implementation for reference types: Simply return the transformed value by reference.
     */
    template<typename T, typename std::enable_if< std::is_reference<T>::value >::type* = nullptr>
    return_type& get_value_() const
    {
        // Internal check: This function needs type T only for the SFINAE of std::enable_if to work.
        // But we only want this for the result_type that we are actually using here!
        static_assert(
            std::is_same<T, result_type>::value, "Function has to be called using T=result_type"
        );

        // More internal checks, just to make sure that the SFINAE works correctly.
        static_assert(
            std::is_reference<result_type>::value,
            "Function SFINAE is activated although result_type is not a reference type"
        );
        static_assert(
            std::is_same<cache_type, bool>::value,
            "cache_type is not bool although result_type is a reference type"
        );

        // Return the result immediately. This returns by reference, so that no copy is made.
        return functor_( *current_ );
    }

    /**
     * @brief Impelmentation for non-reference types: Use the cache, then return a reference to that.
     */
    template<typename T, typename std::enable_if< !std::is_reference<T>::value >::type* = nullptr>
    return_type& get_value_() const
    {
        // Internal check: This function needs type T only for the SFINAE of std::enable_if to work.
        // But we only want this for the result_type that we are actually using here!
        static_assert(
            std::is_same<T, result_type>::value, "Function has to be called using T=result_type"
        );

        // Another internal check, just to make sure that the SFINAE works correctly.
        static_assert(
            ! std::is_reference<result_type>::value,
            "Function SFINAE is activated although result_type is a reference type"
        );
        static_assert(
            std::is_same<cache_type, result_type>::value,
            "cache_type is not the same as result_type although result_type is a not reference type"
        );

        // Fill the cache as needed, and return a reference to the cached object.
        if( ! cache_valid_ ) {
            cache_ = functor_( *current_ );
            cache_valid_ = true;
        }
        return cache_;
    }

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    TransformFunctor functor_;
    BaseIterator current_;

    // Store the cached value, if needed. In cases where the TransformFunctor returns by reference,
    // we do not want to use a cache, to avoid unnecessary copies. In that case, cache_type is
    // simply set to bool as a dummy type, and never used.
    // Furthermore, the cache_valid_ check is used by some of the iteration functions even if
    // we do not have a valid cache (that is, for references). That is okay, as this is simply
    // an overhead of setting one bool value that is not used. We can live with that.
    mutable cache_type cache_;
    mutable bool cache_valid_ = false;

};

// =================================================================================================
//     Make Transforming Iterator
// =================================================================================================

/**
 * @brief Construct a transforming iterator, given the underlying base iterator and the
 * transformation function.
 *
 * This constructor needs to be used for both the begin and end of the range that one wants
 * to iterate, using the same transformation function both times. See also make_transform_iterator()
 * and make_transform_range() for helper functions to easily create an instance.
 */
template<typename TransformFunctor, typename BaseIterator>
TransformIterator<TransformFunctor, BaseIterator> make_transform_iterator(
    TransformFunctor unary_func,
    BaseIterator     iterator
) {
    return TransformIterator<TransformFunctor, BaseIterator>( unary_func, iterator );
}

/**
 * @brief Construct a transforming range, given the transformation function as well as
 * the underlying base iterator begin and end.
 */
template<typename TransformFunctor, typename BaseIterator>
Range<TransformIterator<TransformFunctor, BaseIterator>> make_transform_range(
    TransformFunctor unary_func,
    BaseIterator     begin,
    BaseIterator     end
) {
    return Range<TransformIterator<TransformFunctor, BaseIterator>>(
        TransformIterator<TransformFunctor, BaseIterator>( unary_func, begin ),
        TransformIterator<TransformFunctor, BaseIterator>( unary_func, end   )
    );
}

/**
 * @brief Construct a transforming range, given the transformation function as well as
 * a container, whose begin() and end() iterators are used, respectively.
 */
template<typename TransformFunctor, typename Container>
Range<TransformIterator<TransformFunctor, typename Container::iterator>> make_transform_range(
    TransformFunctor unary_func,
    Container& container
) {
    return Range<TransformIterator<TransformFunctor, typename Container::iterator>>(
        TransformIterator<TransformFunctor, typename Container::iterator>(
            unary_func, container.begin()
        ),
        TransformIterator<TransformFunctor, typename Container::iterator>(
            unary_func, container.end()
        )
    );
}

/**
 * @brief Construct a transforming range, given the transformation function as well as
 * a const container, whose begin() and end() iterators are used, respectively.
 */
template<typename TransformFunctor, typename Container>
Range<TransformIterator<TransformFunctor, typename Container::const_iterator>> make_transform_range(
    TransformFunctor unary_func,
    Container const& container
) {
    return Range<TransformIterator<TransformFunctor, typename Container::const_iterator>>(
        TransformIterator<TransformFunctor, typename Container::const_iterator>(
            unary_func, container.begin()
        ),
        TransformIterator<TransformFunctor, typename Container::const_iterator>(
            unary_func, container.end()
        )
    );
}

} // namespace utils
} // namespace genesis

#endif // include guard

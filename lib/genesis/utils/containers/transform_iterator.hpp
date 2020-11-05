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
 * Inspired by the Boost Transform Iterator
 * (https://www.boost.org/doc/libs/1_66_0/libs/iterator/doc/html/iterator/specialized/transform.html)
 * and the Intel TBB Transform Iterator
 * (https://software.intel.com/content/www/us/en/develop/documentation/onetbb-documentation/top/intel-174-oneapi-threading-building-blocks-onetbb-developer-reference/iterators/transform-iterator.html).
 * See also https://www.fluentcpp.com/2019/02/12/the-terrible-problem-of-incrementing-a-smart-iterator/
 * for a discussuion on the issue of calling the transform functor multiple times. Might need to
 * add caching later.
 */
template< typename TransformFunctor, typename BaseIterator >
class TransformIterator
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    // using iterator_category = typename std::random_access_iterator_tag;
    using iterator_category = typename std::iterator_traits<BaseIterator>::iterator_category;

    using value_type        = typename std::iterator_traits<BaseIterator>::value_type;
    using reference         = typename std::result_of<
        TransformFunctor( typename std::iterator_traits<BaseIterator>::reference )
    >::type;
    using pointer           = typename std::iterator_traits<BaseIterator>::pointer;
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

    reference const& operator*() const
    {
        if( ! cache_valid_ ) {
            cache_ = functor_( *current_ );
            cache_valid_ = true;
        }
        return cache_;
    }

    reference const* operator->() const
    {
        if( ! cache_valid_ ) {
            cache_ = functor_( *current_ );
            cache_valid_ = true;
        }
        return &cache_;
    }

    reference operator[]( difference_type i ) const
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
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    TransformFunctor functor_;
    BaseIterator current_;

    mutable reference cache_;
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

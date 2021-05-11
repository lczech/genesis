#ifndef GENESIS_UTILS_CONTAINERS_FILTER_ITERATOR_H_
#define GENESIS_UTILS_CONTAINERS_FILTER_ITERATOR_H_

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
//     Filtering Iterator
// =================================================================================================

/**
 * @brief Iterator class that allows to filter an underlying iterator by skipping elements that do
 * not satisfy a given predicate.
 *
 * The constructor is used to set the filter predicate function. See also make_filter_iterator()
 * and make_filter_range() for helper functions to easily create an instance.
 *
 * Inspired by the Boost Filter Iterator (https://www.boost.org/doc/libs/1_67_0/libs/iterator/doc/html/iterator/specialized/filter.html), StackOverflow (https://stackoverflow.com/a/44350827/4184258),
 * and Jez Higgins (https://www.jezuk.co.uk/blog/2008/02/a-filtering-iterator-in-c.html).
 */
template< typename PredicateFunctor, typename BaseIterator >
class FilterIterator
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using iterator_category = std::input_iterator_tag;

    using value_type        = typename std::iterator_traits<BaseIterator>::value_type;
    using reference         = typename std::iterator_traits<BaseIterator>::reference;
    using const_reference   = typename std::iterator_traits<BaseIterator>::reference const;
    using pointer           = typename std::iterator_traits<BaseIterator>::pointer;
    using difference_type   = typename std::iterator_traits<BaseIterator>::difference_type;

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Construct a filtering iterator, given the filter predicate function as well as
     * the underlying base iterator begin and end.
     *
     * Caveat: Note that by default, we use a default-constructed @p end iterator. This only works
     * for containers where the default-constructed iterator is the end iterator! In particular, this
     * does NOT work for many STL containers. For example, for a `std::vector`, one needs to
     * explicitly use `vector.end()` here.
     *
     * This constructor needs to be used for both the begin and end of the range that one wants
     * to iterate, using the same filter function both times. See also make_filter_iterator()
     * and make_filter_range() for helper functions to easily create an instance.
     */
    FilterIterator( PredicateFunctor unary_func, BaseIterator begin, BaseIterator end = BaseIterator{} )
        : predicate_( unary_func )
        , current_( begin )
        , end_( end )
    {
        // Iterate to the first element that passes the filter.
        while(( current_ != end_ ) && ( !predicate_( *current_ ))) {
            ++current_;
        }
    }

    ~FilterIterator() = default;

    FilterIterator( FilterIterator const& ) = default;
    FilterIterator( FilterIterator&& )      = default;

    FilterIterator& operator= ( FilterIterator const& ) = default;
    FilterIterator& operator= ( FilterIterator&& )      = default;

    // Explicit copy constructor and assignment. Not needed.
    // FilterIterator& operator=( FilterIterator const& input )
    // {
    //     current_ = input.current_;
    //     return *this;
    // }

    // -------------------------------------------------------------------------
    //     Basic Iterator Operators
    // -------------------------------------------------------------------------

    reference operator*()
    {
        return *current_;
    }

    const_reference operator*() const
    {
        return *current_;
    }

    reference operator->() const
    {
        return *current_;
    }

    BaseIterator base() const
    {
        return current_;
    }

    BaseIterator end() const
    {
        return end_;
    }

    // -------------------------------------------------------------------------
    //     Iterator Navigation
    // -------------------------------------------------------------------------

    FilterIterator& operator++()
    {
        advance_();
        return *this;
    }

    FilterIterator operator++(int)
    {
        FilterIterator it(*this);
        ++(*this);
        return it;
    }

    FilterIterator operator+( difference_type n ) const
    {
        // Use the += operator to advance an iterator to the desired n.
        auto copy = *this;
        copy += n;
        return copy;
    }

    FilterIterator& operator+=( difference_type n )
    {
        // Call advance for n many times, but check for premature ending.
        for( difference_type i = 0; i < n; ++i ) {
            advance_();
            if( current_ == end_ ) {
                break;
            }
        }
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Iterator Interaction
    // -------------------------------------------------------------------------

    friend FilterIterator operator+( difference_type n, FilterIterator const& it )
    {
        // Use the + operator, which uses the += operator, to advance to the desired n.
        return it + n;
    }

    // -------------------------------------------------------------------------
    //     Iterator Comparison
    // -------------------------------------------------------------------------

    bool operator==( FilterIterator const& it ) const
    {
        return current_ == it.current_;
    }

    bool operator!=( FilterIterator const& it ) const
    {
        return !(*this == it);
    }

    bool operator<( FilterIterator const& it ) const
    {
        return current_ - it.current_ < 0;
    }

    bool operator>( FilterIterator const& it ) const
    {
        return it < *this;
    }

    bool operator<=( FilterIterator const& it ) const
    {
        return !(*this > it);
    }

    bool operator>=( FilterIterator const& it ) const
    {
        return !(*this < it);
    }

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    void advance_()
    {
        // Advance at least one element, and then until the filter passes.
        do {
            ++current_;
        } while(( current_ != end_ ) && ( !predicate_( *current_ )));
    }

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    PredicateFunctor predicate_;

    BaseIterator current_;
    BaseIterator end_;

};

// =================================================================================================
//     Make Filtering Iterator
// =================================================================================================

/**
 * @brief Construct a filtering iterator, given the filter predicate function as well as
 * the underlying base iterator begin and end.
 *
 * Caveat: Note that by default, we use a default-constructed @p end iterator. This only works for
 * containers where the default-constructed iterator is the end iterator! In particular, this
 * does NOT work for many STL containers. For example, for a `std::vector`, one needs to
 * explicitly use `vector.end()` here.
 *
 * This constructor needs to be used for both the begin and end of the range that one wants
 * to iterate, using the same filter function both times. See also make_filter_iterator()
 * and make_filter_range() for helper functions to easily create an instance.
 */
template<typename PredicateFunctor, typename BaseIterator>
FilterIterator<PredicateFunctor, BaseIterator> make_filter_iterator(
    PredicateFunctor unary_func,
    BaseIterator     begin,
    BaseIterator     end = BaseIterator{}
) {
    return FilterIterator<PredicateFunctor, BaseIterator>( unary_func, begin, end );
}

/**
 * @brief Construct a filtering range, given the filter predicate function as well as
 * the underlying base iterator begin and end.
 */
template<typename PredicateFunctor, typename BaseIterator>
Range<FilterIterator<PredicateFunctor, BaseIterator>> make_filter_range(
    PredicateFunctor unary_func,
    BaseIterator     begin,
    BaseIterator     end
) {
    return Range<FilterIterator<PredicateFunctor, BaseIterator>>(
        FilterIterator<PredicateFunctor, BaseIterator>( unary_func, begin, end ),
        FilterIterator<PredicateFunctor, BaseIterator>( unary_func, end,   end )
    );
}

/**
 * @brief Construct a filtering range, given the filter predicate function as well as
 * a container, whose begin() and end() iterators are used, respectively.
 */
template<typename PredicateFunctor, typename Container>
Range<FilterIterator<PredicateFunctor, typename Container::iterator>> make_filter_range(
    PredicateFunctor unary_func,
    Container& container
) {
    return Range<FilterIterator<PredicateFunctor, typename Container::iterator>>(
        FilterIterator<PredicateFunctor, typename Container::iterator>(
            unary_func, container.begin(), container.end()
        ),
        FilterIterator<PredicateFunctor, typename Container::iterator>(
            unary_func, container.end(), container.end()
        )
    );
}

/**
 * @brief Construct a filtering range, given the filter predicate function as well as
 * a const container, whose begin() and end() iterators are used, respectively.
 */
template<typename PredicateFunctor, typename Container>
Range<FilterIterator<PredicateFunctor, typename Container::const_iterator>> make_filter_range(
    PredicateFunctor unary_func,
    Container const& container
) {
    return Range<FilterIterator<PredicateFunctor, typename Container::const_iterator>>(
        FilterIterator<PredicateFunctor, typename Container::const_iterator>(
            unary_func, container.begin(), container.end()
        ),
        FilterIterator<PredicateFunctor, typename Container::const_iterator>(
            unary_func, container.end(), container.end()
        )
    );
}

} // namespace utils
} // namespace genesis

#endif // include guard

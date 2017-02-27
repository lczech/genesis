#ifndef GENESIS_UTILS_CORE_RANGE_H_
#define GENESIS_UTILS_CORE_RANGE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

namespace genesis {
namespace utils {

// =================================================================================================
//     Range
// =================================================================================================

/**
 * @brief
 */
template <typename IteratorType>
class Range
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using iterator = IteratorType;

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    Range () = default;

    template <class Container>
    Range ( Container& cont )
        : begin_( cont.begin() )
        , end_(   cont.end() )
    {}

    template <class Container>
    Range ( Container const& cont )
        : begin_( cont.begin() )
        , end_(   cont.end() )
    {}

    Range( iterator begin, iterator end )
        : begin_(begin)
        , end_(end)
    {}

    Range( Range const& ) = default;
    Range( Range&& )      = default;

    Range& operator= ( Range const& ) = default;
    Range& operator= ( Range&& )      = default;

    ~Range() = default;

    // -------------------------------------------------------------------------
    //     Iterators
    // -------------------------------------------------------------------------

    iterator begin()
    {
        return begin_;
    }

    iterator end()
    {
        return end_;
    }

    /*
    const_iterator cbegin()
    {
        return RangeIterator<const value_type> (*this);
    }

    const_iterator cend()
    {
        return RangeIterator<const value_type> (*this, size());
    }
    */

    // -------------------------------------------------------------------------
    //     Element Access
    // -------------------------------------------------------------------------

    /*
    reference operator[] (size_type n)
    {
        return *(content_[n]);
    }

    const_reference operator[] (size_type n) const
    {
        return *(content_[n]);
    }

    reference at (size_type n)
    {
        return *(content_.at(n));
    }

    const_reference at (size_type n) const
    {
        return *(content_.at(n));
    }

    reference front()
    {
        return *(content_.front());
    }

    const_reference front() const
    {
        return *(content_.front());
    }

    reference back()
    {
        return *(content_.back());
    }

    const_reference back() const
    {
        return *(content_.back());
    }
    */

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    iterator begin_;
    iterator end_;

};

} // namespace utils
} // namespace genesis

#endif // include guard

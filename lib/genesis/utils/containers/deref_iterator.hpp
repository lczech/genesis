#ifndef GENESIS_UTILS_CONTAINERS_DEREF_ITERATOR_H_
#define GENESIS_UTILS_CONTAINERS_DEREF_ITERATOR_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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

#include <cstddef>

namespace genesis {
namespace utils {

// =================================================================================================
//     Dereference Iterator
// =================================================================================================

/**
 * @brief Iterator class that exposes elements in a container of pointers.
 *
 * This class can be used as an iterator into a container of pointers, in order to avoid double
 * dereferencing of the elements:
 *
 *     std::vector<std::unique_ptr<T>> foo;
 *     for( auto& it : foo ){
 *         auto const baz = (*it)->bar;
 *     }
 *
 * This is cumbersome. By using this class, the double dereferencing is not needed.
 * See Tree class for an exemplary usage of this technique.
 *
 * Inspired by https://jonasdevlieghere.com/containers-of-unique-pointers/
 */
template <class BaseIterator>
class DereferenceIterator : public BaseIterator
{
public:

    using value_type      = typename BaseIterator::value_type::element_type;
    using difference_type = typename BaseIterator::difference_type;

    using pointer    = value_type *;
    using reference  = value_type &;

    DereferenceIterator( BaseIterator const& other )
        : BaseIterator( other )
    {}

    reference operator*() const
    {
        return *(this->BaseIterator::operator*());
    }

    pointer operator->() const
    {
        return this->BaseIterator::operator*().get();
    }

    reference operator[]( difference_type n ) const
    {
        return *( this->BaseIterator::operator[]( n ));
    }
};

} // namespace utils
} // namespace genesis

#endif // include guard

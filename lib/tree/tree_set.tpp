/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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
 * @brief Implementation of TreeSet class.
 *
 * @file
 * @ingroup tree
 */

namespace genesis {
namespace tree {

// =================================================================================================
//     Constructors and Rule of Five
// =================================================================================================

template <class TreeType>
void TreeSet<TreeType>::swap( TreeSet<TreeType>& other )
{
    using std::swap;
    swap( trees_, other.trees_ );
}

// =================================================================================================
//     Modifiers
// =================================================================================================

/**
* @brief Add a Tree with a name to the TreeSet.
*
* The Tree is copied.
*/
template <class TreeType>
void TreeSet<TreeType>::add( std::string const& name, TreeType const& tree )
{
    trees_.push_back( { name, tree } );
}

/**
 * @brief Remove the Tree at a certain index position.
 *
 * As this function moves Tree%s in the container around, all iterators and pointers to
 * the elements of this TreeSet are considered to be invalidated.
 */
template <class TreeType>
void TreeSet<TreeType>::remove_at( size_t index )
{
    trees_.erase( trees_.begin() + index );
}

/**
 * @brief Clear the TreeSet and destroy all contained Trees.
 */
template <class TreeType>
void TreeSet<TreeType>::clear ()
{
    trees_.clear();
}

// =================================================================================================
//     Accessors
// =================================================================================================

template <class TreeType>
typename TreeSet<TreeType>::iterator TreeSet<TreeType>::begin()
{
    return trees_.begin();
}

template <class TreeType>
typename TreeSet<TreeType>::const_iterator TreeSet<TreeType>::begin() const
{
    return trees_.cbegin();
}

template <class TreeType>
typename TreeSet<TreeType>::iterator TreeSet<TreeType>::end()
{
    return trees_.end();
}

template <class TreeType>
typename TreeSet<TreeType>::const_iterator TreeSet<TreeType>::end() const
{
    return trees_.cend();
}

template <class TreeType>
typename TreeSet<TreeType>::NamedTree& TreeSet<TreeType>::at ( size_t index )
{
    return trees_.at(index);
}

template <class TreeType>
typename TreeSet<TreeType>::NamedTree const& TreeSet<TreeType>::at ( size_t index ) const
{
    return trees_.at(index);
}

template <class TreeType>
typename TreeSet<TreeType>::NamedTree& TreeSet<TreeType>::operator [] (const std::size_t index)
{
    return trees_[index];
}

template <class TreeType>
typename TreeSet<TreeType>::NamedTree const& TreeSet<TreeType>::operator [] (const std::size_t index) const
{
    return trees_[index];
}

/**
 * @brief Return whether the TreeSet is empty.
 */
template <class TreeType>
bool TreeSet<TreeType>::empty() const
{
    return trees_.empty();
}

/**
 * @brief Return the size of the TreeSet, i.e., the number of stored Tree%s.
 */
template <class TreeType>
size_t TreeSet<TreeType>::size() const
{
    return trees_.size();
}

} // namespace tree
} // namespace genesis

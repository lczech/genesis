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
 * @brief Implementation of TreeSet class.
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/tree_set.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Constructors and Rule of Five
// =================================================================================================

void TreeSet::swap( TreeSet& other )
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
void TreeSet::add( std::string const& name, Tree const& tree )
{
    trees_.push_back( { name, tree } );
}

void TreeSet::add( std::string&& name, Tree&& tree )
{
    trees_.push_back( { std::move( name ), std::move( tree ) } );
}

/**
 * @brief Remove the Tree at a certain index position.
 *
 * As this function moves Tree%s in the container around, all iterators and pointers to
 * the elements of this TreeSet are considered to be invalidated.
 */
void TreeSet::remove_at( size_t index )
{
    trees_.erase( trees_.begin() + index );
}

/**
 * @brief Clear the TreeSet and destroy all contained Trees.
 */
void TreeSet::clear ()
{
    trees_.clear();
}

// =================================================================================================
//     Accessors
// =================================================================================================

TreeSet::iterator TreeSet::begin()
{
    return trees_.begin();
}

TreeSet::const_iterator TreeSet::begin() const
{
    return trees_.cbegin();
}

TreeSet::iterator TreeSet::end()
{
    return trees_.end();
}

TreeSet::const_iterator TreeSet::end() const
{
    return trees_.cend();
}

TreeSet::NamedTree& TreeSet::at ( size_t index )
{
    return trees_.at(index);
}

TreeSet::NamedTree const& TreeSet::at ( size_t index ) const
{
    return trees_.at(index);
}

TreeSet::NamedTree& TreeSet::operator [] (const std::size_t index)
{
    return trees_[index];
}

TreeSet::NamedTree const& TreeSet::operator [] (const std::size_t index) const
{
    return trees_[index];
}

/**
 * @brief Return whether the TreeSet is empty.
 */
bool TreeSet::empty() const
{
    return trees_.empty();
}

/**
 * @brief Return the size of the TreeSet, i.e., the number of stored Tree%s.
 */
size_t TreeSet::size() const
{
    return trees_.size();
}

} // namespace tree
} // namespace genesis

#ifndef GENESIS_TREE_TREE_SET_H_
#define GENESIS_TREE_TREE_SET_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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
 * @ingroup tree
 */

#include "genesis/tree/tree.hpp"

#include <cassert>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Tree Set
// =================================================================================================

class TreeSet
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs
    // -------------------------------------------------------------------------

    using iterator       = typename std::vector<Tree>::iterator;
    using const_iterator = typename std::vector<Tree>::const_iterator;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    TreeSet()  = default;
    ~TreeSet() = default;

    TreeSet( TreeSet const& ) = default;
    TreeSet( TreeSet&& )      = default;

    TreeSet& operator= ( TreeSet const& ) = default;
    TreeSet& operator= ( TreeSet&& )      = default;

    void swap( TreeSet& other )
    {
        using std::swap;
        swap( names_, other.names_ );
        swap( trees_, other.trees_ );
    }

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    /**
     * @brief Add a Tree with a name to the TreeSet.
     *
     * The Tree is copied.
     */
    void add( Tree const& tree, std::string const& name = "" )
    {
        names_.push_back( name );
        trees_.push_back( tree );
    }

    /**
     * @brief Add a Tree with a name to the TreeSet.
     *
     * The Tree is moved.
     */
    void add( Tree&& tree, std::string const& name = "" )
    {
        names_.push_back( name );
        trees_.push_back( std::move( tree ));
    }

    /**
     * @brief Remove the Tree at a certain index position.
     *
     * As this function moves Tree%s in the container around, all iterators and pointers to
     * the elements of this TreeSet are considered to be invalidated.
     */
    void remove_at( size_t index )
    {
        if( index >= trees_.size() ) {
            throw std::invalid_argument(
                "Cannot remove element at index " + std::to_string( index ) + " from TreeSet with " +
                std::to_string( trees_.size() ) + " trees."
            );
        }
        assert( names_.size() == trees_.size() );

        names_.erase( names_.begin() + index );
        trees_.erase( trees_.begin() + index );
    }

    /**
     * @brief Clear the TreeSet and destroy all contained Trees.
     */
    void clear()
    {
        names_.clear();
        trees_.clear();
    }

    // -------------------------------------------------------------------------
    //     Name Accessors
    // -------------------------------------------------------------------------

    std::string const& name_at( size_t index ) const
    {
        if( index >= names_.size() ) {
            throw std::invalid_argument(
                "Cannot access element at index " + std::to_string( index ) + " from TreeSet with " +
                std::to_string( trees_.size() ) + " trees."
            );
        }
        return names_[index];
    }

    std::vector<std::string> const& names() const
    {
        return names_;
    }

    // -------------------------------------------------------------------------
    //     Tree Accessors
    // -------------------------------------------------------------------------

    iterator begin()
    {
        return trees_.begin();
    }

    const_iterator begin() const
    {
        return trees_.cbegin();
    }

    iterator end()
    {
        return trees_.end();
    }

    const_iterator end() const
    {
        return trees_.cend();
    }

    Tree& at( size_t index )
    {
        return trees_.at(index);
    }

    Tree const& at( size_t index ) const
    {
        return trees_.at(index);
    }

    Tree& operator [] (const std::size_t index)
    {
        return trees_[index];
    }

    Tree const& operator [] (const std::size_t index) const
    {
        return trees_[index];
    }

    std::vector<Tree> const& trees() const
    {
        return trees_;
    }

    /**
     * @brief Implicit conversion, so that a TreeSet can be used in functions
     * that expect a `std::vector` instead.
     */
    operator std::vector<Tree> const&() const
    {
        return trees_;
    }

    // -------------------------------------------------------------------------
    //     General Properties
    // -------------------------------------------------------------------------

    /**
     * @brief Return whether the TreeSet is empty.
     */
    bool empty() const
    {
        assert( names_.empty() == trees_.empty() );
        return trees_.empty();
    }

    /**
     * @brief Return the size of the TreeSet, i.e., the number of stored Tree%s.
     */
    size_t size() const
    {
        assert( names_.size() == trees_.size() );
        return trees_.size();
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::vector<std::string> names_;
    std::vector<Tree>        trees_;

};

} // namespace tree
} // namespace genesis

#endif // include guard

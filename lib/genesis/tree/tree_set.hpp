#ifndef GENESIS_TREE_TREE_SET_H_
#define GENESIS_TREE_TREE_SET_H_

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
 * @ingroup tree
 */

#include "genesis/tree/tree.hpp"

#include <ostream>
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

    /**
     * @brief Store a Tree together with a name for it.
     */
    struct NamedTree
    {
        std::string name;
        Tree        tree;
    };

    using iterator       = typename std::vector<NamedTree>::iterator;
    using const_iterator = typename std::vector<NamedTree>::const_iterator;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    TreeSet()  = default;
    ~TreeSet() = default;

    TreeSet( TreeSet const& ) = default;
    TreeSet( TreeSet&& )      = default;

    TreeSet& operator= ( TreeSet const& ) = default;
    TreeSet& operator= ( TreeSet&& )      = default;

    void swap( TreeSet& other );

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    void add( std::string const& name, Tree const& tree );
    void add( std::string&&      name, Tree&&      tree );

    void remove_at( size_t index );
    void clear();

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    iterator       begin();
    const_iterator begin() const;

    iterator       end();
    const_iterator end() const;

          NamedTree& at ( size_t index );
    const NamedTree& at ( size_t index ) const;

          NamedTree& operator [] (const std::size_t index);
    const NamedTree& operator [] (const std::size_t index) const;

    bool   empty() const;
    size_t size()  const;

    // -------------------------------------------------------------------------
    //     Output
    // -------------------------------------------------------------------------

    /**
     * @brief Write a list of all names of the Tree%s in a TreeSet to a stream.
     */
    // friend std::ostream& operator << ( std::ostream& out, TreeSet const& tset )
    // {
    //     // If provided with the optional parameter `full`, also dump all Trees.
    //     // TODO this was meant for full output. turn it into a printer instead!
    //     bool full = false;
    //
    //     for (auto& ct : tset) {
    //         out << ct.name << "\n";
    //         // TODO
    //         if (full) {
    //             out << ct.tree.dump() << "\n";
    //         }
    //     }
    //     return out;
    // }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // We use a vector of elements here, because we want to preserve the order in which
    // elements are inserted into the TreeMap. This is not the case with simple maps.
    std::vector<NamedTree> trees_;

};

} // namespace tree
} // namespace genesis

#endif // include guard

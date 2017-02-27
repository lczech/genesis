#ifndef GENESIS_TREE_TREE_LINK_H_
#define GENESIS_TREE_TREE_LINK_H_

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

#include <string>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward declarations
// =================================================================================================

class Tree;
class TreeEdge;
class TreeNode;

// =================================================================================================
//     TreeLink
// =================================================================================================

class TreeLink
{
public:

    // ---------------------------------------------------------------------
    //     Typedefs and Enums
    // ---------------------------------------------------------------------

    friend class Tree;

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    TreeLink()
        : index_( 0       )
        , next_(  nullptr )
        , outer_( nullptr )
        , node_(  nullptr )
        , edge_(  nullptr )
    {}

    TreeLink( size_t index, TreeLink* next, TreeLink* outer, TreeNode* node, TreeEdge* edge )
        : index_( index )
        , next_(  next  )
        , outer_( outer )
        , node_(  node  )
        , edge_(  edge  )
    {}

    ~TreeLink() = default;

    // avoid copy constructor and assignment operator.
    // creating copies is maintained by Tree only.

    TreeLink( TreeLink const& ) = delete;
    TreeLink( TreeLink&& )      = delete;

    TreeLink& operator= ( TreeLink const& ) = delete;
    TreeLink& operator= ( TreeLink&& )      = delete;

    // ---------------------------------------------------------------------
    //     Accessors
    // ---------------------------------------------------------------------

    size_t index() const;

    TreeLink      & next();
    TreeLink const& next() const;

    TreeLink      & prev();
    TreeLink const& prev() const;

    TreeLink      & outer();
    TreeLink const& outer() const;

    TreeEdge      & edge();
    TreeEdge const& edge() const;

    TreeNode      & node();
    TreeNode const& node() const;

    // ---------------------------------------------------------------------
    //     Modifiers
    // ---------------------------------------------------------------------

    TreeLink& reset_index( size_t val );

    TreeLink& reset_next(  TreeLink* val );
    TreeLink& reset_outer( TreeLink* val );

    TreeLink& reset_node(  TreeNode* val );
    TreeLink& reset_edge(  TreeEdge* val );

    // ---------------------------------------------------------------------
    //     Member Functions
    // ---------------------------------------------------------------------

    bool is_leaf() const;
    bool is_inner() const;

    std::string dump() const;

    // ---------------------------------------------------------------------
    //     Member Variables
    // ---------------------------------------------------------------------

private:

    size_t index_;

    TreeLink* next_;
    TreeLink* outer_;

    TreeNode* node_;
    TreeEdge* edge_;
};

} // namespace tree
} // namespace genesis

#endif // include guard

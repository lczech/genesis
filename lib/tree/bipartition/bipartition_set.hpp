#ifndef GENESIS_TREE_BIPARTITION_BIPARTITION_SET_H_
#define GENESIS_TREE_BIPARTITION_BIPARTITION_SET_H_

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
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <string>
#include <unordered_set>
#include <vector>

#include "tree/bipartition/bipartition.hpp"

namespace genesis {
namespace tree {

template <typename Tree>
class BipartitionSet
{
public:

    // -------------------------------------------------------------
    //     Declarations and Constructor
    // -------------------------------------------------------------

    typedef Bipartition <Tree> BipartitionType;

    typedef Tree                    TreeType;
    typedef typename Tree::NodeType NodeType;
    typedef typename Tree::LinkType LinkType;
    typedef typename Tree::EdgeType EdgeType;

    BipartitionSet (TreeType& tree) : tree_(tree) {};

    // -------------------------------------------------------------
    //     Member Functions
    // -------------------------------------------------------------

    void make();
    void make_index();

    BipartitionType*           find_smallest_subtree (std::vector<NodeType*> nodes);
    std::unordered_set<size_t> get_subtree_edges     (LinkType*              subtree);

    bool        validate();
    std::string dump();

    // -------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------

protected:

    TreeType&              tree_;

    std::vector<int>             node_to_leaf_map_;
    std::vector<size_t>          leaf_to_node_map_;

    std::vector<BipartitionType> bipartitions_;

};

} // namespace tree
} // namespace genesis

// =============================================================================
//     Inclusion of the implementation
// =============================================================================

// This is a class template, so do the inclusion here.
#include "tree/bipartition/bipartition_set.tpp"

#endif // include guard

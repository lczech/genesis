#ifndef GENESIS_TREE_BIPARTITION_BIPARTITION_SET_H_
#define GENESIS_TREE_BIPARTITION_BIPARTITION_SET_H_

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
#include <unordered_set>
#include <vector>

#include "genesis/tree/bipartition/bipartition.hpp"

namespace genesis {
namespace tree {

class BipartitionSet
{
public:

    // -------------------------------------------------------------
    //     Declarations and Constructor
    // -------------------------------------------------------------

    BipartitionSet (Tree& tree) : tree_(tree) {};

    // -------------------------------------------------------------
    //     Member Functions
    // -------------------------------------------------------------

    void make();
    void make_index();

    Bipartition* find_smallest_subtree ( std::vector<TreeNode*> nodes );
    std::unordered_set<size_t> get_subtree_edges( TreeLink* subtree );

    bool        validate();
    std::string dump();

    // -------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------

protected:

    Tree&              tree_;

    std::vector<int>             node_to_leaf_map_;
    std::vector<size_t>          leaf_to_node_map_;

    std::vector<Bipartition> bipartitions_;

};

} // namespace tree
} // namespace genesis

#endif // include guard

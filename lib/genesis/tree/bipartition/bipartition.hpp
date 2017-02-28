#ifndef GENESIS_TREE_BIPARTITION_BIPARTITION_H_
#define GENESIS_TREE_BIPARTITION_BIPARTITION_H_

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
#include "genesis/utils/math/bitvector.hpp"

namespace genesis {
namespace tree {

// =============================================================================
//     Forward Declarations
// =============================================================================

class BipartitionSet;

// =============================================================================
//     Bipartition
// =============================================================================

class Bipartition
{
public:

    // -------------------------------------------------------------
    //     Declarations and Constructor
    // -------------------------------------------------------------

    friend  BipartitionSet;

    Bipartition (size_t num_leaves)
        : leaf_nodes_( utils::Bitvector(num_leaves) )
        , link_(nullptr)
    {}

    // -------------------------------------------------------------
    //     Member Functions
    // -------------------------------------------------------------

    TreeLink* link()
    {
        return link_;
    }

    void invert()
    {
        leaf_nodes_.invert();
        link_ = &link_->outer();
    }

    // -------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------

protected:

    utils::Bitvector leaf_nodes_;
    TreeLink*        link_;

};

} // namespace tree
} // namespace genesis

#endif // include guard

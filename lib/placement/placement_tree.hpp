#ifndef GENESIS_PLACEMENT_PLACEMENT_TREE_H_
#define GENESIS_PLACEMENT_PLACEMENT_TREE_H_

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
 * @ingroup placement
 */

#include <string>
#include <vector>

#include "tree/default/tree.hpp"
#include "tree/tree.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class PqueryPlacement;

// =================================================================================================
//     Placement Tree Node Data
// =================================================================================================

class PlacementTreeNodeData : public tree::DefaultTreeNodeData
{
public:

};

// =================================================================================================
//     Placement Tree Edge Data
// =================================================================================================

class PlacementTreeEdgeData : public tree::DefaultTreeEdgeData
{
public:

    // -----------------------------------------------------
    //     Class Functions
    // -----------------------------------------------------

    inline bool operator == (const PlacementTreeEdgeData &other) const
    {
        // TODO add a comparison of pqueries as well ?! is that good?!
        return other.branch_length == branch_length && other.edge_num_ == edge_num_;
    }

    inline bool operator != (const PlacementTreeEdgeData &other) const
    {
        return !(other == *this);
    }

    // -----------------------------------------------------
    //     Member Functions
    // -----------------------------------------------------

    inline std::string dump() const
    {
        return "Length: " + std::to_string(branch_length) + "\tEdge Num: " + std::to_string(edge_num_);
    }

    int edge_num() const;
    void reset_edge_num( int val );

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

private:

    int edge_num_;
};

// =================================================================================================
//     Definitions and Typedefs
// =================================================================================================

// let's avoid tedious names!
typedef tree::TreeEdge <PlacementTreeNodeData, PlacementTreeEdgeData> PlacementTreeEdge;
typedef tree::TreeLink <PlacementTreeNodeData, PlacementTreeEdgeData> PlacementTreeLink;
typedef tree::TreeNode <PlacementTreeNodeData, PlacementTreeEdgeData> PlacementTreeNode;

typedef tree::Tree     <PlacementTreeNodeData, PlacementTreeEdgeData> PlacementTree;

} // namespace placement
} // namespace genesis

#endif // include guard

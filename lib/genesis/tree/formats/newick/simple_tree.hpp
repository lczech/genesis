#ifndef GENESIS_TREE_FORMATS_NEWICK_SIMPLE_TREE_H_
#define GENESIS_TREE_FORMATS_NEWICK_SIMPLE_TREE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2022 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/tree.hpp"
#include "genesis/tree/common_tree/tree.hpp"
#include "genesis/tree/function/operators.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Typedefs
// =================================================================================================

/**
 * @brief Alias for a Tree that stores TreeNode%s and TreeEdge%s with the standard Newick
 * elements on them.
 *
 * This tree contains node and edge data which are derived from the CommonTreeNodeData
 * and CommonTreeEdgeData, respectively, but additionally contains support for all other elements
 * that can occur in a Newick file format tree:
 *
 *  - The Rich/Rice Newick format extension allows to annotate bootstrap values and probabilities
 *    per branch, by adding additional `:[bootstrap]:[prob]` fields after the branch length.
 *  - The Newick format allows for comments in square brackets `[]`,
 *    which are also often (mis-)used for ad-hoc and more established extensions such as the
 *    New Hampshire eXtended (NHX) format `[&&NHX:key=value:...]`.
 *  - The Jplace file format for phylogenetic placements also uses a custom Newick extension,
 *    by introducing curly brackets to annotate edge numbers in the tree `{1}`.
 *
 * With additional data are stored in the tree nodes (comments) and edges (branch values and
 * jplace tags), respectively.
 *
 * Hence, this tree type is also the most direct representation of our internal NewickBrokerElement
 * data, which we use for parsing Newick files in the NewickReader.
 */
using SimpleNewickTree = Tree;

/**
 * @brief Alias for a TreeEdge of an @link SimpleNewickTree SimpleNewickTree@endlink.
 * See there for more information.
 */
using SimpleNewickTreeEdge = TreeEdge;

/**
 * @brief Alias for a TreeLink of an @link SimpleNewickTree SimpleNewickTree@endlink.
 * See there for more information.
 */
using SimpleNewickTreeLink = TreeLink;

/**
 * @brief Alias for a TreeNode of an @link SimpleNewickTree SimpleNewickTree@endlink.
 * See there for more information.
 */
using SimpleNewickTreeNode = TreeNode;

// =================================================================================================
//     Simple Newick Tree Node Data
// =================================================================================================

/**
 * @brief Data class for @link SimpleNewickTreeNode SimpleNewickTreeNodes@endlink.
 *
 * See @link SimpleNewickTree SimpleNewickTree@endlink for more information.
 */
class SimpleNewickNodeData : public CommonNodeData
{
public:

    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

    virtual ~SimpleNewickNodeData() override = default;

    // Move ctor and assignment.
    SimpleNewickNodeData( SimpleNewickNodeData&& )             = delete;
    SimpleNewickNodeData& operator= ( SimpleNewickNodeData&& ) = delete;

protected:

    SimpleNewickNodeData() = default;

    // Copy ctor and assignment.
    SimpleNewickNodeData( SimpleNewickNodeData const& )             = default;
    SimpleNewickNodeData& operator= ( SimpleNewickNodeData const& ) = default;

public:

    static std::unique_ptr< SimpleNewickNodeData > create()
    {
        return std::unique_ptr< SimpleNewickNodeData >( new SimpleNewickNodeData() );
    }

    virtual std::unique_ptr< BaseNodeData > recreate() const override
    {
        return std::unique_ptr< SimpleNewickNodeData >( new SimpleNewickNodeData() );
    }

    virtual std::unique_ptr< BaseNodeData > clone() const override
    {
        return std::unique_ptr< SimpleNewickNodeData >( new SimpleNewickNodeData( *this ));
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    /**
     * @brief List of comments such as NHX elements.
     */
    std::vector<std::string> comments;

};

// =================================================================================================
//     SimpleNewick Tree Edge Data
// =================================================================================================

/**
 * @brief Data class for @link SimpleNewickTreeEdge SimpleNewickTreeEdges@endlink.
 *
 * See @link SimpleNewickTree SimpleNewickTree@endlink for more information.
 */
class SimpleNewickEdgeData : public CommonEdgeData
{
public:

    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

    virtual ~SimpleNewickEdgeData() override = default;

    // Move ctor and assignment.
    SimpleNewickEdgeData( SimpleNewickEdgeData&& )             = delete;
    SimpleNewickEdgeData& operator= ( SimpleNewickEdgeData&& ) = delete;

protected:

    SimpleNewickEdgeData() = default;

    // Copy ctor and assignment.
    SimpleNewickEdgeData( SimpleNewickEdgeData const& )             = default;
    SimpleNewickEdgeData& operator= ( SimpleNewickEdgeData const& ) = default;

public:

    static std::unique_ptr< SimpleNewickEdgeData > create()
    {
        return std::unique_ptr< SimpleNewickEdgeData >( new SimpleNewickEdgeData() );
    }

    virtual std::unique_ptr< BaseEdgeData > recreate() const override
    {
        return std::unique_ptr< SimpleNewickEdgeData >( new SimpleNewickEdgeData() );
    }

    virtual std::unique_ptr< BaseEdgeData > clone() const override
    {
        return std::unique_ptr< SimpleNewickEdgeData >( new SimpleNewickEdgeData( *this ));
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    /**
     * @brief Numerical values associated with the node, excluding branch lengths.
     *
     * Edge values are part of the origina Newick format, and hence already stored in the
     * CommonEdgeData.
     */
    std::vector<std::string> values;

    /**
     * @brief Arbitrary strings that can be attached to a node via curly brackets "{}".
     *
     * This is a custom extension of the Newick format used in the Jplace file format for
     * phylogenetic placements.
     */
    std::vector<std::string> tags;

};

// =================================================================================================
//     Common Converter
// =================================================================================================

/**
 * @brief Helper function that takes a CommonTree (or any Tree with Node and Edge data derived
 * from it) and turns its data into an SimpleNewickTree, that is, a Tree with
 * @link SimpleNewickNodeData SimpleNewickNodeData@endlink and
 * @link SimpleNewickEdgeData SimpleNewickEdgeData@endlink.
 */
inline SimpleNewickTree convert_common_tree_to_simple_newick_tree( CommonTree const& source )
{
    return convert(
        source,
        [] ( tree::BaseNodeData const& node_data ) {
            auto attr_node = tree::SimpleNewickNodeData::create();
            auto const& orig_node = dynamic_cast< CommonNodeData const& >( node_data );
            attr_node->name = orig_node.name;

            return attr_node;
        },
        [] ( tree::BaseEdgeData const& edge_data ) {
            auto attr_edge = tree::SimpleNewickEdgeData::create();
            auto const& orig_edge = dynamic_cast< CommonEdgeData const& >( edge_data );
            attr_edge->branch_length = orig_edge.branch_length;

            return attr_edge;
        }
    );
}

} // namespace tree
} // namespace genesis

#endif // include guard

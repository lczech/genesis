#ifndef GENESIS_TREE_DRAWING_RECTANGULAR_LAYOUT_H_
#define GENESIS_TREE_DRAWING_RECTANGULAR_LAYOUT_H_

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

#include "tree/default/distances.hpp"
#include "tree/function/distances.hpp"
#include "tree/iterator/eulertour.hpp"
#include "tree/iterator/postorder.hpp"

#include <assert.h>
#include <string>
#include <stdexcept>
#include <vector>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace utils {

    class SvgDocument;

} // namespace utils

namespace tree {

// =================================================================================================
//     Rectangular Layout
// =================================================================================================

class RectangularLayout
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    struct Node {
        double x = -1.0;
        double y = -1.0;
        int    parent = -1;

        double children_min_y = -1.0;
        double children_max_y = -1.0;

        std::string name;
    };

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    RectangularLayout()  = default;
    ~RectangularLayout() = default;

    RectangularLayout( RectangularLayout const& ) = default;
    RectangularLayout( RectangularLayout&& )      = default;

    RectangularLayout& operator= ( RectangularLayout const& ) = default;
    RectangularLayout& operator= ( RectangularLayout&& )      = default;

    // -------------------------------------------------------------
    //     Drawing
    // -------------------------------------------------------------

    template< class TreeType >
    void from_tree( TreeType const& tree )
    {
        if( tree.empty() ) {
            throw std::runtime_error( "Cannot draw an empty tree." );
        }

        nodes_ = std::vector< Node >( tree.node_count() );

        // Set node x-coords according to branch lengths (distance from root).
        auto node_dists = node_branch_length_distance_vector( tree );
        set_node_x_phylogram_( node_dists );
        // auto node_dists = node_path_length_vector( tree );
        // set_node_x_cladogram_( node_dists );

        // Set node parents and y-coord of leaves.
        size_t leaf_count = 0;
        size_t parent = 0;
        for( auto it : eulertour( tree )) {
            auto& node = nodes_[ it.node().index() ];

            if( node.parent == -1 ) {
                node.parent = parent;
                // edge_to_layout[it.edge()] = &node;
            }
            if( it.node().is_leaf() ) {
                node.y = scaler_y_ * leaf_count++;
            }

            node.name = it.node().data.name;
            parent = it.node().index();
        }

        // Set remaining y-coords of inner nodes to mid-points of their children.
        for( auto it : postorder( tree )) {
            auto& node = nodes_[ it.node().index() ];
            auto& parent_node = nodes_[ node.parent ];

            if( node.y < 0.0 ) {
                node.y = node.children_min_y + ( node.children_max_y - node.children_min_y ) / 2;
            }

            if( parent_node.children_min_y < 0.0 || parent_node.children_min_y > node.y ) {
                parent_node.children_min_y = node.y;
            }
            if( parent_node.children_max_y < 0.0 || parent_node.children_max_y < node.y ) {
                parent_node.children_max_y = node.y;
            }
        }
    }

    utils::SvgDocument to_svg_document() const;

    // -------------------------------------------------------------
    //     Internal Functions
    // -------------------------------------------------------------

private:

    void set_node_x_phylogram_( std::vector<double> node_dists );
    void set_node_x_cladogram_( std::vector<int>    node_dists );

    // -------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------

private:

    std::vector< Node > nodes_;

    double scaler_x_ = 100.0;
    double scaler_y_ = 10.0;

};

} // namespace tree
} // namespace genesis

#endif // include guard

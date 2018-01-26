/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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

#include "genesis/tree/drawing/circular_layout.hpp"

#include "genesis/tree/function/operators.hpp"

#include "genesis/utils/math/common.hpp"

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <stdexcept>

namespace genesis {
namespace tree {

// =================================================================================================
//     Settings
// =================================================================================================

CircularLayout& CircularLayout::radius( double const value )
{
    radius_ = value;
    return *this;
}

double CircularLayout::radius() const
{
    return radius_;
}

// =================================================================================================
//     Virtual Functions
// =================================================================================================

utils::SvgDocument CircularLayout::to_svg_document_() const
{
    using namespace utils;
    SvgDocument doc;
    SvgGroup    tree_lines;
    SvgGroup    taxa_names;
    SvgGroup    edge_shapes;
    SvgGroup    node_shapes;

    // If the radius was not set, use automatic:
    // minimum of 50, and grow with tree size.
    auto radius = radius_;
    if( radius <= 0.0 ) {
        radius = std::max( 50.0, static_cast<double>( tree().node_count() ));
    }

    size_t max_text_len = 0;

    for( auto const& node_it : tree().nodes() ) {
        auto const& node = *node_it;

        auto const& node_data = node.data<LayoutNodeData>();
        auto const& prnt_data = tree().node_at( node_data.parent_index ).data<LayoutNodeData>();

        auto const node_spreading = 2.0 * utils::PI * node_data.spreading;
        auto const prnt_spreading = 2.0 * utils::PI * prnt_data.spreading;

        auto const node_x = node_data.distance * radius * cos( node_spreading );
        auto const node_y = node_data.distance * radius * sin( node_spreading );

        // Get the edge between the node and its parent.
        auto edge_ptr = edge_between( node, tree().node_at( node_data.parent_index ) );

        // If there is an edge (i.e., we are not at the root), draw lines between the nodes.
        if( edge_ptr ) {
            auto const& edge_data = edge_ptr->data<LayoutEdgeData>();

            // Get line strokes
            auto spreading_stroke = edge_data.spreading_stroke;
            auto distance_stroke = edge_data.distance_stroke;
            spreading_stroke.line_cap = utils::SvgStroke::LineCap::kRound;
            distance_stroke.line_cap = utils::SvgStroke::LineCap::kRound;

            // Calculate circular spreading
            auto start_a = prnt_spreading;
            auto end_a   = node_spreading;
            if( prnt_spreading > node_spreading ) {
                std::swap( start_a, end_a );
            }

            // Calculate linear distance
            auto const dist_start_x = prnt_data.distance * radius * cos( node_spreading );
            auto const dist_start_y = prnt_data.distance * radius * sin( node_spreading );

            // Draw lines
            tree_lines << SvgPath(
                { svg_arc( 0, 0, prnt_data.distance * radius, start_a, end_a ) },
                spreading_stroke,
                SvgFill( SvgFill::Type::kNone )
            );
            tree_lines << SvgLine(
                dist_start_x, dist_start_y,
                node_x,  node_y,
                distance_stroke
            );

            // If there is an edge shape, draw it to the middle of the edge
            if( ! edge_data.shape.empty() ) {
                auto const shape_x = ( dist_start_x + node_x ) / 2.0;
                auto const shape_y = ( dist_start_y + node_y ) / 2.0;

                auto es = edge_data.shape;
                es.transform.append( SvgTransform::Translate( shape_x, shape_y ));
                edge_shapes << std::move( es );
            }

        } else {

            // If there is no edge, it must be the root.
            assert( node.is_root() );
        }

        // If the node has a name, print it.
        if( node_data.name != "" ) {
            // auto label = SvgText( node_data.name, SvgPoint( node_data.x + 5, node_data.y ) );
            // label.dy = "0.4em";

            auto label = text_template();
            label.text = node_data.name;
            label.alignment_baseline = SvgText::AlignmentBaseline::kMiddle;

            // Move label to tip node.
            label.transform.append( SvgTransform::Translate(
                ( node_data.distance * radius + 10 ) * cos( node_spreading ),
                ( node_data.distance * radius + 10 ) * sin( node_spreading )
            ));

            // Rotate label so that its orientation is correct.
            // Caveat: here, we use the spreading [0, 1] value directly.
            if( node_data.spreading > 0.25 && node_data.spreading <= 0.75 ) {
                // Left hemisphere.
                label.anchor = SvgText::Anchor::kEnd;
                label.transform.append( SvgTransform::Rotate( 360 * node_data.spreading + 180 ));
            } else {
                // Right hemisphere.
                label.transform.append( SvgTransform::Rotate( 360 * node_data.spreading ));
            }

            taxa_names << std::move( label );
            max_text_len = std::max( max_text_len, node_data.name.size() );
        }

        // If there is a node shape, draw it.
        if( ! node_data.shape.empty() ) {
            auto ns = node_data.shape;
            ns.transform.append( SvgTransform::Translate( node_x, node_y ));
            node_shapes << std::move( ns );
        }
    }

    // Make sure that the drawing is done from outside to inside,
    // so that the overlapping parts look nice.
    tree_lines.reverse();

    // Set the margins according to longest label.
    auto const marg = std::max( 30.0, max_text_len * text_template().font.size );
    doc.margin = SvgMargin( marg );

    // We are sure that we won't use the groups again, so let's move them!
    doc << std::move( tree_lines );
    if( ! taxa_names.empty() ) {
        doc << std::move( taxa_names );
    }
    if( ! edge_shapes.empty() ) {
        doc << std::move( edge_shapes );
    }
    if( ! node_shapes.empty() ) {
        doc << std::move( node_shapes );
    }
    return doc;
}

} // namespace tree
} // namespace genesis

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
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/tree/common_tree/newick_reader.hpp"
#include "genesis/tree/drawing/heat_tree.hpp"
#include "genesis/tree/drawing/circular_layout.hpp"
#include "genesis/tree/drawing/rectangular_layout.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/tree.hpp"

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/formats/svg/svg.hpp"
#include "genesis/utils/tools/color.hpp"
#include "genesis/utils/tools/color/names.hpp"
#include "genesis/utils/tools/color/norm_linear.hpp"

using namespace genesis;
using namespace tree;

TEST(Tree, Drawing)
{
    // std::string input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";
    std::string input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L,(M,N)O)R;";

    Tree tree = CommonTreeNewickReader().read( utils::from_string( input ));
    // EXPECT_TRUE( CommonTreeNewickReader().from_file( "/home/lucas/best_tree.newick", tree ));

    // auto layout = RectangularLayout( tree, LayoutType::kPhylogram );
    auto layout = CircularLayout( tree, LayoutType::kPhylogram );
    // auto const spreading = LayoutSpreading::kLeafNodesOnly;
    // auto const spreading = LayoutSpreading::kAllNodes;
    auto const spreading = LayoutSpreading::kAllNodesButRoot;

    std::vector<std::string> scheme = {
        "Crimson",
        "DarkCyan",
        "DarkGoldenRod",
        "DarkGreen",
        "DarkOrchid",
        "DeepPink",
        "DodgerBlue",
        "DimGray",
        "GreenYellow",
        "Indigo",
        "MediumVioletRed",
        "MidnightBlue",
        "Olive",
        "Orange",
        "OrangeRed",
        "Peru",
        "Purple",
        "SeaGreen",
        "DeepSkyBlue",
        "RoyalBlue",
        "SlateBlue",
        "Tomato",
        "YellowGreen"
    };

    // Set colourful edges.
    std::vector<utils::SvgStroke> strokes;
    for( size_t i = 0; i < tree.edge_count(); ++i ) {
        auto const& cv = scheme[ i % scheme.size() ];
        strokes.push_back( utils::SvgStroke() );
        strokes.back().color = utils::color_from_name_web( cv );

        // auto& node = tree.edge_at(i).secondary_link().node();
        // if( is_leaf(node) ) {
        //     node.data<CommonNodeData>().name = cv;
        // }
    }
    layout.set_edge_strokes( strokes );

    // Set label alignment.
    layout.align_labels( true );
    layout.extra_spacer( 50.0 );
    auto spacer_stroke = utils::SvgStroke( utils::Color( 0.8, 0.8, 0.8 ));
    spacer_stroke.dash_array = std::vector<double>({ 2.0, 0.5 });
    spacer_stroke.dash_offset = 2.0;
    layout.set_label_spacer_strokes( spacer_stroke, spreading );

    // Set colourful node shapes.
    std::vector<utils::SvgGroup> node_shapes;
    node_shapes.resize( tree.node_count() );
    for( size_t i = 0; i < tree.node_count(); ++i ) {
        node_shapes[i].add( utils::SvgCircle(
            utils::SvgPoint( 0, 0 ),
            10,
            utils::SvgStroke(),
            utils::SvgFill( utils::Color() )
        ));
        // node_shapes[i].add( utils::SvgImage(
        //     "http://files.gamebanana.com/img/ico/sprays/516c32f08e03d.png",
        //     utils::SvgPoint( -60, -60 ),
        //     utils::SvgSize( 120, 120 )
        // ));
    }
    // layout.set_node_shapes( node_shapes );

    // Set node spreading
    layout.inner_node_spreading( spreading );

    // Do the drawing.
    std::ostringstream out;
    layout.to_svg_document().write( out );

    // LOG_DBG << out.str();
    // utils::file_write( out.str(), "/home/lucas/tree.svg" );

}

TEST( Tree, DrawingHeatTree )
{
    using namespace genesis::utils;

    HeatTreeParameters params;

    // std::string input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";
    std::string input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L,(M,N)O)R;";
    params.tree = CommonTreeNewickReader().read( utils::from_string( input ));

    // auto const row_count = params.tree.node_count();
    // auto const row_count = params.tree.edge_count();
    auto const row_count = leaf_node_count( params.tree );

    // Set colourful edges.
    std::vector<utils::SvgStroke> strokes;
    for( size_t i = 0; i < params.tree.edge_count(); ++i ) {
        auto const ve = static_cast<double>( i ) / static_cast<double>( params.tree.edge_count() );
        params.color_per_branch.push_back( Color( ve, 0.0, 0.0 ));
    }

    // Make a matrix with the red-blue color plane.
    params.matrix = Matrix<Color>( row_count, 28 );
    for( size_t r = 0; r < params.matrix.rows(); ++r ) {
        for( size_t c = 0; c < params.matrix.cols(); ++c ) {
            auto const vr = static_cast<double>( r ) / static_cast<double>( params.matrix.rows() );
            auto const vc = static_cast<double>( c ) / static_cast<double>( params.matrix.cols() );
            params.matrix( r, c ) = Color( 0.0, vr, vc );
        }
    }

    // Labels.
    for( size_t c = 0; c < params.matrix.cols(); ++c ) {
        params.column_labels.push_back( std::to_string( c ) );
    }

    // Matrix color scale.
    auto matrix_color_map = ColorMap({ Color( 0.0, 1.0, 0.0 ), Color( 0.0, 0.0, 1.0 ) });
    auto matrix_color_norm = ColorNormalizationLinear( 0.0, params.matrix.cols() );

    // Tree color scale.
    auto tree_color_map = ColorMap({ Color( 0.0, 0.0, 0.0 ), Color( 1.0, 0.0, 0.0 ) });
    auto tree_color_norm = ColorNormalizationLinear( 0.0, row_count );

    auto const doc = heat_tree(
        params, matrix_color_map, matrix_color_norm, tree_color_map, tree_color_norm
    );

    std::ostringstream out;
    doc.write( out );
    // LOG_DBG << out.str();
    // utils::file_write( out.str(), "/home/lucas/tree.svg" );
}

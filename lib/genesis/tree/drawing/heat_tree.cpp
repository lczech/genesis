/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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

#include "genesis/tree/drawing/heat_tree.hpp"

#include "genesis/tree/drawing/layout_base.hpp"
#include "genesis/tree/drawing/layout_tree.hpp"
#include "genesis/tree/drawing/rectangular_layout.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/iterator/eulertour.hpp"

#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/formats/bmp/writer.hpp"
#include "genesis/utils/formats/svg/matrix.hpp"
#include "genesis/utils/tools/color/norm_linear.hpp"

#include <stdexcept>

namespace genesis {
namespace tree {

// =================================================================================================
//     Row Order Helpers
// =================================================================================================

static std::vector<size_t> heat_tree_row_order_(
    Tree const&     tree,
    LayoutSpreading spreading
) {
    // Error check. For now, we only support heattrees with inner node rows for bifurcating trees.
    if( spreading != LayoutSpreading::kLeafNodesOnly && ! is_bifurcating( tree )) {
        throw std::invalid_argument(
            "Tree is not bifurcating. Cannot draw heat tree with inner node rows."
        );
    }

    std::vector<size_t> tmp;
    auto visits = std::vector<size_t>( tree.node_count(), 0 );
    for( auto it : eulertour( tree )) {
        auto const node_index = it.node().index();

        // Count the how many-th visit this is.
        // As we have a bifurcating tree, it can never surpass 3 visits.
        ++visits[ node_index ];
        assert(( spreading == LayoutSpreading::kLeafNodesOnly ) || ( visits[ node_index ] <= 3 ));

        switch( spreading ) {
            case LayoutSpreading::kLeafNodesOnly: {
                if( is_leaf( it.node() )) {
                    tmp.push_back( node_index );
                }
                break;
            }
            case LayoutSpreading::kAllNodesButRoot:
            case LayoutSpreading::kAllNodes: {
                if( spreading == LayoutSpreading::kAllNodesButRoot && is_root( it.node() )) {
                    continue;
                } else if( is_leaf( it.node() ) || visits[ node_index ] == 2 ) {
                    tmp.push_back( node_index );
                }
                break;
            }
            default: {
                assert( false );
            }
        }
    }

    auto const sorting = utils::sort_indices( tmp.begin(), tmp.end() );
    auto result = std::vector<size_t>( sorting.size() );
    for( size_t i = 0; i < sorting.size(); ++i ) {
        assert( sorting[i] < result.size() );
        result[ sorting[i] ] = i;
    }

    // For each spreading, assert the correct result size.
    assert(
        ( spreading != LayoutSpreading::kLeafNodesOnly ) ||
        ( result.size() == leaf_node_count( tree ))
    );
    assert(
        ( spreading != LayoutSpreading::kAllNodesButRoot ) ||
        ( result.size() == tree.node_count() - 1 )
    );
    assert(
        ( spreading != LayoutSpreading::kAllNodes ) ||
        ( result.size() == tree.node_count() )
    );

    return result;
}

template<class T>
utils::Matrix<T> heat_tree_reorder_rows_(
    utils::Matrix<T> const& mat,
    std::vector<size_t> const& order
) {
    if( order.size() != mat.rows() ) {
        throw std::invalid_argument( "Wrong order size for reordering matrix rows." );
    }

    auto result = utils::Matrix<T>( mat.rows(), mat.cols() );
    for( size_t r = 0; r < order.size(); ++r ) {
        assert( order[r] < mat.rows() );
        result.row(r) = mat.row( order[r] ).to_vector();
    }
    return result;
}

// =================================================================================================
//     Internal Functions
// =================================================================================================

/**
 * @brief Local helper struct to communicate positions in the svg doc between functions.
 */
struct HeatTreeGrid
{
    /**
     * @brief Top y-coordinate for the gradients/palettes.
     */
    double pal_top;

    /**
     * @brief Height of the gradients/palettes.
     */
    double pal_height;

    /**
     * @brief Left x-coordinate of the matrix.
     */
    double matrix_left;

    /**
     * @brief Width of the matrix.
     */
    double matrix_width;
};

RectangularLayout heat_tree_tree_layout_(
    HeatTreeParameters const& params
) {
    using namespace genesis::utils;

    // Prepare layout for the tree.
    auto layout = RectangularLayout( params.tree, params.type, params.ladderize );
    layout.height( std::max( 100.0, 6.0 * static_cast<double>( params.tree.node_count() )));
    layout.width( layout.height() / 2.0 );
    layout.align_labels( true );
    layout.text_template().anchor = SvgText::Anchor::kEnd;

    // Use matrix rows to determine which inner node spreading we need.
    if( params.matrix.rows() == params.tree.node_count() ) {
        layout.inner_node_spreading( LayoutSpreading::kAllNodes );
    } else if( params.matrix.rows() == params.tree.node_count() -1 ) {
        layout.inner_node_spreading( LayoutSpreading::kAllNodesButRoot );
    } else if( params.matrix.rows() == leaf_node_count( params.tree )) {
        layout.inner_node_spreading( LayoutSpreading::kLeafNodesOnly );
    } else {
        throw std::runtime_error(
            "Matrix has wrong size for making a heat tree. Has to be either tree.node_count(), "
            "tree.node_count() - 1, or leaf_edge_count( tree )."
        );
    }

    // Set a nice spacer stroke for the needed nodes.
    auto spacer_stroke = SvgStroke( Color( 0.8, 0.8, 0.8 ));
    spacer_stroke.dash_array = std::vector<double>({ 2.0, 0.5 });
    spacer_stroke.dash_offset = 2.0;
    layout.set_label_spacer_strokes( spacer_stroke, layout.inner_node_spreading() );

    // Set edge colors.
    if( ! params.color_per_branch.empty() ) {
        std::vector<SvgStroke> strokes;
        for( auto const& color : params.color_per_branch ) {
            auto stroke = params.stroke;
            stroke.color = color;
            stroke.line_cap = SvgStroke::LineCap::kRound;
            strokes.push_back( std::move( stroke ));
        }
        layout.set_edge_strokes( strokes );
    }

    return layout;
}

void heat_tree_add_tree_color_scale_(
    RectangularLayout const&         layout,
    utils::ColorMap const&           tree_color_map,
    utils::ColorNormalization const& tree_color_norm,
    utils::SvgDocument&              svg_doc,
    HeatTreeGrid&                    grid
) {
    using namespace genesis::utils;

    // Add color scale for the tree.
    double const svg_pal_top = 1.2 * svg_doc.bounding_box().height();
    double const svg_pal_height = svg_doc.bounding_box().width() / 10.0;
    if( ! tree_color_map.empty() ) {
        // Make the scale.
        auto svg_pal_settings = SvgColorBarSettings();
        svg_pal_settings.direction = SvgColorBarSettings::Direction::kLeftToRight;
        svg_pal_settings.width = layout.width(); // svg_doc.bounding_box().width();
        svg_pal_settings.height = svg_pal_height;
        // svg_pal_settings.text_size = svg_pal_height / 3.0;
        auto svg_scale = make_svg_color_bar( svg_pal_settings, tree_color_map, tree_color_norm );

        // Move it to below the tree.
        svg_scale.second.transform.append( SvgTransform::Translate(
            0.0, svg_pal_top
        ));
        svg_doc.margin.bottom = 0.2 * svg_doc.bounding_box().height() + 2 * svg_pal_settings.height + 200;

        // Add it to the doc.
        if( ! svg_scale.first.empty() ) {
            svg_doc.defs.push_back( svg_scale.first );
        }
        svg_doc << std::move( svg_scale.second );
    }

    // The height of the tree (plus some space) is what we use  as the top coordinate for the
    // gradients. We store this, so that the matrix gradient can use this as well.
    grid.pal_top = svg_pal_top;
    grid.pal_height = svg_pal_height;
}

void heat_tree_add_heat_matrix_svg_(
    HeatTreeParameters const&          params,
    RectangularLayout const&           layout,
    utils::Matrix<utils::Color> const& matrix,
    utils::SvgDocument&                svg_doc,
    HeatTreeGrid&                      grid
) {
    using namespace genesis::utils;

    // Make the heat matrix, with row heights so that it fits the total tree height.
    SvgMatrixSettings svg_mat_set;
    svg_mat_set.pixel_height = layout.height() / static_cast<double>( matrix.rows() - 1 );
    svg_mat_set.pixel_width = svg_mat_set.pixel_height;
    auto svg_matrix = make_svg_matrix(
        matrix, svg_mat_set, std::vector<std::string>{}, params.column_labels
    );
    svg_matrix.transform.append( SvgTransform::Translate(
        svg_doc.bounding_box().width() + 20.0, - svg_mat_set.pixel_width / 2.0
    ));
    auto const svg_matrix_left = svg_doc.bounding_box().width() + 20.0;
    auto const svg_matrix_width = svg_matrix.bounding_box().width();
    svg_doc << std::move( svg_matrix );
    svg_doc.margin.right += svg_matrix_width + 200;

    // Lastly, set the needed grid params, so that downstream functions can use them.
    grid.matrix_left = svg_matrix_left;
    grid.matrix_width = svg_matrix_width;
}

void heat_tree_add_heat_matrix_bmp_(
    HeatTreeParameters const&          params,
    RectangularLayout const&           layout,
    utils::Matrix<utils::Color> const& matrix,
    utils::SvgDocument&                svg_doc,
    HeatTreeGrid&                      grid
) {
    using namespace genesis::utils;

    // TODO
    (void) params.column_labels;

    // Make a bmp version of the matrix.
    std::string matrix_bmp;
    BmpWriter().write( matrix, to_string( matrix_bmp ));

    // Get position and scale for the matrix.
    auto const pixel_height = layout.height() / static_cast<double>( matrix.rows() - 1 );
    auto const pixel_width = pixel_height;
    auto const offset_x = svg_doc.bounding_box().width() + 20.0;
    auto const offset_y = - pixel_width / 2.0;

    // Add to svg.
    auto img = SvgImage(
        svg_data_uri( "image/bmp", matrix_bmp, true ),
        SvgPoint{ offset_x, offset_y },
        SvgSize{
            static_cast<double>( matrix.cols() ) * pixel_width,
            static_cast<double>( matrix.rows() ) * pixel_height
        }
    );
    img.rendering = SvgImage::ImageRendering::kPixelated;
    svg_doc << std::move( img );
    svg_doc.margin.right += static_cast<double>( matrix.cols() ) * pixel_width + 200;

    // Lastly, set the needed grid params, so that downstream functions can use them.
    grid.matrix_left = offset_x;
    grid.matrix_width = static_cast<double>( matrix.cols() ) * pixel_width;
}

void heat_tree_add_matrix_color_scale_(
    utils::ColorMap const&           matrix_color_map,
    utils::ColorNormalization const& matrix_color_norm,
    utils::SvgDocument&              svg_doc,
    HeatTreeGrid&                    grid
) {
    using namespace genesis::utils;

    // Add color scale for the matrix.
    if( ! matrix_color_map.empty() ) {
        // Make the scale.
        auto svg_pal_settings = SvgColorBarSettings();
        svg_pal_settings.direction = SvgColorBarSettings::Direction::kLeftToRight;
        svg_pal_settings.width = grid.matrix_width;
        svg_pal_settings.height = grid.pal_height;
        // svg_pal_settings.text_size = svg_pal_height / 3.0;
        auto svg_scale = make_svg_color_bar( svg_pal_settings, matrix_color_map, matrix_color_norm );

        // Move it to below the tree.
        svg_scale.second.transform.append( SvgTransform::Translate(
            grid.matrix_left, grid.pal_top
        ));
        svg_doc.margin.bottom = 0.2 * svg_doc.bounding_box().height() + 2 * svg_pal_settings.height + 200;

        // Add it to the doc.
        if( ! svg_scale.first.empty() ) {
            svg_doc.defs.push_back( svg_scale.first );
        }
        svg_doc << std::move( svg_scale.second );
    }
}

// =================================================================================================
//     SVG Functions
// =================================================================================================

utils::SvgDocument heat_tree(
    HeatTreeParameters const&        params
) {
    // We use a dummy linear norm here, to satisfy the compiler (because the standard norm has
    // purely virtual functiosn and can thus not be instanciated). As the color map however is
    // empty, the called function will not use the norm.
    return heat_tree(
        params, {}, utils::ColorNormalizationLinear(), {}, utils::ColorNormalizationLinear()
    );
}

utils::SvgDocument heat_tree(
    HeatTreeParameters const&        params,
    utils::ColorMap const&           matrix_color_map,
    utils::ColorNormalization const& matrix_color_norm
) {
    // We use a dummy linear norm here, to satisfy the compiler (because the standard norm has
    // purely virtual functiosn and can thus not be instanciated). As the color map however is
    // empty, the called function will not use the norm.
    return heat_tree(
        params, matrix_color_map, matrix_color_norm, {}, utils::ColorNormalizationLinear()
    );
}

utils::SvgDocument heat_tree(
    HeatTreeParameters const&        params,
    utils::ColorMap const&           matrix_color_map,
    utils::ColorNormalization const& matrix_color_norm,
    utils::ColorMap const&           tree_color_map,
    utils::ColorNormalization const& tree_color_norm
) {
    using namespace genesis::utils;
    HeatTreeGrid grid;

    // Get the tree layout, including colored branches.
    auto layout = heat_tree_tree_layout_( params );

    // Prepare svg doc where all elements are added to.
    // This already includes the tree drawing.
    auto svg_doc = layout.to_svg_document();
    svg_doc.margin.left = svg_doc.margin.top = svg_doc.margin.bottom = svg_doc.margin.right = 200;

    // Add color scale for the tree.
    heat_tree_add_tree_color_scale_(
        layout, tree_color_map, tree_color_norm, svg_doc, grid
    );

    // Sort matrix rows to fit with tree node order.
    auto const row_order = heat_tree_row_order_( params.tree, layout.inner_node_spreading() );
    auto const matrix = heat_tree_reorder_rows_( params.matrix, row_order );

    // Make the heat matrix, with row heights so that it fits the total tree height.
    if( params.matrix_as_bmp ) {
        heat_tree_add_heat_matrix_bmp_( params, layout, matrix, svg_doc, grid );
    } else {
        heat_tree_add_heat_matrix_svg_( params, layout, matrix, svg_doc, grid );
    }

    // Add color scale for the matrix.
    heat_tree_add_matrix_color_scale_(
        matrix_color_map, matrix_color_norm, svg_doc, grid
    );

    return svg_doc;
}

} // namespace tree
} // namespace genesis

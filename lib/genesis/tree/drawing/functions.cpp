/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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

#include "genesis/tree/drawing/functions.hpp"

#include "genesis/tree/common_tree/functions.hpp"
#include "genesis/tree/common_tree/newick_writer.hpp"
#include "genesis/tree/formats/newick/writer.hpp"
#include "genesis/tree/formats/newick/color_writer_plugin.hpp"
#include "genesis/tree/formats/phyloxml/writer.hpp"
#include "genesis/tree/formats/phyloxml/color_writer_plugin.hpp"

#include "genesis/tree/common_tree/newick_writer.hpp"
#include "genesis/tree/common_tree/phyloxml_writer.hpp"
#include "genesis/tree/drawing/circular_layout.hpp"
#include "genesis/tree/drawing/layout_tree.hpp"
#include "genesis/tree/drawing/rectangular_layout.hpp"

#include "genesis/utils/formats/nexus/block.hpp"
#include "genesis/utils/formats/nexus/document.hpp"
#include "genesis/utils/formats/nexus/taxa.hpp"
#include "genesis/utils/formats/nexus/trees.hpp"
#include "genesis/utils/formats/nexus/writer.hpp"

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/output_stream.hpp"
#include "genesis/utils/color/norm_linear.hpp"

#include <fstream>
#include <memory>
#include <stdexcept>

namespace genesis {
namespace tree {

// =================================================================================================
//     Newick Functions
// =================================================================================================

void write_tree_to_newick_file(
    CommonTree const&  tree,
    std::string const& newick_filename
) {
    CommonTreeNewickWriter().write( tree, utils::to_file( newick_filename ));
}

// =================================================================================================
//     Phyloxml Functions
// =================================================================================================

void write_tree_to_phyloxml_file(
    CommonTree const& tree,
    std::string const& phyloxml_filename
) {
    write_color_tree_to_phyloxml_file(
        tree, std::vector<utils::Color>{}, phyloxml_filename
    );
}

void write_color_tree_to_phyloxml_file(
    CommonTree const&                tree,
    std::vector<utils::Color> const& color_per_branch,
    std::string const&               phyloxml_filename
) {
    // We use a normal Phyloxml writer...
    auto writer = CommonTreePhyloxmlWriter();

    // ... but also wrap it in a Color Mixin in order to allow for color branches if needed.
    auto color_plugin = tree::PhyloxmlColorWriterPlugin();
    if( ! color_per_branch.empty() ) {
        color_plugin.register_with( writer );
        color_plugin.edge_colors( color_per_branch );
    }

    writer.write( tree, utils::to_file( phyloxml_filename ));
}

void write_color_tree_to_phyloxml_file(
    CommonTree const&                tree,
    std::vector<double> const&       value_per_branch,
    utils::ColorMap const&           color_map,
    utils::ColorNormalization const& color_norm,
    std::string const&               phyloxml_filename
) {
    write_color_tree_to_phyloxml_file(
        tree, color_map( color_norm, value_per_branch ), phyloxml_filename
    );
}

// =================================================================================================
//     Nexus Functions
// =================================================================================================

void write_tree_to_nexus_file(
    CommonTree const&  tree,
    std::string const& nexus_filename
) {
    write_color_tree_to_nexus_file(
        tree, std::vector<utils::Color>{}, nexus_filename
    );
}

void write_color_tree_to_nexus_file(
    CommonTree const&                tree,
    std::vector<utils::Color> const& color_per_branch,
    std::string const&               nexus_filename
) {
    // We use a normal Newick writer...
    auto newick_writer = CommonTreeNewickWriter();
    newick_writer.trailing_new_line( false );

    // ... but also wrap it in a Color Mixin in order to allow for color branches if needed.
    auto color_plugin = tree::NewickColorWriterPlugin();
    if( ! color_per_branch.empty() ) {
        color_plugin.register_with( newick_writer );
        color_plugin.edge_colors( color_per_branch );
    }

    // Create an (empty) Nexus document.
    auto nexus_doc = utils::NexusDocument();

    // Add the taxa of the tree to the document.
    auto taxa = utils::make_unique<utils::NexusTaxa>();
    taxa->add_taxa( node_names( tree ));
    nexus_doc.set_block( std::move( taxa ));

    // Add the tree itself to the document.
    auto trees = utils::make_unique<utils::NexusTrees>();
    trees->add_tree( "tree1", newick_writer.to_string(tree) );
    nexus_doc.set_block( std::move(trees) );

    // Write the document to a Nexus file.
    auto nexus_writer = utils::NexusWriter();
    nexus_writer.write( nexus_doc, utils::to_file( nexus_filename ));
}

void write_color_tree_to_nexus_file(
    CommonTree const&                tree,
    std::vector<double> const&       value_per_branch,
    utils::ColorMap const&           color_map,
    utils::ColorNormalization const& color_norm,
    std::string const&               nexus_filename
) {
    write_color_tree_to_nexus_file(
        tree, color_map( color_norm, value_per_branch ), nexus_filename
    );
}

// =================================================================================================
//     SVG Functions
// =================================================================================================

utils::SvgDocument get_color_tree_svg_doc_(
    CommonTree const&                tree,
    LayoutParameters const&          params,
    std::vector<utils::Color> const& color_per_branch
) {
    // Make a layout tree. We need a pointer to it in order to allow for the two different classes
    // (circular/rectancular) to be returned here. Make it a unique ptr for automatic cleanup.
    std::unique_ptr<LayoutBase> layout = [&]() -> std::unique_ptr<LayoutBase> {
        if( params.shape == LayoutShape::kCircular ) {
            return utils::make_unique<CircularLayout>( tree, params.type, params.ladderize );
        }
        if( params.shape == LayoutShape::kRectangular ) {
            return utils::make_unique<RectangularLayout>( tree, params.type, params.ladderize );
        }
        throw std::runtime_error( "Unknown Tree shape parameter." );
    }();

    // Set edge colors and strokes.
    if( ! color_per_branch.empty() ) {
        std::vector<utils::SvgStroke> strokes;
        for( auto const& color : color_per_branch ) {
            auto stroke = params.stroke;
            stroke.color = color;
            stroke.line_cap = utils::SvgStroke::LineCap::kRound;
            strokes.push_back( std::move( stroke ));
        }
        layout->set_edge_strokes( strokes );
    }

    // Prepare svg doc.
    auto svg_doc = layout->to_svg_document();
    svg_doc.margin.left = svg_doc.margin.top = svg_doc.margin.bottom = svg_doc.margin.right = 200;
    return svg_doc;
}

void write_tree_to_svg_file(
    CommonTree const&       tree,
    LayoutParameters const& params,
    std::string const&      svg_filename
) {
    // We use a dummy linear norm here, to satisfy the compiler (because the standard norm has
    // purely virtual functiosn and can thus not be instanciated). As the color map however is
    // empty, the called function will not use the norm.
    write_color_tree_to_svg_file(
        tree, params, std::vector<utils::Color>{},
        {}, utils::ColorNormalizationLinear(),
        svg_filename
    );
}

void write_color_tree_to_svg_file(
    CommonTree const&                tree,
    LayoutParameters const&          params,
    std::vector<utils::Color> const& color_per_branch,
    std::string const&               svg_filename
) {
    // We use a dummy linear norm here, to satisfy the compiler (because the standard norm has
    // purely virtual functiosn and can thus not be instanciated). As the color map however is
    // empty, the called function will not use the norm.
    write_color_tree_to_svg_file(
        tree, params, color_per_branch,
        {}, utils::ColorNormalizationLinear(),
        svg_filename
    );
}

void write_color_tree_to_svg_file(
    CommonTree const&                tree,
    LayoutParameters const&          params,
    std::vector<double> const&       value_per_branch,
    utils::ColorMap const&           color_map,
    utils::ColorNormalization const& color_norm,
    std::string const&               svg_filename
) {
    write_color_tree_to_svg_file(
        tree, params, color_map( color_norm, value_per_branch ), color_map, color_norm, svg_filename
    );
}

void write_color_tree_to_svg_file(
    CommonTree const&                tree,
    LayoutParameters const&          params,
    std::vector<utils::Color> const& color_per_branch,
    utils::ColorMap const&           color_map,
    utils::ColorNormalization const& color_norm,
    std::string const&               svg_filename
) {
    // Get the basic svg tree layout.
    auto svg_doc = get_color_tree_svg_doc_( tree, params, color_per_branch );

    // Add the color legend / scale.
    if( ! color_map.empty() ) {

        // Make the scale with nice sizes.
        auto svg_pal_settings = utils::SvgColorBarSettings();
        svg_pal_settings.height = svg_doc.bounding_box().height() / 2.0;
        svg_pal_settings.width = svg_pal_settings.height / 10.0;
        svg_pal_settings.text_size = svg_pal_settings.height / 30.0;
        auto svg_scale = make_svg_color_bar( svg_pal_settings, color_map, color_norm );

        // Move it to the bottom right corner.
        if( params.shape == LayoutShape::kCircular ) {
            svg_scale.second.transform.append( utils::SvgTransform::Translate(
                1.2 * svg_doc.bounding_box().width() / 2.0, 0.0
            ));
            svg_doc.margin.right = 0.2 * svg_doc.bounding_box().width() / 2.0 + 2 * svg_pal_settings.width + 200;
        }
        if( params.shape == LayoutShape::kRectangular ) {
            svg_scale.second.transform.append( utils::SvgTransform::Translate(
                1.2 * svg_doc.bounding_box().width(), svg_pal_settings.height
            ));
            svg_doc.margin.right = 0.2 * svg_doc.bounding_box().width() + 2 * svg_pal_settings.width + 200;
        }

        // Add it to the svg doc.
        if( ! svg_scale.first.empty() ) {
            svg_doc.defs.push_back( svg_scale.first );
        }
        svg_doc.add( svg_scale.second );
    }

    // Write the whole svg doc to file.
    std::ofstream ofs;
    utils::file_output_stream( svg_filename, ofs );
    svg_doc.write( ofs );
}

void write_color_tree_to_svg_file(
    CommonTree const&                tree,
    LayoutParameters const&          params,
    std::vector<utils::Color> const& color_per_branch,
    std::vector<utils::Color> const& color_list,
    std::vector<std::string> const&  color_labels,
    std::string const&               svg_filename
) {
    // Get the basic svg tree layout.
    auto svg_doc = get_color_tree_svg_doc_( tree, params, color_per_branch );

    // Add the color legend / scale.

    // Make the color list.
    auto svg_color_list = make_svg_color_list( color_list, color_labels );

    // Move it to the bottom right corner.
    if( params.shape == LayoutShape::kCircular ) {
        svg_color_list.transform.append( utils::SvgTransform::Translate(
            1.2 * svg_doc.bounding_box().width() / 2.0, 0.0
        ));
        // svg_doc.margin.right = 0.2 * svg_doc.bounding_box().width() / 2.0 + 2 * svg_pal_settings.width + 200;
    }
    if( params.shape == LayoutShape::kRectangular ) {
        svg_color_list.transform.append( utils::SvgTransform::Translate(
            1.2 * svg_doc.bounding_box().width(), svg_doc.bounding_box().height() / 2.0
        ));
        // svg_doc.margin.right = 0.2 * svg_doc.bounding_box().width() + 2 * svg_pal_settings.width + 200;
    }

    // Apply a scale factor that scales the box to be half the figure height.
    // The denominator is the number items in the list times their height (15px, used by make_svg_color_list)
    auto const sf = ( svg_doc.bounding_box().height() / 2.0 ) / (static_cast<double>( color_list.size() ) * 15.0 );
    svg_color_list.transform.append( utils::SvgTransform::Scale( sf ));

    // Add it to the svg doc.
    svg_doc.add( svg_color_list );

    // Write the whole svg doc to file.
    std::ofstream ofs;
    utils::file_output_stream( svg_filename, ofs );
    svg_doc.write( ofs );
}

} // namespace tree
} // namespace genesis

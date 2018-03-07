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

#include "genesis/tree/drawing/functions.hpp"

#include "genesis/tree/default/functions.hpp"
#include "genesis/tree/default/newick_writer.hpp"
#include "genesis/tree/formats/newick/writer.hpp"
#include "genesis/tree/formats/newick/color_writer_plugin.hpp"
#include "genesis/tree/formats/phyloxml/writer.hpp"
#include "genesis/tree/formats/phyloxml/color_writer_plugin.hpp"

#include "genesis/tree/default/newick_writer.hpp"
#include "genesis/tree/default/phyloxml_writer.hpp"
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

#include <fstream>
#include <memory>
#include <stdexcept>

namespace genesis {
namespace tree {

// =================================================================================================
//     Newick Functions
// =================================================================================================

void write_tree_to_newick_file(
    DefaultTree const& tree,
    std::string const& newick_filename
) {
    DefaultTreeNewickWriter().to_file( tree, newick_filename );
}

// =================================================================================================
//     Phyloxml Functions
// =================================================================================================

void write_tree_to_phyloxml_file(
    DefaultTree const& tree,
    std::string const& phyloxml_filename
) {
    write_color_tree_to_phyloxml_file(
        tree, {}, phyloxml_filename
    );
}

void write_color_tree_to_phyloxml_file(
    DefaultTree const&               tree,
    std::vector<utils::Color> const& color_per_branch,
    std::string const&               phyloxml_filename
) {
    // We use a normal Phyloxml writer...
    auto writer = DefaultTreePhyloxmlWriter();

    // ... but also wrap it in a Color Mixin in order to allow for color branches if needed.
    auto color_plugin = tree::PhyloxmlColorWriterPlugin();
    if( ! color_per_branch.empty() ) {
        color_plugin.register_with( writer );
        color_plugin.edge_colors( color_per_branch );
    }

    writer.to_file( tree, phyloxml_filename );
}

void write_color_tree_to_phyloxml_file(
    DefaultTree const&               tree,
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
    DefaultTree const& tree,
    std::string const& nexus_filename
) {
    write_color_tree_to_nexus_file(
        tree, {}, nexus_filename
    );
}

void write_color_tree_to_nexus_file(
    DefaultTree const&               tree,
    std::vector<utils::Color> const& color_per_branch,
    std::string const&               nexus_filename
) {
    // We use a normal Newick writer...
    auto newick_writer = DefaultTreeNewickWriter();

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
    nexus_writer.to_file( nexus_doc, nexus_filename );
}

void write_color_tree_to_nexus_file(
    DefaultTree const&               tree,
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

void write_tree_to_svg_file(
    DefaultTree const&      tree,
    LayoutParameters const& params,
    std::string const&      svg_filename
) {
    write_color_tree_to_svg_file(
        tree, params, std::vector<utils::Color>{}, {}, {}, svg_filename
    );
}

void write_color_tree_to_svg_file(
    DefaultTree const&               tree,
    LayoutParameters const&          params,
    std::vector<utils::Color> const& color_per_branch,
    std::string const&               svg_filename
) {
    write_color_tree_to_svg_file(
        tree, params, color_per_branch, {}, {}, svg_filename
    );
}

void write_color_tree_to_svg_file(
    DefaultTree const&               tree,
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
    DefaultTree const&               tree,
    LayoutParameters const&          params,
    std::vector<utils::Color> const& color_per_branch,
    utils::ColorMap const&           color_map,
    utils::ColorNormalization const& color_norm,
    std::string const&               svg_filename
) {

    // Make a layout tree.
    std::unique_ptr<LayoutBase> layout = [&]() -> std::unique_ptr<LayoutBase> {
        if( params.shape == LayoutShape::kCircular ) {
            return utils::make_unique<CircularLayout>( tree, params.type, params.ladderize );
        }
        if( params.shape == LayoutShape::kRectangular ) {
            return utils::make_unique<RectangularLayout>( tree, params.type, params.ladderize );
        }
        throw std::runtime_error( "Unknown Tree shape parameter." );
    }();

    // Set edge colors.
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

    // Add scale.
    if( ! color_map.empty() ) {
        // Make the scale.
        auto svg_pal = utils::SvgColorBarSettings();
        svg_pal.height = svg_doc.bounding_box().height() / 2.0;
        svg_pal.width = svg_pal.height / 10.0;
        auto svg_scale = make_svg_color_bar( svg_pal, color_map, color_norm );

        // Move it to the bottom right corner.
        if( params.shape == LayoutShape::kCircular ) {
            svg_scale.second.transform.append( utils::SvgTransform::Translate(
                svg_doc.bounding_box().width() / 2.0, 0.0
            ));
        }
        if( params.shape == LayoutShape::kRectangular ) {
            svg_scale.second.transform.append( utils::SvgTransform::Translate(
                svg_doc.bounding_box().width(), svg_pal.height / 2.0
            ));
        }

        // Add it to the doc.
        if( ! svg_scale.first.empty() ) {
            svg_doc.defs.push_back( svg_scale.first );
        }
        svg_doc.add( svg_scale.second );
    }

    // Write to file.
    svg_doc.margin.left = svg_doc.margin.top = svg_doc.margin.bottom = svg_doc.margin.right = 200;
    std::ofstream ofs;
    utils::file_output_stream( svg_filename, ofs );
    svg_doc.write( ofs );
}

} // namespace tree
} // namespace genesis

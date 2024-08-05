#ifndef GENESIS_TREE_DRAWING_FUNCTIONS_H_
#define GENESIS_TREE_DRAWING_FUNCTIONS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/common_tree/tree.hpp"
#include "genesis/tree/drawing/layout_base.hpp"
#include "genesis/utils/color/color.hpp"
#include "genesis/utils/color/map.hpp"
#include "genesis/utils/color/normalization.hpp"
#include "genesis/utils/formats/svg/svg.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Newick Functions
// =================================================================================================

/**
 * @brief Write a newick file containing a tree.
 *
 * This is a very simple wrapper for common cases.
 */
void write_tree_to_newick_file(
    CommonTree const&  tree,
    std::string const& newick_filename
);

// =================================================================================================
//     Phyloxml Functions
// =================================================================================================

/**
 * @brief Write a phyloxml file containing a tree.
 *
 * The file format can be read and visualized by, e.g., Archaeopteryx.
 */
void write_tree_to_phyloxml_file(
    CommonTree const&  tree,
    std::string const& phyloxml_filename
);

void write_color_tree_to_phyloxml_file(
    CommonTree const&                tree,
    std::vector<utils::Color> const& color_per_branch,
    std::string const&               phyloxml_filename
);

void write_color_tree_to_phyloxml_file(
    CommonTree const&                tree,
    std::vector<double> const&       value_per_branch,
    utils::ColorMap const&           color_map,
    utils::ColorNormalization const& color_norm,
    std::string const&               phyloxml_filename
);

// =================================================================================================
//     Nexus Functions
// =================================================================================================

/**
 * @brief Write a nexus file containing a tree.
 *
 * The file format can be read and visualized by, e.g., FigTree.
 */
void write_tree_to_nexus_file(
    CommonTree const&  tree,
    std::string const& nexus_filename
);

void write_color_tree_to_nexus_file(
    CommonTree const&                tree,
    std::vector<utils::Color> const& color_per_branch,
    std::string const&               nexus_filename
);

void write_color_tree_to_nexus_file(
    CommonTree const&                tree,
    std::vector<double> const&       value_per_branch,
    utils::ColorMap const&           color_map,
    utils::ColorNormalization const& color_norm,
    std::string const&               nexus_filename
);

// =================================================================================================
//     SVG Params
// =================================================================================================

/**
 * @brief Collection of params for simple tree to svg wrapper functions.
 *
 * See
 */
struct LayoutParameters
{
    LayoutShape      shape     = LayoutShape::kCircular;
    LayoutType       type      = LayoutType::kCladogram;
    bool             ladderize = true;
    utils::SvgStroke stroke;
};

// =================================================================================================
//     Tree to SVG Document
// =================================================================================================

utils::SvgDocument get_tree_svg_document(
    CommonTree const&       tree,
    LayoutParameters const& params
);

utils::SvgDocument get_color_tree_svg_document(
    CommonTree const&                tree,
    LayoutParameters const&          params,
    std::vector<utils::Color> const& color_per_branch
);

utils::SvgDocument get_color_tree_svg_document(
    CommonTree const&                tree,
    LayoutParameters const&          params,
    std::vector<double> const&       value_per_branch,
    utils::ColorMap const&           color_map,
    utils::ColorNormalization const& color_norm
);

utils::SvgDocument get_color_tree_svg_document(
    CommonTree const&                tree,
    LayoutParameters const&          params,
    std::vector<utils::Color> const& color_per_branch,
    utils::ColorMap const&           color_map,
    utils::ColorNormalization const& color_norm
);

utils::SvgDocument get_color_tree_svg_document(
    CommonTree const&                tree,
    LayoutParameters const&          params,
    std::vector<utils::Color> const& color_per_branch,
    utils::ColorMap const&           color_map,
    utils::ColorNormalization const& color_norm,
    std::vector<utils::SvgGroup> const& node_shapes,
    std::vector<utils::SvgGroup> const& edge_shapes
);

utils::SvgDocument get_color_tree_svg_document(
    CommonTree const&                tree,
    LayoutParameters const&          params,
    std::vector<utils::Color> const& color_per_branch,
    std::vector<utils::Color> const& color_list,
    std::vector<std::string> const&  color_labels
);

// =================================================================================================
//     Tree to SVG File
// =================================================================================================

void write_tree_to_svg_file(
    CommonTree const&       tree,
    LayoutParameters const& params,
    std::string const&      svg_filename
);

void write_color_tree_to_svg_file(
    CommonTree const&                tree,
    LayoutParameters const&          params,
    std::vector<utils::Color> const& color_per_branch,
    std::string const&               svg_filename
);

void write_color_tree_to_svg_file(
    CommonTree const&                tree,
    LayoutParameters const&          params,
    std::vector<double> const&       value_per_branch,
    utils::ColorMap const&           color_map,
    utils::ColorNormalization const& color_norm,
    std::string const&               svg_filename
);

void write_color_tree_to_svg_file(
    CommonTree const&                tree,
    LayoutParameters const&          params,
    std::vector<utils::Color> const& color_per_branch,
    utils::ColorMap const&           color_map,
    utils::ColorNormalization const& color_norm,
    std::string const&               svg_filename
);

void write_color_tree_to_svg_file(
    CommonTree const&                tree,
    LayoutParameters const&          params,
    std::vector<utils::Color> const& color_per_branch,
    utils::ColorMap const&           color_map,
    utils::ColorNormalization const& color_norm,
    std::vector<utils::SvgGroup> const& node_shapes,
    std::vector<utils::SvgGroup> const& edge_shapes,
    std::string const&               svg_filename
);

void write_color_tree_to_svg_file(
    CommonTree const&                tree,
    LayoutParameters const&          params,
    std::vector<utils::Color> const& color_per_branch,
    std::vector<utils::Color> const& color_list,
    std::vector<std::string> const&  color_labels,
    std::string const&               svg_filename
);

} // namespace tree
} // namespace genesis

#endif // include guard

#ifndef GENESIS_TREE_DRAWING_FUNCTIONS_H_
#define GENESIS_TREE_DRAWING_FUNCTIONS_H_

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

#include "genesis/tree/default/tree.hpp"
#include "genesis/tree/drawing/layout_base.hpp"
#include "genesis/utils/tools/color.hpp"
#include "genesis/utils/tools/color/map.hpp"
#include "genesis/utils/tools/color/normalization.hpp"
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
    DefaultTree const& tree,
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
    DefaultTree const& tree,
    std::string const& phyloxml_filename
);

void write_color_tree_to_phyloxml_file(
    DefaultTree const&               tree,
    std::vector<utils::Color> const& color_per_branch,
    std::string const&               phyloxml_filename
);

void write_color_tree_to_phyloxml_file(
    DefaultTree const&               tree,
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
    DefaultTree const& tree,
    std::string const& nexus_filename
);

void write_color_tree_to_nexus_file(
    DefaultTree const&               tree,
    std::vector<utils::Color> const& color_per_branch,
    std::string const&               nexus_filename
);

void write_color_tree_to_nexus_file(
    DefaultTree const&               tree,
    std::vector<double> const&       value_per_branch,
    utils::ColorMap const&           color_map,
    utils::ColorNormalization const& color_norm,
    std::string const&               nexus_filename
);

// =================================================================================================
//     SVG Functions
// =================================================================================================

struct LayoutParameters
{
    LayoutShape      shape     = LayoutShape::kCircular;
    LayoutType       type      = LayoutType::kCladogram;
    bool             ladderize = true;
    utils::SvgStroke stroke;
};

void write_tree_to_svg_file(
    DefaultTree const&      tree,
    LayoutParameters const& params,
    std::string const&      svg_filename
);

void write_color_tree_to_svg_file(
    DefaultTree const&               tree,
    LayoutParameters const&          params,
    std::vector<utils::Color> const& color_per_branch,
    std::string const&               svg_filename
);

void write_color_tree_to_svg_file(
    DefaultTree const&               tree,
    LayoutParameters const&          params,
    std::vector<double> const&       value_per_branch,
    utils::ColorMap const&           color_map,
    utils::ColorNormalization const& color_norm,
    std::string const&               svg_filename
);

void write_color_tree_to_svg_file(
    DefaultTree const&               tree,
    LayoutParameters const&          params,
    std::vector<utils::Color> const& color_per_branch,
    utils::ColorMap const&           color_map,
    utils::ColorNormalization const& color_norm,
    std::string const&               svg_filename
);

} // namespace tree
} // namespace genesis

#endif // include guard

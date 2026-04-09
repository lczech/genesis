#ifndef GENESIS_TREE_DRAWING_HEAT_TREE_H_
#define GENESIS_TREE_DRAWING_HEAT_TREE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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

#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/drawing/layout_base.hpp>
#include <genesis/util/container/matrix.hpp>
#include <genesis/util/format/svg/svg.hpp>
#include <genesis/util/color/color.hpp>
#include <genesis/util/color/map.hpp>
#include <genesis/util/color/normalization.hpp>

#include <string>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     SVG Functions
// =================================================================================================

struct HeatTreeParameters
{
    // Tree parameters
    CommonTree                  tree;
    LayoutType                  type      = LayoutType::kCladogram;
    bool                        ladderize = true;
    std::vector<genesis::util::color::Color>   color_per_branch;
    genesis::util::format::SvgStroke            stroke;

    // Matrix parameters
    genesis::util::container::Matrix<genesis::util::color::Color> matrix;
    std::vector<std::string>    column_labels;
    bool                        matrix_as_bmp = false;
};

genesis::util::format::SvgDocument heat_tree(
    HeatTreeParameters const&        params
);

genesis::util::format::SvgDocument heat_tree(
    HeatTreeParameters const&        params,
    genesis::util::color::ColorMap const&           matrix_color_map,
    genesis::util::color::ColorNormalization const& matrix_color_norm
);

genesis::util::format::SvgDocument heat_tree(
    HeatTreeParameters const&        params,
    genesis::util::color::ColorMap const&           matrix_color_map,
    genesis::util::color::ColorNormalization const& matrix_color_norm,
    genesis::util::color::ColorMap const&           tree_color_map,
    genesis::util::color::ColorNormalization const& tree_color_norm
);

} // namespace tree
} // namespace genesis

#endif // include guard

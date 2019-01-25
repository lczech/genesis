#ifndef GENESIS_TREE_DRAWING_HEAT_TREE_H_
#define GENESIS_TREE_DRAWING_HEAT_TREE_H_

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
 * @ingroup tree
 */

#include "genesis/tree/common_tree/tree.hpp"
#include "genesis/tree/drawing/layout_base.hpp"
#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/formats/svg/svg.hpp"
#include "genesis/utils/tools/color.hpp"
#include "genesis/utils/tools/color/map.hpp"
#include "genesis/utils/tools/color/normalization.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     SVG Functions
// =================================================================================================

struct HeatTreeParameters
{
    CommonTree                  tree;
    LayoutType                  type      = LayoutType::kCladogram;
    bool                        ladderize = true;
    std::vector<utils::Color>   color_per_branch;
    utils::SvgStroke            stroke;

    utils::Matrix<utils::Color> matrix;
    std::vector<std::string>    column_labels;
};

utils::SvgDocument heat_tree(
    HeatTreeParameters const&        params
);

utils::SvgDocument heat_tree(
    HeatTreeParameters const&        params,
    utils::ColorMap const&           matrix_color_map,
    utils::ColorNormalization const& matrix_color_norm
);

utils::SvgDocument heat_tree(
    HeatTreeParameters const&        params,
    utils::ColorMap const&           matrix_color_map,
    utils::ColorNormalization const& matrix_color_norm,
    utils::ColorMap const&           tree_color_map,
    utils::ColorNormalization const& tree_color_norm
);

} // namespace tree
} // namespace genesis

#endif // include guard

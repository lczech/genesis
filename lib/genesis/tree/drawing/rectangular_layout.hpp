#ifndef GENESIS_TREE_DRAWING_RECTANGULAR_LAYOUT_H_
#define GENESIS_TREE_DRAWING_RECTANGULAR_LAYOUT_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include "genesis/utils/formats/svg/svg.hpp"
#include "genesis/tree/drawing/rectangular_tree.hpp"

#include <string>
#include <vector>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace tree {

    class Tree;

// =================================================================================================
//     Rectangular Layout
// =================================================================================================

class RectangularLayout
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    RectangularLayout()  = default;
    RectangularLayout( Tree const& tree );

    ~RectangularLayout() = default;

    RectangularLayout( RectangularLayout const& ) = default;
    RectangularLayout( RectangularLayout&& )      = default;

    RectangularLayout& operator= ( RectangularLayout const& ) = default;
    RectangularLayout& operator= ( RectangularLayout&& )      = default;

    // -------------------------------------------------------------
    //     Member Access
    // -------------------------------------------------------------

    Tree& tree();

    // -------------------------------------------------------------
    //     Drawing
    // -------------------------------------------------------------

    void set_edge_strokes( std::vector< utils::SvgStroke > strokes );

    utils::SvgDocument to_svg_document() const;

    // -------------------------------------------------------------
    //     Internal Functions
    // -------------------------------------------------------------

private:

    void set_node_x_phylogram_();
    void set_node_x_cladogram_();

    // -------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------

private:

    // std::vector< Node > nodes_;

    RectangularTree tree_;

    double scaler_x_ = 100.0;
    double scaler_y_ = 10.0;

};

} // namespace tree
} // namespace genesis

#endif // include guard

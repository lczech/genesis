#ifndef GENESIS_TREE_DRAWING_CIRCULAR_LAYOUT_H_
#define GENESIS_TREE_DRAWING_CIRCULAR_LAYOUT_H_

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

#include "genesis/utils/formats/svg/svg.hpp"
#include "genesis/tree/drawing/layout_base.hpp"
#include "genesis/tree/drawing/circular_tree.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Circular Layout
// =================================================================================================

class CircularLayout : public LayoutBase
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    CircularLayout()  = default;
    CircularLayout( Tree const& orig_tree, Type const drawing_type = Type::kPhylogram )
    {
        type( drawing_type );
        tree(orig_tree);
    }

    virtual ~CircularLayout() = default;

    CircularLayout( CircularLayout const& ) = default;
    CircularLayout( CircularLayout&& )      = default;

    CircularLayout& operator= ( CircularLayout const& ) = default;
    CircularLayout& operator= ( CircularLayout&& )      = default;

    // -------------------------------------------------------------
    //     Options
    // -------------------------------------------------------------

    CircularLayout& radius_scaler( double const value );
    double radius_scaler() const;

    // -------------------------------------------------------------
    //     Virtual Functions
    // -------------------------------------------------------------

private:

    void init_node_( TreeNode& node, TreeNode const& orig_node ) override;
    void init_edge_( TreeEdge& edge, TreeEdge const& orig_edge ) override;
    void init_layout_() override;

    utils::SvgDocument to_svg_document_() const override;

    // -------------------------------------------------------------
    //     Internal Functions
    // -------------------------------------------------------------

private:

    void set_node_a_();
    void set_node_r_phylogram_();
    void set_node_r_cladogram_();

    // -------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------

private:

    double scaler_r_ = 10.0;

};

} // namespace tree
} // namespace genesis

#endif // include guard

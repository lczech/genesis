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
    CircularLayout(
        Tree const& orig_tree,
        LayoutType const drawing_type = LayoutType::kCladogram,
        bool ladderize = true
    )
        : LayoutBase( orig_tree, drawing_type, ladderize )
    {}

    virtual ~CircularLayout() = default;

    CircularLayout( CircularLayout const& ) = default;
    CircularLayout( CircularLayout&& )      = default;

    CircularLayout& operator= ( CircularLayout const& ) = default;
    CircularLayout& operator= ( CircularLayout&& )      = default;

    // -------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------

    CircularLayout& radius( double const value );
    double radius() const;

    // -------------------------------------------------------------
    //     Virtual Functions
    // -------------------------------------------------------------

private:

    utils::SvgDocument to_svg_document_() const override;

    // -------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------

private:

    double radius_ = 0.0;

};

} // namespace tree
} // namespace genesis

#endif // include guard

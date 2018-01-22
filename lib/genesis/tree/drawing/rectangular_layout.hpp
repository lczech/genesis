#ifndef GENESIS_TREE_DRAWING_RECTANGULAR_LAYOUT_H_
#define GENESIS_TREE_DRAWING_RECTANGULAR_LAYOUT_H_

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
//     Rectangular Layout
// =================================================================================================

class RectangularLayout : public LayoutBase
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    RectangularLayout()  = default;
    RectangularLayout(
        Tree const& orig_tree,
        LayoutType const drawing_type = LayoutType::kCladogram,
        bool ladderize = true
    )
        : LayoutBase( orig_tree, drawing_type, ladderize )
    {}

    virtual ~RectangularLayout() = default;

    RectangularLayout( RectangularLayout const& ) = default;
    RectangularLayout( RectangularLayout&& )      = default;

    RectangularLayout& operator= ( RectangularLayout const& ) = default;
    RectangularLayout& operator= ( RectangularLayout&& )      = default;

    // -------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------

    RectangularLayout& width( double const value );
    double width() const;

    RectangularLayout& height( double const value );
    double height() const;

    // -------------------------------------------------------------
    //     Virtual Functions
    // -------------------------------------------------------------

private:

    utils::SvgDocument to_svg_document_() const override;

    // -------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------

private:

    double width_ = 0.0;
    double height_ = 0.0;

};

} // namespace tree
} // namespace genesis

#endif // include guard

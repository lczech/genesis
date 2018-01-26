#ifndef GENESIS_TREE_DRAWING_LAYOUT_BASE_H_
#define GENESIS_TREE_DRAWING_LAYOUT_BASE_H_

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

#include "genesis/tree/drawing/layout_tree.hpp"
#include "genesis/utils/formats/svg/svg.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Layout Shape and Type
// =================================================================================================

enum class LayoutShape
{
    kCircular,
    kRectangular
};

enum class LayoutType
{
    kPhylogram,
    kCladogram
};

// =================================================================================================
//     Layout Base
// =================================================================================================

class LayoutBase
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    LayoutBase()  = default;

    LayoutBase(
        Tree const& orig_tree,
        LayoutType const drawing_type = LayoutType::kCladogram,
        bool ladderize = true
    ) {
        type( drawing_type );
        tree( orig_tree, ladderize );
    }

    virtual ~LayoutBase() = default;

    LayoutBase( LayoutBase const& ) = default;
    LayoutBase( LayoutBase&& )      = default;

    LayoutBase& operator= ( LayoutBase const& ) = default;
    LayoutBase& operator= ( LayoutBase&& )      = default;

    // -------------------------------------------------------------
    //     Tree
    // -------------------------------------------------------------

    void tree( Tree const& orig_tree, bool ladderize = true );
    Tree const& tree() const;

    void set_edge_strokes( utils::SvgStroke const& stroke );
    void set_edge_strokes( std::vector< utils::SvgStroke > const& strokes );

    void set_edge_spreading_strokes( utils::SvgStroke const& stroke );
    void set_edge_spreading_strokes( std::vector< utils::SvgStroke > const& strokes );

    void set_edge_distance_strokes( utils::SvgStroke const& stroke );
    void set_edge_distance_strokes( std::vector< utils::SvgStroke > const& strokes );

    void set_edge_shapes( utils::SvgGroup const& shape );
    void set_edge_shapes( std::vector< utils::SvgGroup> const& shapes );

    void set_node_shapes( utils::SvgGroup const& shape );
    void set_node_shapes( std::vector< utils::SvgGroup> const& shapes );

    // -------------------------------------------------------------
    //     Drawing
    // -------------------------------------------------------------

    utils::SvgDocument to_svg_document() const
    {
        return to_svg_document_();
    }

    // -------------------------------------------------------------
    //     Options
    // -------------------------------------------------------------

    void text_template( utils::SvgText const& tt );
    utils::SvgText& text_template();
    utils::SvgText const& text_template() const;

    void type( LayoutType const drawing_type );
    LayoutType type() const;

    // -------------------------------------------------------------
    //     Protected Functions
    // -------------------------------------------------------------

protected:

    Tree& tree();

    // -------------------------------------------------------------
    //     Virtual Functions
    // -------------------------------------------------------------

protected:

    virtual utils::SvgDocument to_svg_document_() const = 0;

    // -------------------------------------------------------------
    //     Private Members
    // -------------------------------------------------------------

private:

    void init_tree_( Tree const& orig_tree );
    void init_layout_();

    void set_node_spreadings_();
    void set_node_distances_phylogram_();
    void set_node_distances_cladogram_();

    // -------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------

private:

    LayoutTree tree_;

    LayoutType type_ = LayoutType::kCladogram;

    utils::SvgText text_template_ = utils::SvgText();

};

} // namespace tree
} // namespace genesis

#endif // include guard

#ifndef GENESIS_TREE_DRAWING_LAYOUT_BASE_H_
#define GENESIS_TREE_DRAWING_LAYOUT_BASE_H_

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

#include "genesis/tree/drawing/layout_tree.hpp"
#include "genesis/utils/formats/svg/svg.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Layout Shape and Type
// =================================================================================================

/**
 * @brief Shape of the tree for drawing, either circular or rectangular.
 */
enum class LayoutShape
{
    kCircular,
    kRectangular
};

/**
 * @brief Type of tree for drawing, either phylogram or cladogram.
 *
 * A phylogram uses and shows branch lengths, while a cladegram aligns all leaf nodes to each
 * other, and adjusts inner nodes accordingly.
 */
enum class LayoutType
{
    kPhylogram,
    kCladogram
};

/**
 * @brief Spreading of the nodes of a tree for drawing.
 *
 * In tree drawing, one axis is usually used for the branch lengths (or at least, for distancing
 * nodes from each other in a cladegram), while the other axis does not have a biological meaning.
 * It is instead used to spread out the nodes so that the tree is actually drawn in a plane instead
 * of just a line.
 *
 * Using this setting, the spreading can be controlled: Default is to spread out the leaves evenly,
 * giving the typical tree layout. Sometimes however it is necessary to also make space for inner
 * nodes. This is what the other options are for (with or without the root as a special case).
 */
enum class LayoutSpreading
{
    kLeafNodesOnly,
    kAllNodesButRoot,
    kAllNodes
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

    // -------------------------------------------------------------
    //     Edge Strokes
    // -------------------------------------------------------------

    void set_edge_strokes( utils::SvgStroke const& stroke );
    void set_edge_strokes( std::vector< utils::SvgStroke > const& strokes );

    void set_edge_spreading_strokes( utils::SvgStroke const& stroke );
    void set_edge_spreading_strokes( std::vector< utils::SvgStroke > const& strokes );

    void set_edge_distance_strokes( utils::SvgStroke const& stroke );
    void set_edge_distance_strokes( std::vector< utils::SvgStroke > const& strokes );

    void set_label_spacer_strokes(
        utils::SvgStroke const& stroke,
        LayoutSpreading spreading = LayoutSpreading::kLeafNodesOnly
    );
    void set_label_spacer_strokes( std::vector< utils::SvgStroke > const& strokes );

    // -------------------------------------------------------------
    //     Edge and Node Shapes
    // -------------------------------------------------------------

    void set_edge_shapes( utils::SvgGroup const& shape );
    void set_edge_shapes( std::vector<utils::SvgGroup> const& shapes );

    void set_node_shapes( utils::SvgGroup const& shape );
    void set_node_shapes( std::vector<utils::SvgGroup> const& shapes );

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

    void type( LayoutType const drawing_type );
    LayoutType type() const;

    void inner_node_spreading( LayoutSpreading value );
    LayoutSpreading inner_node_spreading() const;

    /**
     * @brief Align the taxon labels/node names/texts to each other.
     *
     * If set to `true`, the taxon names are aligned with each other. This is particularly interesting
     * in phylograms, where they otherwise are placed close to their nodes. In cladograms, the tip
     * nodes are aligned to each other anyway, so there is not much of a difference there.
     *
     * In order to still be able to see which label belongs to which node, a label spacer line is
     * drawn for each node, using LayoutNodeData::spacer_stroke. These strokes can be set with
     * set_label_spacer_strokes().
     *
     * Note also the extra_spacer() setting, which modifies how much longer the spacer line is
     * than the exact alignment. This setting is useful to create some extra space between the
     * tree and the aligned labels.
     */
    void align_labels( bool value );
    bool align_labels() const;

    /**
     * @brief Set some extra space betwen the tree nodes and their labels when align_labels()
     * is set to `true`.
     */
    void extra_spacer( double value );
    double extra_spacer() const;

    void text_template( utils::SvgText const& tt );
    utils::SvgText& text_template();
    utils::SvgText const& text_template() const;

    // -------------------------------------------------------------
    //     Protected Functions
    // -------------------------------------------------------------

protected:

    // Tree& tree();

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

    void set_node_spreadings_leaves_();
    void set_node_spreadings_all_( LayoutSpreading spreading );
    void set_node_distances_phylogram_();
    void set_node_distances_cladogram_();

    // -------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------

private:

    LayoutTree tree_;

    LayoutType type_ = LayoutType::kCladogram;
    LayoutSpreading inner_node_spreading_ = LayoutSpreading::kLeafNodesOnly;

    bool align_labels_ = false;
    double extra_spacer_ = 10.0;

    utils::SvgText text_template_ = utils::SvgText();

};

} // namespace tree
} // namespace genesis

#endif // include guard

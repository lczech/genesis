#ifndef GENESIS_PLACEMENT_IO_PHYLOXML_PROCESSOR_H_
#define GENESIS_PLACEMENT_IO_PHYLOXML_PROCESSOR_H_

/**
 * @brief Header of Placement Tree Phyloxml Processor class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/functions.hpp"
#include "placement/placement_tree.hpp"
#include "tree/default/phyloxml_mixin.hpp"
#include "tree/io/phyloxml_color_mixin.hpp"
#include "tree/io/phyloxml_processor.hpp"
#include "utils/tools/color.hpp"
#include "utils/tools/color/gradient.hpp"

namespace genesis {

// =================================================================================================
//     Placement Tree Phyloxml Processor
// =================================================================================================

/**
 * @brief
 */
class PlacementTreePhyloxmlProcessor : public
    PhyloxmlColorMixin <
        DefaultTreePhyloxmlMixin <
            PhyloxmlProcessor <
                PlacementTree
            >
        >
    >
{
private:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    typedef PhyloxmlColorMixin <
        DefaultTreePhyloxmlMixin <
            PhyloxmlProcessor <
                PlacementTree
            >
        >
    > Base;

    typedef typename Base::TreeType TreeType;
    typedef typename Base::NodeType NodeType;
    typedef typename Base::EdgeType EdgeType;
    typedef typename Base::LinkType LinkType;

    // -------------------------------------------------------------------------
    //     Overridden Virtual Functions
    // -------------------------------------------------------------------------

    virtual void prepare_writing( TreeType const& tree, XmlDocument& xml )
    {
        Base::prepare_writing(tree, xml);
        max_placements_per_edge_ = placement_count_max_edge(tree).second;
    }

    virtual void tree_edge_to_element( EdgeType const& edge, XmlElement& element )
    {
        Base::tree_edge_to_element(edge, element);

        Color edge_color (128,128,128);
        if (max_placements_per_edge_ > 0 && edge.data.placements.size() > 0) {
            edge_color = heat_gradient(
                log(edge.data.placements.size()) / log(max_placements_per_edge_)
            );
        }

        // LOG_DBG <<  edge.placements.size() << " --> " << 100 * edge.placements.size() / max_placements_per_edge_ << " = " << edge_color.dump();

        set_color(element, edge_color);
    }

    // -------------------------------------------------------------------------
    //     Member Data
    // -------------------------------------------------------------------------

private:

    size_t max_placements_per_edge_ = 0;

};

} // namespace genesis

#endif // include guard

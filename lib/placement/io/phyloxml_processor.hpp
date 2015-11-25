#ifndef GENESIS_PLACEMENT_IO_PHYLOXML_PROCESSOR_H_
#define GENESIS_PLACEMENT_IO_PHYLOXML_PROCESSOR_H_

/**
 * @brief Header of Placement Tree Phyloxml Processor class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/placement_tree.hpp"
#include "tree/default/phyloxml_mixin.hpp"
#include "tree/io/phyloxml/color_mixin.hpp"
#include "tree/io/phyloxml/processor.hpp"

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

    virtual void prepare_writing( TreeType const& tree, XmlDocument& xml );

    virtual void tree_edge_to_element( EdgeType const& edge, XmlElement& element );

    // -------------------------------------------------------------------------
    //     Member Data
    // -------------------------------------------------------------------------

private:

    size_t max_placements_per_edge_ = 0;

};

} // namespace genesis

#endif // include guard

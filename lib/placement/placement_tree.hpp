#ifndef GENESIS_PLACEMENT_PLACEMENT_TREE_H_
#define GENESIS_PLACEMENT_PLACEMENT_TREE_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include <string>
#include <vector>

#include "tree/newick_processor.hpp"
#include "tree/tree.hpp"
#include "utils/logging.hpp"

namespace genesis {

// =============================================================================
//     Forward Declarations
// =============================================================================

struct PqueryPlacement;

// =============================================================================
//     Placement Tree Node Data
// =============================================================================

class PlacementTreeNodeData : public DefaultTreeNodeData
{
public:

    // -----------------------------------------------------
    //     Class Functions
    // -----------------------------------------------------

    inline bool operator == (const PlacementTreeNodeData &other) const
    {
        return other.name == name;
    }

    inline bool operator != (const PlacementTreeNodeData &other) const
    {
        return !(other == *this);
    }

    // -----------------------------------------------------
    //     Default Functions
    // -----------------------------------------------------

    inline void FromNewickBrokerElement (NewickBrokerElement* node)
    {
        name = node->name;
    }

    inline void ToNewickBrokerElement (NewickBrokerElement* node) const
    {
        node->name = name;
    }

    inline std::string Dump() const
    {
        return "Name: '" + name + "'";
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    /**
     * Name of the node. In case it is a leaf, this is usually the name of
     * the taxon represented by the node.
     */
    std::string name;
};

// =============================================================================
//     Placement Tree Edge Data
// =============================================================================

class PlacementTreeEdgeData : public DefaultTreeEdgeData
{
public:

    // -----------------------------------------------------
    //     Class Functions
    // -----------------------------------------------------

    inline bool operator == (const PlacementTreeEdgeData &other) const
    {
        // TODO add a comparison of pqueries as well ?! is that good?!
        return other.branch_length == branch_length && other.edge_num == edge_num;
    }

    inline bool operator != (const PlacementTreeEdgeData &other) const
    {
        return !(other == *this);
    }

    // -----------------------------------------------------
    //     Default Functions
    // -----------------------------------------------------

    inline void FromNewickBrokerElement (NewickBrokerElement* nbe)
    {
        branch_length = nbe->branch_length;
        edge_num      = -1;

        if (nbe->tags.size() != 1) {
            LOG_WARN << "Edge for element '" << nbe->name << "' does not contain the single "
                     << "tag value denoting the edge_num for placements.";
            return;
        }
        edge_num = std::stoi(nbe->tags[0]);
    }

    inline void ToNewickBrokerElement (NewickBrokerElement* nbe) const
    {
        nbe->branch_length = branch_length;
        nbe->comments.push_back(std::to_string(PlacementCount()));
        nbe->tags.push_back(std::to_string(edge_num));
    }

    inline std::string Dump() const
    {
        return "Length: " + std::to_string(branch_length) + "\tEdge Num: " + std::to_string(edge_num);
    }

    // -----------------------------------------------------
    //     Member Functions
    // -----------------------------------------------------

    size_t PlacementCount() const;
    double PlacementMass() const;

    void SortPlacements();

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    int    edge_num;

    std::vector<PqueryPlacement*> placements;
};

// =============================================================================
//     Definitions and Typedefs
// =============================================================================

// let's avoid tedious names!
typedef Tree<PlacementTreeNodeData, PlacementTreeEdgeData> PlacementTree;

} // namespace genesis

#endif // include guard

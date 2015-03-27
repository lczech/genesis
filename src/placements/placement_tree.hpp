#ifndef GNS_PLACEMENT_PLACEMENTTREE_H_
#define GNS_PLACEMENT_PLACEMENTTREE_H_

/**
 * @brief
 *
 * @file
 * @ingroup placements
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
//     PlacementEdgeData
// =============================================================================

class PlacementEdgeData
{
public:

    // -----------------------------------------------------
    //     Class Functions
    // -----------------------------------------------------

    inline bool operator == (const PlacementEdgeData &other) const
    {
        // TODO add a comparison of pqueries as well ?! is that good?!
        return other.branch_length == branch_length && other.edge_num == edge_num;
    }

    inline bool operator != (const PlacementEdgeData &other) const
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

    double branch_length;
    int    edge_num;

    std::vector<PqueryPlacement*> placements;
};

// =============================================================================
//     PlacementNodeData
// =============================================================================

class PlacementNodeData
{
public:

    // -----------------------------------------------------
    //     Class Functions
    // -----------------------------------------------------

    inline bool operator == (const PlacementNodeData &other) const
    {
        return other.name == name;
    }

    inline bool operator != (const PlacementNodeData &other) const
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
//     Definitions and Typedefs
// =============================================================================

// let's avoid tedious names!
typedef Tree<PlacementNodeData, PlacementEdgeData> PlacementTree;

} // namespace genesis

#endif // include guard

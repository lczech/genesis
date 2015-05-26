#ifndef GENESIS_PLAUSIBILITY_PLAUSIBILITY_TREE_H_
#define GENESIS_PLAUSIBILITY_PLAUSIBILITY_TREE_H_

/**
 * @brief
 *
 * @file
 * @ingroup plausibility
 */

#include <string>

#include "tree/tree.hpp"
#include "tree/tree_set.hpp"

namespace genesis {

// =============================================================================
//     Plausibility Node Data
// =============================================================================

class PlausibilityNodeData
{
public:
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
    size_t      preorder_id;
};

// =============================================================================
//     Plausibility Edge Data
// =============================================================================

class PlausibilityEdgeData
{
public:

    // -----------------------------------------------------
    //     Default Functions
    // -----------------------------------------------------

    inline void FromNewickBrokerElement (NewickBrokerElement* nbe)
    {
        branch_length = nbe->branch_length;
    }

    inline void ToNewickBrokerElement (NewickBrokerElement* nbe) const
    {
        nbe->branch_length = branch_length;
    }

    inline std::string Dump() const
    {
        return "Length: " + std::to_string(branch_length);
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    double branch_length;
};

// =============================================================================
//     Definitions and Typedefs
// =============================================================================

// let's avoid tedious names!
typedef Tree   <PlausibilityNodeData, PlausibilityEdgeData> PlausibilityTree;
typedef TreeSet<PlausibilityNodeData, PlausibilityEdgeData> PlausibilityTreeSet;

} // namespace genesis

#endif // include guard

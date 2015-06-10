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

class PlausibilityTreeNodeData : public DefaultTreeNodeData
{
public:

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    size_t      preorder_id;
};

// =============================================================================
//     Plausibility Edge Data
// =============================================================================

class PlausibilityTreeEdgeData : public DefaultTreeEdgeData
{
public:

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

};

// =============================================================================
//     Definitions and Typedefs
// =============================================================================

// let's avoid tedious names!
typedef Tree   <PlausibilityTreeNodeData, PlausibilityTreeEdgeData> PlausibilityTree;
typedef TreeSet<PlausibilityTree>                                   PlausibilityTreeSet;

} // namespace genesis

#endif // include guard

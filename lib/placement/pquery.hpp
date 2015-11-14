#ifndef GENESIS_PLACEMENT_PQUERY_H_
#define GENESIS_PLACEMENT_PQUERY_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include <memory>
#include <string>
#include <vector>

#include "placement/pquery/name.hpp"
#include "placement/pquery/placement.hpp"

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

template<class NodeDataType, class EdgeDataType>
class TreeEdge;

class PlacementTreeEdgeData;
class PlacementTreeNodeData;

typedef TreeEdge <PlacementTreeNodeData, PlacementTreeEdgeData> PlacementTreeEdge;

// =================================================================================================
//     Pquery
// =================================================================================================

class Pquery
{
public:

    // -------------------------------------------------------------------
    //     Placements
    // -------------------------------------------------------------------

    PqueryPlacement* emplace_placement(PlacementTreeEdge* edge);

    PqueryPlacement* insert_placement(
        const PqueryPlacement& val,
        PlacementTreeEdge* edge = nullptr
    );

    inline size_t placement_size() const
    {
        return placements.size();
    }

    inline PqueryPlacement const & placement_at (const size_t index) const
    {
        return *placements[index];
    }

    // -------------------------------------------------------------------
    //     Names
    // -------------------------------------------------------------------

    PqueryName* emplace_name(std::string name = "", double multiplicity = 0.0);

    PqueryName* insert_name(const PqueryName& other);

    inline size_t name_size() const
    {
        return names.size();
    }

    inline PqueryName const & name_at (const size_t index) const
    {
        return *names[index];
    }

    // -------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------

    std::vector<std::unique_ptr<PqueryPlacement>> placements;
    std::vector<std::unique_ptr<PqueryName>>      names;
};

// =================================================================================================
//     Pquery Placement Plain
// =================================================================================================

/**
 * @brief Simple POD struct for a Placement used for speeding up some calculations.
 *
 * It is not as flexible as the default representation, but its memory footprint is compact, because
 * of the lack of pointers.
 */
struct PqueryPlacementPlain
{
    size_t edge_index;
    size_t primary_node_index;
    size_t secondary_node_index;

    double branch_length;
    double pendant_length;
    double proximal_length;
    double like_weight_ratio;
};

// =================================================================================================
//     Pquery Plain
// =================================================================================================

/**
 *
 */
struct PqueryPlain
{
    size_t                            index;
    std::vector<PqueryPlacementPlain> placements;
};

} // namespace genesis

#endif // include guard

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


namespace placement {
    class PlacementTreeEdgeData;
    class PlacementTreeNodeData;
}

typedef TreeEdge< placement::PlacementTreeNodeData, placement::PlacementTreeEdgeData > PlacementTreeEdge;

// =================================================================================================
//     Pquery
// =================================================================================================

namespace placement {

class Pquery
{
public:

    // -------------------------------------------------------------------
    //     Placements
    // -------------------------------------------------------------------

    typedef std::vector<std::unique_ptr<PqueryPlacement>>::iterator       iterator_placements;
    typedef std::vector<std::unique_ptr<PqueryPlacement>>::const_iterator const_iterator_placements;

    PqueryPlacement* emplace_placement(PlacementTreeEdge* edge);

    PqueryPlacement* insert_placement(
        PqueryPlacement const& val,
        PlacementTreeEdge* edge = nullptr
    );

    size_t placement_size() const
    {
        return placements.size();
    }

    PqueryPlacement const& placement_at( size_t index ) const
    {
        return *placements[index];
    }

    iterator_placements begin_placements()
    {
        return placements.begin();
    }

    iterator_placements end_placements()
    {
        return placements.end();
    }

    const_iterator_placements begin_placements() const
    {
        return placements.cbegin();
    }

    const_iterator_placements end_placements() const
    {
        return placements.cend();
    }

    // -------------------------------------------------------------------
    //     Names
    // -------------------------------------------------------------------

    PqueryName* emplace_name(std::string name = "", double multiplicity = 0.0);

    PqueryName* insert_name( PqueryName const& other );

    size_t name_size() const
    {
        return names.size();
    }

    PqueryName const& name_at( size_t index ) const
    {
        return *names.at(index);
    }

    // -------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------

    std::vector<std::unique_ptr<PqueryPlacement>> placements;
    std::vector<std::unique_ptr<PqueryName>>      names;
};

} // namespace placement
} // namespace genesis

#endif // include guard

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

namespace tree {
    template<class NodeDataType, class EdgeDataType>
    class TreeEdge;
}

namespace placement {
    class PlacementTreeEdgeData;
    class PlacementTreeNodeData;

    typedef tree::TreeEdge< PlacementTreeNodeData, PlacementTreeEdgeData > PlacementTreeEdge;
}

// =================================================================================================
//     Pquery
// =================================================================================================

namespace placement {

class Pquery
{
public:

    // -------------------------------------------------------------------
    //     Placement Iterators
    // -------------------------------------------------------------------

    typedef std::vector<std::unique_ptr<PqueryPlacement>>::iterator       iterator_placements;
    typedef std::vector<std::unique_ptr<PqueryPlacement>>::const_iterator const_iterator_placements;

    iterator_placements       begin_placements();
    const_iterator_placements begin_placements() const;

    iterator_placements       end_placements();
    const_iterator_placements end_placements() const;

    // -------------------------------------------------------------------
    //     Placement Accessors and Modifiers
    // -------------------------------------------------------------------

    PqueryPlacement& add_placement( PlacementTreeEdge* edge );

    PqueryPlacement& add_placement(
        PqueryPlacement const& val,
        PlacementTreeEdge* edge = nullptr
    );

    size_t placement_size() const;
    PqueryPlacement const& placement_at( size_t index ) const;

    // -------------------------------------------------------------------
    //     Name Iterators
    // -------------------------------------------------------------------

    typedef std::vector<PqueryName>::iterator       iterator_names;
    typedef std::vector<PqueryName>::const_iterator const_iterator_names;

    iterator_names       begin_names();
    const_iterator_names begin_names() const;

    iterator_names       end_names();
    const_iterator_names end_names() const;

    // -------------------------------------------------------------------
    //     Name Accessors and Modifiers
    // -------------------------------------------------------------------

    PqueryName& add_name( std::string name = "", double multiplicity = 0.0 );
    PqueryName& add_name( PqueryName const& other );

    size_t name_size() const;
    PqueryName const& name_at( size_t index ) const;

    void clear_names();

    // -------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------

    std::vector<std::unique_ptr<PqueryPlacement>> placements;

private:

    std::vector<PqueryName>      names_;
};

} // namespace placement
} // namespace genesis

#endif // include guard

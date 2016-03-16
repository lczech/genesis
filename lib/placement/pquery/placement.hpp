#ifndef GENESIS_PLACEMENT_PQUERY_PLACEMENT_H_
#define GENESIS_PLACEMENT_PQUERY_PLACEMENT_H_

/**
 * @brief Header of PqueryPlacement class.
 *
 * @file
 * @ingroup placement
 */

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
    class Pquery;

    typedef tree::TreeEdge< PlacementTreeNodeData, PlacementTreeEdgeData > PlacementTreeEdge;
}

// =================================================================================================
//     Pquery Placement
// =================================================================================================

namespace placement {

/**
 * @brief
 */
class PqueryPlacement
{
public:

    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

    // TODO introduce rule of five, make awesome c++ 11 stuff!
    // TODO make construction private and Pquery a friend who is the only one to construct a Placement.

    PqueryPlacement (
        // const Pquery* pquery, const PlacementTreeEdge* edge
    ) :
        // edge_num(edge->edge_num),
        edge_num(0),
        likelihood(0.0),
        like_weight_ratio(0.0),
        proximal_length(0.0),
        pendant_length(0.0),
        parsimony(0),
        // pquery(pquery),
        // edge(edge)
        // pquery(nullptr),
        edge(nullptr)
    {}

    /**
     * @brief Something lika copy constructor. Takes the pquery and edge for inserting this
     * placement, plus another placement, from which the other values are copied.
     */
    PqueryPlacement (
        // const Pquery* pquery, const PlacementTreeEdge* edge, const PqueryPlacement& other
        const PqueryPlacement& other
    ) :
        // edge_num(edge.edge_num),
        edge_num(other.edge_num),
        likelihood(other.likelihood),
        like_weight_ratio(other.like_weight_ratio),
        proximal_length(other.proximal_length),
        pendant_length(other.pendant_length),
        parsimony(other.parsimony),
        // pquery(pquery),
        // edge(edge)
        // pquery(nullptr),
        edge(nullptr)
    {}

    ~PqueryPlacement() = default;

    // -------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------

    // TODO fix the get_ part in here!!! make members protected, refactor all occurences

    inline const PlacementTreeEdge& get_edge() const
    {
        return *edge;
    }

    // -------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------

    // const int edge_num;
    int       edge_num;
    double    likelihood;
    double    like_weight_ratio;
    double    proximal_length;
    double    pendant_length;
    int       parsimony;

// protected:

    // const Pquery*                  pquery_;
    // const PlacementTreeEdge* edge_;

    PlacementTreeEdge* edge;
};

} // namespace placement
} // namespace genesis

#endif // include guard

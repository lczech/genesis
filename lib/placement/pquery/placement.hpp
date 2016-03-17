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

    friend Pquery;

    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

    // TODO introduce rule of five, make awesome c++ 11 stuff!
    // TODO make construction private and Pquery a friend who is the only one to construct a Placement.

    PqueryPlacement () :
        likelihood(0.0),
        like_weight_ratio(0.0),
        proximal_length(0.0),
        pendant_length(0.0),
        parsimony(0),
        edge_num_(0),
        edge_(nullptr)
    {}

    /**
     * @brief Something lika copy constructor. Takes the pquery and edge for inserting this
     * placement, plus another placement, from which the other values are copied.
     */
    PqueryPlacement (
        const PqueryPlacement& other
    ) :
        likelihood(other.likelihood),
        like_weight_ratio(other.like_weight_ratio),
        proximal_length(other.proximal_length),
        pendant_length(other.pendant_length),
        parsimony(other.parsimony),
        edge_num_(other.edge_num_),
        edge_(nullptr)
    {}

    ~PqueryPlacement() = default;

    // -------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------

    int edge_num() const;
    void reset_edge_num( int val );

    const PlacementTreeEdge& edge() const;
          PlacementTreeEdge& edge();

    void reset_edge( PlacementTreeEdge* edge );

    // -------------------------------------------------------------------
    //     Public Properties
    // -------------------------------------------------------------------

    double    likelihood;
    double    like_weight_ratio;
    double    proximal_length;
    double    pendant_length;
    int       parsimony;

    // -------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------

private:

    int       edge_num_;
    PlacementTreeEdge* edge_;
};

} // namespace placement
} // namespace genesis

#endif // include guard

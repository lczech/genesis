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
        likelihood_(0.0),
        like_weight_ratio_(0.0),
        proximal_length_(0.0),
        pendant_length_(0.0),
        parsimony_(0),
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
        likelihood_(other.likelihood_),
        like_weight_ratio_(other.like_weight_ratio_),
        proximal_length_(other.proximal_length_),
        pendant_length_(other.pendant_length_),
        parsimony_(other.parsimony_),
        edge_num_(other.edge_num_),
        edge_(nullptr)
    {}

    ~PqueryPlacement() = default;

    // -------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------

    double likelihood() const;
    PqueryPlacement& likelihood( double val );

    double like_weight_ratio() const;
    PqueryPlacement& like_weight_ratio( double val );

    double proximal_length() const;
    PqueryPlacement& proximal_length( double val );

    double pendant_length() const;
    PqueryPlacement& pendant_length( double val );

    int parsimony() const;
    PqueryPlacement& parsimony( int val );

    // -------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------

    int edge_num() const;
    void reset_edge_num( int val );

    const PlacementTreeEdge& edge() const;
          PlacementTreeEdge& edge();

    void reset_edge( PlacementTreeEdge* edge );

    // -------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------

private:

    double    likelihood_;
    double    like_weight_ratio_;
    double    proximal_length_;
    double    pendant_length_;
    int       parsimony_;

    int       edge_num_;
    PlacementTreeEdge* edge_;
};

} // namespace placement
} // namespace genesis

#endif // include guard

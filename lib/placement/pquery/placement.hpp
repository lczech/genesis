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
 * @brief One placement position of a Pquery on a PlacementTree.
 *
 * This class is modeled after the `jplace` standard, which allows for multiple placement positions
 * for a Pquery. Usually, those positions are on different branches of the tree. The property
 * values of this class describe one such placement position.
 *
 * In order to check the position of this placement on the tree, see #proximal_length,
 * #pendant_length and edge(). In order to check the likelihood and probability of this placement
 * beging placed exaclty where it is, see #likelihood and #like_weight_ratio.
 */
class PqueryPlacement
{
public:

    friend class Pquery;

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
    //     Public Properties
    // -------------------------------------------------------------------

    // Yes, the following members are public data members. It's neither nice nor consistent,
    // but makes life so much easier for the moment. Maybe we'll fix that in the future...

    /**
     * @brief Total likelihood of the tree with this placement attached to it.
     *
     * This property is defined by the `jplace` standard.
     */
    double    likelihood;

    /**
    * @brief Likelihood weight ratio of this placement.
    *
    * The likelihood weight ratio is a probability-like value of how certain the placement
    * algorithm was when placing the Pquery at the edge of this placement.
    * The `like_weight_ratio`s of all placements for one Pquery sum up to 1.0. As not all of them
    * might be stored in the Pquery, however, the sum might be lower.
    *
    * This property is defined by the `jplace` standard.
    */
    double    like_weight_ratio;

    /**
    * @brief Distance of this placement to the next node towards the root.
    *
    * This value determines the distance of the placement attachement position on the edge to the
    * next PlacementTreeNode that lies towards the root of the PlacementTree.
    *
    * This property is not defined by the `jplace` standard. Instead, the standard uses
    * `distal_length`, which is the opposite of this value: It determines the distance to the next
    * node that lies away from the root. We use the `proximal_length` instead, as it is much more
    * convenient for most purposes.
    */
    double    proximal_length;

    /**
    * @brief Length of the attached branch of this placement.
    *
    * The placement can be interpreted as a new branch on the PlacementTree. This value then gives
    * the length of that branch.
    *
    * This property is defined by the `jplace` standard.
    */
    double    pendant_length;

    /**
    * @brief Parsimony value.
    *
    * This property is defined by the `jplace` standard. It is currently not used.
    */
    int       parsimony;

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

    int                edge_num_;
    PlacementTreeEdge* edge_;
};

} // namespace placement
} // namespace genesis

#endif // include guard

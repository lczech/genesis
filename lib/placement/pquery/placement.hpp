#ifndef GENESIS_PLACEMENT_PQUERY_PLACEMENT_H_
#define GENESIS_PLACEMENT_PQUERY_PLACEMENT_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

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

    class TreeEdge;

}

namespace placement {

    using PlacementTreeEdge = tree::TreeEdge;

    class PlacementEdgeData;
    class PlacementNodeData;
    class Pquery;

}

// =================================================================================================
//     Pquery Placement
// =================================================================================================

namespace placement {

/**
 * @brief One placement position of a Pquery on a Tree.
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

    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

    /**
     * @brief Default constructor. Sets all values to 0.
     */
    PqueryPlacement ()
        : likelihood(0.0)
        , like_weight_ratio(0.0)
        , proximal_length(0.0)
        , pendant_length(0.0)
        , parsimony(0)
        , edge_(nullptr)
    {}

    /**
     * @brief Constructor that takes the edge where this placement is being placed at.
     */
    PqueryPlacement( PlacementTreeEdge& edge )
        : likelihood(0.0)
        , like_weight_ratio(0.0)
        , proximal_length(0.0)
        , pendant_length(0.0)
        , parsimony(0)
        , edge_(&edge)
    {}

    ~PqueryPlacement() = default;

    PqueryPlacement( PqueryPlacement const& ) = default;
    PqueryPlacement( PqueryPlacement&& )      = default;

    PqueryPlacement& operator= ( PqueryPlacement const& ) = default;
    PqueryPlacement& operator= ( PqueryPlacement&& )      = default;

    // -------------------------------------------------------------------
    //     Public Property Data Members
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
    * next TreeNode that lies towards the root of the Tree.
    *
    * This property is not defined by the `jplace` standard. Instead, the standard uses
    * `distal_length`, which is the opposite of this value: It determines the distance to the next
    * node that lies away from the root. We use the `proximal_length` instead, as it is much more
    * convenient for most purposes. In order to obtain the `distal_length`, use
    *
    *     PqueryPlacement p;
    *     double distal_length = p.edge().data.branch_length - p.proximal_length;
    *
    * This is also the formula that is internally used to convert between the two.
    */
    double    proximal_length;

    /**
    * @brief Length of the attached branch of this placement.
    *
    * The placement can be interpreted as a new branch on the Tree. This value then gives
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
    //     Properties
    // -------------------------------------------------------------------

    int edge_num() const;

    const PlacementTreeEdge& edge() const;
          PlacementTreeEdge& edge();

    void reset_edge( PlacementTreeEdge& edge );

    // -------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------

private:

    PlacementTreeEdge* edge_;
};

} // namespace placement
} // namespace genesis

#endif // include guard

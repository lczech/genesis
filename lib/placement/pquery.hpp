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

#include "placement/placement_tree.hpp"

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Pquery;

// =================================================================================================
//     Pquery Placement
// =================================================================================================

/**
 *
 */
class PqueryPlacement
{
public:

    // TODO introduce rule of five, make awesome c++ 11 stuff!

    PqueryPlacement (
        // const Pquery* pquery, const PlacementTree::EdgeType* edge
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
        pquery(nullptr),
        edge(nullptr)
    {}

    /**
     * @brief Something lika copy constructor. Takes the pquery and edge for inserting this
     * placement, plus another placement, from which the other values are copied.
     */
    PqueryPlacement (
        // const Pquery* pquery, const PlacementTree::EdgeType* edge, const PqueryPlacement& other
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
        pquery(nullptr),
        edge(nullptr)
    {}

    // const int edge_num;
    int       edge_num;
    double    likelihood;
    double    like_weight_ratio;
    double    proximal_length;
    double    pendant_length;
    int       parsimony;

    // TODO fix the get_ part in here!!! make members protected, refactor all occurences

    inline const Pquery& get_pquery() const
    {
        return *pquery;
    }

    inline const PlacementTree::EdgeType& get_edge() const
    {
        return *edge;
    }

// protected:

    // const Pquery*                  pquery_;
    // const PlacementTree::EdgeType* edge_;

    Pquery*                  pquery;
    PlacementTree::EdgeType* edge;
};

// =================================================================================================
//     Pquery Name
// =================================================================================================

/**
 *
 */
class PqueryName
{
public:

    PqueryName(std::string name = "") : name(name), multiplicity(0.0), pquery(nullptr)
    {}

    /**
     * @brief Copy constructor.
     */
    PqueryName(const PqueryName& other) :
        name(other.name),
        multiplicity(other.multiplicity),
        pquery(nullptr)
    {}

    std::string name;
    double      multiplicity;

    Pquery* pquery;
};

// =================================================================================================
//     Pquery
// =================================================================================================

/**
 *
 */
class Pquery
{
public:

    // -------------------------------------------------------------------
    //     Placements
    // -------------------------------------------------------------------

    PqueryPlacement* add_placement(PlacementTree::EdgeType* edge);

    inline size_t placement_size() const
    {
        return placements.size();
    }

    inline PqueryPlacement const & get_placement (const size_t index) const
    {
        return *placements[index];
    }

    // -------------------------------------------------------------------
    //     Names
    // -------------------------------------------------------------------

    PqueryName*      add_name(std::string name = "");

    inline size_t name_size() const
    {
        return names.size();
    }

    inline PqueryName const & get_name (const size_t index) const
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

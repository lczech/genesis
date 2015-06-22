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

struct Pquery;

// =================================================================================================
//     Pquery Placement
// =================================================================================================

/**
 *
 */
struct PqueryPlacement
{
    PqueryPlacement() : edge_num(0), likelihood(0.0), like_weight_ratio(0.0), proximal_length(0.0),
                        pendant_length(0.0), parsimony(0), pquery(nullptr), edge(nullptr)
    {}

    /**
     * @brief Copy constructor.
     */
    PqueryPlacement(const PqueryPlacement& other) :
        edge_num(other.edge_num),
        likelihood(other.likelihood),
        like_weight_ratio(other.like_weight_ratio),
        proximal_length(other.proximal_length),
        pendant_length(other.pendant_length),
        parsimony(other.parsimony),
        pquery(nullptr),
        edge(nullptr)
    {}

    int     edge_num;
    double  likelihood;
    double  like_weight_ratio;
    double  proximal_length;
    double  pendant_length;
    int     parsimony;

    Pquery* pquery;
    PlacementTree::EdgeType* edge;
};

// =================================================================================================
//     Pquery Name
// =================================================================================================

/**
 *
 */
struct PqueryName
{
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
struct Pquery
{
    PqueryPlacement* add_placement(PlacementTree::EdgeType* edge);
    PqueryName*      add_name(std::string name = "");

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

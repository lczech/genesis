#ifndef GENESIS_PLACEMENT_PQUERY_H_
#define GENESIS_PLACEMENT_PQUERY_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include <string>
#include <vector>

#include "placement/placement_tree.hpp"

namespace genesis {

// =============================================================================
//     Forward Declarations
// =============================================================================

struct Pquery;

// =============================================================================
//     Pquery Placement
// =============================================================================

struct PqueryPlacement
{
    PqueryPlacement() : edge_num(0), likelihood(0.0), like_weight_ratio(0.0), proximal_length(0.0),
                        pendant_length(0.0), parsimony(0), pquery(nullptr), edge(nullptr)
    {}

    PqueryPlacement(const PqueryPlacement* other) :
        edge_num(other->edge_num),
        likelihood(other->likelihood),
        like_weight_ratio(other->like_weight_ratio),
        proximal_length(other->proximal_length),
        pendant_length(other->pendant_length),
        parsimony(other->parsimony),
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

// =============================================================================
//     Pquery Name
// =============================================================================

struct PqueryName
{
    PqueryName() : name(""), multiplicity(0.0), pquery(nullptr)
    {}

    PqueryName(const PqueryName* other) :
        name(other->name),
        multiplicity(other->multiplicity),
        pquery(nullptr)
    {}

    std::string name;
    double      multiplicity;

    Pquery* pquery;
};

// =============================================================================
//     Pquery
// =============================================================================

struct Pquery
{
    ~Pquery()
    {
        for (PqueryPlacement* p : placements) {
            delete p;
        }
        for (PqueryName* n : names) {
            delete n;
        }
        placements.clear();
        names.clear();
    }

    std::vector<PqueryPlacement*> placements;
    std::vector<PqueryName*>      names;
};

} // namespace genesis

#endif // include guard

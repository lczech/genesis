#ifndef GNS_PLACEMENT_PLACEMENTS_H_
#define GNS_PLACEMENT_PLACEMENTS_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include <deque>
#include <string>
#include <unordered_map>

#include "placement/placement_tree.hh"
#include "tree/tree.hh"

namespace genesis {

// =============================================================================
//     Pquery
// =============================================================================

class Pquery
{
public:
    struct Placement;

//~ protected:
    std::deque<Placement> placements;
};

struct Pquery::Placement
{
    int    edge_num;
    double likelihood;
    double like_weight_ratio;
    double distal_length;
    double pendant_length;
    int    parsimony;
};

// =============================================================================
//     Placements
// =============================================================================

class Placements
{
public:
    Placements () {}
    Placements (PlacementTree& ptree) : tree(ptree) {}

    std::deque<Pquery> pqueries;
    PlacementTree tree;

    typedef std::unordered_map<std::string, std::string> Metadata;
    Metadata metadata;
};

} // namespace genesis

#endif // include guard

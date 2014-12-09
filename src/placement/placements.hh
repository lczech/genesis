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

struct Pquery
{
    struct Placement
    {
        Placement() : edge_num(0), likelihood(0.0), like_weight_ratio(0.0),
                      distal_length(0.0), pendant_length(0.0), parsimony(0)
        {}

        int    edge_num;
        double likelihood;
        double like_weight_ratio;
        double distal_length;
        double pendant_length;
        int    parsimony;
    };

    struct Name
    {
        Name() : name(""), multiplicity(0.0) {}

        std::string name;
        double      multiplicity;
    };

    std::deque<Placement> placements;
    std::deque<Name>      names;
};

// =============================================================================
//     Placements
// =============================================================================

class Placements
{
public:
    Placements () {}
    Placements (PlacementTree& ptree) : tree(ptree) {}
    void clear();
    ~Placements();

    std::deque<Pquery*> pqueries;
    PlacementTree tree;

    typedef std::unordered_map<std::string, std::string> Metadata;
    Metadata metadata;

    std::string DumpAll();
    std::string DumpPqueries();
    std::string DumpTree();
};

} // namespace genesis

#endif // include guard

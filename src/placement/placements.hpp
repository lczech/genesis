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

namespace genesis {

// =============================================================================
//     Forward Declarations
// =============================================================================

class JsonDocument;
class JsonLexer;

// =============================================================================
//     Pquery
// =============================================================================

struct Pquery
{
    // -----------------------------------------------------
    //     Placement
    // -----------------------------------------------------

    struct Placement
    {
        Placement() : edge_num(0), likelihood(0.0), like_weight_ratio(0.0), distal_length(0.0),
                      pendant_length(0.0), parsimony(0)//, edge(nullptr)
        {}

        int    edge_num;
        double likelihood;
        double like_weight_ratio;
        double distal_length;
        double pendant_length;
        int    parsimony;

        // TODO the default copy ctor and assignment op do not treat this pointer correctly, as it still refers to the tree of the other placements object! (they are used by the Merge() function!)
        //~ PlacementTree::EdgeType* edge;
    };

    // -----------------------------------------------------
    //     Name
    // -----------------------------------------------------

    struct Name
    {
        Name() : name(""), multiplicity(0.0) {}

        std::string name;
        double      multiplicity;
    };

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    std::deque<Placement> placements;
    std::deque<Name>      names;
};

// =============================================================================
//     Placements
// =============================================================================

} // namespace genesis

#include "placement/placement_tree.hpp"
#include "tree/tree.hpp"

namespace genesis {

class Placements
{
public:
    typedef std::unordered_map<std::string, std::string> Metadata;

    // -----------------------------------------------------
    //     Constructor & Destructor
    // -----------------------------------------------------

    Placements () {}
    Placements (PlacementTree& ptree) : tree(ptree) {}
    void clear();
    ~Placements();

    bool FromJplaceFile   (const std::string&  fn);
    bool FromJplaceString (const std::string&  jplace);
    bool FromJsonLexer    (const JsonLexer&    lexer);
    bool FromJsonDocument (const JsonDocument& doc);

    bool Merge(Placements& other);

    // -----------------------------------------------------
    //     Placement Weight
    // -----------------------------------------------------

    double PlacementMassSum();
    double EMD(Placements& other);
    void   COG();
    double Variance();

    // -----------------------------------------------------
    //     Dump and Debug
    // -----------------------------------------------------

    std::string DumpAll();
    std::string DumpPqueries();
    std::string DumpTree();

    bool Validate();

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    std::deque<Pquery*> pqueries;
    PlacementTree       tree;
    Metadata            metadata;
};

} // namespace genesis

#endif // include guard

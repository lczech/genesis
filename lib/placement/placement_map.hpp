#ifndef GENESIS_PLACEMENT_PLACEMENTSET_H_
#define GENESIS_PLACEMENT_PLACEMENTSET_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include <string>
#include <unordered_map>
#include <vector>

#include "placement/placement_tree.hpp"

namespace genesis {

// =============================================================================
//     Forward Declarations
// =============================================================================

class  JsonDocument;
class  JsonLexer;
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

// =============================================================================
//     PlacementMap
// =============================================================================

class PlacementMap
{
public:
    // -----------------------------------------------------
    //     Constructor & Destructor
    // -----------------------------------------------------

    PlacementMap () {}
    PlacementMap (PlacementTree& ptree) : tree(ptree) {}

    PlacementMap (const PlacementMap& other);
    PlacementMap& operator = (const PlacementMap& other);

    ~PlacementMap();
    void clear();

    typedef std::unordered_map<int, PlacementTree::EdgeType*> EdgeNumMapType;
    EdgeNumMapType* EdgeNumMap() const;

    bool Merge(const PlacementMap& other);
    void NormalizeWeightRatios();
    void RestrainToMaxWeightPlacements();

    // -----------------------------------------------------
    //     Placement Weight
    // -----------------------------------------------------

    size_t PlacementCount() const;
    double PlacementMass() const;

    std::vector<int> ClosestLeafDepthHistogram() const;
    std::vector<int> ClosestLeafDistanceHistogram (
        const double min, const double max, const int bins = 10
    ) const;
    std::vector<int> ClosestLeafDistanceHistogramAuto (
        double& min, double& max, const int bins = 10
    ) const;

    static double EMD (const PlacementMap& left, const PlacementMap& right);
    double EMD (const PlacementMap& other) const;

    void   COG() const;

    // -----------------------------------------------------
    //     Variance
    // -----------------------------------------------------

public:
    double Variance() const;

protected:
    /** @brief Intermediate POD struct used for speeding up the variance calculations. */
    typedef struct {
        size_t index;
        size_t edge_index;
        size_t primary_node_index;
        size_t secondary_node_index;

        double pendant_length;
        double proximal_length;
        double branch_length;
        double like_weight_ratio;
    } VarianceData;

    void VarianceThread (
        const int                        offset,
        const int                        incr,
        const std::vector<VarianceData>* pqrys,
        const Matrix<double>*            node_distances,
        double*                          partial,
        double*                          count
    ) const;

    double VariancePartial (
        const VarianceData&              place_a,
        const std::vector<VarianceData>& pqrys_b,
        const Matrix<double>&            node_distances
    ) const;

    // -----------------------------------------------------
    //     Dump and Debug
    // -----------------------------------------------------

public:
    std::string Dump() const;
    bool Validate (bool check_values = false, bool break_on_values = false) const;

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    std::vector<Pquery*>                         pqueries;
    PlacementTree                                tree;
    std::unordered_map<std::string, std::string> metadata;
};

} // namespace genesis

#endif // include guard

#ifndef GENESIS_PLACEMENT_PLACEMENT_MAP_H_
#define GENESIS_PLACEMENT_PLACEMENT_MAP_H_

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
#include "placement/pquery.hpp"

namespace genesis {

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
    EdgeNumMapType* edge_num_map() const;

    bool merge(const PlacementMap& other);
    void normalize_weight_ratios();
    void restrain_to_max_weight_placements();

    // -----------------------------------------------------
    //     Placement Weight
    // -----------------------------------------------------

    size_t placement_count() const;
    double placement_mass() const;

    std::vector<int> closest_leaf_depth_histogram() const;
    std::vector<int> closest_leaf_distance_histogram (
        const double min, const double max, const int bins = 10
    ) const;
    std::vector<int> closest_leaf_distance_histogram_auto (
        double& min, double& max, const int bins = 10
    ) const;

    static double earth_movers_distance (const PlacementMap& left, const PlacementMap& right, const bool with_pendant_length = true);
    	   double earth_movers_distance (const PlacementMap& other, const bool with_pendant_length = true) const;

    void   center_of_gravity() const;

    // -----------------------------------------------------
    //     variance
    // -----------------------------------------------------

public:
    double variance() const;

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

    void variance_thread (
        const int                        offset,
        const int                        incr,
        const std::vector<VarianceData>* pqrys,
        const Matrix<double>*            node_distances,
        double*                          partial,
        double*                          count
    ) const;

    double variance_partial (
        const VarianceData&              place_a,
        const std::vector<VarianceData>& pqrys_b,
        const Matrix<double>&            node_distances
    ) const;

    // -----------------------------------------------------
    //     Dump and Debug
    // -----------------------------------------------------

public:
    std::string dump() const;
    bool validate (bool check_values = false, bool break_on_values = false) const;

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    std::vector<Pquery*>                         pqueries;
    PlacementTree                                tree;
    std::unordered_map<std::string, std::string> metadata;
};

} // namespace genesis

#endif // include guard

#ifndef GENESIS_PLACEMENT_SAMPLE_H_
#define GENESIS_PLACEMENT_SAMPLE_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "placement/placement_tree.hpp"
#include "placement/pquery.hpp"

// TODO remove once the helper methods are outsourced
#include "placement/pquery/plain.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Sample
// =================================================================================================

class Sample
{
public:
    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    Sample ()
        : tree_(std::make_shared<PlacementTree>())
    {}

    Sample (std::shared_ptr<PlacementTree> ptree)
        : tree_(ptree)
    {}

    Sample( Sample const& );
    Sample( Sample&& ) noexcept;

    Sample& operator= (Sample const&);
    Sample& operator= (Sample&&) noexcept;

    // Sample( Sample&& ) = default;
    //
    // Sample& operator= (Sample const&) = default;
    // Sample& operator= (Sample&&) = default;

    ~Sample();

    void swap (Sample& other) noexcept;

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    Pquery* add_pquery();

    bool merge(const Sample& other);

    void clear();
    void clear_placements();

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    inline std::shared_ptr<PlacementTree> tree_ptr()
    {
        return tree_;
    }

    inline PlacementTree& tree()
    {
        return *tree_.get();
    }

    inline PlacementTree const& tree() const
    {
        return *tree_.get();
    }

    inline std::vector<std::unique_ptr<Pquery>>& pqueries()
    {
        return pqueries_;
    }

    inline const std::vector<std::unique_ptr<Pquery>>& pqueries() const
    {
        return pqueries_;
    }

    inline size_t pquery_size() const
    {
        return pqueries_.size();
    }

    inline Pquery const & pquery (const size_t index)
    {
        return *pqueries_[index];
    }

    // TODO add pquery iterator.

    // -------------------------------------------------------------------------
    //     Helper Methods
    // -------------------------------------------------------------------------

    typedef std::unordered_map<int, PlacementTree::EdgeType*> EdgeNumMapType;
    EdgeNumMapType edge_num_map() const;

    std::vector<PqueryPlain> plain_queries() const;

    void detach_pqueries_from_tree();
    void reattach_pqueries_to_tree();

    // -------------------------------------------------------------------------
    //     Placement Mass
    // -------------------------------------------------------------------------

    size_t placement_count() const;
    double placement_mass()  const;

    std::vector<int> closest_leaf_depth_histogram() const;
    std::vector<int> closest_leaf_distance_histogram (
        const double min, const double max, const int bins = 10
    ) const;
    std::vector<int> closest_leaf_distance_histogram_auto (
        double& min, double& max, const int bins = 10
    ) const;

    // -------------------------------------------------------------------------
    //     Dump and Debug
    // -------------------------------------------------------------------------

public:

    std::string dump_tree() const;
    bool validate (bool check_values = false, bool break_on_values = false) const;

    // -------------------------------------------------------------------------
    //     Members
    // -------------------------------------------------------------------------

protected:
    std::vector<std::unique_ptr<Pquery>>         pqueries_;
    std::shared_ptr<PlacementTree>               tree_;

public:
    // There is not much to mess up here for a user, so we can simply make this public.
    std::unordered_map<std::string, std::string> metadata;
};

} // namespace placement
} // namespace genesis

#endif // include guard

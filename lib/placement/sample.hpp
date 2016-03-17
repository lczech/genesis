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

    ~Sample() = default;

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

    size_t pquery_size() const;

    Pquery      & pquery_at( const size_t index );
    Pquery const& pquery_at( const size_t index ) const;

    // TODO add pquery iterator.

    // -------------------------------------------------------------------------
    //     Placement Mass
    // -------------------------------------------------------------------------

    size_t placement_count() const;
    double placement_mass()  const;

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

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

namespace genesis {
namespace placement {

// =================================================================================================
//     Sample
// =================================================================================================

/**
 * @brief Manage a set of @link Pquery Pqueries @endlink along with the PlacementTree where
 * the PqueryPlacement%s are placed on.
 *
 * This class stores both the tree and the set of pqueries with their placements. It is thus
 * a representation of a whole `jplace` file according to the standard.
 *
 * For more information on the `jplace` format, see the following publication:
 *
 *     Matsen FA, Hoffman NG, Gallagher A, Stamatakis A. 2012.
 *     A Format for Phylogenetic Sample.
 *     PLoS ONE 7(2): e31009. doi:10.1371/journal.pone.0031009
 *     http://journals.plos.org/plosone/article?id=10.1371/journal.pone.0031009
 *
 * This class and other related classes are modeled after this standard.
 */
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
    void clear_pqueries();

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

    // -------------------------------------------------------------------------
    //     Pquery Iterator
    // -------------------------------------------------------------------------

    typedef std::vector<std::unique_ptr<Pquery>>::iterator       iterator_pqueries;
    typedef std::vector<std::unique_ptr<Pquery>>::const_iterator const_iterator_pqueries;

    iterator_pqueries       begin();
    const_iterator_pqueries begin() const;

    iterator_pqueries       end();
    const_iterator_pqueries end() const;

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

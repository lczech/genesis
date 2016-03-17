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

    Sample() = default;

    Sample( PlacementTree const& tree )
        : tree_(tree)
    {}

    Sample( Sample const& ) = default;
    Sample( Sample&& )      = default;

    Sample& operator= ( Sample const& ) = default;
    Sample& operator= ( Sample&& )      = default;

    ~Sample() = default;

    void swap( Sample& other );

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    void clear();

    // -------------------------------------------------------------------------
    //     Tree Accessors and Modifiers
    // -------------------------------------------------------------------------

    PlacementTree      & tree();
    PlacementTree const& tree() const;

    // -------------------------------------------------------------------------
    //     Pquery Accessors and Modifiers
    // -------------------------------------------------------------------------

    Pquery& add_pquery();
    Pquery& add_pquery( Pquery const& other );

    std::vector<Pquery> const& pqueries() const;
    size_t pquery_size() const;

    Pquery      & pquery_at( size_t index );
    Pquery const& pquery_at( size_t index ) const;

    void remove_pquery_at( size_t index );
    void clear_pqueries();

    // -------------------------------------------------------------------------
    //     Pquery Iterator
    // -------------------------------------------------------------------------

    typedef std::vector<Pquery>::iterator       iterator_pqueries;
    typedef std::vector<Pquery>::const_iterator const_iterator_pqueries;

    iterator_pqueries       begin();
    const_iterator_pqueries begin() const;

    iterator_pqueries       end();
    const_iterator_pqueries end() const;

    // -------------------------------------------------------------------------
    //     Members
    // -------------------------------------------------------------------------

private:

    std::vector<Pquery> pqueries_;
    PlacementTree       tree_;

public:

    // There is not much to mess up here for a user, so we can simply make this public.
    // Okay, this is ugly. But for now, it's way easier. We never use the metadata anyway...
    std::unordered_map<std::string, std::string> metadata;

};

} // namespace placement
} // namespace genesis

#endif // include guard

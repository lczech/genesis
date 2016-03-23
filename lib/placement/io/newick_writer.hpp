#ifndef GENESIS_PLACEMENT_IO_NEWICK_WRITER_H_
#define GENESIS_PLACEMENT_IO_NEWICK_WRITER_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include <assert.h>
#include <stdexcept>
#include <string>
#include <vector>

#include "placement/function/helper.hpp"
#include "placement/placement_tree.hpp"
#include "placement/sample.hpp"
#include "tree/default/newick_writer.hpp"
#include "tree/io/newick/writer.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Placement Tree Newick Writer Mixin
// =================================================================================================

/**
 * @brief
 */
template <typename Base>
class PlacementTreeNewickWriterMixin : public Base
{
    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

public:

    typedef typename Base::TreeType TreeType;
    typedef typename Base::NodeType NodeType;
    typedef typename Base::EdgeType EdgeType;
    typedef typename Base::LinkType LinkType;

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

public:

    bool enable_edge_nums() const
    {
        return enable_edge_nums_;
    }

    void enable_edge_nums(bool value)
    {
        enable_edge_nums_ = value;
    }

    bool enable_placement_counts() const
    {
        return enable_placement_counts_;
    }

    void enable_placement_counts(bool value)
    {
        enable_placement_counts_ = value;
    }

    void prepare_sample( Sample const& smp )
    {
        auto place_map = placements_per_edge( smp );
        placement_counts_.resize( place_map.size(), 0 );

        for( auto const p : place_map ) {
            placement_counts_[ p.first ] = p.second.size();
        }
    }

    // -------------------------------------------------------------------------
    //     Overridden Member Functions
    // -------------------------------------------------------------------------

protected:

    virtual void edge_to_element( EdgeType const& edge, tree::NewickBrokerElement& element ) override
    {
        Base::edge_to_element(edge, element);

        if (enable_edge_nums_) {
            element.tags.push_back(std::to_string(edge.data.edge_num()));
        }
        if (enable_placement_counts_) {
            element.comments.push_back(std::to_string( placement_counts_[ edge.index() ]));
        }
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    bool enable_edge_nums_        = true;
    bool enable_placement_counts_ = false;

    std::vector<size_t> placement_counts_;
};

// =================================================================================================
//     Placement Tree Newick Writer
// =================================================================================================

typedef PlacementTreeNewickWriterMixin<
        tree::DefaultTreeNewickWriterMixin< tree::NewickWriter< PlacementTree > >
    >
    PlacementTreeNewickWriter;

} // namespace placement
} // namespace genesis

#endif // include guard

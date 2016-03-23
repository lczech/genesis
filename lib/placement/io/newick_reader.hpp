#ifndef GENESIS_PLACEMENT_IO_NEWICK_READER_H_
#define GENESIS_PLACEMENT_IO_NEWICK_READER_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include <assert.h>
#include <stdexcept>

#include "placement/placement_tree.hpp"
#include "tree/default/newick_reader.hpp"
#include "tree/io/newick/reader.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Placement Tree Newick Reader Mixin
// =================================================================================================

/**
 * @brief
 */
template <typename Base>
class PlacementTreeNewickReaderMixin : public Base
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
    //     Overridden Member Functions
    // -------------------------------------------------------------------------

protected:

    virtual void element_to_edge( tree::NewickBrokerElement const& element, EdgeType& edge ) override
    {
        Base::element_to_edge(element, edge);

        // Process the edge num.
        edge.data.reset_edge_num(-1);
        if (element.tags.size() == 0) {
            throw std::invalid_argument(
                "Edge at node '" + element.name + "' does not contain a tag value like '{42}'" +
                " for the placement edge_num of this edge."
            );
        }
        if (element.tags.size() > 1) {
            throw std::invalid_argument(
                "Edge at node '" + element.name + "' contains more than one tag value like " +
                "'{xyz}'. Expecting only one for the placement edge_num of this edge."
            );
        }
        assert(element.tags.size() == 1);
        edge.data.reset_edge_num( std::stoi( element.tags[0] ));
    }

};

// =================================================================================================
//     Placement Tree Newick Reader
// =================================================================================================

typedef PlacementTreeNewickReaderMixin<
        tree::DefaultTreeNewickReaderMixin< tree::NewickReader< PlacementTree > >
    >
    PlacementTreeNewickReader;

} // namespace placement
} // namespace genesis

#endif // include guard

#ifndef GENESIS_PLACEMENT_IO_NEWICK_PROCESSOR_H_
#define GENESIS_PLACEMENT_IO_NEWICK_PROCESSOR_H_

/**
 * @brief Header of Placement Tree Newick Processor class.
 *
 * @file
 * @ingroup placement
 */

#include <assert.h>
#include <stdexcept>
#include <string>

#include "placement/placement_tree.hpp"
#include "tree/default/newick_mixin.hpp"
#include "tree/io/newick/processor.hpp"

namespace genesis {

// =================================================================================================
//     Placement Tree Newick Processor
// =================================================================================================

/**
 * @brief
 */
class PlacementTreeNewickProcessor : public DefaultTreeNewickMixin< NewickProcessor< PlacementTree > >
{
    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

public:

    typedef DefaultTreeNewickMixin< NewickProcessor< PlacementTree > > Base;

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

public:

    bool print_edge_nums() const
    {
        return print_edge_nums_;
    }

    void print_edge_nums(bool value)
    {
        print_edge_nums_ = value;
    }

    bool print_placement_counts() const
    {
        return print_placement_counts_;
    }

    void print_placement_counts(bool value)
    {
        print_placement_counts_ = value;
    }

    // -------------------------------------------------------------------------
    //     Overridden Member Functions
    // -------------------------------------------------------------------------

protected:

    virtual void element_to_edge( NewickBrokerElement const& element, EdgeType& edge ) override
    {
        Base::element_to_edge(element, edge);

        // Process the edge num.
        edge.data.edge_num      = -1;
        if (element.tags.size() == 0) {
            throw std::invalid_argument(
                "Edge for element '" + element.name + "' does not contain a tag value like '[42]'" +
                " for the placement edge_num of this edge."
            );
        }
        if (element.tags.size() > 1) {
            throw std::invalid_argument(
                "Edge for element '" + element.name + "' contains more than one tag value like " +
                "'[xyz]'."
            );
        }
        assert(element.tags.size() == 1);
        edge.data.edge_num = std::stoi(element.tags[0]);
    }

    virtual void edge_to_element( EdgeType const& edge, NewickBrokerElement& element ) override
    {
        Base::edge_to_element(edge, element);

        if (print_edge_nums_) {
            element.tags.push_back(std::to_string(edge.data.edge_num));
        }
        if (print_placement_counts_) {
            element.comments.push_back(std::to_string(edge.data.placement_count()));
        }
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    bool print_edge_nums_        = true;
    bool print_placement_counts_ = false;
};

} // namespace genesis

#endif // include guard

#ifndef GNS_PLACEMENT_PLACEMENTTREE_H_
#define GNS_PLACEMENT_PLACEMENTTREE_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include <deque>
#include <string>

#include "tree/newick_processor.hpp"
#include "utils/logging.hpp"

namespace genesis {

// =============================================================================
//     Forward Declarations
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class Tree;

struct Pquery;

class PlacementEdgeDataIteratorPlacements;

// =============================================================================
//     PlacementEdgeData
// =============================================================================

// TODO maybe add a Validate function here, that checks whether at least one placement of each
// TODO pquerie actually points to this edge. this function could be called from tree validate.
// TODO see Placements::Validate()

class PlacementEdgeData
{
public:

    // -----------------------------------------------------
    //     Class Functions
    // -----------------------------------------------------

    inline bool operator == (const PlacementEdgeData &other) const
    {
        // TODO add a comparison of pqueries as well ?! is that good?!
        return other.branch_length == branch_length && other.edge_num == edge_num;
    }

    inline bool operator != (const PlacementEdgeData &other) const
    {
        return !(other == *this);
    }

    // -----------------------------------------------------
    //     Default Functions
    // -----------------------------------------------------

    inline void FromNewickBrokerElement (NewickBrokerElement* node)
    {
        branch_length = node->branch_length;
        edge_num      = -1;

        if (node->tags.size() != 1) {
            LOG_WARN << "Edge for node '" << node->name << "' does not contain the single "
                     << "tag value denoting the edge_num for placements.";
            return;
        }
        edge_num = std::stoi(node->tags[0]);
    }

    inline void ToNewickBrokerElement (NewickBrokerElement* node) const
    {
        node->branch_length = branch_length;
    }

    inline std::string Dump() const
    {
        return "Length: " + std::to_string(branch_length) + "\tEdge Num: " + std::to_string(edge_num);
    }

    // -----------------------------------------------------
    //     Iterator
    // -----------------------------------------------------

    typedef PlacementEdgeDataIteratorPlacements IteratorPlacements;

    // the definitions are outsourced into the implementation file to avoid circular dependencies
    IteratorPlacements BeginPlacements(int sort = 0);
    IteratorPlacements EndPlacements();

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    double PlacementMass();

    double branch_length;
    int    edge_num;

    std::deque<Pquery*> pqueries;
};

// =============================================================================
//     PlacementNodeData
// =============================================================================

class PlacementNodeData
{
public:

    // -----------------------------------------------------
    //     Class Functions
    // -----------------------------------------------------

    inline bool operator == (const PlacementNodeData &other) const
    {
        return other.name == name;
    }

    inline bool operator != (const PlacementNodeData &other) const
    {
        return !(other == *this);
    }

    // -----------------------------------------------------
    //     Default Functions
    // -----------------------------------------------------

    inline void FromNewickBrokerElement (NewickBrokerElement* node)
    {
        name = node->name;
    }

    inline void ToNewickBrokerElement (NewickBrokerElement* node) const
    {
        node->name = name;
    }

    inline std::string Dump() const
    {
        return "Name: '" + name + "'";
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    /**
     * Name of the node. In case it is a leaf, this is usually the name of
     * the taxon represented by the node.
     */
    std::string name;
};

// =============================================================================
//     Definitions and Typedefs
// =============================================================================

// let's avoid tedious names!
typedef Tree<PlacementNodeData, PlacementEdgeData> PlacementTree;

} // namespace genesis

// =============================================================================
//     Edge Data Iterator Placements
// =============================================================================

#include "placement/placements.hpp"
#include "tree/tree.hpp"

namespace genesis {

class PlacementEdgeDataIteratorPlacements
{
public:
    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    typedef PlacementEdgeDataIteratorPlacements self_type;
    typedef std::forward_iterator_tag           iterator_category;
    typedef Pquery::Placement                   value_type;
    typedef Pquery::Placement&                  reference;
    typedef Pquery::Placement*                  pointer;

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    PlacementEdgeDataIteratorPlacements (PlacementEdgeData& edge_data, int sort = 0);

    PlacementEdgeDataIteratorPlacements (PlacementTree::EdgeType* edge, int sort = 0)
    {
        PlacementEdgeDataIteratorPlacements(edge->data, sort);
    }

    PlacementEdgeDataIteratorPlacements () {}

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    inline self_type operator ++ ()
    {
        placements_.pop_front();
        return *this;
    }

    inline self_type operator ++ (int)
    {
        self_type tmp = *this;
        ++(*this);
        return tmp;
    }

    /**
     * @brief Equality operator for the iterator.
     *
     * It checks whether both iterators have the same number of remaining Placements and whether the
     * current Placement is the same. For speed reasons, it does not check for the rest of the
     * elements, because it would be a strange case to compare different types of iterators anyway.
     */
    inline bool operator == (const self_type &other) const
    {
        if (placements_.size() != other.placements_.size()) {
            return false;
        }
        if (placements_.size() > 0) {
            return placements_.front() == other.placements_.front();
        } else {
            return true;
        }
    }

    inline bool operator != (const self_type &other) const
    {
        return !(other == *this);
    }

    inline reference operator * ()
    {
        return *placements_.front();
    }

    inline pointer operator -> ()
    {
        return placements_.front();
    }

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

protected:
    std::deque<Pquery::Placement*> placements_;
};

} // namespace genesis

#endif // include guard

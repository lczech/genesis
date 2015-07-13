#ifndef GENESIS_TREE_DEFAULT_TREE_H_
#define GENESIS_TREE_DEFAULT_TREE_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <string>

#include "tree/newick_broker.hpp"

namespace genesis {

// =============================================================================
//     Forward declarations
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class  Tree;

template <class NodeDataType, class EdgeDataType>
class  TreeEdge;

template <class NodeDataType, class EdgeDataType>
class  TreeLink;

template <class NodeDataType, class EdgeDataType>
class  TreeNode;

// =============================================================================
//     Default Tree Node Data
// =============================================================================

/**
 * @brief Base class containing the commonly needed data for tree nodes.
 *
 * The Tree class takes two template parameters in order to determine what kind of data it stores:
 * one parameter for the node data, and one for the edge data. In most cases, nodes will contain
 * the node's name and edges will contain a branch length.
 *
 * This class is a base for node data, and can be derived from if a node name is wanted. It is
 * however not necessary -- so if there is no need for node names, you can provide your own,
 * independent class as template parameter for tree.
 *
 * For reasons of speed, this class contains no virtual functions. It is still valid, as we never
 * do any upcasting when working with the tree: All types are known at compile time (as they are
 * given as fixed template parameters).
 *
 * However, slicing can happen, for example when the
 * operator == is called on some derived class. Then, both objects are sliced, so that all that
 * is left of them is the data of the base class. Thus, it is still a valid operation.
 * It simply results in comparing the base data members (currently, the node name only) to each
 * other, while all additional data of the derived class is neglected. In consequence, if you don't
 * want those extra data members to influence the outcome of the comparision, you do not need to
 * rewrite this function. Only if you want to compare this data as well, the function has to be
 * overridden.
 */
class DefaultTreeNodeData
{
public:

    // -----------------------------------------------------
    //     Class Functions
    // -----------------------------------------------------

    inline bool operator == (const DefaultTreeNodeData &other) const
    {
        return other.name == name;
    }

    inline bool operator != (const DefaultTreeNodeData &other) const
    {
        return !(other == *this);
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    inline std::string dump() const
    {
        return "Name: '" + name + "'";
    }

    /**
     * Name of the node. In case it is a leaf, this is usually the name of
     * the taxon represented by the node.
     */
    std::string name;

};

// =============================================================================
//     Default Tree Edge Data
// =============================================================================

/**
 * @brief Base class containing the commonly needed data for tree edges.
 *
 * This class is the equivalent of DefaultTreeNodeData for the tree edges. It stores a branch length
 * for each edge. For more information, see DefaultTreeNodeData.
 *
 * If you for example do not need a single branch length, but multiple ones (e.g., when working with
 * partitions), simply do not derive from this class, but provide your own implementation.
 */
class DefaultTreeEdgeData
{
public:

    // -----------------------------------------------------
    //     Class Functions
    // -----------------------------------------------------

    inline bool operator == (const DefaultTreeEdgeData &other) const
    {
        return other.branch_length == branch_length;
    }

    inline bool operator != (const DefaultTreeEdgeData &other) const
    {
        return !(other == *this);
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    inline std::string dump() const
    {
        return "Length: " + std::to_string(branch_length);
    }

    double branch_length;

};

// =============================================================================
//     Definitions and Typedefs
// =============================================================================

/**
 * @brief Short name for a Tree using the default data types DefaultTreeNodeData and
 * DefaultTreeEdgeData.
 *
 * The same type can also be produced by using `Tree<>`.
 */
typedef Tree     <DefaultTreeNodeData, DefaultTreeEdgeData> DefaultTree;

typedef TreeEdge <DefaultTreeNodeData, DefaultTreeEdgeData> DefaultTreeEdge;
typedef TreeLink <DefaultTreeNodeData, DefaultTreeEdgeData> DefaultTreeLink;
typedef TreeNode <DefaultTreeNodeData, DefaultTreeEdgeData> DefaultTreeNode;

} // namespace genesis

#endif // include guard

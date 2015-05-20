#ifndef GENESIS_TREE_TREESET_H_
#define GENESIS_TREE_TREESET_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <string>
#include <unordered_map>

namespace genesis {

template <class NodeDataType, class EdgeDataType>
class TreeSet
{
public:

    // -----------------------------------------------------
    //     Constructor and Typedefs
    // -----------------------------------------------------

    // avoid cumbersome names!
    typedef Tree<NodeDataType, EdgeDataType>          TreeType;
    typedef std::unordered_map<std::string, TreeType> MapType;

    // -----------------------------------------------------
    //     Tree Set Methods
    // -----------------------------------------------------

    void AddTree(const std::string& name, TreeType& tree);

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

private:
    // map from name of a tree to tree object.
    MapType map_;

};

} // namespace genesis

// =============================================================================
//     Inclusion of the implementation
// =============================================================================

// This is a class template, so do the inclusion here.
#include "tree/tree_set.tpp"

#endif // include guard

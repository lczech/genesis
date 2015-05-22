#ifndef GENESIS_TREE_TREESET_H_
#define GENESIS_TREE_TREESET_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <memory>
#include <string>
#include <utility>
#include <vector>

//~ #include "tree/tree.hpp"

namespace genesis {

// =============================================================================
//     Forward declarations
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class  Tree;

// =============================================================================
//     Tree Set
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class TreeSet
{
public:

    // -----------------------------------------------------
    //     Constructor and Typedefs
    // -----------------------------------------------------

    // avoid cumbersome names!
    typedef Tree<NodeDataType, EdgeDataType>                  TreeType;
    typedef std::pair<std::string, std::unique_ptr<TreeType>> PairType;

    // -----------------------------------------------------
    //     Modifiers
    // -----------------------------------------------------

    void Add (const std::string& name, TreeType* tree);

    // -----------------------------------------------------
    //     Accessors
    // -----------------------------------------------------

    TreeType* GetFirst(const std::string& name);

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

private:
    // We use a vector of <string, tree> pairs here, because we want to preserve the order in which
    // elements were inserted into the TreeMap. This is not the case with simple maps.
    std::vector<PairType> trees_;
};

} // namespace genesis

// =============================================================================
//     Inclusion of the implementation
// =============================================================================

// This is a class template, so do the inclusion here.
#include "tree/tree_set.tpp"

#endif // include guard

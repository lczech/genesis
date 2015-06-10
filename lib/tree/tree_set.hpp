#ifndef GENESIS_TREE_TREE_SET_H_
#define GENESIS_TREE_TREE_SET_H_

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

template <class TreeType>
class TreeSet
{
public:

    // -----------------------------------------------------
    //     Constructor and Typedefs
    // -----------------------------------------------------

    struct NamedTree
    {
        std::string               name;
        std::unique_ptr<TreeType> tree;
    };

    typedef typename std::vector<NamedTree>::iterator       iterator;
    typedef typename std::vector<NamedTree>::const_iterator const_iterator;

    // -----------------------------------------------------
    //     Modifiersy
    // -----------------------------------------------------

    void add (const std::string& name, TreeType* tree);

    // -----------------------------------------------------
    //     Accessors
    // -----------------------------------------------------

    TreeType* get_first (const std::string& name);

    inline iterator begin()
    {
        return trees_.begin();
    }

    inline iterator end()
    {
        return trees_.end();
    }

    inline const_iterator cbegin() const
    {
        return trees_.cbegin();
    }

    inline const_iterator cend() const
    {
        return trees_.cend();
    }

    inline NamedTree& operator [] (const std::size_t index) const
    {
        return trees_[index];
    }

    /**
     * @brief Returns whether the tree set is empty.
     */
    inline bool empty() const
    {
        return trees_.empty();
    }

    /**
     * @brief Returns the size of the tree set.
     */
    inline size_t size() const
    {
        return trees_.size();
    }

    // -----------------------------------------------------
    //     Dump & Debug
    // -----------------------------------------------------

    std::string dump (bool full = false);

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

private:

    // We use a vector of <string, tree> pairs here, because we want to preserve the order in which
    // elements were inserted into the TreeMap. This is not the case with simple maps.
    std::vector<NamedTree> trees_;
};

} // namespace genesis

// =============================================================================
//     Inclusion of the implementation
// =============================================================================

// This is a class template, so do the inclusion here.
#include "tree/tree_set.tpp"

#endif // include guard

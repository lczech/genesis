#ifndef GENESIS_TREE_ITERATOR_NODES_H_
#define GENESIS_TREE_ITERATOR_NODES_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <iterator>

namespace genesis {
namespace tree {

// =================================================================================================
//     Nodes Iterator
// =================================================================================================

template <typename TreeType>
class TreeIteratorNodes
{
public:
    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    typedef TreeIteratorNodes<TreeType> self_type;
    typedef std::forward_iterator_tag iterator_category;

    typedef typename TreeType::NodeContainer container_type;
    typedef typename TreeType::NodeType& reference;
    typedef typename TreeType::NodeType* pointer;

    // typedef NodeType  value_type;
    // typedef size_t    difference_type;
    // typedef NodeType* pointer;
    // typedef NodeType& reference;

    reference operator * ()
    {
        return **it_;
    }

    pointer operator -> ()
    {
        return it_->get();
    }

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    TreeIteratorNodes (container_type::iterator it) : it_(it)
    {}

    // -----------------------------------------------------
    //     Iteration
    // -----------------------------------------------------

    // self_type begin()
    // {
    //     return self_type(start_);
    // }
    //
    // self_type end()
    // {
    //     return self_type(nullptr);
    // }

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    self_type& operator ++ ()
    {
        ++it_;
        return *this;
    }

    self_type operator ++ (int)
    {
        self_type tmp = *this;
        ++(*this);
        return tmp;
    }

    bool operator == (const self_type &other) const
    {
        return other.it_ == it_;
    }

    bool operator != (const self_type &other) const
    {
        return !(other == *this);
    }

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

private:
    container_type::iterator it_;
};

} // namespace tree
} // namespace genesis

#endif // include guard

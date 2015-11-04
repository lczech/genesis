#ifndef GENESIS_TREE_ITERATORS_NODES_H_
#define GENESIS_TREE_ITERATORS_NODES_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <iterator>

namespace genesis {

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

    // TODO !!!!!!!!!!!!!!!!!!!!!!!!!!! delete all copy and move stuff!!!

    typedef TreeIteratorNodes<TreeType> self_type;
    typedef std::forward_iterator_tag iterator_category;

    typedef typename TreeType::NodeContainer container_type;
    typedef typename TreeType::NodeType& reference;
    typedef typename TreeType::NodeType* pointer;

    // typedef NodeType  value_type;
    // typedef size_t    difference_type;
    // typedef NodeType* pointer;
    // typedef NodeType& reference;

    inline reference operator * ()
    {
        return **it_;
    }

    inline pointer operator -> ()
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

    // inline self_type begin()
    // {
    //     return self_type(start_);
    // }
    //
    // inline self_type end()
    // {
    //     return self_type(nullptr);
    // }

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    inline self_type& operator ++ ()
    {
        ++it_;
        return *this;
    }

    inline self_type operator ++ (int)
    {
        self_type tmp = *this;
        ++(*this);
        return tmp;
    }

    inline bool operator == (const self_type &other) const
    {
        return other.it_ == it_;
    }

    inline bool operator != (const self_type &other) const
    {
        return !(other == *this);
    }

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

private:
    container_type::iterator it_;
};

} // namespace genesis

#endif // include guard

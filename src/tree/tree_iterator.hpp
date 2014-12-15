#ifndef GNS_TREE_TREEITERATOR_H_
#define GNS_TREE_TREEITERATOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

namespace genesis {

// =============================================================================
//     Forward Declarations
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class  Tree;

template <class NodeDataType, class EdgeDataType>
class  TreeLink;

template <class NodeDataType, class EdgeDataType>
class TreeNode;

template <class NodeDataType, class EdgeDataType>
class TreeEdge;

// =============================================================================
//     Round Trip Iterator
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class TreeIteratorRoundtrip
{
public:
    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    typedef TreeIteratorRoundtrip<NodeDataType, EdgeDataType> self_type;
    typedef std::forward_iterator_tag                         iterator_category;
    //~ typedef int                                               difference_type;

    typedef Tree<NodeDataType, EdgeDataType>                  tree_type;
    typedef TreeLink<NodeDataType, EdgeDataType>              value_type;
    typedef TreeLink<NodeDataType, EdgeDataType>&             reference;
    typedef TreeLink<NodeDataType, EdgeDataType>*             pointer;

    // -----------------------------------------------------
    //     Constructors
    // -----------------------------------------------------

    TreeIteratorRoundtrip (tree_type* tree) : tree_(tree)
    {
        link_ = tree_->links_.front();
    }

    TreeIteratorRoundtrip (tree_type*  tree, value_type* link) : tree_(tree), link_(link) {}

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    self_type operator++ ()
    {
        link_ = link_->next_->outer_;
        return link_;
    }

    self_type operator++ (int)
    {
        link_ = link_->next_->outer_;
        return link_;
    }

protected:
    tree_type*  tree_;
    value_type* link_;
};

/*
 * http://accu.org/index.php/journals/389
 * https://gist.github.com/jeetsukumaran/307264
 *
 *
class iterator
{
public:
self_type operator++() { self_type i = *this; ptr_++; return i; }
self_type operator++(int junk) { ptr_++; return *this; }
reference operator*() { return *ptr_; }
pointer operator->() { return ptr_; }
bool operator==(const self_type& rhs) { return ptr_ == rhs.ptr_; }
bool operator!=(const self_type& rhs) { return ptr_ != rhs.ptr_; }
private:
pointer ptr_;
};
* */

} // namespace genesis

#endif // include guard

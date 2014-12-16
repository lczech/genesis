#ifndef GNS_TREE_TREEITERATOR_H_
#define GNS_TREE_TREEITERATOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

namespace genesis {

// TODO all iterators: http://en.wikipedia.org/wiki/Tree_traversal

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
    typedef TreeLink<NodeDataType, EdgeDataType>              value_type;
    typedef TreeLink<NodeDataType, EdgeDataType>&             reference;
    typedef TreeLink<NodeDataType, EdgeDataType>*             pointer;

    // -----------------------------------------------------
    //     Constructors
    // -----------------------------------------------------

    TreeIteratorRoundtrip (value_type* link) : link_(link), start_(link)
    {}

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    inline self_type operator ++ ()
    {
        link_ = link_->Next()->Outer();
        if (link_ == start_) {
            link_ = nullptr;
        }
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
        return other.link_ == link_;
    }

    inline bool operator != (const self_type &other) const
    {
        return !(other == *this);
    }

    inline reference operator * ()
    {
        return *link_;
    }

    inline pointer operator -> ()
    {
        return link_;
    }

    // -----------------------------------------------------
    //     Member Variables
    // -----------------------------------------------------

protected:
    value_type* link_;
    value_type* start_;
};

/*
 * http://accu.org/index.php/journals/389
 * https://gist.github.com/jeetsukumaran/307264
 *
 * http://www.cplusplus.com/reference/iterator/iterator/
 *
 * http://www.cs.northwestern.edu/~riesbeck/programming/c++/stl-iterators.html
 * http://www.cs.northwestern.edu/~riesbeck/programming/c++/stl-iterator-define.html
 *
 * http://www.cs.northwestern.edu/~riesbeck/programming/c++/stl-algorithms.html
 */

} // namespace genesis

#endif // include guard

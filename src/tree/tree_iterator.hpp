#ifndef GNS_TREE_TREEITERATOR_H_
#define GNS_TREE_TREEITERATOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <assert.h>
#include <deque>

#include "tree/tree_link.hpp"
#include "tree/tree_node.hpp"
#include "tree/tree_edge.hpp"

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
    typedef TreeLink<NodeDataType, EdgeDataType>              value_type;
    typedef TreeLink<NodeDataType, EdgeDataType>&             reference;
    typedef TreeLink<NodeDataType, EdgeDataType>*             pointer;

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    TreeIteratorRoundtrip (value_type* link) : link_(link), start_(link)
    {}

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    // FIXME if we start this iterator on a non-root node, this node is not included again
    // at the end of the trip (which is done for the root). maybe also use a stack?!
    inline self_type& operator ++ ()
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
    //     Members
    // -----------------------------------------------------

    inline TreeLink<NodeDataType, EdgeDataType>* Link()
    {
        return link_;
    }

    inline TreeNode<NodeDataType, EdgeDataType>* Node()
    {
        return link_->Node();
    }

    inline TreeEdge<NodeDataType, EdgeDataType>* Edge()
    {
        return link_->Edge();
    }

protected:
    value_type* link_;
    value_type* start_;
};

// =============================================================================
//     Preorder Iterator
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class TreeIteratorPreorder
{
public:
    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    typedef TreeIteratorPreorder<NodeDataType, EdgeDataType> self_type;
    typedef std::forward_iterator_tag                        iterator_category;
    typedef TreeNode<NodeDataType, EdgeDataType>             value_type;
    typedef TreeNode<NodeDataType, EdgeDataType>&            reference;
    typedef TreeNode<NodeDataType, EdgeDataType>*            pointer;

    typedef TreeLink<NodeDataType, EdgeDataType>*            link_pointer;

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    TreeIteratorPreorder (link_pointer link) : link_(link), start_(link)
    {
        if (link) {
            PushFrontChildren(link);
            if (link->Outer() != link) {
                stack_.push_front(link->Outer());
            }
        }
    }

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    inline self_type operator ++ ()
    {
        if (stack_.empty()) {
            link_ = nullptr;
        } else {
            link_ = stack_.front();
            stack_.pop_front();
            PushFrontChildren(link_);
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
        return *(link_->Node());
    }

    inline pointer operator -> ()
    {
        return link_->Node();
    }

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    inline TreeLink<NodeDataType, EdgeDataType>* Link()
    {
        return link_;
    }

    inline TreeNode<NodeDataType, EdgeDataType>* Node()
    {
        return link_->Node();
    }

    inline TreeEdge<NodeDataType, EdgeDataType>* Edge()
    {
        if (link_ == start_) {
            return nullptr;
        }
        return link_->Edge();
    }

protected:
    inline void PushFrontChildren(link_pointer link)
    {
        // we need to push to a tmp queue first, in order to get the order right.
        // otherwise, we would still do a preorder traversal, but starting with
        // the last child of each node instead of the first one.
        std::deque<link_pointer> tmp;
        link_pointer c = link->Next();
        while (c != link) {
            if (c->Outer() != c) {
                tmp.push_front(c->Outer());
            }
            c = c->Next();
        }
        for (link_pointer l : tmp) {
            stack_.push_front(l);
        }
    }

    link_pointer             link_;
    link_pointer             start_;
    std::deque<link_pointer> stack_;
};

// =============================================================================
//     Postorder Iterator
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class TreeIteratorPostorder
{
public:
    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    typedef TreeIteratorPostorder<NodeDataType, EdgeDataType> self_type;
    typedef std::forward_iterator_tag                         iterator_category;
    typedef TreeNode<NodeDataType, EdgeDataType>              value_type;
    typedef TreeNode<NodeDataType, EdgeDataType>&             reference;
    typedef TreeNode<NodeDataType, EdgeDataType>*             pointer;

    typedef TreeLink<NodeDataType, EdgeDataType>*             link_pointer;

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    TreeIteratorPostorder (link_pointer link) : start_(link)
    {
        if (link) {
            stack_.push_back(link);
            if (link->Outer() != link) {
                stack_.push_front(link->Outer());
                link = link->Outer();
            }
            while (link->IsInner()) {
                PushFrontChildren(link);
                link = link->Next()->Outer();
            }
            assert(link == stack_.front());
            stack_.pop_front();
        }
        link_ = link;
    }

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    inline self_type operator ++ ()
    {
        if (stack_.empty()) {
            link_ = nullptr;
        } else if (link_->Outer()->Next() == stack_.front()) {
            link_ = stack_.front();
            stack_.pop_front();
        } else {
            link_ = stack_.front();
            while (link_->IsInner()) {
                PushFrontChildren(link_);
                link_ = link_->Next()->Outer();
            }
            assert(link_ == stack_.front());
            stack_.pop_front();
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
        return *(link_->Node());
    }

    inline pointer operator -> ()
    {
        return link_->Node();
    }

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    inline TreeLink<NodeDataType, EdgeDataType>* Link()
    {
        return link_;
    }

    inline TreeNode<NodeDataType, EdgeDataType>* Node()
    {
        return link_->Node();
    }

    inline TreeEdge<NodeDataType, EdgeDataType>* Edge()
    {
        if (link_ == start_) {
            return nullptr;
        }
        return link_->Edge();
    }

protected:
    inline void PushFrontChildren(link_pointer link)
    {
        // we need to push to a tmp queue first, in order to get the order right.
        // otherwise, we would still do a postorder traversal, but starting with
        // the last child of each node instead of the first one.
        std::deque<link_pointer> tmp;
        link_pointer c = link->Next();
        while (c != link) {
            if (c->Outer() != c) {
                tmp.push_front(c->Outer());
            }
            c = c->Next();
        }
        for (link_pointer l : tmp) {
            stack_.push_front(l);
        }
    }

    link_pointer             link_;
    link_pointer             start_;
    std::deque<link_pointer> stack_;
};

// =============================================================================
//     Levelorder Iterator
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class TreeIteratorLevelorder
{
public:
    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    typedef TreeIteratorLevelorder<NodeDataType, EdgeDataType> self_type;
    typedef std::forward_iterator_tag                          iterator_category;
    typedef TreeNode<NodeDataType, EdgeDataType>               value_type;
    typedef TreeNode<NodeDataType, EdgeDataType>&              reference;
    typedef TreeNode<NodeDataType, EdgeDataType>*              pointer;

    typedef TreeLink<NodeDataType, EdgeDataType>*              link_pointer;

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    TreeIteratorLevelorder (link_pointer link) : start_(link)
    {
        if (link) {
            PushBackChildren(link);
            if (link->Outer() != link) {
                stack_.push_front(link->Outer());
            }
        }
        link_ = link;
    }

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    inline self_type operator ++ ()
    {
        if (stack_.empty()) {
            link_ = nullptr;
        } else {
            link_ = stack_.front();
            stack_.pop_front();
            PushBackChildren(link_);
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
        return *(link_->Node());
    }

    inline pointer operator -> ()
    {
        return link_->Node();
    }

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    inline TreeLink<NodeDataType, EdgeDataType>* Link()
    {
        return link_;
    }

    inline TreeNode<NodeDataType, EdgeDataType>* Node()
    {
        return link_->Node();
    }

    inline TreeEdge<NodeDataType, EdgeDataType>* Edge()
    {
        if (link_ == start_) {
            return nullptr;
        }
        return link_->Edge();
    }

protected:
    inline void PushBackChildren(link_pointer link)
    {
        link_pointer c = link->Next();
        while (c != link) {
            if (c->Outer() != c) {
                stack_.push_back(c->Outer());
            }
            c = c->Next();
        }
    }

    link_pointer             link_;
    link_pointer             start_;
    std::deque<link_pointer> stack_;
};

} // namespace genesis

#endif // include guard

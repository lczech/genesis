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
    typedef const TreeLink<NodeDataType, EdgeDataType>*       link_pointer;

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    TreeIteratorRoundtrip (link_pointer link) : link_(link), start_(link)
    {}

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    inline self_type& operator ++ ()
    {
        link_ = link_->Outer()->Next();
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

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    inline link_pointer Link() const
    {
        return link_;
    }

    inline TreeNode<NodeDataType, EdgeDataType>* Node() const
    {
        return link_->Node();
    }

    inline TreeEdge<NodeDataType, EdgeDataType>* Edge() const
    {
        return link_->Edge();
    }

    inline link_pointer StartLink() const
    {
        return start_;
    }

    inline TreeNode<NodeDataType, EdgeDataType>* StartNode() const
    {
        return start_->Node();
    }

protected:
    link_pointer link_;
    link_pointer start_;
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
    typedef const TreeLink<NodeDataType, EdgeDataType>*      link_pointer;

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    TreeIteratorPreorder (link_pointer link) : link_(link), start_(link)
    {
        if (link) {
            PushFrontChildren(link);
            stack_.push_front(link->Outer());
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

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    inline bool IsFirstIteration() const
    {
        return link_ == start_;
    }

    inline link_pointer Link() const
    {
        return link_;
    }

    inline TreeNode<NodeDataType, EdgeDataType>* Node() const
    {
        return link_->Node();
    }

    inline TreeEdge<NodeDataType, EdgeDataType>* Edge() const
    {
        return link_->Edge();
    }

    inline link_pointer StartLink() const
    {
        return start_;
    }

    inline TreeNode<NodeDataType, EdgeDataType>* StartNode() const
    {
        return start_->Node();
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
            tmp.push_front(c->Outer());
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
//     Inorder Iterator
// =============================================================================

/* *
 * @brief Iterates over a Tree in inorder manner.
 *
 * This iterator usually is used on binary trees and started at the root. As we are dealing with
 * (possibly) multifurcating (non-binary) trees, and also might start this iterator on some inner
 * node, it behaves a bit different from its standard mode of operation:
 *
 *  *  In multifurcating trees, inner nodes will be visited more than once, namely their number of
 *     neighbors - 2 many times. This is simply a consequence of the multifurcation. In a binary
 *     tree, each node has three neighbors, so is visited 3-2=1 many times, as usual, with notable
 *     exceptions:
 *  *  In an unrooted binary tree, the "virtual root" (meaning, the node that is used as a starting
 *     point into the tree) usually has three children, as it actually is a normal inner node that
 *     just happened to be the first one in memory. Thus, when starting this iterator at this kind
 *     of root, the root node will be visited twice during the traversal. Again, this is a
 *     consequence of the mechanism of inorder traversal.
 *  *  The same is true when the iterator is started on some other inner node (as they are
 *     topologically the same as the "unrooted" root node).
 *  *  A special side-effect of this implementation: If the iterator is started on a leaf (unusual,
 *     but can be done), this node is visited as the very first one.
 * %
 * /
template <class NodeDataType, class EdgeDataType>
class TreeIteratorInorder
{
public:
    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    typedef TreeIteratorInorder<NodeDataType, EdgeDataType> self_type;
    typedef std::forward_iterator_tag                       iterator_category;
    typedef const TreeLink<NodeDataType, EdgeDataType>*     link_pointer;

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    TreeIteratorInorder (link_pointer link) : start_(link)
    {
        // the end iterator is created by handing over a nullptr to this constructor, so we first
        // need to check for this:
        if (link) {
            link_ = link;

            // if we start the iterator on an inner node, we actually need to add ALL children
            // (which means all its neighbor nodes) of this node to the stack, not only the ones
            // that point away from the given link of the node (which is what PushFrontChildren
            // does). this means, we also need to add the one link that is given as argument here.
            if (link->Outer() != link) {
                // this loop just takes care to add the given node first, and then the other
                // children. this is more or less beaty only, as it just makes sure that this link's
                // path is taken first, instead of last.
                while (link->Next() != link_) {
                    link = link->Next();
                }

                // now add the needed extra child
                stack_.push_front(link->Outer());
            }

            // in any case (no matter whether we start at the root or some other node), we run this
            // loop. it goes down the tree until a leaf is encountered. this leaf will be the first
            // node that the iterator points to. while going there, we add yet-to-be-visited nodes
            // to the stack.
            while (link->IsInner()) {
                PushFrontChildren(link);
                link = link->Next()->Outer();
            }
        }
        link_ = link;
    }

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

// TODO Tree iterator inorder is NOT WORKING!!!

    inline self_type operator ++ ()
    {
        std::string m = "  ";
        if (link_) {
           m += "@" + link_->Node()->name + "   ";
        }
        for (link_pointer p : stack_) {
            m += p->Node()->name + " ";
        }
        LOG_DBG2 << m;

        if (link_ == stack_.front()) {
            while (!stack_.empty() && link_ == stack_.front()) {
                link_ = link_->Outer()->Next();
                stack_.pop_front();
            }
            while (link_->Outer() == link_) {
                link_ = link_->Next();
            }
            if (stack_.empty()) {
                link_ = nullptr;
            }
        } else {
            assert(link_->Outer() == stack_.front());
            link_ = link_->Outer();
            while (link_->IsInner()) {
                PushFrontChildren(link_);
                link_ = link_->Next()->Outer();
            }
            while (link_->Outer() == link_) {
                link_ = link_->Next();
            }
        }

        m = "  ";
        if (link_) {
           m += "@" + link_->Node()->name + "   ";
        }
        for (link_pointer p : stack_) {
            m += p->Node()->name + " ";
        }
        LOG_DBG2 << m;

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

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    inline link_pointer Link() const
    {
        return link_;
    }

    inline TreeNode<NodeDataType, EdgeDataType>* Node() const
    {
        return link_->Node();
    }

    inline TreeEdge<NodeDataType, EdgeDataType>* Edge() const
    {
        return link_->Edge();
    }

    inline link_pointer StartLink() const
    {
        return start_;
    }

    inline TreeNode<NodeDataType, EdgeDataType>* StartNode() const
    {
        return start_->Node();
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
            tmp.push_front(c->Outer());
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

*/

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
    typedef const TreeLink<NodeDataType, EdgeDataType>*       link_pointer;

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    TreeIteratorPostorder (link_pointer link) : start_(link)
    {
        if (link) {
            stack_.push_back(link);
            stack_.push_front(link->Outer());
            link = link->Outer();
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
            // this condition marks the end of the traversal
            link_ = nullptr;
        } else if (link_->Outer()->Next() == stack_.front()) {
            // this condition is active when seeing an inner node the last time,
            // meaning that it is its turn to be traversed
            link_ = stack_.front();
            stack_.pop_front();
        } else {
            // this condition is active in all other cases: going down the tree towards the tips
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

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    inline bool IsLastIteration() const
    {
        return link_ == start_;
    }

    inline link_pointer Link() const
    {
        return link_;
    }

    inline TreeNode<NodeDataType, EdgeDataType>* Node() const
    {
        return link_->Node();
    }

    inline TreeEdge<NodeDataType, EdgeDataType>* Edge() const
    {
        return link_->Edge();
    }

    inline link_pointer StartLink() const
    {
        return start_;
    }

    inline TreeNode<NodeDataType, EdgeDataType>* StartNode() const
    {
        return start_->Node();
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
            tmp.push_front(c->Outer());
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
    typedef const TreeLink<NodeDataType, EdgeDataType>*        link_pointer;

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    TreeIteratorLevelorder (link_pointer link) : link_(link), start_(link)
    {
        if (link) {
            PushBackChildren(link);
            stack_.push_front(link->Outer());
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

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    inline bool IsFirstIteration() const
    {
        return link_ == start_;
    }

    inline link_pointer Link() const
    {
        return link_;
    }

    inline TreeNode<NodeDataType, EdgeDataType>* Node() const
    {
        return link_->Node();
    }

    inline TreeEdge<NodeDataType, EdgeDataType>* Edge() const
    {
        return link_->Edge();
    }

    inline link_pointer StartLink() const
    {
        return start_;
    }

    inline TreeNode<NodeDataType, EdgeDataType>* StartNode() const
    {
        return start_->Node();
    }

protected:
    inline void PushBackChildren(link_pointer link)
    {
        link_pointer c = link->Next();
        while (c != link) {
            stack_.push_back(c->Outer());
            c = c->Next();
        }
    }

    link_pointer             link_;
    link_pointer             start_;
    std::deque<link_pointer> stack_;
};

// =============================================================================
//     Path Iterator
// =============================================================================

/*

template <class NodeDataType, class EdgeDataType>
class TreeIteratorPath
{
public:
    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    typedef TreeIteratorPath<NodeDataType, EdgeDataType> self_type;
    typedef std::forward_iterator_tag                    iterator_category;
    typedef const TreeLink<NodeDataType, EdgeDataType>*  link_pointer;

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    TreeIteratorPath (link_pointer from, link_pointer to) : start_(from), to_(to);
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

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    inline link_pointer Link() const
    {
        return link_;
    }

    inline TreeNode<NodeDataType, EdgeDataType>* Node() const
    {
        return link_->Node();
    }

    inline TreeEdge<NodeDataType, EdgeDataType>* Edge() const
    {
        return link_->Edge();
    }

    inline link_pointer FromLink() const
    {
        return from_;
    }

    inline TreeNode<NodeDataType, EdgeDataType>* FromNode() const
    {
        return from_->Node();
    }

    inline link_pointer ToLink() const
    {
        return to_;
    }

    inline TreeNode<NodeDataType, EdgeDataType>* ToNode() const
    {
        return to_->Node();
    }

protected:
    link_pointer             link_;
    link_pointer             from_;
    link_pointer             to_;
    std::deque<link_pointer> stack_;
};

*/

} // namespace genesis

#endif // include guard

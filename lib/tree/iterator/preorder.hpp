#ifndef GENESIS_TREE_ITERATOR_PREORDER_H_
#define GENESIS_TREE_ITERATOR_PREORDER_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <assert.h>
#include <deque>
#include <iterator>

namespace genesis {
namespace tree {

// =================================================================================================
//     Preorder Iterator
// =================================================================================================

template <typename LinkType, typename NodeType, typename EdgeType>
class TreeIteratorPreorder
{
public:
    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    typedef TreeIteratorPreorder<LinkType, NodeType, EdgeType> self_type;
    typedef std::forward_iterator_tag iterator_category;

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    TreeIteratorPreorder ()
        : start_( nullptr )
        , link_ ( nullptr )
    {}

    TreeIteratorPreorder (LinkType& link)
        : start_( &link )
        , link_(  &link )
    {
        push_front_children(&link);
        stack_.push_front(&link.outer());
    }

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    self_type operator ++ ()
    {
        if (stack_.empty()) {
            link_ = nullptr;
        } else {
            link_ = stack_.front();
            stack_.pop_front();
            push_front_children(link_);
        }

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
        return other.link_ == link_;
    }

    bool operator != (const self_type &other) const
    {
        return !(other == *this);
    }

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    bool is_first_iteration() const
    {
        return link_ == start_;
    }

    LinkType& link() const
    {
        return *link_;
    }

    NodeType& node() const
    {
        return link_->node();
    }

    EdgeType& edge() const
    {
        return link_->edge();
    }

    LinkType& start_link() const
    {
        return *start_;
    }

    NodeType& start_node() const
    {
        return start_->node();
    }

protected:
    void push_front_children( LinkType* link )
    {
        // we need to push to a tmp queue first, in order to get the order right.
        // otherwise, we would still do a preorder traversal, but starting with
        // the last child of each node instead of the first one.
        std::deque<LinkType*> tmp;
        LinkType* c = &link->next();
        while (c != link) {
            tmp.push_front( &c->outer() );
            c = &c->next();
        }
        for (LinkType* l : tmp) {
            stack_.push_front(l);
        }
    }

    // TODO take a stack or vector instead of deque here!!!

    LinkType*             start_;
    LinkType*             link_;

    std::deque<LinkType*> stack_;
};

} // namespace tree
} // namespace genesis

#endif // include guard

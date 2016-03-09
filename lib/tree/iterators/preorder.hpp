#ifndef GENESIS_TREE_ITERATORS_PREORDER_H_
#define GENESIS_TREE_ITERATORS_PREORDER_H_

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

template <typename LinkPointerType, typename NodePointerType, typename EdgePointerType>
class TreeIteratorPreorder
{
public:
    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    typedef TreeIteratorPreorder<LinkPointerType, NodePointerType, EdgePointerType> self_type;
    typedef std::forward_iterator_tag iterator_category;

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    TreeIteratorPreorder (LinkPointerType link) : link_(link), start_(link)
    {
        if (link) {
            push_front_children(link);
            stack_.push_front(link->outer());
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
            push_front_children(link_);
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

    inline bool is_first_iteration() const
    {
        return link_ == start_;
    }

    inline LinkPointerType link() const
    {
        return link_;
    }

    inline NodePointerType node() const
    {
        return link_->node();
    }

    inline EdgePointerType edge() const
    {
        return link_->edge();
    }

    inline LinkPointerType start_link() const
    {
        return start_;
    }

    inline NodePointerType start_node() const
    {
        return start_->node();
    }

protected:
    inline void push_front_children(const LinkPointerType link)
    {
        // we need to push to a tmp queue first, in order to get the order right.
        // otherwise, we would still do a preorder traversal, but starting with
        // the last child of each node instead of the first one.
        std::deque<LinkPointerType> tmp;
        LinkPointerType c = link->next();
        while (c != link) {
            tmp.push_front(c->outer());
            c = c->next();
        }
        for (LinkPointerType l : tmp) {
            stack_.push_front(l);
        }
    }

    // TODO take a stack or vector instead of deque here!!!

    LinkPointerType             link_;
    LinkPointerType             start_;
    std::deque<LinkPointerType> stack_;
};

} // namespace tree
} // namespace genesis

#endif // include guard

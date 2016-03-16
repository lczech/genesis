#ifndef GENESIS_TREE_ITERATOR_LEVELORDER_H_
#define GENESIS_TREE_ITERATOR_LEVELORDER_H_

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
//     Levelorder Iterator
// =================================================================================================

template <typename LinkPointerType, typename NodePointerType, typename EdgePointerType>
class TreeIteratorLevelorder
{
public:
    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    typedef TreeIteratorLevelorder<LinkPointerType, NodePointerType, EdgePointerType> self_type;
    typedef std::forward_iterator_tag iterator_category;

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    TreeIteratorLevelorder (LinkPointerType link) : link_(link), depth_(0), start_(link)
    {
        if (link) {
            push_back_children(link, 0);
            stack_.push_front({link->outer(), 1});
        }
    }

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    inline self_type operator ++ ()
    {
        if (stack_.empty()) {
            link_  = nullptr;
            depth_ = -1;
        } else {
            StackElement se = stack_.front();
            link_  = se.link;
            depth_ = se.depth;
            stack_.pop_front();
            push_back_children(link_, depth_);
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

    inline int depth() const
    {
        return depth_;
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
    inline void push_back_children(LinkPointerType link, int link_depth)
    {
        LinkPointerType c = link->next();
        while (c != link) {
            stack_.push_back({c->outer(), link_depth + 1});
            c = c->next();
        }
    }

    // TODO add depth information to other iterators, as well.
    typedef struct {
        LinkPointerType link;
        int             depth;
    } StackElement;

    LinkPointerType          link_;
    int                      depth_;
    LinkPointerType          start_;
    std::deque<StackElement> stack_;
};

} // namespace tree
} // namespace genesis

#endif // include guard

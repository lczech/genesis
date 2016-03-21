#ifndef GENESIS_TREE_ITERATOR_POSTORDER_H_
#define GENESIS_TREE_ITERATOR_POSTORDER_H_

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
//     Postorder Iterator
// =================================================================================================

template <typename LinkPointerType, typename NodePointerType, typename EdgePointerType>
class TreeIteratorPostorder
{
public:
    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    typedef TreeIteratorPostorder<LinkPointerType, NodePointerType, EdgePointerType> self_type;
    typedef std::forward_iterator_tag iterator_category;

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    TreeIteratorPostorder (LinkPointerType link) : start_(link)
    {
        if (link) {
            stack_.push_back(link);
            stack_.push_front(link->outer());
            link = link->outer();
            while (link->is_inner()) {
                push_front_children(link);
                link = link->next()->outer();
            }
            assert(link == stack_.front());
            stack_.pop_front();
        }
        link_ = link;
    }

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    self_type operator ++ ()
    {
        if (stack_.empty()) {
            // this condition marks the end of the traversal
            link_ = nullptr;
        } else if (link_->outer()->next() == stack_.front()) {
            // this condition is active when seeing an inner node the last time,
            // meaning that it is its turn to be traversed
            link_ = stack_.front();
            stack_.pop_front();
        } else {
            // this condition is active in all other cases: going down the tree towards the leaves
            link_ = stack_.front();
            while (link_->is_inner()) {
                push_front_children(link_);
                link_ = link_->next()->outer();
            }
            assert(link_ == stack_.front());
            stack_.pop_front();
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

    bool is_last_iteration() const
    {
        return link_ == start_;
    }

    LinkPointerType link() const
    {
        return link_;
    }

    NodePointerType node() const
    {
        return link_->node();
    }

    EdgePointerType edge() const
    {
        return link_->edge();
    }

    LinkPointerType start_link() const
    {
        return start_;
    }

    NodePointerType start_node() const
    {
        return start_->node();
    }

protected:
    void push_front_children(LinkPointerType link)
    {
        // we need to push to a tmp queue first, in order to get the order right.
        // otherwise, we would still do a postorder traversal, but starting with
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

    LinkPointerType             link_;
    LinkPointerType             start_;
    std::deque<LinkPointerType> stack_;
};

} // namespace tree
} // namespace genesis

#endif // include guard

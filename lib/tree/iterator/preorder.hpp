#ifndef GENESIS_TREE_ITERATOR_PREORDER_H_
#define GENESIS_TREE_ITERATOR_PREORDER_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "utils/core/range.hpp"

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

    using TreeType          = typename LinkType::TreeType;

    using iterator_category = std::forward_iterator_tag;
    using self_type         = TreeIteratorPreorder<LinkType, NodeType, EdgeType>;

    self_type operator * ()
    {
        return *this;
    }

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    TreeIteratorPreorder ()
        : start_( nullptr )
        , link_ ( nullptr )
    {}

    explicit TreeIteratorPreorder( TreeType& tree )
        : TreeIteratorPreorder( tree.root_link() )
    {}

    explicit TreeIteratorPreorder( TreeType const& tree )
        : TreeIteratorPreorder( tree.root_link() )
    {}

    explicit TreeIteratorPreorder( NodeType& node )
        : TreeIteratorPreorder( node.primary_link() )
    {}

    explicit TreeIteratorPreorder( LinkType& link )
        : start_( &link )
        , link_(  &link )
    {
        push_front_children( &link );
        stack_.push_front( &link.outer() );
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

    // TODO take a stack or vector instead of deque here; maybe reverse pushing order

    LinkType*             start_;
    LinkType*             link_;

    std::deque<LinkType*> stack_;
};

// =================================================================================================
//     Preorder Wrapper Functions
// =================================================================================================

template<typename ElementType>
utils::Range< TreeIteratorPreorder<
    typename ElementType::LinkType const,
    typename ElementType::NodeType const,
    typename ElementType::EdgeType const
>> preorder( ElementType const& element )
{
    using LinkType = typename ElementType::LinkType;
    using NodeType = typename ElementType::NodeType;
    using EdgeType = typename ElementType::EdgeType;

    return {
        TreeIteratorPreorder< const LinkType, const NodeType, const EdgeType >( element ),
        TreeIteratorPreorder< const LinkType, const NodeType, const EdgeType >()
    };
}

template<typename ElementType>
utils::Range< TreeIteratorPreorder<
    typename ElementType::LinkType,
    typename ElementType::NodeType,
    typename ElementType::EdgeType
>> preorder( ElementType& element )
{
    using LinkType = typename ElementType::LinkType;
    using NodeType = typename ElementType::NodeType;
    using EdgeType = typename ElementType::EdgeType;

    return {
        TreeIteratorPreorder< LinkType, NodeType, EdgeType >( element ),
        TreeIteratorPreorder< LinkType, NodeType, EdgeType >()
    };
}

} // namespace tree
} // namespace genesis

#endif // include guard

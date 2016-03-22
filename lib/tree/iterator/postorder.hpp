#ifndef GENESIS_TREE_ITERATOR_POSTORDER_H_
#define GENESIS_TREE_ITERATOR_POSTORDER_H_

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
//     Postorder Iterator
// =================================================================================================

template <typename LinkType, typename NodeType, typename EdgeType>
class TreeIteratorPostorder
{

public:

    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    using TreeType          = typename LinkType::TreeType;

    using iterator_category = std::forward_iterator_tag;
    using self_type         = TreeIteratorPostorder<LinkType, NodeType, EdgeType>;

    self_type operator * ()
    {
        return *this;
    }

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    TreeIteratorPostorder()
        : start_( nullptr )
        , link_ ( nullptr )
    {}

    explicit TreeIteratorPostorder( TreeType& tree )
        : TreeIteratorPostorder( tree.root_link() )
    {}

    explicit TreeIteratorPostorder( TreeType const& tree )
        : TreeIteratorPostorder( tree.root_link() )
    {}

    explicit TreeIteratorPostorder( NodeType& node )
        : TreeIteratorPostorder( node.primary_link() )
    {}

    explicit TreeIteratorPostorder( LinkType& link )
        : start_( &link )
    {
        auto link_ptr = &link;
        stack_.push_back(link_ptr);
        stack_.push_front(&link_ptr->outer());
        link_ptr = &link_ptr->outer();
        while (link_ptr->is_inner()) {
            push_front_children(link_ptr);
            link_ptr = &link_ptr->next().outer();
        }
        assert(link_ptr == stack_.front());
        stack_.pop_front();
        link_ = link_ptr;
    }

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    self_type operator ++ ()
    {
        if( stack_.empty() ) {
            // this condition marks the end of the traversal
            link_ = nullptr;
        } else if( &link_->outer().next() == stack_.front() ) {
            // this condition is active when seeing an inner node the last time,
            // meaning that it is its turn to be traversed
            link_ = stack_.front();
            stack_.pop_front();
        } else {
            // this condition is active in all other cases: going down the tree towards the leaves
            link_ = stack_.front();
            while (link_->is_inner()) {
                push_front_children(link_);
                link_ = &link_->next().outer();
            }
            assert( link_ == stack_.front() );
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

private:

    void push_front_children( LinkType* link )
    {
        // we need to push to a tmp queue first, in order to get the order right.
        // otherwise, we would still do a postorder traversal, but starting with
        // the last child of each node instead of the first one.
        std::deque<LinkType*> tmp;
        LinkType* c = &link->next();
        while( c != link ) {
            tmp.push_front( &c->outer() );
            c = &c->next();
        }
        for( LinkType* l : tmp ) {
            stack_.push_front(l);
        }
    }

    LinkType*             start_;
    LinkType*             link_;

    std::deque<LinkType*> stack_;
};

// =================================================================================================
//     Postorder Wrapper Functions
// =================================================================================================

template<typename ElementType>
utils::Range< TreeIteratorPostorder<
    typename ElementType::LinkType const,
    typename ElementType::NodeType const,
    typename ElementType::EdgeType const
>> postorder( ElementType const& element )
{
    using LinkType = typename ElementType::LinkType;
    using NodeType = typename ElementType::NodeType;
    using EdgeType = typename ElementType::EdgeType;

    return {
        TreeIteratorPostorder< const LinkType, const NodeType, const EdgeType >( element ),
        TreeIteratorPostorder< const LinkType, const NodeType, const EdgeType >()
    };
}

template<typename ElementType>
utils::Range< TreeIteratorPostorder<
    typename ElementType::LinkType,
    typename ElementType::NodeType,
    typename ElementType::EdgeType
>> postorder( ElementType& element )
{
    using LinkType = typename ElementType::LinkType;
    using NodeType = typename ElementType::NodeType;
    using EdgeType = typename ElementType::EdgeType;

    return {
        TreeIteratorPostorder< LinkType, NodeType, EdgeType >( element ),
        TreeIteratorPostorder< LinkType, NodeType, EdgeType >()
    };
}

} // namespace tree
} // namespace genesis

#endif // include guard

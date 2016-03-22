#ifndef GENESIS_TREE_ITERATOR_LEVELORDER_H_
#define GENESIS_TREE_ITERATOR_LEVELORDER_H_

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
//     Levelorder Iterator
// =================================================================================================

template <typename LinkType, typename NodeType, typename EdgeType>
class TreeIteratorLevelorder
{

public:

    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    using TreeType          = typename LinkType::TreeType;

    using iterator_category = std::forward_iterator_tag;
    using self_type         = TreeIteratorLevelorder<LinkType, NodeType, EdgeType>;

    self_type operator * ()
    {
        return *this;
    }

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    TreeIteratorLevelorder()
        : start_( nullptr )
        , link_(  nullptr )
        , depth_( 0       )
    {}

    explicit TreeIteratorLevelorder( TreeType& tree )
        : TreeIteratorLevelorder( tree.root_link() )
    {}

    explicit TreeIteratorLevelorder( TreeType const& tree )
        : TreeIteratorLevelorder( tree.root_link() )
    {}

    explicit TreeIteratorLevelorder( NodeType& node )
        : TreeIteratorLevelorder( node.primary_link() )
    {}

    explicit TreeIteratorLevelorder( LinkType& link )
        : start_( &link )
        , link_(  &link )
        , depth_( 0     )
    {
        push_back_children( &link, 0 );
        stack_.push_front({ &link.outer(), 1 });
    }

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    self_type operator ++ ()
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

    int depth() const
    {
        return depth_;
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

    void push_back_children( LinkType* link, int link_depth )
    {
        LinkType* c = &link->next();
        while( c != link ) {
            stack_.push_back({ &c->outer(), link_depth + 1 });
            c = &c->next();
        }
    }

    // TODO add depth information to other iterators, as well.
    typedef struct {
        LinkType*     link;
        int           depth;
    } StackElement;

    LinkType*         start_;
    LinkType*         link_;
    int               depth_;

    std::deque<StackElement> stack_;
};

// =================================================================================================
//     Levelorder Wrapper Functions
// =================================================================================================

template<typename ElementType>
utils::Range< TreeIteratorLevelorder<
    typename ElementType::LinkType const,
    typename ElementType::NodeType const,
    typename ElementType::EdgeType const
>> levelorder( ElementType const& element )
{
    using LinkType = typename ElementType::LinkType;
    using NodeType = typename ElementType::NodeType;
    using EdgeType = typename ElementType::EdgeType;

    return {
        TreeIteratorLevelorder< const LinkType, const NodeType, const EdgeType >( element ),
        TreeIteratorLevelorder< const LinkType, const NodeType, const EdgeType >()
    };
}

template<typename ElementType>
utils::Range< TreeIteratorLevelorder<
    typename ElementType::LinkType,
    typename ElementType::NodeType,
    typename ElementType::EdgeType
>> levelorder( ElementType& element )
{
    using LinkType = typename ElementType::LinkType;
    using NodeType = typename ElementType::NodeType;
    using EdgeType = typename ElementType::EdgeType;

    return {
        TreeIteratorLevelorder< LinkType, NodeType, EdgeType >( element ),
        TreeIteratorLevelorder< LinkType, NodeType, EdgeType >()
    };
}

} // namespace tree
} // namespace genesis

#endif // include guard

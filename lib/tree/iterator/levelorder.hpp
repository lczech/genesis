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
class IteratorLevelorder
{

public:

    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    using TreeType          = typename LinkType::TreeType;

    using iterator_category = std::forward_iterator_tag;
    using self_type         = IteratorLevelorder<LinkType, NodeType, EdgeType>;

    // -----------------------------------------------------
    //     Constructors and Rule of Five
    // -----------------------------------------------------

    IteratorLevelorder()
        : start_( nullptr )
        , link_(  nullptr )
        , depth_( 0       )
    {}

    explicit IteratorLevelorder( TreeType& tree )
        : IteratorLevelorder( tree.root_link() )
    {}

    explicit IteratorLevelorder( TreeType const& tree )
        : IteratorLevelorder( tree.root_link() )
    {}

    explicit IteratorLevelorder( NodeType& node )
        : IteratorLevelorder( node.primary_link() )
    {}

    explicit IteratorLevelorder( LinkType& link )
        : start_( &link )
        , link_(  &link )
        , depth_( 0     )
    {
        push_back_children( &link, 0 );
        stack_.push_front({ &link.outer(), 1 });
    }

    ~IteratorLevelorder() = default;

    IteratorLevelorder( IteratorLevelorder const& ) = default;
    IteratorLevelorder( IteratorLevelorder&& )      = default;

    IteratorLevelorder& operator= ( IteratorLevelorder const& ) = default;
    IteratorLevelorder& operator= ( IteratorLevelorder&& )      = default;

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    self_type operator * ()
    {
        return *this;
    }

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
utils::Range< IteratorLevelorder<
    typename ElementType::LinkType const,
    typename ElementType::NodeType const,
    typename ElementType::EdgeType const
>> levelorder( ElementType const& element )
{
    using LinkType = typename ElementType::LinkType;
    using NodeType = typename ElementType::NodeType;
    using EdgeType = typename ElementType::EdgeType;

    return {
        IteratorLevelorder< const LinkType, const NodeType, const EdgeType >( element ),
        IteratorLevelorder< const LinkType, const NodeType, const EdgeType >()
    };
}

template<typename ElementType>
utils::Range< IteratorLevelorder<
    typename ElementType::LinkType,
    typename ElementType::NodeType,
    typename ElementType::EdgeType
>> levelorder( ElementType& element )
{
    using LinkType = typename ElementType::LinkType;
    using NodeType = typename ElementType::NodeType;
    using EdgeType = typename ElementType::EdgeType;

    return {
        IteratorLevelorder< LinkType, NodeType, EdgeType >( element ),
        IteratorLevelorder< LinkType, NodeType, EdgeType >()
    };
}

} // namespace tree
} // namespace genesis

#endif // include guard

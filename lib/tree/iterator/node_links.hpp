#ifndef GENESIS_TREE_ITERATOR_NODE_LINKS_H_
#define GENESIS_TREE_ITERATOR_NODE_LINKS_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "utils/core/range.hpp"

#include <iterator>

namespace genesis {
namespace tree {

// =============================================================================
//     Iterator Node Links
// =============================================================================

template <typename LinkType, typename NodeType, typename EdgeType>
class IteratorNodeLinks
{

public:

    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    using TreeType          = typename LinkType::TreeType;

    using iterator_category = std::forward_iterator_tag;
    using self_type         = IteratorNodeLinks<LinkType, NodeType, EdgeType>;

    // -----------------------------------------------------
    //     Constructors and Rule of Five
    // -----------------------------------------------------

    IteratorNodeLinks()
        : start_( nullptr )
        , link_(  nullptr )
    {}

    explicit IteratorNodeLinks( NodeType& node )
        : start_( &node.primary_link() )
        , link_(  &node.primary_link() )
    {}

    explicit IteratorNodeLinks( LinkType& link )
        : start_( &link )
        , link_(  &link )
    {}

    ~IteratorNodeLinks() = default;

    IteratorNodeLinks( IteratorNodeLinks const& ) = default;
    IteratorNodeLinks( IteratorNodeLinks&& )      = default;

    IteratorNodeLinks& operator= ( IteratorNodeLinks const& ) = default;
    IteratorNodeLinks& operator= ( IteratorNodeLinks&& )      = default;

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    self_type operator * ()
    {
        return *this;
    }

    self_type operator ++ ()
    {
        link_ = &link_->next();
        if (link_ == start_) {
            link_ = nullptr;
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

private:

    LinkType* start_;
    LinkType* link_;
};

// =================================================================================================
//     Node Links Wrapper Functions
// =================================================================================================

template<typename ElementType>
utils::Range< IteratorNodeLinks<
    typename ElementType::LinkType const,
    typename ElementType::NodeType const,
    typename ElementType::EdgeType const
>> node_links( ElementType const& element )
{
    using LinkType = typename ElementType::LinkType;
    using NodeType = typename ElementType::NodeType;
    using EdgeType = typename ElementType::EdgeType;

    return {
        IteratorNodeLinks< const LinkType, const NodeType, const EdgeType >( element ),
        IteratorNodeLinks< const LinkType, const NodeType, const EdgeType >()
    };
}

template<typename ElementType>
utils::Range< IteratorNodeLinks<
    typename ElementType::LinkType,
    typename ElementType::NodeType,
    typename ElementType::EdgeType
>> node_links( ElementType& element )
{
    using LinkType = typename ElementType::LinkType;
    using NodeType = typename ElementType::NodeType;
    using EdgeType = typename ElementType::EdgeType;

    return {
        IteratorNodeLinks< LinkType, NodeType, EdgeType >( element ),
        IteratorNodeLinks< LinkType, NodeType, EdgeType >()
    };
}

} // namespace tree
} // namespace genesis

#endif // include guard

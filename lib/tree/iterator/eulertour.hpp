#ifndef GENESIS_TREE_ITERATOR_EULERTOUR_H_
#define GENESIS_TREE_ITERATOR_EULERTOUR_H_

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

// =================================================================================================
//     Euler Tour Iterator
// =================================================================================================

template <typename LinkType, typename NodeType, typename EdgeType>
class IteratorEulertour
{

public:

    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    using TreeType          = typename LinkType::TreeType;

    using iterator_category = std::forward_iterator_tag;
    using self_type         = IteratorEulertour<LinkType, NodeType, EdgeType>;

    // -----------------------------------------------------
    //     Constructors and Rule of Five
    // -----------------------------------------------------

    IteratorEulertour()
        : start_( nullptr )
        , link_(  nullptr )
    {}

    explicit IteratorEulertour (TreeType& tree)
        : start_( &tree.root_link() )
        , link_(  &tree.root_link() )
    {}

    explicit IteratorEulertour (TreeType const& tree)
        : start_( &tree.root_link() )
        , link_(  &tree.root_link() )
    {}

    explicit IteratorEulertour (NodeType& node)
        : start_( &node.primary_link() )
        , link_(  &node.primary_link() )
    {}

    explicit IteratorEulertour (LinkType& link)
        : start_( &link )
        , link_(  &link )
    {}

    ~IteratorEulertour() = default;

    IteratorEulertour( IteratorEulertour const& ) = default;
    IteratorEulertour( IteratorEulertour&& )      = default;

    IteratorEulertour& operator= ( IteratorEulertour const& ) = default;
    IteratorEulertour& operator= ( IteratorEulertour&& )      = default;

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    self_type operator * ()
    {
        return *this;
    }

    self_type& operator ++ ()
    {
        link_ = &link_->outer().next();
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

    NodeType& start_node() const
    {
        return start_->node();
    }

private:

    LinkType* start_;
    LinkType* link_;
};

// =================================================================================================
//     Euler Tour Wrapper Functions
// =================================================================================================

template<typename ElementType>
utils::Range< IteratorEulertour<
    typename ElementType::LinkType const,
    typename ElementType::NodeType const,
    typename ElementType::EdgeType const
>> eulertour( ElementType const& element )
{
    using LinkType = typename ElementType::LinkType;
    using NodeType = typename ElementType::NodeType;
    using EdgeType = typename ElementType::EdgeType;

    return {
        IteratorEulertour< const LinkType, const NodeType, const EdgeType >( element ),
        IteratorEulertour< const LinkType, const NodeType, const EdgeType >()
    };
}

template<typename ElementType>
utils::Range< IteratorEulertour<
    typename ElementType::LinkType,
    typename ElementType::NodeType,
    typename ElementType::EdgeType
>> eulertour( ElementType& element )
{
    using LinkType = typename ElementType::LinkType;
    using NodeType = typename ElementType::NodeType;
    using EdgeType = typename ElementType::EdgeType;

    return {
        IteratorEulertour< LinkType, NodeType, EdgeType >( element ),
        IteratorEulertour< LinkType, NodeType, EdgeType >()
    };
}

} // namespace tree
} // namespace genesis

#endif // include guard

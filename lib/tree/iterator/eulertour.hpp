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
class TreeIteratorEulertour
{
public:
    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    typedef TreeIteratorEulertour<LinkType, NodeType, EdgeType> self_type;
    typedef std::forward_iterator_tag iterator_category;

    typedef typename LinkType::TreeType TreeType;

    // typedef NodeType  value_type;
    // typedef size_t    difference_type;
    typedef NodeType* pointer;
    typedef NodeType& reference;

    self_type operator * ()
    // reference operator * ()
    {
        return *this;
        // return *(link_->node());
        // return link_;
    }

    self_type operator -> ()
    {
        return *this;
    }

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    TreeIteratorEulertour()
        : start_( nullptr )
        , link_(  nullptr )
    {}

    explicit TreeIteratorEulertour (TreeType& tree)
        : start_( &tree.root_link() )
        , link_(  &tree.root_link() )
    {}

    explicit TreeIteratorEulertour (TreeType const& tree)
        : start_( &tree.root_link() )
        , link_(  &tree.root_link() )
    {}

    explicit TreeIteratorEulertour (NodeType& node)
        : start_( &node.primary_link() )
        , link_(  &node.primary_link() )
    {}

    explicit TreeIteratorEulertour (LinkType& link)
        : start_( &link )
        , link_(  &link )
    {}

    // TreeIteratorEulertour () :
    //     link_(nullptr),
    //     start_(nullptr)
    // {}
    //
    // TreeIteratorEulertour (const LinkType* link) :
    //     link_(link),
    //     start_(link)
    // {}
    //
    // TreeIteratorEulertour (const NodeType* node) :
    //     link_(node->primary_link()->next()),
    //     start_(node->primary_link()->next())
    // {}

    // -----------------------------------------------------
    //     Iteration
    // -----------------------------------------------------

    // self_type begin()
    // {
    //     return self_type(start_);
    // }
    //
    // self_type end()
    // {
    //     return self_type(nullptr);
    // }

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

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
    LinkType* start_;
    LinkType* link_;
};

// =================================================================================================
//     Euler Tour Iterator Range
// =================================================================================================

template<typename TreeType>
class TreeIteratorEulertourRange
{
public:

    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    using LinkType = typename TreeType::LinkType;
    using NodeType = typename TreeType::NodeType;
    using EdgeType = typename TreeType::EdgeType;

    using       iterator = TreeIteratorEulertour <      LinkType,       NodeType,       EdgeType>;
    using const_iterator = TreeIteratorEulertour <const LinkType, const NodeType, const EdgeType>;

    // -----------------------------------------------------
    //     Constructors
    // -----------------------------------------------------

    explicit TreeIteratorEulertourRange(TreeType& tree)
        : link_(&tree.root_link())
    {}

    explicit TreeIteratorEulertourRange(LinkType* link)
        : link_(link)
    {}

    explicit TreeIteratorEulertourRange(NodeType* node)
        : link_(&node->primary_link())
    {}

    // -----------------------------------------------------
    //     Range-based loop functions
    // -----------------------------------------------------

    iterator begin()
    {
        return iterator(*link_);
    }

    iterator end()
    {
        return iterator();
    }

    const_iterator begin() const
    {
        return const_iterator(*link_);
    }

    const_iterator end() const
    {
        return const_iterator();
    }

    const_iterator cbegin()
    {
        return const_iterator(*link_);
    }

    const_iterator cend()
    {
        return const_iterator();
    }

    // -----------------------------------------------------
    //     Data members
    // -----------------------------------------------------

private:

    LinkType* link_;
};

// =================================================================================================
//     Euler Tour Wrapper Functions
// =================================================================================================

// template<typename TreeType>
// TreeIteratorEulertourRange<TreeType> eulertour(TreeType& tree)
// {
//     return TreeIteratorEulertourRange<TreeType>(tree);
// }
//
// template<typename PointerType>
// TreeIteratorEulertourRange<typename PointerType::TreeType> eulertour(PointerType* link_or_node)
// {
//     return TreeIteratorEulertourRange<typename PointerType::TreeType>(link_or_node);
// }

// -----------------------------------------------------
//     Tree
// -----------------------------------------------------

template<typename ElementType>
utils::Range< TreeIteratorEulertour<
    typename ElementType::LinkType const,
    typename ElementType::NodeType const,
    typename ElementType::EdgeType const
>> eulertour( ElementType const& element )
{
    using LinkType = typename ElementType::LinkType;
    using NodeType = typename ElementType::NodeType;
    using EdgeType = typename ElementType::EdgeType;

    return {
        TreeIteratorEulertour< const LinkType, const NodeType, const EdgeType >( element ),
        TreeIteratorEulertour< const LinkType, const NodeType, const EdgeType >()
    };
}

template<typename ElementType>
utils::Range< TreeIteratorEulertour<
    typename ElementType::LinkType,
    typename ElementType::NodeType,
    typename ElementType::EdgeType
>> eulertour( ElementType& element )
{
    using LinkType = typename ElementType::LinkType;
    using NodeType = typename ElementType::NodeType;
    using EdgeType = typename ElementType::EdgeType;

    return {
        TreeIteratorEulertour< LinkType, NodeType, EdgeType >( element ),
        TreeIteratorEulertour< LinkType, NodeType, EdgeType >()
    };
}

// -----------------------------------------------------
//     Link
// -----------------------------------------------------

template<typename LinkType>
utils::Range< TreeIteratorEulertour<
    typename LinkType::LinkType const,
    typename LinkType::NodeType const,
    typename LinkType::EdgeType const
>> eulertour_from_link( LinkType const& link )
{
    using LT = typename LinkType::LinkType;
    using NT = typename LinkType::NodeType;
    using ET = typename LinkType::EdgeType;

    return {
        TreeIteratorEulertour< LT const, NT const, ET const >( link ),
        TreeIteratorEulertour< LT const, NT const, ET const >()
    };
}

template<typename LinkType>
utils::Range< TreeIteratorEulertour<
    typename LinkType::LinkType,
    typename LinkType::NodeType,
    typename LinkType::EdgeType
>> eulertour_from_link( LinkType& link )
{
    using LT = typename LinkType::LinkType;
    using NT = typename LinkType::NodeType;
    using ET = typename LinkType::EdgeType;

    return {
        TreeIteratorEulertour< LT, NT, ET >( link ),
        TreeIteratorEulertour< LT, NT, ET >()
    };
}

// -----------------------------------------------------
//     Node
// -----------------------------------------------------

template<typename NodeType>
utils::Range< TreeIteratorEulertour<
    typename NodeType::LinkType,
    typename NodeType::NodeType,
    typename NodeType::EdgeType
>> eulertour_from_node( NodeType& node )
{
    using LT = typename NodeType::LinkType;
    using NT = typename NodeType::NodeType;
    using ET = typename NodeType::EdgeType;

    return {
        TreeIteratorEulertour< LT, NT, ET >( node.link() ),
        TreeIteratorEulertour< LT, NT, ET >()
    };
}

// template<class TreeType>
// class Eulertour
// {
// public:
//
//     // -----------------------------------------------------
//     //     Typedefs
//     // -----------------------------------------------------
//
//     using LinkType = typename TreeType::LinkType;
//     using NodeType = typename TreeType::NodeType;
//     using EdgeType = typename TreeType::EdgeType;
//
//     using       iterator = TreeIteratorEulertour <      LinkType,       NodeType,       EdgeType>;
//     using const_iterator = TreeIteratorEulertour <const LinkType, const NodeType, const EdgeType>;
//
//     // -----------------------------------------------------
//     //     Constructors and Rule of Five
//     // -----------------------------------------------------
//
//     explicit TreeIteratorEulertourRange(TreeType& tree)
//         : link_(tree.root_link())
//     {}
//
//     explicit TreeIteratorEulertourRange(LinkType* link)
//         : link_(link)
//     {}
//
//     explicit TreeIteratorEulertourRange(NodeType* node)
//         : link_(node->primary_link())
//     {}
//
// private:
//
//
//
// };

} // namespace tree
} // namespace genesis

#endif // include guard

#ifndef GENESIS_TREE_ITERATORS_EULERTOUR_H_
#define GENESIS_TREE_ITERATORS_EULERTOUR_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <iterator>

namespace genesis {

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

    // typedef NodeType  value_type;
    // typedef size_t    difference_type;
    typedef NodeType* pointer;
    typedef NodeType& reference;

    inline reference operator * ()
    {
        return *(link_->node());
    }

    // inline pointer operator -> ()
    // {
    //     return link_->node();
    // }

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    explicit TreeIteratorEulertour (LinkType* link) : link_(link), start_(link)
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

    // inline self_type begin()
    // {
    //     return self_type(start_);
    // }
    //
    // inline self_type end()
    // {
    //     return self_type(nullptr);
    // }

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    inline self_type& operator ++ ()
    {
        link_ = link_->outer()->next();
        if (link_ == start_) {
            link_ = nullptr;
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

    inline LinkType* link() const
    {
        return link_;
    }

    inline NodeType* node() const
    {
        return link_->node();
    }

    inline EdgeType* edge() const
    {
        return link_->edge();
    }

    inline LinkType* start_link() const
    {
        return start_;
    }

    inline NodeType* start_node() const
    {
        return start_->node();
    }

protected:
    LinkType* link_;
    LinkType* start_;
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
        : link_(tree.root_link())
    {}

    explicit TreeIteratorEulertourRange(LinkType* link)
        : link_(link)
    {}

    explicit TreeIteratorEulertourRange(NodeType* node)
        : link_(node->primary_link())
    {}

    // -----------------------------------------------------
    //     Range-based loop functions
    // -----------------------------------------------------

    iterator begin()
    {
        return iterator(link_);
    }

    iterator end()
    {
        return iterator(nullptr);
    }

    const_iterator begin() const
    {
        return const_iterator(link_);
    }

    const_iterator end() const
    {
        return const_iterator(nullptr);
    }

    const_iterator cbegin()
    {
        return const_iterator(link_);
    }

    const_iterator cend()
    {
        return const_iterator(nullptr);
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

template<typename TreeType>
TreeIteratorEulertourRange<TreeType> eulertour(TreeType& tree)
{
    return TreeIteratorEulertourRange<TreeType>(tree);
}

template<typename PointerType>
TreeIteratorEulertourRange<typename PointerType::TreeType> eulertour(PointerType* link_or_node)
{
    return TreeIteratorEulertourRange<typename PointerType::TreeType>(link_or_node);
}

} // namespace genesis

#endif // include guard

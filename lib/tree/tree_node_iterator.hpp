#ifndef GENESIS_TREE_TREE_NODE_ITERATOR_H_
#define GENESIS_TREE_TREE_NODE_ITERATOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

namespace genesis {
namespace tree {

// =============================================================================
//     Iterator Links
// =============================================================================

template <typename LinkPointerType, typename NodePointerType, typename EdgePointerType>
class TreeNodeIteratorLinks
{
public:
    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    typedef TreeNodeIteratorLinks<LinkPointerType, NodePointerType, EdgePointerType> self_type;
    typedef std::forward_iterator_tag iterator_category;

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    TreeNodeIteratorLinks (LinkPointerType link) :
    link_(link), start_(link)
    {}

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

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

    LinkPointerType link()
    {
        return link_;
    }

    NodePointerType node()
    {
        return link_->node();
    }

    EdgePointerType edge()
    {
        return link_->edge();
    }

    LinkPointerType start_link()
    {
        return start_;
    }

protected:
    LinkPointerType link_;
    LinkPointerType start_;
};

} // namespace tree
} // namespace genesis

#endif // include guard

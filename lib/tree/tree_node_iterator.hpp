#ifndef GENESIS_TREE_TREE_NODE_ITERATOR_H_
#define GENESIS_TREE_TREE_NODE_ITERATOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

namespace genesis {

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

    inline self_type operator ++ ()
    {
        link_ = link_->Next();
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

    inline LinkPointerType Link()
    {
        return link_;
    }

    inline NodePointerType Node()
    {
        return link_->Node();
    }

    inline EdgePointerType Edge()
    {
        return link_->Edge();
    }

    inline LinkPointerType StartLink()
    {
        return start_;
    }

protected:
    LinkPointerType link_;
    LinkPointerType start_;
};

} // namespace genesis

#endif // include guard

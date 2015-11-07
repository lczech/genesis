#ifndef GENESIS_TREE_ITERATORS_NODE_RANGE_H_
#define GENESIS_TREE_ITERATORS_NODE_RANGE_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

namespace genesis {

// =================================================================================================
//     Iterator Range Wrapper
// =================================================================================================

template <typename ContainerType>
class TreeIteratorContainerRange
{
public:

    typedef ContainerType::iterator             iterator;
    typedef ContainerType::const_iterator const_iterator;

    TreeIteratorContainerRange (ContainerType& container)
        : c_(container)
    {}

    iterator begin()
    {
        return c_.begin();
    }

    iterator end()
    {
        return c_.end();
    }

    const_iterator begin() const
    {
        return c_.cbegin();
    }

    const_iterator end() const
    {
        return c_.cend();
    }

    const_iterator cbegin()
    {
        return c_.cbegin();
    }

    const_iterator cend()
    {
        return c_.cend();
    }

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

private:

    ContainerType& c_;
}

} // namespace genesis

#endif // include guard

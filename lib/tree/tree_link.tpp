/**
 * @brief Implementation of TreeLink class template.
 *
 * For reasons of readability, in this implementation file, the template data types
 * NodeDataType and EdgeDataType are abbreviated NDT and EDT, respectively.
 *
 * @file
 * @ingroup tree
 */

namespace genesis {
namespace tree {

// =============================================================================
//     Accessors
// =============================================================================

/**
 * @brief Return the next TreeLink within the TreeNode of this link.
 */
template <class NDT, class EDT>
TreeLink<NDT, EDT>& TreeLink<NDT, EDT>::next()
{
    return *next_;
}

/**
 * @brief Return the next TreeLink within the TreeNode of this link.
 */
template <class NDT, class EDT>
TreeLink<NDT, EDT> const& TreeLink<NDT, EDT>::next() const
{
    return *next_;
}

/**
 * @brief Return the previous TreeLink within the TreeNode of this link.
 *
 * The previous link of a given link `L` is the one whose next-pointer is pointing to `L`.
 * As this link first has to be found, this function is not as cheap as next().
 */
template <class NDT, class EDT>
TreeLink<NDT, EDT>& TreeLink<NDT, EDT>::prev()
{
    LinkType* res = this;
    while( &res->next() != this ) {
        res = &res->next();
    }
    return *res;
}

/**
 * @brief Return the previous TreeLink within the TreeNode of this link.
 *
 * The previous link of a given link `L` is the one whose next-pointer is pointing to `L`.
 * As this link first has to be found, this function is not as cheap as next().
 */
template <class NDT, class EDT>
TreeLink<NDT, EDT> const& TreeLink<NDT, EDT>::prev() const
{
    LinkType const* res = this;
    while( &res->next() != this ) {
        res = &res->next();
    }
    return *res;
}

/**
 * @brief Return the TreeLink of the adjacent TreeNode.
 */
template <class NDT, class EDT>
TreeLink<NDT, EDT>& TreeLink<NDT, EDT>::outer()
{
    return *outer_;
}

/**
 * @brief Return the TreeLink of the adjacent TreeNode.
 */
template <class NDT, class EDT>
TreeLink<NDT, EDT> const& TreeLink<NDT, EDT>::outer() const
{
    return *outer_;
}

/**
 * @brief Return the TreeEdge of this TreeLink.
 */
template <class NDT, class EDT>
TreeEdge<NDT, EDT>& TreeLink<NDT, EDT>::edge()
{
    return *edge_;
}

/**
 * @brief Return the TreeEdge of this TreeLink.
 */
template <class NDT, class EDT>
TreeEdge<NDT, EDT> const& TreeLink<NDT, EDT>::edge() const
{
    return *edge_;
}

/**
 * @brief Return the TreeNode of this TreeLink.
 */
template <class NDT, class EDT>
TreeNode<NDT, EDT>& TreeLink<NDT, EDT>::node()
{
    return *node_;
}

/**
 * @brief Return the TreeNode of this TreeLink.
 */
template <class NDT, class EDT>
TreeNode<NDT, EDT> const& TreeLink<NDT, EDT>::node() const
{
    return *node_;
}

} // namespace tree
} // namespace genesis

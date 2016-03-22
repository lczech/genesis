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

// =================================================================================================
//     Accessors
// =================================================================================================

/**
 * @brief Return the index of this Link.
 */
template <class NDT, class EDT>
size_t TreeLink<NDT, EDT>::index() const
{
    return index_;
}

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

// =================================================================================================
//     Modifiers
// =================================================================================================

template <class NDT, class EDT>
TreeLink<NDT, EDT>& TreeLink<NDT, EDT>::reset_index( size_t val )
{
    index_ = val;
    return *this;
}

template <class NDT, class EDT>
TreeLink<NDT, EDT>& TreeLink<NDT, EDT>::reset_next(  LinkType* val )
{
    next_ = val;
    return *this;
}

template <class NDT, class EDT>
TreeLink<NDT, EDT>& TreeLink<NDT, EDT>::reset_outer( LinkType* val )
{
    outer_ = val;
    return *this;
}

template <class NDT, class EDT>
TreeLink<NDT, EDT>& TreeLink<NDT, EDT>::reset_node(  NodeType* val )
{
    node_ = val;
    return *this;
}

template <class NDT, class EDT>
TreeLink<NDT, EDT>& TreeLink<NDT, EDT>::reset_edge(  EdgeType* val )
{
    edge_ = val;
    return *this;
}

// =================================================================================================
//     Member Functions
// =================================================================================================

/**
 * @brief Return true iff the node of this link is a leaf node.
 */
template <class NDT, class EDT>
bool TreeLink<NDT, EDT>::is_leaf() const
{
    return next_ == this;
}

/**
 * @brief Return true iff the node of this link is an inner node.
 */
template <class NDT, class EDT>
bool TreeLink<NDT, EDT>::is_inner() const
{
    return next_ != this;
}

/**
 * @brief Return a string containing dump information about this link.
 *
 * At the moment, a link does not contain any information, so an empty string is returned.
 * This might change in the future, in case links also contain data.
 */
template <class NDT, class EDT>
std::string TreeLink<NDT, EDT>::dump() const
{
    return "";
}

} // namespace tree
} // namespace genesis

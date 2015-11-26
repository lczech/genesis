#ifndef GENESIS_TREE_IO_NEWICK_ADAPTER_H_
#define GENESIS_TREE_IO_NEWICK_ADAPTER_H_

/**
 * @brief Header of NewickAdapter class.
 *
 * @file
 * @ingroup tree
 */

#include <string>

#include "tree/io/newick/broker.hpp"

namespace genesis {

// =================================================================================================
//     NewickAdapter
// =================================================================================================

/**
 * @brief
 */
template <typename Tree>
class NewickAdapter
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Destructor
    // -------------------------------------------------------------------------

    typedef Tree TreeType;

    // NewickAdapter();
    // ~NewickAdapter();

    // -------------------------------------------------------------------------
    //     Property Setters
    // -------------------------------------------------------------------------

protected:

    inline void set_name (const std::string& name, NewickBrokerElement& node)
    {
        node.name = name;
    }

    inline void set_branch_length (const double branch_length, NewickBrokerElement& node)
    {
        node.branch_length = branch_length;
    }

    inline void add_tag (const std::string& tag, NewickBrokerElement& node)
    {
        node.tags.push_back(tag);
    }

    inline void add_comment (const std::string& comment, NewickBrokerElement& node)
    {
        node.comments.push_back(comment);
    }
};

} // namespace genesis

#endif // include guard

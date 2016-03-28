#ifndef GENESIS_TREE_DEFAULT_NEWICK_READER_H_
#define GENESIS_TREE_DEFAULT_NEWICK_READER_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "tree/default/tree.hpp"
#include "tree/io/newick/element.hpp"
#include "tree/io/newick/reader.hpp"
#include "utils/core/std.hpp"
#include "utils/text/string.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Default Tree Newick Reader Mixin
// =================================================================================================

/**
 * @brief
 */
template <typename Base>
class DefaultTreeNewickReaderMixin : public Base
{
    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

public:

    typedef typename Base::TreeType TreeType;
    typedef typename Base::NodeType NodeType;
    typedef typename Base::EdgeType EdgeType;
    typedef typename Base::LinkType LinkType;

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    // -------------------------------------------------------------------------
    //     Overridden Member Functions
    // -------------------------------------------------------------------------

protected:

    virtual void element_to_node( NewickBrokerElement const& element, NodeType& node ) override
    {
        Base::element_to_node(element, node);

        std::string name = element.name;

        // Insert default names if needed.
        if (name.empty() && use_default_names) {
            if (element.is_leaf) {
                name = default_leaf_name;
            } else if(element.depth == 0) {
                name = default_root_name;
            } else {
                name = default_internal_name;
            }
        }

        // Handle underscores/spaces.
        if (replace_name_underscores) {
            name = utils::replace_all(name, "_", " ");
        }

        node.data.name = name;
    }

    virtual void element_to_edge( NewickBrokerElement const& element, EdgeType& edge ) override
    {
        Base::element_to_edge(element, edge);

        // We assume that the branch length is always the first (or only) value.
        // If there is an interpretation where this is not the case, it is best to introduce
        // an array index for this as a paramter of this class.
        if (element.values.size() > 0) {
            edge.data.branch_length = std::stod(element.values[0]);
        }
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

public:

    // TODO for now, this is all public. use getters and setters instead, and outsource those
    // properties that belong to the (yet to create) superclass DefaultNewickMixinBase or so.

    std::string default_leaf_name     = "Leaf_Node";
    std::string default_internal_name = "Internal_Node";
    std::string default_root_name     = "Root_Node";

    /**
     * @brief If set to true, unnamed nodes are named using one of the default names.
     *
     * The default names can be set using #default_leaf_name, #default_internal_name and
     * #default_root_name.
     */
    bool        use_default_names = false;

    bool        replace_name_underscores = false;

};

// =================================================================================================
//     Default Tree Newick Reader
// =================================================================================================

typedef DefaultTreeNewickReaderMixin<NewickReader<DefaultTree>> DefaultTreeNewickReader;

} // namespace tree
} // namespace genesis

#endif // include guard

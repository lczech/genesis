#ifndef GENESIS_TREE_DEFAULT_NEWICK_MIXIN_H_
#define GENESIS_TREE_DEFAULT_NEWICK_MIXIN_H_

/**
 * @brief Header of DefaultTreeNewickAdapter class.
 *
 * @file
 * @ingroup tree
 */

#include "tree/io/newick/element.hpp"
#include "utils/core/std.hpp"
#include "utils/string/string.hpp"

namespace genesis {

// =================================================================================================
//     Default TreeNewick Adapter
// =================================================================================================

/**
 * @brief
 */
template <typename Base>
class DefaultTreeNewickMixin : public Base
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

public:

    void enable_names( bool value )
    {
        enable_names_ = value;
    }

    bool enable_names ()
    {
        return enable_names_;
    }

    void enable_branch_lengths( bool value )
    {
        enable_branch_lengths_ = value;
    }

    bool enable_branch_lengths()
    {
        return enable_branch_lengths_;
    }

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
            name = string_replace_all(name, "_", " ");
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

    virtual void node_to_element( NodeType const& node, NewickBrokerElement& element ) override
    {
        Base::node_to_element(node, element);

        if (enable_names_) {
            std::string name = node.data.name;

            // Handle spaces/underscores.
            if (replace_name_underscores) {
                name = string_replace_all(name, " ", "_");
            } else {
                if (contains(name, std::string(" "))) {
                    name = "\"" + name + "\"";
                }
            }

            // Filter out default names if needed.
            if (use_default_names             && (
                name == default_leaf_name     ||
                name == default_internal_name ||
                name == default_root_name
            )) {
                name = "";
            }

            element.name = name;
        }
    }

    virtual void edge_to_element( EdgeType const& edge, NewickBrokerElement& element ) override
    {
        Base::edge_to_element(edge, element);

        if (enable_branch_lengths_) {
            auto bl = to_string_precise(edge.data.branch_length, precision);
            element.values.insert(element.values.begin(), bl);
        }
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

public:

    // TODO for now, this is all public. use getters and setters instead, and outsource those
    // properties that belong to the (yet to create) superclass TreeProcessor or so.


    /**
     * @brief The precision used for printing floating point numbers, particularly the branch_length.
     */
    int  precision = 6;

    std::string default_leaf_name     = "Leaf_Node";
    std::string default_internal_name = "Internal_Node";
    std::string default_root_name     = "Root_Node";

    /**
     * @brief If set to true, unnamed nodes are named using one of the default names.
     *
     * The default names can be set using `default_leaf_name`, `default_internal_name` and
     * `default_root_name`. They are used both when parsing and printing a Newick file.
     */
    bool        use_default_names = false;

    bool        replace_name_underscores = false;

private:

    bool enable_names_          = true;
    bool enable_branch_lengths_ = false;
    // bool print_comments       = false;
    // bool print_tags           = false;
};

} // namespace genesis

#endif // include guard

#ifndef GENESIS_TREE_DEFAULT_NEWICK_WRITER_H_
#define GENESIS_TREE_DEFAULT_NEWICK_WRITER_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "tree/default/tree.hpp"
#include "tree/io/newick/element.hpp"
#include "tree/io/newick/writer.hpp"
#include "utils/core/std.hpp"
#include "utils/text/string.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Default Tree Newick Writer Mixin
// =================================================================================================

/**
 * @brief
 */
template <typename Base>
class DefaultTreeNewickWriterMixin : public Base
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

    virtual void node_to_element( NodeType const& node, NewickBrokerElement& element ) override
    {
        Base::node_to_element(node, element);

        if (enable_names_) {
            std::string name = node.data.name;

            // Handle spaces/underscores.
            if (replace_name_underscores) {
                name = utils::replace_all(name, " ", "_");
            } else {
                if (std::string::npos != name.find(" ")) {
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
            auto bl = utils::to_string_precise(edge.data.branch_length, precision);
            element.values.insert(element.values.begin(), bl);
        }
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

public:

    // TODO for now, this is all public. use getters and setters instead, and outsource those
    // properties that belong to the (yet to create) superclass DefaultNewickMixinBase or so.

    /**
     * @brief The precision used for printing floating point numbers, particularly the branch_length.
     */
    int  precision = 6;

    std::string default_leaf_name     = "Leaf_Node";
    std::string default_internal_name = "Internal_Node";
    std::string default_root_name     = "Root_Node";

    /**
     * @brief If set to true, nodes that are named using one of the default names are written
     * without names.
     *
     * This option effictively reverses the effect of the same option in the
     * DefaultTreeNewickReaderMixin. Thus, when both are set to the same value, the resulting
     * Newick file contains the same names.
     *
     * The default names can be set using #default_leaf_name, #default_internal_name and
     * #default_root_name. They are used both when parsing and printing a Newick file.
     */
    bool        use_default_names = false;

    bool        replace_name_underscores = true;

private:

    bool enable_names_          = true;
    bool enable_branch_lengths_ = false;

    // bool print_comments       = false;
    // bool print_tags           = false;
};

// =================================================================================================
//     Default Tree Newick Writer
// =================================================================================================

typedef DefaultTreeNewickWriterMixin<NewickWriter<DefaultTree>> DefaultTreeNewickWriter;

} // namespace tree
} // namespace genesis

#endif // include guard

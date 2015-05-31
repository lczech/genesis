#ifndef GENESIS_TREE_NEWICK_PROCESSOR_H_
#define GENESIS_TREE_NEWICK_PROCESSOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <assert.h>
#include <string>

#include "tree/newick_lexer.hpp"

namespace genesis {

// =============================================================================
//     Forward declarations
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class  Tree;

template <class NodeDataType, class EdgeDataType>
class  TreeSet;

class  NewickBroker;
struct NewickBrokerElement;

// =============================================================================
//     Newick Processor
// =============================================================================

class NewickProcessor
{
public:

    static std::string default_leaf_name;
    static std::string default_internal_name;
    static std::string default_root_name;

    static bool        use_default_names;

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

    template <class NodeDataType, class EdgeDataType>
    static bool from_file   (const std::string& fn,     Tree<NodeDataType, EdgeDataType>& tree);

    template <class NodeDataType, class EdgeDataType>
    static bool from_string (const std::string& ts,     Tree<NodeDataType, EdgeDataType>& tree);

    template <class NodeDataType, class EdgeDataType>
    static bool from_file   (const std::string& fn,     TreeSet<NodeDataType, EdgeDataType>& tset);

    template <class NodeDataType, class EdgeDataType>
    static bool from_string (const std::string& ts,     TreeSet<NodeDataType, EdgeDataType>& tset);

protected:
    static bool parse_tree  (
              NewickLexer::iterator& ct,
        const NewickLexer::iterator& end,
              NewickBroker&          broker
    );

    template <class NodeDataType, class EdgeDataType>
    static void build_tree (NewickBroker& broker, Tree<NodeDataType, EdgeDataType>& tree);

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

public:
    static bool print_names;
    static bool print_branch_lengths;
    static bool print_comments;
    static bool print_tags;

    static int  precision;

    template <class NodeDataType, class EdgeDataType>
    static bool to_file   (const Tree<NodeDataType, EdgeDataType>& tree, const std::string fn);

    template <class NodeDataType, class EdgeDataType>
    static void to_string (const Tree<NodeDataType, EdgeDataType>& tree, std::string& ts);

    template <class NodeDataType, class EdgeDataType>
    static std::string to_string (const Tree<NodeDataType, EdgeDataType>& tree);

protected:
    template <class NodeDataType, class EdgeDataType>
    static void to_broker (const Tree<NodeDataType, EdgeDataType>& tree, NewickBroker& broker);

    static std::string to_string_rec(const NewickBroker& broker, size_t position);
    static std::string element_to_string(const NewickBrokerElement* bn);
};

} // namespace genesis

// =============================================================================
//     Inclusion of the implementation
// =============================================================================

// This class contains function templates, so do the inclusion here.
#include "tree/newick_processor.tpp"

#endif // include guard

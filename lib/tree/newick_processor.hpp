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
    static bool FromFile   (const std::string& fn,     Tree<NodeDataType, EdgeDataType>& tree);

    template <class NodeDataType, class EdgeDataType>
    static bool FromString (const std::string& ts,     Tree<NodeDataType, EdgeDataType>& tree);

    template <class NodeDataType, class EdgeDataType>
    static bool FromFile   (const std::string& fn,     TreeSet<NodeDataType, EdgeDataType>& tset);

    template <class NodeDataType, class EdgeDataType>
    static bool FromString (const std::string& ts,     TreeSet<NodeDataType, EdgeDataType>& tset);

protected:
    static bool ParseTree  (
              NewickLexer::iterator& ct,
        const NewickLexer::iterator& end,
              NewickBroker&          broker
    );

    template <class NodeDataType, class EdgeDataType>
    static void BuildTree (NewickBroker& broker, Tree<NodeDataType, EdgeDataType>& tree);

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
    static bool ToFile   (const std::string& fn, const Tree<NodeDataType, EdgeDataType>& tree);

    template <class NodeDataType, class EdgeDataType>
    static void ToString (std::string& ts,      const Tree<NodeDataType, EdgeDataType>& tree);

    template <class NodeDataType, class EdgeDataType>
    static std::string ToString (               const Tree<NodeDataType, EdgeDataType>& tree);

protected:
    template <class NodeDataType, class EdgeDataType>
    static void ToBroker (NewickBroker& broker, const Tree<NodeDataType, EdgeDataType>& tree);

    static std::string ToStringRec(const NewickBroker& broker, size_t position);
    static std::string ElementToString(const NewickBrokerElement* bn);
};

} // namespace genesis

// =============================================================================
//     Inclusion of the implementation
// =============================================================================

// This class contains function templates, so do the inclusion here.
#include "tree/newick_processor.tpp"

#endif // include guard

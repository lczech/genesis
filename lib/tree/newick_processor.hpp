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
#include <vector>

#include "tree/newick_lexer.hpp"

namespace genesis {

// =============================================================================
//     Forward declarations
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class  Tree;

template <class TreeType>
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

    template <class TreeType>
    static bool from_file    (const std::string& filename,    TreeType& tree);

    template <class TreeType>
    static bool from_string  (const std::string& tree_string, TreeType& tree);

    template <class TreeType>
    static bool from_file    (const std::string& filename,    TreeSet<TreeType>& tree_set);

    template <class TreeType>
    static bool from_string  (const std::string& tree_string, TreeSet<TreeType>& tree_set);

    template <class TreeType>
    static bool from_files   (const std::vector<std::string>& filenames,    TreeSet<TreeType>& tree_set);

    template <class TreeType>
    static bool from_strings (const std::vector<std::string>& tree_strings, TreeSet<TreeType>& tree_set);

protected:
    static bool parse_tree  (
              NewickLexer::iterator& ct,
        const NewickLexer::iterator& end,
              NewickBroker&          broker
    );

    template <class TreeType>
    static void build_tree (NewickBroker& broker, TreeType& tree);

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

public:
    static bool print_names;
    static bool print_branch_lengths;
    static bool print_comments;
    static bool print_tags;

    static int  precision;

    template <class TreeType>
    static bool to_file   (const TreeType& tree, const std::string fn);

    template <class TreeType>
    static void to_string (const TreeType& tree, std::string& ts);

    template <class TreeType>
    static std::string to_string (const TreeType& tree);

protected:
    template <class TreeType>
    static void to_broker (const TreeType& tree, NewickBroker& broker);

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

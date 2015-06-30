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

// =================================================================================================
//     Forward declarations
// =================================================================================================

template <class NodeDataType, class EdgeDataType>
class  Tree;

template <class TreeType>
class  TreeSet;

class  NewickBroker;
struct NewickBrokerElement;

// =================================================================================================
//     Newick Processor
// =================================================================================================

class NewickProcessor
{
    // -------------------------------------------------------------------------
    //     Parsing
    // -------------------------------------------------------------------------

public:
    template <class TreeType>
    bool from_file    (const std::string& filename,    TreeType& tree);

    template <class TreeType>
    bool from_string  (const std::string& tree_string, TreeType& tree);

    template <class TreeType>
    bool from_file    (
        const std::string& filename,
        TreeSet<TreeType>& tree_set
    );

    template <class TreeType>
    bool from_string  (
        const std::string& tree_string,
        TreeSet<TreeType>& tree_set,
        const std::string& default_name = ""
    );

    template <class TreeType>
    bool from_files   (
        const std::vector<std::string>& filenames,
        TreeSet<TreeType>& tree_set
    );

    template <class TreeType>
    bool from_strings (
        const std::vector<std::string>& tree_strings,
        TreeSet<TreeType>& tree_set,
        const std::string& default_name = ""
    );

    // -----------------------------------------------------
    //     Internal
    // -----------------------------------------------------

protected:
    bool parse_tree  (
              NewickLexer::iterator& ct,
        const NewickLexer::iterator& end,
              NewickBroker&          broker
    );

    template <class TreeType>
    void build_tree (NewickBroker& broker, TreeType& tree);

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

public:
    std::string default_leaf_name     = "Leaf Node";
    std::string default_internal_name = "Internal Node";
    std::string default_root_name     = "Root Node";

    /**
     * @brief If set to true, unnamed nodes are named using one of the default names.
     *
     * The default names can be set using `default_leaf_name`, `default_internal_name` and
     * `default_root_name`. They are used both when parsing and printing a Newick file.
     */
    bool        use_default_names = false;

    // -------------------------------------------------------------------------
    //     Printing
    // -------------------------------------------------------------------------

public:
    template <class TreeType>
    bool to_file   (const TreeType& tree, const std::string fn);

    template <class TreeType>
    void to_string (const TreeType& tree, std::string& ts);

    template <class TreeType>
    std::string to_string (const TreeType& tree);

    // -----------------------------------------------------
    //     Internal
    // -----------------------------------------------------

protected:
    template <class TreeType>
    void to_broker (const TreeType& tree, NewickBroker& broker);

    std::string to_string_rec(const NewickBroker& broker, size_t position);
    std::string element_to_string(const NewickBrokerElement* bn);

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

public:
    bool print_names          = true;
    bool print_branch_lengths = false;
    bool print_comments       = false;
    bool print_tags           = false;

    /**
     * @brief The precision used for printing floating point numbers, particularly the branch_length.
     */
    int  precision = 6;
};

} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This class contains function templates, so do the inclusion here.
#include "tree/newick_processor.tpp"

#endif // include guard

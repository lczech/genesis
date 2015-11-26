#ifndef GENESIS_TREE_IO_NEWICK_PROCESSOR_H_
#define GENESIS_TREE_IO_NEWICK_PROCESSOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <assert.h>
#include <string>
#include <vector>

namespace genesis {

// =================================================================================================
//     Forward declarations
// =================================================================================================

template <class TreeType>
class  TreeSet;

class  NewickBroker;
struct NewickBrokerElement;

// =================================================================================================
//     Newick Processor
// =================================================================================================

template <typename AdapterType>
class NewickProcessor
{
public:

    typedef typename AdapterType::TreeType TreeType;

    // -------------------------------------------------------------------------
    //     Constructors
    // -------------------------------------------------------------------------

    NewickProcessor()                     : adapter_(AdapterType()) {}
    NewickProcessor(AdapterType& adapter) : adapter_(adapter)       {}

    // -------------------------------------------------------------------------
    //     Parsing
    // -------------------------------------------------------------------------

    bool from_file    (const std::string& filename,    TreeType& tree);
    bool from_string  (const std::string& tree_string, TreeType& tree);

    bool from_file    (
        const std::string& filename,
        TreeSet<TreeType>& tree_set
    );
    bool from_string  (
        const std::string& tree_string,
        TreeSet<TreeType>& tree_set,
        const std::string& default_name = ""
    );

    bool from_files   (
        const std::vector<std::string>& filenames,
        TreeSet<TreeType>& tree_set
    );

    bool from_strings (
        const std::vector<std::string>& tree_strings,
        TreeSet<TreeType>& tree_set,
        const std::string& default_name = ""
    );

    // -----------------------------------------------------
    //     Internal
    // -----------------------------------------------------

protected:

    bool build_tree (NewickBroker const& broker, TreeType& tree);

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

public:

    // -------------------------------------------------------------------------
    //     Printing
    // -------------------------------------------------------------------------

public:

    bool        to_file   (const TreeType& tree, const std::string fn);
    void        to_string (const TreeType& tree, std::string& ts);
    std::string to_string (const TreeType& tree);

    // -----------------------------------------------------
    //     Internal
    // -----------------------------------------------------

protected:
    void to_broker (const TreeType& tree, NewickBroker& broker);

    std::string to_string_rec(const NewickBroker& broker, size_t position);
    std::string element_to_string(NewickBrokerElement const& bn);

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

protected:
    AdapterType adapter_;
};

} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This is a class template, so do the inclusion here.
#include "tree/io/newick/processor.tpp"

#endif // include guard

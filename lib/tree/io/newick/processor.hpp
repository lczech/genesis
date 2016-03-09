#ifndef GENESIS_TREE_IO_NEWICK_PROCESSOR_H_
#define GENESIS_TREE_IO_NEWICK_PROCESSOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <string>
#include <vector>

namespace genesis {
namespace tree {

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

template <typename TreeType_>
class NewickProcessor
{

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

public:

    typedef TreeType_ TreeType;
    typedef typename TreeType::NodeType NodeType;
    typedef typename TreeType::EdgeType EdgeType;
    typedef typename TreeType::LinkType LinkType;

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

public:

    virtual ~NewickProcessor() {}

    // -------------------------------------------------------------------------
    //     Parsing
    // -------------------------------------------------------------------------

public:

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

protected:

    virtual void prepare_reading( NewickBroker const& broker, TreeType& tree );
    virtual void element_to_node( NewickBrokerElement const& element, NodeType& node );
    virtual void element_to_edge( NewickBrokerElement const& element, EdgeType& edge );
    virtual void finish_reading( NewickBroker const& broker, TreeType& tree );

    // -------------------------------------------------------------------------
    //     Printing
    // -------------------------------------------------------------------------

public:

    bool        to_file   (const TreeType& tree, const std::string fn);
    void        to_string (const TreeType& tree, std::string& ts);
    std::string to_string (const TreeType& tree);

protected:

    virtual void prepare_writing( TreeType const& tree, NewickBroker& broker );
    virtual void node_to_element( NodeType const& node, NewickBrokerElement& element );
    virtual void edge_to_element( EdgeType const& edge, NewickBrokerElement& element );
    virtual void finish_writing( TreeType const& tree, NewickBroker& broker );

    // -------------------------------------------------------------------------
    //     Internal Member Functions
    // -------------------------------------------------------------------------

private:

    void broker_to_tree (NewickBroker const& broker, TreeType& tree);
    void tree_to_broker (const TreeType& tree, NewickBroker& broker);

};

} // namespace tree
} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This is a class template, so do the inclusion here.
#include "tree/io/newick/processor.tpp"

#endif // include guard

#ifndef GENESIS_TREE_IO_NEWICK_READER_H_
#define GENESIS_TREE_IO_NEWICK_READER_H_

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
//     Newick Reader
// =================================================================================================

template <typename TreeType_>
class NewickReader
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

    NewickReader() = default;
    virtual ~NewickReader() {}

    NewickReader(NewickReader const&) = default;
    NewickReader(NewickReader&&)      = default;

    NewickReader& operator= (NewickReader const&) = default;
    NewickReader& operator= (NewickReader&&)      = default;

    // -------------------------------------------------------------------------
    //     Reading
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

    // -------------------------------------------------------------------------
    //     Virtual Parsing Helpers
    // -------------------------------------------------------------------------

protected:

    virtual void prepare_reading( NewickBroker const& broker, TreeType& tree );
    virtual void element_to_node( NewickBrokerElement const& element, NodeType& node );
    virtual void element_to_edge( NewickBrokerElement const& element, EdgeType& edge );
    virtual void finish_reading( NewickBroker const& broker, TreeType& tree );

    // -------------------------------------------------------------------------
    //     Internal Member Functions
    // -------------------------------------------------------------------------

private:

    void broker_to_tree (NewickBroker const& broker, TreeType& tree);

};

} // namespace tree
} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This is a class template, so do the inclusion here.
#include "tree/io/newick/reader.tpp"

#endif // include guard

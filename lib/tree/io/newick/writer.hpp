#ifndef GENESIS_TREE_IO_NEWICK_WRITER_H_
#define GENESIS_TREE_IO_NEWICK_WRITER_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <string>

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
//     Newick Writer
// =================================================================================================

template <typename TreeType_>
class NewickWriter
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

    NewickWriter() = default;
    virtual ~NewickWriter() {}

    NewickWriter(NewickWriter const&) = default;
    NewickWriter(NewickWriter&&)      = default;

    NewickWriter& operator= (NewickWriter const&) = default;
    NewickWriter& operator= (NewickWriter&&)      = default;

    // -------------------------------------------------------------------------
    //     Writing
    // -------------------------------------------------------------------------

public:

    void        to_file   (const TreeType& tree, const std::string filename);
    void        to_string (const TreeType& tree, std::string& ts);
    std::string to_string (const TreeType& tree);

    // -------------------------------------------------------------------------
    //     Virtual Printing Helpers
    // -------------------------------------------------------------------------

protected:

    virtual void prepare_writing( TreeType const& tree, NewickBroker& broker );
    virtual void node_to_element( NodeType const& node, NewickBrokerElement& element );
    virtual void edge_to_element( EdgeType const& edge, NewickBrokerElement& element );
    virtual void finish_writing( TreeType const& tree, NewickBroker& broker );

    // -------------------------------------------------------------------------
    //     Internal Member Functions
    // -------------------------------------------------------------------------

private:

    void tree_to_broker (const TreeType& tree, NewickBroker& broker);

};

} // namespace tree
} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This is a class template, so do the inclusion here.
#include "tree/io/newick/writer.tpp"

#endif // include guard

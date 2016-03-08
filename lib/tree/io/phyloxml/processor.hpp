#ifndef GENESIS_TREE_IO_PHYLOXML_PROCESSOR_H_
#define GENESIS_TREE_IO_PHYLOXML_PROCESSOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <string>

namespace genesis {

// =================================================================================================
//     Forward declarations
// =================================================================================================

namespace utils {
    class XmlDocument;
    class XmlElement;
}

// =================================================================================================
//     Phyloxml Processor
// =================================================================================================

template <typename TreeType_>
class PhyloxmlProcessor
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

    virtual ~PhyloxmlProcessor() {}

    // ---------------------------------------------------------------------
    //     Reading
    // ---------------------------------------------------------------------

protected:

    // virtual void prepare_reading( XmlDocument const& xml, TreeType& tree );
    // virtual void element_to_node( XmlElement const& element, NodeType& edge );
    // virtual void element_to_edge( XmlElement const& element, EdgeType& node );
    // virtual void finish_reading( XmlDocument const& xml, TreeType& tree );

    // ---------------------------------------------------------------------
    //     Writing
    // ---------------------------------------------------------------------

public:

    bool        to_file     (const TreeType& tree, const std::string fn);
    void        to_string   (const TreeType& tree, std::string& ts);
    std::string to_string   (const TreeType& tree);
    void        to_document (const TreeType& tree, utils::XmlDocument& xml);

protected:

    virtual void prepare_writing( TreeType const& tree, utils::XmlDocument& xml );
    virtual void node_to_element( NodeType const& node, utils::XmlElement&  element );
    virtual void edge_to_element( EdgeType const& edge, utils::XmlElement&  element );
    virtual void finish_writing(  TreeType const& tree, utils::XmlDocument& xml );

};

} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This is a class template, so do the inclusion here.
#include "tree/io/phyloxml/processor.tpp"

#endif // include guard

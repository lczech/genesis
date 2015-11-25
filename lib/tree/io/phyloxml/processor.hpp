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

class XmlDocument;
class XmlElement;

// =================================================================================================
//     Phyloxml Processor
// =================================================================================================

template <typename TreeType_>
class PhyloxmlProcessor
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    typedef TreeType_ TreeType;
    typedef typename TreeType::NodeType NodeType;
    typedef typename TreeType::EdgeType EdgeType;
    typedef typename TreeType::LinkType LinkType;

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    virtual ~PhyloxmlProcessor() {}

    // ---------------------------------------------------------------------
    //     Reading
    // ---------------------------------------------------------------------

protected:

    // virtual void prepare_reading( XmlDocument const& xml, TreeType& tree );
    // virtual void element_to_tree_node( XmlElement const& element, EdgeType& edge );
    // virtual void element_to_tree_edge( XmlElement const& element, NodeType& node );
    // virtual void finish_reading( XmlDocument const& xml, TreeType& tree );

    // ---------------------------------------------------------------------
    //     Writing
    // ---------------------------------------------------------------------

public:

    bool        to_file     (const TreeType& tree, const std::string fn);
    void        to_string   (const TreeType& tree, std::string& ts);
    std::string to_string   (const TreeType& tree);
    void        to_document (const TreeType& tree, XmlDocument& xml);

protected:

    virtual void prepare_writing( TreeType const& tree, XmlDocument& xml );
    virtual void tree_node_to_element( NodeType const& node, XmlElement& element );
    virtual void tree_edge_to_element( EdgeType const& edge, XmlElement& element );
    virtual void finish_writing( TreeType const& tree, XmlDocument& xml );

};

} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This is a class template, so do the inclusion here.
#include "tree/io/phyloxml/processor.tpp"

#endif // include guard

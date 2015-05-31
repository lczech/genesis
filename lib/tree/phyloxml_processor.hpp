#ifndef GENESIS_TREE_PHYLOXML_PROCESSOR_H_
#define GENESIS_TREE_PHYLOXML_PROCESSOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <string>

//~ #include "utils/lexer.hpp"

namespace genesis {

// =============================================================================
//     Forward declarations
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class  Tree;

class XmlDocument;

// =============================================================================
//     PhyloXML Processor
// =============================================================================

class PhyloXmlProcessor
{
public:

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

    template <class NodeDataType, class EdgeDataType>
    static bool to_file   (const std::string fn, const Tree<NodeDataType, EdgeDataType>& tree);

    template <class NodeDataType, class EdgeDataType>
    static void to_string (std::string& ts,      const Tree<NodeDataType, EdgeDataType>& tree);

    template <class NodeDataType, class EdgeDataType>
    static std::string to_string (const Tree<NodeDataType, EdgeDataType>& tree);

    template <class NodeDataType, class EdgeDataType>
    static void to_document (XmlDocument& xml, const Tree<NodeDataType, EdgeDataType>& tree);
};

} // namespace genesis

// =============================================================================
//     Inclusion of the implementation
// =============================================================================

// This class contains function templates, so do the inclusion here.
#include "tree/phyloxml_processor.tpp"

#endif // include guard

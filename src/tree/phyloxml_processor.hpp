#ifndef GNS_TREE_PHYLOXMLPROCESSOR_H_
#define GNS_TREE_PHYLOXMLPROCESSOR_H_

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
    static bool ToFile   (const std::string fn, Tree<NodeDataType, EdgeDataType>& tree);

    template <class NodeDataType, class EdgeDataType>
    static void ToString (std::string& ts,      Tree<NodeDataType, EdgeDataType>& tree);

    template <class NodeDataType, class EdgeDataType>
    static std::string ToString (Tree<NodeDataType, EdgeDataType>& tree);

    template <class NodeDataType, class EdgeDataType>
    static void ToDocument (XmlDocument& xml, Tree<NodeDataType, EdgeDataType>& tree);
};

} // namespace genesis

// =============================================================================
//     Inclusion of the implementation
// =============================================================================

// This class contains function templates, so do the inclusion here.
#include "tree/phyloxml_processor.tpp"

#endif // include guard

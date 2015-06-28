#ifndef GENESIS_TREE_PHYLOXML_PROCESSOR_H_
#define GENESIS_TREE_PHYLOXML_PROCESSOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <string>

#include "tree/phyloxml_adapter.hpp"

namespace genesis {

// =================================================================================================
//     Forward declarations
// =================================================================================================

template <class NodeDataType, class EdgeDataType>
class  Tree;

class XmlDocument;

// =================================================================================================
//     Phyloxml Processor
// =================================================================================================

class PhyloxmlProcessor
{
public:

    PhyloxmlProcessor()                         : adapter_(PhyloxmlAdapter()) {}
    PhyloxmlProcessor(PhyloxmlAdapter& adapter) : adapter_(adapter)           {}

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

    template <class NodeDataType, class EdgeDataType>
    bool to_file   (const std::string fn, const Tree<NodeDataType, EdgeDataType>& tree);

    template <class NodeDataType, class EdgeDataType>
    void to_string (std::string& ts,      const Tree<NodeDataType, EdgeDataType>& tree);

    template <class NodeDataType, class EdgeDataType>
    std::string to_string (const Tree<NodeDataType, EdgeDataType>& tree);

    template <class NodeDataType, class EdgeDataType>
    void to_document (XmlDocument& xml, const Tree<NodeDataType, EdgeDataType>& tree);

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

protected:
    PhyloxmlAdapter adapter_;
};

} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This class contains function templates, so do the inclusion here.
#include "tree/phyloxml_processor.tpp"

#endif // include guard

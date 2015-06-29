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

    template <class TreeType>
    bool to_file   (const std::string fn, const TreeType& tree);

    template <class TreeType>
    void to_string (std::string& ts,      const TreeType& tree);

    template <class TreeType>
    std::string to_string (const TreeType& tree);

    template <class TreeType>
    void to_document (XmlDocument& xml, const TreeType& tree);

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

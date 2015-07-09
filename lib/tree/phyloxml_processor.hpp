#ifndef GENESIS_TREE_PHYLOXML_PROCESSOR_H_
#define GENESIS_TREE_PHYLOXML_PROCESSOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <string>

#include "placement/phyloxml_adapter.hpp"
// #include "tree/phyloxml_adapter.hpp"

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

    PhyloxmlProcessor()                         : adapter_(PlacementTreePhyloxmlAdapter()) {}
    PhyloxmlProcessor(PlacementTreePhyloxmlAdapter& adapter) : adapter_(adapter)           {}

    // TODO !!! this classes uses the placement adapter only. this is just a short term solution to
    // make it work for now.

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

    template <class TreeType>
    bool to_file   (const TreeType& tree, const std::string fn);

    template <class TreeType>
    void to_string (const TreeType& tree, std::string& ts);

    template <class TreeType>
    std::string to_string (const TreeType& tree);

    template <class TreeType>
    void to_document (const TreeType& tree, XmlDocument& xml);

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

protected:
    PlacementTreePhyloxmlAdapter adapter_;
};

} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This class contains function templates, so do the inclusion here.
#include "tree/phyloxml_processor.tpp"

#endif // include guard

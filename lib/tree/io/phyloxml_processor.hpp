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

// =================================================================================================
//     Phyloxml Processor
// =================================================================================================

template <typename AdapterType>
class PhyloxmlProcessor
{
public:

    typedef typename AdapterType::TreeType TreeType;

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    PhyloxmlProcessor()                     : adapter_(AdapterType()) {}
    PhyloxmlProcessor(AdapterType& adapter) : adapter_(adapter)       {}

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

    bool        to_file     (const TreeType& tree, const std::string fn);
    void        to_string   (const TreeType& tree, std::string& ts);
    std::string to_string   (const TreeType& tree);
    void        to_document (const TreeType& tree, XmlDocument& xml);

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

protected:
    AdapterType adapter_;
};

} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This is a class template, so do the inclusion here.
#include "tree/io/phyloxml_processor.tpp"

#endif // include guard

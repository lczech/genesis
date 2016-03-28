#ifndef GENESIS_PLACEMENT_IO_PHYLOXML_WRITER_H_
#define GENESIS_PLACEMENT_IO_PHYLOXML_WRITER_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include "placement/placement_tree.hpp"
#include "tree/default/phyloxml_writer.hpp"
#include "tree/io/phyloxml/writer.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Placement Tree Phyloxml Writer
// =================================================================================================

typedef tree::DefaultTreePhyloxmlWriterMixin< tree::PhyloxmlWriter< PlacementTree > >
    PlacementTreePhyloxmlWriter;

} // namespace placement
} // namespace genesis

#endif // include guard

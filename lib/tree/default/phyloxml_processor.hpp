#ifndef GENESIS_TREE_DEFAULT_PHYLOXML_PROCESSOR_H_
#define GENESIS_TREE_DEFAULT_PHYLOXML_PROCESSOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "tree/default/tree.hpp"
#include "tree/default/phyloxml_mixin.hpp"
#include "tree/io/phyloxml/processor.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Default Tree Phyloxml Processor
// =================================================================================================

typedef DefaultTreePhyloxmlMixin<PhyloxmlProcessor<DefaultTree>> DefaultTreePhyloxmlProcessor;

} // namespace tree
} // namespace genesis

#endif // include guard

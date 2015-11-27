#ifndef GENESIS_TREE_DEFAULT_NEWICK_PROCESSOR_H_
#define GENESIS_TREE_DEFAULT_NEWICK_PROCESSOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "tree/default/tree.hpp"
#include "tree/default/newick_mixin.hpp"
#include "tree/io/newick/processor.hpp"

namespace genesis {

// =================================================================================================
//     Default Tree Newick Processor
// =================================================================================================

typedef DefaultTreeNewickMixin<NewickProcessor<DefaultTree>> DefaultTreeNewickProcessor;

} // namespace genesis

#endif // include guard

#ifndef GENESIS_H_
#define GENESIS_H_

/**
 * @brief This header includes all other genesis headers (*.hpp).
 * This makes it easy to integrate the code as a library, as all
 * symbols of genesis are available after including this header.
 *
 * You do not need to edit this file manually. Simply run the script
 * make_genesis_header.sh in ./tools to update this file.
 *
 * @file
 */

#include "alignment/io/fasta_lexer.hpp"
#include "alignment/io/fasta_processor.hpp"
#include "alignment/io/phylip_lexer.hpp"
#include "alignment/io/phylip_processor.hpp"
#include "alignment/sequence.hpp"
#include "alignment/sequence_set.hpp"

#include "placement/functions.hpp"
#include "placement/io/jplace_processor.hpp"
#include "placement/io/newick_adapter.hpp"
#include "placement/io/phyloxml_adapter.hpp"
#include "placement/io/serializer.hpp"
#include "placement/measures.hpp"
#include "placement/operators.hpp"
#include "placement/placement_map.hpp"
#include "placement/placement_map_set.hpp"
#include "placement/placement_tree.hpp"
#include "placement/pquery/distances.hpp"
#include "placement/pquery.hpp"
#include "placement/simulator.hpp"

#include "tree/bipartition/bipartition.hpp"
#include "tree/bipartition/bipartition_set.hpp"
#include "tree/default/distances.hpp"
#include "tree/default/functions.hpp"
#include "tree/default_tree.hpp"
#include "tree/default_tree_newick_adapter.hpp"
#include "tree/default_tree_phyloxml_adapter.hpp"
#include "tree/distances.hpp"
#include "tree/io/newick_adapter.hpp"
#include "tree/io/newick_broker.hpp"
#include "tree/io/newick_lexer.hpp"
#include "tree/io/newick_processor.hpp"
#include "tree/io/phyloxml_adapter.hpp"
#include "tree/io/phyloxml_processor.hpp"
#include "tree/iterators/eulertour.hpp"
#include "tree/iterators/inorder.hpp"
#include "tree/iterators/levelorder.hpp"
#include "tree/iterators/node_range.hpp"
#include "tree/iterators/nodes.hpp"
#include "tree/iterators/path.hpp"
#include "tree/iterators/postorder.hpp"
#include "tree/iterators/preorder.hpp"
#include "tree/operators.hpp"
#include "tree/tree_edge.hpp"
#include "tree/tree.hpp"
#include "tree/tree_link.hpp"
#include "tree/tree_node.hpp"
#include "tree/tree_node_iterator.hpp"
#include "tree/tree_set.hpp"
#include "tree/tree_view.hpp"

#include "utils/core/fs.hpp"
#include "utils/core/genesis.hpp"
#include "utils/core/logging.hpp"
#include "utils/core/options.hpp"
#include "utils/core/std.hpp"
#include "utils/io/json_document.hpp"
#include "utils/io/json_lexer.hpp"
#include "utils/io/json_processor.hpp"
#include "utils/io/lexer.hpp"
#include "utils/io/lexer_iterator.hpp"
#include "utils/io/lexer_token.hpp"
#include "utils/io/serializer.hpp"
#include "utils/io/xml_document.hpp"
#include "utils/io/xml_processor.hpp"
#include "utils/math/bitvector.hpp"
#include "utils/math/histogram/accumulator.hpp"
#include "utils/math/histogram/distances.hpp"
#include "utils/math/histogram.hpp"
#include "utils/math/histogram/operations.hpp"
#include "utils/math/histogram/operators.hpp"
#include "utils/math/histogram/stats.hpp"
#include "utils/math/matrix.hpp"
#include "utils/math/matrix/operators.hpp"
#include "utils/string/string.hpp"
#include "utils/tools/color/gradient.hpp"
#include "utils/tools/color.hpp"
#include "utils/tools/color/operators.hpp"
#include "utils/tools/date_time.hpp"

#endif // include guard

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

#include "alignment/fasta_lexer.hpp"
#include "alignment/fasta_processor.hpp"
#include "alignment/phylip_lexer.hpp"
#include "alignment/phylip_processor.hpp"
#include "alignment/sequence.hpp"
#include "alignment/sequence_set.hpp"

#include "placement/functions.hpp"
#include "placement/jplace_processor.hpp"
#include "placement/measures.hpp"
#include "placement/newick_adapter.hpp"
#include "placement/phyloxml_adapter.hpp"
#include "placement/placement_map.hpp"
#include "placement/placement_map_set.hpp"
#include "placement/placement_tree.hpp"
#include "placement/pquery.hpp"
#include "placement/serializer.hpp"
#include "placement/simulator.hpp"

#include "tree/default_tree.hpp"
#include "tree/default_tree_newick_adapter.hpp"
#include "tree/default_tree_phyloxml_adapter.hpp"
#include "tree/distances.hpp"
#include "tree/newick_adapter.hpp"
#include "tree/newick_broker.hpp"
#include "tree/newick_lexer.hpp"
#include "tree/newick_processor.hpp"
#include "tree/phyloxml_adapter.hpp"
#include "tree/phyloxml_processor.hpp"
#include "tree/tree.hpp"
#include "tree/tree_edge.hpp"
#include "tree/iterators.hpp"
#include "tree/tree_link.hpp"
#include "tree/tree_node.hpp"
#include "tree/tree_node_iterator.hpp"
#include "tree/tree_set.hpp"
#include "tree/tree_view.hpp"

#include "utils/color.hpp"
#include "utils/json_document.hpp"
#include "utils/json_lexer.hpp"
#include "utils/json_processor.hpp"
#include "utils/lexer.hpp"
#include "utils/lexer_iterator.hpp"
#include "utils/lexer_token.hpp"
#include "utils/logging.hpp"
#include "utils/options.hpp"
#include "utils/serializer.hpp"
#include "utils/utils.hpp"
#include "utils/xml_document.hpp"
#include "utils/xml_processor.hpp"

#endif // include guard

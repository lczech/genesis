#ifndef GENESIS_H_
#define GENESIS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief This header includes all other genesis headers (*.hpp).
 * This makes it easy to integrate the code as a library, as all
 * symbols of genesis are available after including this header.
 *
 * You do not need to edit this file manually. Simply run the script
 * make_genesis_header.sh in ./tools to update this file.
 */

#include "placement/formats/edge_color.hpp"
#include "placement/formats/jplace_reader.hpp"
#include "placement/formats/jplace_writer.hpp"
#include "placement/formats/newick_reader.hpp"
#include "placement/formats/newick_writer.hpp"
#include "placement/formats/phyloxml_writer.hpp"
#include "placement/formats/serializer.hpp"
#include "placement/function/functions.hpp"
#include "placement/function/helper.hpp"
#include "placement/function/measures.hpp"
#include "placement/function/operators.hpp"
#include "placement/function/sample_set.hpp"
#include "placement/placement_tree.hpp"
#include "placement/pquery/distances.hpp"
#include "placement/pquery.hpp"
#include "placement/pquery/name.hpp"
#include "placement/pquery/placement.hpp"
#include "placement/pquery/plain.hpp"
#include "placement/sample.hpp"
#include "placement/sample_set.hpp"
#include "placement/simulator/edge_distribution.hpp"
#include "placement/simulator/extra_placement_distribution.hpp"
#include "placement/simulator/functions.hpp"
#include "placement/simulator/like_weight_ratio_distribution.hpp"
#include "placement/simulator/pendant_length_distribution.hpp"
#include "placement/simulator/proximal_length_distribution.hpp"
#include "placement/simulator/simulator.hpp"

#include "sequence/codes.hpp"
#include "sequence/formats/fasta_reader.hpp"
#include "sequence/formats/fasta_writer.hpp"
#include "sequence/formats/phylip_reader.hpp"
#include "sequence/formats/phylip_writer.hpp"
#include "sequence/functions.hpp"
#include "sequence/sequence.hpp"
#include "sequence/sequence_set.hpp"

#include "taxonomy/functions.hpp"
#include "taxonomy/taxon.hpp"
#include "taxonomy/taxonomy.hpp"

#include "tree/bipartition/bipartition.hpp"
#include "tree/bipartition/bipartition_set.hpp"
#include "tree/default/distances.hpp"
#include "tree/default/edge_color.hpp"
#include "tree/default/functions.hpp"
#include "tree/default/newick_reader.hpp"
#include "tree/default/newick_writer.hpp"
#include "tree/default/phyloxml_writer.hpp"
#include "tree/default/tree.hpp"
#include "tree/formats/color_writer_mixin.hpp"
#include "tree/formats/newick/broker.hpp"
#include "tree/formats/newick/color_writer_mixin.hpp"
#include "tree/formats/newick/element.hpp"
#include "tree/formats/newick/lexer.hpp"
#include "tree/formats/newick/parser.hpp"
#include "tree/formats/newick/reader.hpp"
#include "tree/formats/newick/writer.hpp"
#include "tree/formats/phyloxml/color_writer_mixin.hpp"
#include "tree/formats/phyloxml/writer.hpp"
#include "tree/function/distances.hpp"
#include "tree/function/emd.hpp"
#include "tree/function/emd_tree.hpp"
#include "tree/function/functions.hpp"
#include "tree/function/operators.hpp"
#include "tree/function/tree_set.hpp"
#include "tree/iterator/eulertour.hpp"
#include "tree/iterator/inorder.hpp"
#include "tree/iterator/levelorder.hpp"
#include "tree/iterator/node_links.hpp"
#include "tree/iterator/path.hpp"
#include "tree/iterator/postorder.hpp"
#include "tree/iterator/preorder.hpp"
#include "tree/printer/compact.hpp"
#include "tree/printer/detailed.hpp"
#include "tree/printer/table.hpp"
#include "tree/tree_edge.hpp"
#include "tree/tree.hpp"
#include "tree/tree_link.hpp"
#include "tree/tree_node.hpp"
#include "tree/tree_set.hpp"

#include "utils/core/fs.hpp"
#include "utils/core/genesis.hpp"
#include "utils/core/indexed_list.hpp"
#include "utils/core/logging.hpp"
#include "utils/core/options.hpp"
#include "utils/core/range.hpp"
#include "utils/core/std.hpp"
#include "utils/formats/csv/reader.hpp"
#include "utils/formats/json/document.hpp"
#include "utils/formats/json/lexer.hpp"
#include "utils/formats/json/reader.hpp"
#include "utils/formats/json/writer.hpp"
#include "utils/formats/nexus/block.hpp"
#include "utils/formats/nexus/document.hpp"
#include "utils/formats/nexus/taxa.hpp"
#include "utils/formats/nexus/trees.hpp"
#include "utils/formats/nexus/writer.hpp"
#include "utils/formats/xml/document.hpp"
#include "utils/formats/xml/writer.hpp"
#include "utils/io/counting_istream.hpp"
#include "utils/io/lexer.hpp"
#include "utils/io/lexer_iterator.hpp"
#include "utils/io/lexer_token.hpp"
#include "utils/io/parser.hpp"
#include "utils/io/scanner.hpp"
#include "utils/io/serializer.hpp"
#include "utils/math/bitvector.hpp"
#include "utils/math/histogram/accumulator.hpp"
#include "utils/math/histogram/distances.hpp"
#include "utils/math/histogram.hpp"
#include "utils/math/histogram/operations.hpp"
#include "utils/math/histogram/operators.hpp"
#include "utils/math/histogram/stats.hpp"
#include "utils/math/matrix.hpp"
#include "utils/math/matrix/operators.hpp"
#include "utils/text/char.hpp"
#include "utils/text/string.hpp"
#include "utils/text/style.hpp"
#include "utils/text/table.hpp"
#include "utils/tools/char_lookup.hpp"
#include "utils/tools/color/gradient.hpp"
#include "utils/tools/color.hpp"
#include "utils/tools/color/names.hpp"
#include "utils/tools/color/operators.hpp"
#include "utils/tools/date_time.hpp"

#endif // include guard

#ifndef GENESIS_TREE_H_
#define GENESIS_TREE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief This header includes all headers in 'tree'.
 * This makes it easy to integrate the code as a library, as all
 * symbols of these headers are available after including this header.
 *
 * You do not need to edit this file manually. Simply run the script
 * make_genesis_header.sh in ./tools/deploy to update this file.
 */

#include "genesis/tree/attribute_tree/indexed_newick_reader.hpp"
#include "genesis/tree/attribute_tree/keyed_newick_reader.hpp"
#include "genesis/tree/attribute_tree/tree.hpp"
#include "genesis/tree/bipartition/bipartition.hpp"
#include "genesis/tree/bipartition/functions.hpp"
#include "genesis/tree/bipartition/rf.hpp"
#include "genesis/tree/common_tree/distances.hpp"
#include "genesis/tree/common_tree/edge_color.hpp"
#include "genesis/tree/common_tree/functions.hpp"
#include "genesis/tree/common_tree/newick_reader.hpp"
#include "genesis/tree/common_tree/newick_writer.hpp"
#include "genesis/tree/common_tree/operators.hpp"
#include "genesis/tree/common_tree/phyloxml_writer.hpp"
#include "genesis/tree/common_tree/tree.hpp"
#include "genesis/tree/drawing/circular_layout.hpp"
#include "genesis/tree/drawing/functions.hpp"
#include "genesis/tree/drawing/heat_tree.hpp"
#include "genesis/tree/drawing/layout_base.hpp"
#include "genesis/tree/drawing/layout_tree.hpp"
#include "genesis/tree/drawing/rectangular_layout.hpp"
#include "genesis/tree/formats/color_writer_plugin.hpp"
#include "genesis/tree/formats/newick/broker.hpp"
#include "genesis/tree/formats/newick/color_writer_plugin.hpp"
#include "genesis/tree/formats/newick/element.hpp"
#include "genesis/tree/formats/newick/input_iterator.hpp"
#include "genesis/tree/formats/newick/reader.hpp"
#include "genesis/tree/formats/newick/writer.hpp"
#include "genesis/tree/formats/phyloxml/color_writer_plugin.hpp"
#include "genesis/tree/formats/phyloxml/writer.hpp"
#include "genesis/tree/function/distances.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/function/lca_lookup.hpp"
#include "genesis/tree/function/manipulation.hpp"
#include "genesis/tree/function/operators.hpp"
#include "genesis/tree/function/tree_set.hpp"
#include "genesis/tree/iterator/eulertour.hpp"
#include "genesis/tree/iterator/levelorder.hpp"
#include "genesis/tree/iterator/node_links.hpp"
#include "genesis/tree/iterator/path.hpp"
#include "genesis/tree/iterator/path_set.hpp"
#include "genesis/tree/iterator/postorder.hpp"
#include "genesis/tree/iterator/preorder.hpp"
#include "genesis/tree/mass_tree/balances.hpp"
#include "genesis/tree/mass_tree/emd.hpp"
#include "genesis/tree/mass_tree/functions.hpp"
#include "genesis/tree/mass_tree/kmeans.hpp"
#include "genesis/tree/mass_tree/phylo_factor_colors.hpp"
#include "genesis/tree/mass_tree/phylo_factor.hpp"
#include "genesis/tree/mass_tree/phylo_ilr.hpp"
#include "genesis/tree/mass_tree/squash_clustering.hpp"
#include "genesis/tree/mass_tree/tree.hpp"
#include "genesis/tree/printer/compact.hpp"
#include "genesis/tree/printer/detailed.hpp"
#include "genesis/tree/printer/table.hpp"
#include "genesis/tree/tree/edge_data.hpp"
#include "genesis/tree/tree/edge.hpp"
#include "genesis/tree/tree.hpp"
#include "genesis/tree/tree/link.hpp"
#include "genesis/tree/tree/node_data.hpp"
#include "genesis/tree/tree/node.hpp"
#include "genesis/tree/tree_set.hpp"
#include "genesis/tree/tree/subtree.hpp"

#endif // include guard

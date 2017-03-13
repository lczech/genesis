/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/formats/newick/writer.hpp"

#include "genesis/tree/tree_set.hpp"
#include "genesis/tree/tree.hpp"

#include "genesis/tree/formats/newick/broker.hpp"
#include "genesis/tree/formats/newick/parser.hpp"
#include "genesis/tree/function/distances.hpp"
#include "genesis/tree/iterator/postorder.hpp"

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/output_stream.hpp"

#include <assert.h>
#include <deque>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Printing
// =================================================================================================

void NewickWriter::to_file (
    Tree const& tree, const std::string filename
) {
    std::string ts;
    to_string(tree, ts);
    utils::file_write(ts, filename);
}

void NewickWriter::to_string (
    Tree const& tree, std::string& ts
) {
    ts = to_string(tree);
}

std::string NewickWriter::to_string (Tree const& tree)
{
    NewickBroker broker;
    tree_to_broker_(tree, broker);
    broker.assign_ranks();
    return generate_newick_tree(broker);
}

void NewickWriter::tree_to_broker_ (
    Tree const& tree, NewickBroker& broker
) {
    for( auto const& prepare_plugin : prepare_writing_plugins ) {
        prepare_plugin( tree, broker );
    }

    // store the depth from each node to the root. this is needed to assign levels of depth
    // to the nodes for the broker.
    auto depth = node_path_length_vector(tree);

    // now fill the broker with nodes via postorder traversal, so that the root is put on top last.
    broker.clear();
    for( auto it : postorder(tree) ) {
        NewickBrokerElement bn;
        bn.depth = depth[ it.node().index() ];

        for( auto const& node_plugin : node_to_element_plugins ) {
            node_plugin( it.node(), bn );
        }
        // only write edge data to the broker element if it is not the last iteration.
        // the last iteration is the root, which usually does not have edge information in newick.
        // caveat: for the root node, the edge will point to an arbitrary edge away from the root.
        if( !it.is_last_iteration() ) {
            for( auto const& edge_plugin : edge_to_element_plugins ) {
                edge_plugin( it.edge(), bn );
            }
        }

        broker.push_top(bn);
    }

    for( auto const& finish_plugin : finish_writing_plugins ) {
        finish_plugin( tree, broker );
    }
}

} // namespace tree
} // namespace genesis

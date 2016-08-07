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
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "tree/formats/newick/writer.hpp"

#include "tree/tree_set.hpp"
#include "tree/tree.hpp"

#include "tree/formats/newick/broker.hpp"
#include "tree/formats/newick/parser.hpp"
#include "tree/function/distances.hpp"
#include "tree/iterator/postorder.hpp"

#include "utils/core/fs.hpp"
#include "utils/core/logging.hpp"
#include "utils/core/std.hpp"

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

/**
 * @brief Writes the tree to a file in Newick format.
 *
 * If the file already exists, the function throws `std::runtime_error`.
 * The function uses utils::file_write. See there for other exceptions that can be thrown.
 */
void NewickWriter::to_file (
    Tree const& tree, const std::string filename
) {
    if( utils::file_exists(filename) ) {
        throw std::runtime_error( "Newick file '" + filename + "' already exist." );
    }
    std::string ts;
    to_string(tree, ts);
    utils::file_write(ts, filename);
}

/**
 * @brief Gives a Newick string representation of the tree.
 *
 * In case the tree was read from a Newick file, this function should produce the same
 * representation.
 */
void NewickWriter::to_string (
    Tree const& tree, std::string& ts
) {
    ts = to_string(tree);
}

/**
 * @brief Returns a Newick string representation of the tree.
 *
 * In case the tree was read from a Newick file, this function should produce the same
 * representation.
 */
std::string NewickWriter::to_string (Tree const& tree)
{
    NewickBroker broker;
    tree_to_broker(tree, broker);
    broker.assign_ranks();
    return generate_newick_tree(broker);
}

/**
 * @brief Stores the information of the tree into a NewickBroker object.
 */
void NewickWriter::tree_to_broker (
    Tree const& tree, NewickBroker& broker
) {
    prepare_writing(tree, broker);

    // store the depth from each node to the root. this is needed to assign levels of depth
    // to the nodes for the broker.
    auto depth = node_path_length_vector(tree);

    // now fill the broker with nodes via postorder traversal, so that the root is put on top last.
    broker.clear();
    for( auto it : postorder(tree) ) {
        NewickBrokerElement bn;
        bn.depth = depth[ it.node().index() ];

        node_to_element( it.node(), bn );
        // only write edge data to the broker element if it is not the last iteration.
        // the last iteration is the root, which usually does not have edge information in newick.
        // caveat: for the root node, the edge will point to an arbitrary edge away from the root.
        if (!it.is_last_iteration()) {
            edge_to_element( it.edge(), bn );
        }

        broker.push_top(bn);
    }

    finish_writing(tree, broker);
}

// -------------------------------------------------------------------------
//     Virtual Methods
// -------------------------------------------------------------------------

void NewickWriter::prepare_writing( Tree const& tree, NewickBroker& broker )
{
    // Silence unused parameter warnings.
    (void) tree;
    (void) broker;
}

void NewickWriter::node_to_element( TreeNode const& node, NewickBrokerElement& element )
{
    // Silence unused parameter warnings.
    (void) node;
    (void) element;
}

void NewickWriter::edge_to_element( TreeEdge const& edge, NewickBrokerElement& element )
{
    // Silence unused parameter warnings.
    (void) edge;
    (void) element;
}

void NewickWriter::finish_writing( Tree const& tree, NewickBroker& broker )
{
    // Silence unused parameter warnings.
    (void) tree;
    (void) broker;
}

} // namespace tree
} // namespace genesis

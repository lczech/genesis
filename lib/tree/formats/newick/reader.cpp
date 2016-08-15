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

#include "tree/formats/newick/reader.hpp"

#include "tree/tree.hpp"

#include "tree/formats/newick/broker.hpp"
#include "tree/formats/newick/parser.hpp"
#include "tree/tree_set.hpp"

#include "utils/core/fs.hpp"
#include "utils/core/logging.hpp"
#include "utils/core/std.hpp"

#include <assert.h>
#include <deque>
#include <memory>
#include <sstream>
#include <stdexcept>

namespace genesis {
namespace tree {

// =================================================================================================
//     Reading
// =================================================================================================

// -------------------------------------------------------------------------
//     Public Methods
// -------------------------------------------------------------------------

/**
 * @brief Create a Tree from a file containing a Newick tree.
 *
 * Returns true iff successful.
 */
bool NewickReader::from_file (
    const std::string& fn, Tree& tree
) {
    if (!utils::file_exists(fn)) {
        throw std::runtime_error( "Newick file '" + fn + "' does not exist." );
    }
    return from_string( utils::file_read(fn), tree );
}

/**
 * @brief Create a Tree from a string containing a Newick tree.
 *
 * Returns true iff successful.
 */
bool NewickReader::from_string (
    const std::string& ts, Tree& tree
) {
    // run the lexer
    NewickLexer lexer;
    if (!lexer.from_string(ts)) {
        return false;
    }

    // check for lexing errors
    if (lexer.empty()) {
        LOG_INFO << "Tree is empty. Nothing done.";
        return false;
    }
    if (lexer.has_error()) {
        throw std::runtime_error(
            "Lexing error at " + lexer.back().at() + " with message: " + lexer.back().value()
        );
    }

    // parse the tree from lexer into a tree broker
    auto ct  = lexer.begin();
    NewickBroker broker;
    if (!parse_newick_tree(ct, lexer.end(), broker)) {
        return false;
    }

    // see if there is anything other than a comment left
    while (ct != lexer.end()) {
        if (!ct->is_comment()) {
            LOG_WARN << "Tree contains more data after the semicolon.";
            return false;
        }
        ++ct;
    }

    // build the tree from the broker
    broker_to_tree(broker, tree);
    return true;
}

/**
 * @brief Fill a TreeSet from a file containing a list of Newick trees.
 *
 * See from_string() for information on the syntax of this file.
 * The tree names are taken from the content if availabe. Unnamed trees will be prefixed by the
 * file name.
 *
 * Returns true iff successful.
 */
bool NewickReader::from_file (
    const std::string& fn, TreeSet& tset
) {
    if (!utils::file_exists(fn)) {
        throw std::runtime_error( "Tree file '" + fn + "' does not exist." );
    }

    return from_string(
        utils::file_read(fn),
        tset,
        utils::file_filename( utils::file_basename(fn) ) + "_"
    );
}

/**
 * @brief Fill a TreeSet from a string containing a list of Newick trees.
 *
 * These trees can either be named or unnamed, using this syntax:
 *
 *     Tree_A = (...);
 *     'Tree B'=(...);
 *     (...);
 *
 * where the first two lines are named trees and the third line is an unnamed tree.
 * The trees do not have to be on distinct lines of the input, as whitespaces are completely
 * stripped during the lexing phase. However, they are required to end with a semicolon `;`.
 *
 * In case of unnamed trees, a `default_name` can be provided, which will be appended by a counter
 * that counts up all unnamed trees. If no default name is given, the trees will simpye be named
 * using the counter itself.
 *
 * Returns true iff successful.
 */
bool NewickReader::from_string (
    const std::string& ts,
    TreeSet& tset,
    const std::string& default_name
) {
    // Run the Lexer.
    NewickLexer lexer;
    if (!lexer.from_string(ts)) {
        return false;
    }

    // check for lexing errors
    if (lexer.empty()) {
        LOG_INFO << "Tree is empty. Nothing done.";
        return false;
    }
    if (lexer.has_error()) {
        throw std::runtime_error(
            "Lexing error at " + lexer.back().at() + " with message: " + lexer.back().value()
        );
    }

    // Store error message. Also serves as flag whether an error occured.
    std::string error = "";

    // Count how many unnamed trees we have seen.
    size_t unnamed_ctr = 0;

    auto ct = lexer.begin();
    while (ct != lexer.end()) {
        if (ct->is_unknown()) {
            error = "Invalid characters at " + ct->at() + ": '" + ct->value() + "'.";
            break;
        }

        if (ct->is_comment()) {
            continue;
        }

        // Store the name of the current tree; if there is none, use empty string.
        std::string name = "";
        if (ct->is_symbol() || ct->is_string()) {
            name = ct->value();
            ++ct;

            if (ct == lexer.end()) {
                error = "Unexpected end at " + ct->at() + ".";
                break;
            }

            if (!ct->is_operator("=")) {
                error = "Invalid character '" + ct->value() + "' at " + ct->at() + ".";
                break;
            }
            ++ct;

            if (ct == lexer.end()) {
                error = "Unexpected end of tree at " + ct->at() + ".";
                break;
            }
        }

        if (!ct->is_bracket("(")) {
            error = "Invalid character at " + ct->at() + ".";
            break;
        }

        // Parse the tree from Lexer into a TreeBroker.
        NewickBroker broker;
        if (!parse_newick_tree(ct, lexer.end(), broker)) {
            return false;
        }

        auto tree = Tree();
        broker_to_tree(broker, tree);

        if (name.empty()) {
            name = default_name + std::to_string(unnamed_ctr++);
        }
        tset.add(name, tree);

        // Let's clean up all tokens used so far. We don't need them anymore.
        // TODO FIXME Hm. Somehow this may cause a segfault. Need to investigate. Or rewrite!
        // ct.consume_head();
    }

    if (!error.empty()) {
        throw std::runtime_error( error );
    }

    return true;
}

/**
 * @brief Fill a TreeSet from a list of files containing Newick trees.
 *
 * Returns true iff successful.
 */
bool NewickReader::from_files (
    const std::vector<std::string>& fns, TreeSet& set
) {
    for (auto fn : fns) {
        if (!from_file (fn, set)) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Fill a TreeSet from a list of strings containing Newick trees.
 *
 * Returns true iff successful.
 */
bool NewickReader::from_strings (
    const std::vector<std::string>& tss,
    TreeSet& set,
    const std::string& default_name
) {
    for (auto ts : tss) {
        if (!from_string (ts, set, default_name)) {
            return false;
        }
    }
    return true;
}

// -------------------------------------------------------------------------
//     Virtual Parsing Helpers
// -------------------------------------------------------------------------

void NewickReader::prepare_reading( NewickBroker const& broker, Tree& tree )
{
    // Silence unused parameter warnings.
    (void) broker;
    (void) tree;
}

void NewickReader::element_to_node( NewickBrokerElement const& element, TreeNode& node )
{
    // Silence unused parameter warnings.
    (void) element;
    (void) node;
}

void NewickReader::element_to_edge( NewickBrokerElement const& element, TreeEdge& edge )
{
    // Silence unused parameter warnings.
    (void) element;
    (void) edge;
}

void NewickReader::finish_reading( NewickBroker const& broker, Tree& tree )
{
    // Silence unused parameter warnings.
    (void) broker;
    (void) tree;
}

// -------------------------------------------------------------------------
//     Internal Helper Methods
// -------------------------------------------------------------------------

/**
 * @brief Build a Tree from a NewickBroker.
 */
void NewickReader::broker_to_tree (
    NewickBroker const& broker, Tree& tree
) {
    typename Tree::LinkContainer links;
    typename Tree::NodeContainer nodes;
    typename Tree::EdgeContainer edges;

    std::vector< TreeLink* > link_stack;

    // we need the ranks (number of immediate children) of all nodes
    broker.assign_ranks();
    prepare_reading(broker, tree);

    // iterate over all nodes of the tree broker
    for (auto b_itr = broker.cbegin(); b_itr != broker.cend(); ++b_itr) {
        NewickBrokerElement const& broker_node = *b_itr;

        // create the tree node for this broker node
        auto cur_node_u  = utils::make_unique< TreeNode >();
        auto cur_node    = cur_node_u.get();
        cur_node->reset_index( nodes.size() );
        element_to_node( broker_node, *cur_node );
        nodes.push_back(std::move(cur_node_u));

        // create the link that points towards the root.
        // this link is created for every node, root, inner and leaves.
        auto up_link_u  = utils::make_unique< TreeLink >();
        auto up_link    = up_link_u.get();
        up_link->reset_node( cur_node );
        cur_node->reset_primary_link( up_link );
        up_link->reset_index( links.size() );
        links.push_back(std::move(up_link_u));

        // establish the link towards the root
        if (link_stack.empty()) {
            // if the link stack is empty, we are currently at the very beginning of this loop,
            // which means we are at the root itself. in this case, make the "link towards the root"
            // point to itself.
            up_link->reset_outer( up_link );
        } else {
            // if we are however in some other node (leaf or inner, but not the root), we establish
            // the link "upwards" to the root, and back from there.
            up_link->reset_outer( link_stack.back() );
            link_stack.back()->reset_outer( up_link );

            // also, create an edge that connects both nodes
            auto up_edge = utils::make_unique< TreeEdge >(
                edges.size(),
                link_stack.back(),
                up_link
            );

            up_link->reset_edge( up_edge.get() );
            link_stack.back()->reset_edge( up_edge.get() );
            element_to_edge( broker_node, *up_edge );
            edges.push_back(std::move(up_edge));

            // we can now delete the head of the stack, because we just estiablished its "downlink"
            // and thus are done with it
            link_stack.pop_back();
        }

        // in the following, we create the links that will connect to the nodes' children.
        // for leaf nodes, this makes the next pointer point to the node itself (the loop
        // is never executed in this case, as leaves have rank 0).
        // for inner nodes, we create as many "down" links as they have children. each of them
        // is pushed to the stack, so that for the next broker nodes they are available as
        // reciever for the "up" links.
        // in summary, make all next pointers of a node point to each other in a circle.
        auto prev_link = up_link;
        for (int i = 0; i < broker_node.rank(); ++i) {
            auto down_link = utils::make_unique< TreeLink >();
            prev_link->reset_next( down_link.get() );
            prev_link = down_link.get();

            down_link->reset_node( cur_node );
            down_link->reset_index( links.size() );
            link_stack.push_back(down_link.get());
            links.push_back(std::move(down_link));
        }
        prev_link->reset_next( up_link );
    }

    // we pushed elements to the link_stack for all children of the nodes and popped them when we
    // were done processing those children, so there should be no elements left. this assumes that
    // NewickBroker.assign_ranks() does its job properly!
    assert(link_stack.empty());

    // now delete the uplink of the root, in order to make the tree fully unrooted.
    // (we do that after the tree creation, as it is way easier this way)
    assert( &links.front()->outer() == links.front().get() );
    auto next = &links.front()->next();
    while( &next->next() != links.front().get() ) {
        next = &next->next();
    }
    next->reset_next( &next->next().next() );
    assert( &next->next() == &links.front()->next() );
    links.erase(links.begin());
    for (size_t i = 0; i < links.size(); ++i) {
        links[i]->reset_index(i);
    }
    next->node().reset_primary_link( &next->next() );

    // Finish and hand over the elements to the tree.
    finish_reading(broker, tree);
    tree.import_content(links, nodes, edges);
}

} // namespace tree
} // namespace genesis

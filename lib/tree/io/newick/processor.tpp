/**
 * @brief Implementation of functions for reading and writing Newick files.
 *
 * @file
 * @ingroup tree
 */

#include <assert.h>
#include <deque>
#include <memory>
#include <sstream>
#include <stdexcept>

#include "tree/function/distances.hpp"
#include "tree/io/newick/broker.hpp"
#include "tree/io/newick/parser.hpp"
#include "tree/iterator/postorder.hpp"
#include "tree/tree_set.hpp"
#include "tree/tree.hpp"
#include "utils/core/fs.hpp"
#include "utils/core/logging.hpp"
#include "utils/core/std.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Parsing
// =================================================================================================

// -------------------------------------------------------------------------
//     Public Methods
// -------------------------------------------------------------------------

/**
 * @brief Create a Tree from a file containing a Newick tree.
 *
 * Returns true iff successful.
 */
template <typename TreeType>
bool NewickProcessor<TreeType>::from_file (
    const std::string& fn, TreeType& tree
) {
    if (!utils::file_exists(fn)) {
        LOG_WARN << "Newick file '" << fn << "' does not exist.";
        return false;
    }
    return from_string( utils::file_read(fn), tree );
}

/**
 * @brief Create a Tree from a string containing a Newick tree.
 *
 * Returns true iff successful.
 */
template <typename TreeType>
bool NewickProcessor<TreeType>::from_string (
    const std::string& ts, TreeType& tree
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
        LOG_WARN << "Lexing error at " << lexer.back().at()
                 << " with message: " << lexer.back().value();
        return false;
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
template <typename TreeType>
bool NewickProcessor<TreeType>::from_file (
    const std::string& fn, TreeSet<TreeType>& tset
) {
    if (!utils::file_exists(fn)) {
        LOG_WARN << "Tree file '" << fn << "' does not exist.";
        return false;
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
template <typename TreeType>
bool NewickProcessor<TreeType>::from_string (
    const std::string& ts,
    TreeSet<TreeType>& tset,
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
        LOG_WARN << "Lexing error at " << lexer.back().at()
                 << " with message: " << lexer.back().value();
        return false;
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

        auto tree = new TreeType();
        broker_to_tree(broker, &tree);

        if (name.empty()) {
            name = default_name + std::to_string(unnamed_ctr++);
        }
        tset.add(name, tree);

        // Let's clean up all tokens used so far. We don't need them anymore.
        ct.consume_head();
    }

    if (!error.empty()) {
        LOG_WARN << error;
        return false;
    }

    return true;
}

/**
 * @brief Fill a TreeSet from a list of files containing Newick trees.
 *
 * Returns true iff successful.
 */
template <typename TreeType>
bool NewickProcessor<TreeType>::from_files (
    const std::vector<std::string>& fns, TreeSet<TreeType>& set
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
template <typename TreeType>
bool NewickProcessor<TreeType>::from_strings (
    const std::vector<std::string>& tss,
    TreeSet<TreeType>& set,
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
//     Virtual Methods
// -------------------------------------------------------------------------

template <typename TreeType>
void NewickProcessor<TreeType>::prepare_reading( NewickBroker const& broker, TreeType& tree )
{
    // Silence unused parameter warnings.
    (void) broker;
    (void) tree;
}

template <typename TreeType>
void NewickProcessor<TreeType>::element_to_node( NewickBrokerElement const& element, NodeType& node )
{
    // Silence unused parameter warnings.
    (void) element;
    (void) node;
}

template <typename TreeType>
void NewickProcessor<TreeType>::element_to_edge( NewickBrokerElement const& element, EdgeType& edge )
{
    // Silence unused parameter warnings.
    (void) element;
    (void) edge;
}

template <typename TreeType>
void NewickProcessor<TreeType>::finish_reading( NewickBroker const& broker, TreeType& tree )
{
    // Silence unused parameter warnings.
    (void) broker;
    (void) tree;
}

// -------------------------------------------------------------------------
//     Internal Helper Methods
// -------------------------------------------------------------------------

/**
 * @brief Builds a Tree from a NewickBroker.
 *
 * It does not take the NewickBroker by const, because assign_ranks() has to be called in order to
 * get the nesting right.
 * TODO: this could be changed by not assigning ranks to the broker but a tmp struct.
 */
template <typename TreeType>
void NewickProcessor<TreeType>::broker_to_tree (
    NewickBroker const& broker, TreeType& tree
) {
    typename TreeType::LinkContainer links;
    typename TreeType::NodeContainer nodes;
    typename TreeType::EdgeContainer edges;

    std::vector<typename TreeType::LinkType*> link_stack;

    // we need the ranks (number of immediate children) of all nodes
    broker.assign_ranks();
    prepare_reading(broker, tree);

    // iterate over all nodes of the tree broker
    for (auto b_itr = broker.cbegin(); b_itr != broker.cend(); ++b_itr) {
        NewickBrokerElement const& broker_node = *b_itr;

        // create the tree node for this broker node
        auto cur_node_u  = make_unique<typename TreeType::NodeType>();
        auto cur_node    = cur_node_u.get();
        cur_node->index_ = nodes.size();
        element_to_node( broker_node, *cur_node );
        nodes.push_back(std::move(cur_node_u));

        // create the link that points towards the root.
        // this link is created for every node, root, inner and leaves.
        auto up_link_u  = make_unique<typename TreeType::LinkType>();
        auto up_link    = up_link_u.get();
        up_link->node_  = cur_node;
        cur_node->link_ = up_link;
        up_link->index_ = links.size();
        links.push_back(std::move(up_link_u));

        // establish the link towards the root
        if (link_stack.empty()) {
            // if the link stack is empty, we are currently at the very beginning of this loop,
            // which means we are at the root itself. in this case, make the "link towards the root"
            // point to itself.
            up_link->outer_ = up_link;
        } else {
            // if we are however in some other node (leaf or inner, but not the root), we establish
            // the link "upwards" to the root, and back from there.
            up_link->outer_ = link_stack.back();
            link_stack.back()->outer_ = up_link;

            // also, create an edge that connects both nodes
            auto up_edge = make_unique<typename TreeType::EdgeType>(
                edges.size(),
                link_stack.back(),
                up_link
            );

            up_link->edge_           = up_edge.get();
            link_stack.back()->edge_ = up_edge.get();
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
            auto down_link = make_unique<typename TreeType::LinkType>();
            prev_link->next_ = down_link.get();
            prev_link = down_link.get();

            down_link->node_ = cur_node;
            down_link->index_ = links.size();
            link_stack.push_back(down_link.get());
            links.push_back(std::move(down_link));
        }
        prev_link->next_ = up_link;
    }

    // we pushed elements to the link_stack for all children of the nodes and popped them when we
    // were done processing those children, so there should be no elements left. this assumes that
    // NewickBroker.assign_ranks() does its job properly!
    assert(link_stack.empty());

    // now delete the uplink of the root, in order to make the tree fully unrooted.
    // (we do that after the tree creation, as it is way easier this way)
    assert(links.front()->outer_ == links.front().get());
    auto next = links.front()->next_;
    while (next->next_ != links.front().get()) {
        next = next->next_;
    }
    next->next_ = next->next_->next_;
    assert(next->next_ == links.front()->next_);
    links.erase(links.begin());
    for (size_t i = 0; i < links.size(); ++i) {
        links[i]->index_ = i;
    }
    next->node_->link_ = next->next_;

    // Finish and hand over the elements to the tree.
    finish_reading(broker, tree);
    tree.import_content(links, nodes, edges);
}

// =================================================================================================
//     Printing
// =================================================================================================

/**
 * @brief Writes the tree to a file in Newick format.
 *
 * If the file already exists, the function throws `std::runtime_error`.
 * The function uses utils::file_write. See there for other exceptions that can be thrown.
 */
template <typename TreeType>
void NewickProcessor<TreeType>::to_file (
    const TreeType& tree, const std::string filename
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
template <typename TreeType>
void NewickProcessor<TreeType>::to_string (
    const TreeType& tree, std::string& ts
) {
    ts = to_string(tree);
}

/**
 * @brief Returns a Newick string representation of the tree.
 *
 * In case the tree was read from a Newick file, this function should produce the same
 * representation.
 */
template <typename TreeType>
std::string NewickProcessor<TreeType>::to_string (const TreeType& tree)
{
    NewickBroker broker;
    tree_to_broker(tree, broker);
    broker.assign_ranks();
    return generate_newick_tree(broker);
}

/**
 * @brief Stores the information of the tree into a NewickBroker object.
 */
template <typename TreeType>
void NewickProcessor<TreeType>::tree_to_broker (
    const TreeType& tree, NewickBroker& broker
) {
    prepare_writing(tree, broker);

    // store the depth from each node to the root. this is needed to assign levels of depth
    // to the nodes for the broker.
    std::vector<int> depth = node_depth_vector(tree);

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

template <typename TreeType>
void NewickProcessor<TreeType>::prepare_writing( TreeType const& tree, NewickBroker& broker )
{
    // Silence unused parameter warnings.
    (void) tree;
    (void) broker;
}

template <typename TreeType>
void NewickProcessor<TreeType>::node_to_element( NodeType const& node, NewickBrokerElement& element )
{
    // Silence unused parameter warnings.
    (void) node;
    (void) element;
}

template <typename TreeType>
void NewickProcessor<TreeType>::edge_to_element( EdgeType const& edge, NewickBrokerElement& element )
{
    // Silence unused parameter warnings.
    (void) edge;
    (void) element;
}

template <typename TreeType>
void NewickProcessor<TreeType>::finish_writing( TreeType const& tree, NewickBroker& broker )
{
    // Silence unused parameter warnings.
    (void) tree;
    (void) broker;
}

} // namespace tree
} // namespace genesis

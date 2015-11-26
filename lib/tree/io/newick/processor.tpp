/**
 * @brief Implementation of functions for reading and writing Newick files.
 *
 * @file
 * @ingroup tree
 */

#include <deque>
#include <memory>
#include <sstream>

#include "tree/distances.hpp"
#include "tree/io/newick/broker.hpp"
#include "tree/io/newick/parser.hpp"
#include "tree/iterators/postorder.hpp"
#include "tree/tree_set.hpp"
#include "tree/tree.hpp"
#include "utils/core/fs.hpp"
#include "utils/core/logging.hpp"
#include "utils/core/std.hpp"
#include "utils/string/string.hpp"

namespace genesis {

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
template <typename AdapterType>
bool NewickProcessor<AdapterType>::from_file (
    const std::string& fn, typename AdapterType::TreeType& tree
) {
    if (!file_exists(fn)) {
        LOG_WARN << "Newick file '" << fn << "' does not exist.";
        return false;
    }
    return from_string(file_read(fn), tree);
}

/**
 * @brief Create a Tree from a string containing a Newick tree.
 *
 * Returns true iff successful.
 */
template <typename AdapterType>
bool NewickProcessor<AdapterType>::from_string (
    const std::string& ts, typename AdapterType::TreeType& tree
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
    return build_tree(broker, tree);
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
template <typename AdapterType>
bool NewickProcessor<AdapterType>::from_file (
    const std::string& fn, TreeSet<typename AdapterType::TreeType>& tset
) {
    if (!file_exists(fn)) {
        LOG_WARN << "Tree file '" << fn << "' does not exist.";
        return false;
    }

    return from_string(file_read(fn), tset, file_filename(file_basename(fn)) + "_");
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
template <typename AdapterType>
bool NewickProcessor<AdapterType>::from_string (
    const std::string& ts,
    TreeSet<typename AdapterType::TreeType>& tset,
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

        auto tree = new typename AdapterType::TreeType();
        if (!build_tree(broker, &tree)) {
            return false;
        }

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
template <typename AdapterType>
bool NewickProcessor<AdapterType>::from_files (
    const std::vector<std::string>& fns, TreeSet<typename AdapterType::TreeType>& set
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
template <typename AdapterType>
bool NewickProcessor<AdapterType>::from_strings (
    const std::vector<std::string>& tss,
    TreeSet<typename AdapterType::TreeType>& set,
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
//     Internal Helper Methods
// -------------------------------------------------------------------------

/**
 * @brief Builds a Tree from a NewickBroker.
 *
 * It does not take the NewickBroker by const, because assign_ranks() has to be called in order to
 * get the nesting right.
 * TODO: this could be changed by not assigning ranks to the broker but a tmp struct.
 */
template <typename AdapterType>
bool NewickProcessor<AdapterType>::build_tree (
    NewickBroker const& broker, typename AdapterType::TreeType& tree
) {
    typename AdapterType::TreeType::LinkContainer links;
    typename AdapterType::TreeType::NodeContainer nodes;
    typename AdapterType::TreeType::EdgeContainer edges;

    // There may be errors while converting from broker nodes to tree elements, for example missing
    // data for certain formats. This will be reported as result of this method. We however do not
    // stop the method when an error occurs, as this might result in incomplete trees, which would
    // currently be a memory leak.
    // TODO once tree uses smart pointers, we can immediately return on errors in this method!
    bool result = true;

    std::vector<typename AdapterType::TreeType::LinkType*> link_stack;

    // we need the ranks (number of immediate children) of all nodes
    broker.assign_ranks();

    // iterate over all nodes of the tree broker
    for (auto b_itr = broker.cbegin(); b_itr != broker.cend(); ++b_itr) {
        NewickBrokerElement const& broker_node = *b_itr;

        // create the tree node for this broker node
        auto cur_node_u  = make_unique<typename AdapterType::TreeType::NodeType>();
        auto cur_node    = cur_node_u.get();
        result          &= adapter_.to_tree_node(broker_node, *cur_node);
        cur_node->index_ = nodes.size();
        nodes.push_back(std::move(cur_node_u));

        // create the link that points towards the root.
        // this link is created for every node, root, inner and leaves.
        auto up_link_u  = make_unique<typename AdapterType::TreeType::LinkType>();
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
            auto up_edge = make_unique<typename AdapterType::TreeType::EdgeType>();
            up_edge->link_p_         = link_stack.back();
            up_edge->link_s_         = up_link;
            up_link->edge_           = up_edge.get();
            link_stack.back()->edge_ = up_edge.get();
            result &= adapter_.to_tree_edge(broker_node, *up_edge);
            up_edge->index_ = edges.size();
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
            auto down_link = make_unique<typename AdapterType::TreeType::LinkType>();
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

    // hand over the elements to the tree
    tree.import_content(links, nodes, edges);
    return result;
}

// =================================================================================================
//     Printing
// =================================================================================================

/**
 * @brief Writes the tree to a file in Newick format.
 *
 * If the file already exists, the function does not overwrite it.
 */
template <typename AdapterType>
bool NewickProcessor<AdapterType>::to_file (
    const typename AdapterType::TreeType& tree, const std::string fn
) {
    if (file_exists(fn)) {
        LOG_WARN << "Newick file '" << fn << "' already exist. Will not overwrite it.";
        return false;
    }
    std::string ts;
    to_string(tree, ts);
    return file_write(fn, ts);
}

/**
 * @brief Gives a Newick string representation of the tree.
 *
 * In case the tree was read from a Newick file, this function should produce the same
 * representation.
 */
template <typename AdapterType>
void NewickProcessor<AdapterType>::to_string (
    const typename AdapterType::TreeType& tree, std::string& ts
) {
    ts = to_string(tree);
}

/**
 * @brief Returns a Newick string representation of the tree.
 *
 * In case the tree was read from a Newick file, this function should produce the same
 * representation.
 */
template <typename AdapterType>
std::string NewickProcessor<AdapterType>::to_string (const typename AdapterType::TreeType& tree)
{
    NewickBroker broker;
    to_broker(tree, broker);
    broker.assign_ranks();
    return generate_newick_tree(broker);
}

/**
 * @brief Stores the information of the tree into a NewickBroker object.
 */
template <typename AdapterType>
void NewickProcessor<AdapterType>::to_broker (
    const typename AdapterType::TreeType& tree, NewickBroker& broker
) {
    // store the depth from each node to the root. this is needed to assign levels of depth
    // to the nodes for the broker.
    std::vector<int> depth = node_depth_vector(tree);

    // now fill the broker with nodes via postorder traversal, so that the root is put on top last.
    broker.clear();
    for (
        typename AdapterType::TreeType::ConstIteratorPostorder it = tree.begin_postorder();
        it != tree.end_postorder();
        ++it
    ) {
        NewickBrokerElement bn;
        bn.depth = depth[it.node()->index()];

        adapter_.from_tree_node(*it.node(), bn);
        // only write edge data to the broker element if it is not the last iteration.
        // the last iteration is the root, which usually does not have edge information in newick.
        // caveat: for the root node, the edge will point to an arbitrary edge away from the root.
        if (!it.is_last_iteration()) {
            adapter_.from_tree_edge(*it.edge(), bn);
        }

        broker.push_top(bn);
    }
}

} // namespace genesis

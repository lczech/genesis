/**
 * @brief Implementation of functions for reading and writing Newick files.
 *
 * For reasons of readability, in this implementation file, the template data types
 * NodeDataType and EdgeDataType are abbreviated using NDT and EDT, respectively.
 *
 * @file
 * @ingroup tree
 */

#include <deque>
#include <sstream>

#include "tree/newick_broker.hpp"
#include "tree/tree.hpp"
#include "tree/tree_set.hpp"
#include "utils/logging.hpp"
#include "utils/utils.hpp"

namespace genesis {

// =============================================================================
//     Parsing
// =============================================================================

// -------------------------------------------------------------------------
//     Public Methods
// -------------------------------------------------------------------------

/**
 * @brief Create a Tree from a file containing a Newick tree.
 */
template <class NDT, class EDT>
bool NewickProcessor::FromFile (const std::string& fn, Tree<NDT, EDT>& tree)
{
    if (!FileExists(fn)) {
        LOG_WARN << "Newick file '" << fn << "' does not exist.";
        return false;
    }
    return FromString(FileRead(fn), tree);
}

/**
 * @brief Create a Tree from a string containing a Newick tree.
 */
template <class NDT, class EDT>
bool NewickProcessor::FromString (const std::string& ts, Tree<NDT, EDT>& tree)
{
    // run the lexer
    NewickLexer lexer;
    if (!lexer.FromString(ts)) {
        return false;
    }

    // check for lexing errors
    if (lexer.empty()) {
        LOG_INFO << "Tree is empty. Nothing done.";
        return false;
    }
    if (lexer.HasError()) {
        LOG_WARN << "Lexing error at " << lexer.back().at()
                 << " with message: " << lexer.back().value();
        return false;
    }

    // parse the tree from lexer into a tree broker
    auto ct  = lexer.begin();
    NewickBroker broker;
    if (!ParseTree(ct, lexer.end(), broker)) {
        return false;
    }

    // see if there is anything other than a comment left
    while (ct != lexer.end()) {
        if (!ct->IsComment()) {
            LOG_WARN << "Tree contains more data after the semicolon.";
            return false;
        }
        ++ct;
    }

    // build the tree from the broker
    BuildTree(broker, tree);
    return true;
}

/**
 * @brief Create a TreeSet from a file containing a list of Newick trees.
 *
 * See FromString() for information on the syntax of this file.
 */
template <class NDT, class EDT>
bool NewickProcessor::FromFile (const std::string& fn, TreeSet<NDT, EDT>& tset)
{
    if (!FileExists(fn)) {
        LOG_WARN << "Tree file '" << fn << "' does not exist.";
        return false;
    }
    return FromString(FileRead(fn), tset);
}

/**
 * @brief Create a TreeSet from a string containing a list of Newick trees.
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
 */
template <class NDT, class EDT>
bool NewickProcessor::FromString (const std::string& ts, TreeSet<NDT, EDT>& tset)
{
    // Run the Lexer.
    NewickLexer lexer;
    if (!lexer.FromString(ts)) {
        return false;
    }

    // check for lexing errors
    if (lexer.empty()) {
        LOG_INFO << "Tree is empty. Nothing done.";
        return false;
    }
    if (lexer.HasError()) {
        LOG_WARN << "Lexing error at " << lexer.back().at()
                 << " with message: " << lexer.back().value();
        return false;
    }

    // Store error message. Also serves as flag whether an error occured.
    std::string error = "";

    auto ct = lexer.begin();
    while (ct != lexer.end()) {
        if (ct->IsUnknown()) {
            error = "Invalid characters at " + ct->at() + ": '" + ct->value() + "'.";
            break;
        }

        if (ct->IsComment()) {
            continue;
        }

        // Store the name of the current tree; if there is none, use empty string.
        std::string name = "";
        if (ct->IsSymbol() || ct->IsString()) {
            name = ct->value();
            ++ct;

            if (ct == lexer.end()) {
                error = "Unexpected end at " + ct->at() + ".";
                break;
            }

            if (!ct->IsOperator("=")) {
                error = "Invalid character '" + ct->value() + "' at " + ct->at() + ".";
                break;
            }
            ++ct;

            if (ct == lexer.end()) {
                error = "Unexpected end of tree at " + ct->at() + ".";
                break;
            }
        }

        if (!ct->IsBracket("(")) {
            error = "Invalid character at " + ct->at() + ".";
            break;
        }

        // Parse the tree from Lexer into a TreeBroker.
        NewickBroker broker;
        if (!ParseTree(ct, lexer.end(), broker)) {
            return false;
        }

        auto tree = new typename TreeSet<NDT, EDT>::TreeType();
        BuildTree(broker, &tree);
        tset.Add(name, tree);

        // Let's clean up all tokens used so far. We don't need them anymore.
        ct.ConsumeHead();
    }

    if (!error.empty()) {
        LOG_WARN << error;
        return false;
    }

    return true;
}

// -------------------------------------------------------------------------
//     Internal Helper Methods
// -------------------------------------------------------------------------

/**
 * @brief Builds a Tree from a NewickBroker.
 *
 * It does not take the NewickBroker by const, because AssignRanks() has to be called in order to
 * get the nesting right.
 * TODO: this could be changed by not assigning ranks to the broker but a tmp struct.
 */
template <class NDT, class EDT>
void NewickProcessor::BuildTree (NewickBroker& broker, Tree<NDT, EDT>& tree)
{
    typename Tree<NDT, EDT>::LinkArray links;
    typename Tree<NDT, EDT>::NodeArray nodes;
    typename Tree<NDT, EDT>::EdgeArray edges;

    std::vector<typename Tree<NDT, EDT>::LinkType*> link_stack;

    // we need the ranks (number of immediate children) of all nodes
    broker.AssignRanks();

    // iterate over all nodes of the tree broker
    for (NewickBroker::const_iterator b_itr = broker.cbegin(); b_itr != broker.cend(); ++b_itr) {
        NewickBrokerElement* broker_node = *b_itr;

        // create the tree node for this broker node
        typename Tree<NDT, EDT>::NodeType* cur_node = new typename Tree<NDT, EDT>::NodeType();
        cur_node->FromNewickBrokerElement(broker_node);
        cur_node->index_ = nodes.size();
        nodes.push_back(cur_node);

        // create the link that points towards the root.
        // this link is created for every node, root, inner and leaves.
        typename Tree<NDT, EDT>::LinkType* up_link = new typename Tree<NDT, EDT>::LinkType();
        up_link->node_ = cur_node;
        cur_node->link_ = up_link;
        up_link->index_ = links.size();
        links.push_back(up_link);

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
            typename Tree<NDT, EDT>::EdgeType* up_edge = new typename Tree<NDT, EDT>::EdgeType();
            up_edge->link_p_         = link_stack.back();
            up_edge->link_s_         = up_link;
            up_link->edge_           = up_edge;
            link_stack.back()->edge_ = up_edge;
            up_edge->FromNewickBrokerElement(broker_node);
            up_edge->index_ = edges.size();
            edges.push_back(up_edge);

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
        typename Tree<NDT, EDT>::LinkType* prev_link = up_link;
        for (int i = 0; i < broker_node->rank(); ++i) {
            typename Tree<NDT, EDT>::LinkType* down_link = new typename Tree<NDT, EDT>::LinkType();
            prev_link->next_ = down_link;
            prev_link = down_link;

            down_link->node_ = cur_node;
            down_link->index_ = links.size();
            links.push_back(down_link);
            link_stack.push_back(down_link);
        }
        prev_link->next_ = up_link;
    }

    // we pushed elements to the link_stack for all children of the nodes and popped them when we
    // were done processing those children, so there should be no elements left. this assumes that
    // NewickBroker.AssignRanks() does its job properly!
    assert(link_stack.empty());

    // now delete the uplink of the root, in order to make the tree fully unrooted.
    // (we do that after the tree creation, as it is way easier this way)
    assert(links.front()->outer_ == links.front());
    typename Tree<NDT, EDT>::LinkType* next = links.front()->next_;
    while (next->next_ != links.front()) {
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
    tree.Import(links, nodes, edges);
}

// =============================================================================
//     Printing
// =============================================================================

/**
 * @brief Writes the tree to a file in Newick format.
 *
 * If the file already exists, the function does not overwrite it.
 */
template <class NDT, class EDT>
bool NewickProcessor::ToFile   (const std::string& fn, const Tree<NDT, EDT>& tree)
{
    if (FileExists(fn)) {
        LOG_WARN << "Newick file '" << fn << "' already exist. Will not overwrite it.";
        return false;
    }
    std::string ts;
    ToString(ts, tree);
    return FileWrite(fn, ts);
}

/**
 * @brief Gives a Newick string representation of the tree.
 *
 * In case the tree was read from a Newick file, this function should produce the same
 * representation.
 */
template <class NDT, class EDT>
void NewickProcessor::ToString (std::string& ts, const Tree<NDT, EDT>& tree)
{
    ts = ToString(tree);
}

/**
 * @brief Returns a Newick string representation of the tree.
 *
 * In case the tree was read from a Newick file, this function should produce the same
 * representation.
 */
template <class NDT, class EDT>
std::string NewickProcessor::ToString (const Tree<NDT, EDT>& tree)
{
    NewickBroker broker;
    ToBroker(broker, tree);
    broker.AssignRanks();
    return ToStringRec(broker, 0) + ";";
}

/**
 * @brief Stores the information of the tree into a NewickBroker object.
 */
template <class NDT, class EDT>
void NewickProcessor::ToBroker (NewickBroker& broker, const Tree<NDT, EDT>& tree)
{
    // store the depth from each node to the root. this is needed to assign levels of depth
    // to the nodes for the broker.
    std::vector<int> depth = tree.NodeDepthVector();

    // now fill the broker with nodes via postorder traversal, so that the root is put on top last.
    broker.clear();
    for (
        typename Tree<NDT, EDT>::ConstIteratorPostorder it = tree.BeginPostorder();
        it != tree.EndPostorder();
        ++it
    ) {
        NewickBrokerElement* bn = new NewickBrokerElement();
        bn->depth = depth[it.Node()->Index()];

        it.Node()->ToNewickBrokerElement(bn);
        // only write edge data to the broker element if it is not the last iteration.
        // the last iteration is the root, which usually does not have edge information in newick.
        // caveat: for the root node, the edge will point to an arbitrary edge away from the root.
        if (!it.IsLastIteration()) {
            it.Edge()->ToNewickBrokerElement(bn);
        }

        // filter out default names if needed
        if (!use_default_names && bn->name != "" && (
            bn->name == default_leaf_name ||
            bn->name == default_internal_name ||
            bn->name == default_root_name
        )) {
            bn->name = "";
        }

        broker.PushTop(bn);
    }
}

} // namespace genesis

/**
 * @brief Implementation of functions for reading and writing Newick files.
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
    if (!parse_tree(ct, lexer.end(), broker)) {
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
        if (!parse_tree(ct, lexer.end(), broker)) {
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
 * @brief Takes a NewickLexer in form of iterators and parses its tokens into a NewickBroker.
 *
 * This function uses a pair of iterators to lexer tokens in order to process the tree, because
 * for files with multiple trees in them (for example, when reading a TreeSet), this function has
 * to be called with a new broker for each tree. This means, we cannot take the lexer as a whole
 * but have to provide more finely grained access to its elements fo this funtion.
 */
template <typename AdapterType>
bool NewickProcessor<AdapterType>::parse_tree (
          NewickLexer::iterator& ct,
    const NewickLexer::iterator& end,
          NewickBroker&          broker
) {
    broker.clear();

    // the node that is currently being populated with data
    NewickBrokerElement* node = nullptr;

    // how deep is the current token nested in the tree?
    int depth = 0;

    // was it closed at some point? we want to avoid a tree like "()();" to be parsed!
    bool closed = false;

    // acts as pointer to previous token
    Lexer::iterator pt = end;

    // store error message. also serves as check whether an error occured
    std::string error = "";

    // --------------------------------------------------------------
    //     Loop over lexer tokens and check if it...
    // --------------------------------------------------------------
    for (; ct != end; pt=ct, ++ct) {

        // TODO add check for lexer end and error!

        if (ct->is_unknown()) {
            error = "Invalid characters at " + ct->at() + ": '" + ct->value() + "'.";
            break;
        }

        // ------------------------------------------------------
        //     is bracket '('  ==>  begin of subtree
        // ------------------------------------------------------
        if (ct->is_bracket("(")) {
            if (pt != end && !(
                pt->is_bracket("(")  || pt->is_operator(",") || pt->is_comment()
            )) {
                error = "Invalid characters at " + ct->at() + ": '" + ct->value() + "'.";
                break;
            }

            if (closed) {
                error = "Tree was already closed. Cannot reopen it with '(' at " + ct->at() + ".";
                break;
            }

            ++depth;
            continue;
        }

        // ------------------------------------------------------
        //     Prepare for all other tokens.
        // ------------------------------------------------------

        // if we reach this, the previous condition is not fullfilled (otherwise, continue would
        // have been called). so we have a token other than '(', which means we should already
        // be somewhere in the tree (or a comment). check, if that is true.
        if (ct == ct.get_lexer().begin()) {
            if (ct->is_comment()) {
                continue;
            }
            error = "Tree does not start with '(' at " + ct->at() + ".";
            break;
        }

        // if we reached this point in code, this means that ct != begin, so it is not the first
        // iteration in this loop. this means that pt was already set in the loop header (at least
        // once), which means it now points to a valid token.
        assert(pt != end);

        // set up the node that will be filled with data now.
        // if it already exists, this means we are adding more information to it, e.g.
        // a branch length or a tag. so we do not need to create it.
        // however, if this node does not exist, this means we saw a token before that finished an
        // node and pushed it to the stack (either closing bracket or comma), so we need to create a
        // new one here.
        if (!node) {
            node = new NewickBrokerElement();
            node->depth = depth;

            // checks if the new node is a leaf.
            // for this, we need to check whether the previous token was an opening brackt or a
            // comma. however, as comments can appear everywhere, we need to check for the first
            // non-comment-token.
            auto t = pt;
            while (t != pt.get_lexer().begin() && t->is_comment()) {
                --t;
            }
            node->is_leaf = t->is_bracket("(") || t->is_operator(",");
        }

        // ------------------------------------------------------
        //     is symbol or string  ==>  label
        // ------------------------------------------------------
        if (ct->is_symbol() || ct->is_string()) {
            if (!(
                pt->is_bracket("(")  || pt->is_bracket(")") ||
                pt->is_operator(",") || pt->is_comment()
            )) {
                error = "Invalid characters at " + ct->at() + ": '" + ct->value() + "'.";
                break;
            }

            // populate the node
            if (ct->is_symbol()) {
                // unquoted labels need to turn underscores into space
                node->name = string_replace_all(ct->value(), "_", " ");
            } else {
                node->name = ct->value();
            }
            continue;
        }

        // ------------------------------------------------------
        //     is number  ==>  branch length
        // ------------------------------------------------------
        if (ct->is_number()) {
            if (!(
                pt->is_bracket("(") || pt->is_bracket(")")  || pt->is_symbol() || pt->is_string() ||
                pt->is_comment()    || pt->is_operator(",")
            )) {
                error = "Invalid characters at " + ct->at() + ": '" + ct->value() + "'.";
                break;
            }

            // populate the node
            node->branch_length = std::stod(ct->value());
            continue;
        }

        // ------------------------------------------------------
        //     is tag {}  ==>  tag
        // ------------------------------------------------------
        if (ct->is_tag()) {
            // in some newick extensions, a tag has a semantic meaning that belongs to the
            // current node/edge, thus we need to store it

            // populate the node
            node->tags.push_back(ct->value());
            continue;
        }

        // ------------------------------------------------------
        //     is comment []  ==>  comment
        // ------------------------------------------------------
        if (ct->is_comment()) {
            // in some newick extensions, a comment has a semantic meaning that belongs to the
            // current node/edge, thus we need to store it

            // populate the node
            node->comments.push_back(ct->value());
            continue;
        }

        // ------------------------------------------------------
        //     is comma ','  ==>  next subtree
        // ------------------------------------------------------
        if (ct->is_operator(",")) {
            if (!(
                pt->is_bracket("(") || pt->is_bracket(")") || pt->is_comment() || pt->is_symbol() ||
                pt->is_string()     || pt->is_number()     || pt->is_tag()     || pt->is_operator(",")
            )) {
                error = "Invalid ',' at " + ct->at() + ".";
                break;
            }

            // populate the node
            if (node->name.empty() && use_default_names) {
                if (node->is_leaf) {
                    node->name = default_leaf_name;
                } else {
                    node->name = default_internal_name;
                }
            }
            broker.push_top(node);
            node = nullptr;
            continue;
        }

        // ------------------------------------------------------
        //     is bracket ')'  ==>  end of subtree
        // ------------------------------------------------------
        if (ct->is_bracket(")")) {
            if (depth == 0) {
                error = "Too many ')' at " + ct->at() + ".";
                break;
            }
            if (!(
                pt->is_bracket(")") || pt->is_tag()    || pt->is_comment()     || pt->is_symbol() ||
                pt->is_string()     || pt->is_number() || pt->is_operator(",")
            )) {
                error = "Invalid ')' at " + ct->at() + ": '" + ct->value() + "'.";
                break;
            }

            // populate the node
            if (node->name.empty() && use_default_names) {
                if (node->is_leaf) {
                    node->name = default_leaf_name;
                } else {
                    node->name = default_internal_name;
                }
            }
            broker.push_top(node);
            node = nullptr;

            // decrease depth and check if this was the parenthesis that closed the tree
            --depth;
            if (depth == 0) {
                closed = true;
            }
            continue;
        }

        // ------------------------------------------------------
        //     is semicolon ';'  ==>  end of tree
        // ------------------------------------------------------
        if (ct->is_operator(";")) {
            if (depth != 0) {
                error = "Not enough ')' in tree before closing it with ';' at " + ct->at() + ".";
                break;
            }
            if (!(
                pt->is_bracket(")") || pt->is_symbol() || pt->is_string() || pt->is_comment() ||
                pt->is_number()     || pt->is_tag()
            )) {
                error = "Invalid ';' at " + ct->at() + ": '" + ct->value() + "'.";
                break;
            }

            // populate the node
            if (node->name.empty() && use_default_names) {
                node->name = default_root_name;
            }
            broker.push_top(node);
            node = nullptr;
            break;
        }

        // If we reach this part of the code, all checkings for token types are done.
        // as we check for every type that NewickLexer yields, and we use a continue or break
        // in each of them, we should never reach this point, unless we forgot a type!
        assert(false);
    }

    if (!error.empty()) {
        LOG_WARN << error;
        return false;
    }

    if (ct == end || !ct->is_operator(";")) {
        LOG_WARN << "Tree does not finish with a semicolon.";
        return false;
    }

    // Move to the token after the closing semicolon. This is needed for the TreeSet parser.
    ++ct;
    return true;
}

/**
 * @brief Builds a Tree from a NewickBroker.
 *
 * It does not take the NewickBroker by const, because assign_ranks() has to be called in order to
 * get the nesting right.
 * TODO: this could be changed by not assigning ranks to the broker but a tmp struct.
 */
template <typename AdapterType>
bool NewickProcessor<AdapterType>::build_tree (
    NewickBroker& broker, typename AdapterType::TreeType& tree
) {
    typename AdapterType::TreeType::LinkArray links;
    typename AdapterType::TreeType::NodeArray nodes;
    typename AdapterType::TreeType::EdgeArray edges;

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
    for (NewickBroker::const_iterator b_itr = broker.cbegin(); b_itr != broker.cend(); ++b_itr) {
        NewickBrokerElement* broker_node = *b_itr;

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
        for (int i = 0; i < broker_node->rank(); ++i) {
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
    return to_string_rec(broker, 0) + ";";
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
    std::vector<int> depth = tree.node_depth_vector();

    // now fill the broker with nodes via postorder traversal, so that the root is put on top last.
    broker.clear();
    for (
        typename AdapterType::TreeType::ConstIteratorPostorder it = tree.begin_postorder();
        it != tree.end_postorder();
        ++it
    ) {
        NewickBrokerElement* bn = new NewickBrokerElement();
        bn->depth = depth[it.node()->index()];

        adapter_.from_tree_node(*it.node(), bn);
        // only write edge data to the broker element if it is not the last iteration.
        // the last iteration is the root, which usually does not have edge information in newick.
        // caveat: for the root node, the edge will point to an arbitrary edge away from the root.
        if (!it.is_last_iteration()) {
            adapter_.from_tree_edge(*it.edge(), bn);
        }

        // filter out default names if needed
        if (!use_default_names && bn->name != "" && (
            bn->name == default_leaf_name ||
            bn->name == default_internal_name ||
            bn->name == default_root_name
        )) {
            bn->name = "";
        }

        broker.push_top(bn);
    }
}

/**
 * @brief
 */
// TODO this is a quick and dirty (=slow) solution...
template <typename AdapterType>
std::string NewickProcessor<AdapterType>::to_string_rec(const NewickBroker& broker, size_t pos)
{
    // check if it is a leaf, stop recursion if so.
    if (broker[pos]->rank() == 0) {
        return element_to_string(broker[pos]);
    }

    // recurse over all children of the current node. while doing so, build a stack of the resulting
    // substrings in reverse order. this is because newick stores the nodes kind of "backwards",
    // by starting at a leaf node instead of the root.
    std::deque<std::string> children;
    for (size_t i = pos + 1; i < broker.size() && broker[i]->depth > broker[pos]->depth; ++i) {
        // skip if not immediate children (those will be called in later recursion steps)
        if (broker[i]->depth > broker[pos]->depth + 1) {
            continue;
        }

        // do the recursion step for this child, add the result to a stack
        children.push_front(to_string_rec(broker, i));
    }

    // build the string by iterating the stack
    std::ostringstream out;
    out << "(";
    for (size_t i = 0; i < children.size(); ++i) {
        if (i>0) {
            out << ",";
        }
        out << children[i];
    }
    out << ")" << element_to_string(broker[pos]);
    return out.str();
}

/**
 * @brief
 */
template <typename AdapterType>
std::string NewickProcessor<AdapterType>::element_to_string(const NewickBrokerElement* bn)
{
    std::string res = "";
    if (print_names) {
        res += string_replace_all(bn->name, " ", "_");
    }
    if (print_branch_lengths) {
        res += ":" + to_string_precise(bn->branch_length, precision);
    }
    if (print_comments) {
        for (std::string c : bn->comments) {
            res += "[" + c + "]";
        }
    }
    if (print_tags) {
        for (std::string t : bn->tags) {
            res += "{" + t + "}";
        }
    }
    return res;
}

} // namespace genesis

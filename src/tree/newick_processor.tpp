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
#include "utils/logging.hpp"
#include "utils/utils.hpp"

namespace genesis {

// =============================================================================
//     Parsing
// =============================================================================

/**
 * @brief Create a Tree from a file containing a Newick tree.
 */
template <class NDT, class EDT>
bool NewickProcessor::FromFile (const std::string fn, Tree<NDT, EDT>& tree)
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
bool NewickProcessor::FromString (const std::string ts, Tree<NDT, EDT>& tree)
{
    NewickLexer lexer;
    lexer.ProcessString(ts);
    return FromLexer(lexer, tree);
}

// TODO do a validate brackets first?!
// TODO what happens if a tree's nested brackets fully close to depth 0, then open again without
// TODO semicolon like (...)(...); ? do we need to check for this?
template <class NDT, class EDT>
bool NewickProcessor::FromLexer (const NewickLexer& lexer, Tree<NDT, EDT>& tree)
{
    if (lexer.empty()) {
        LOG_INFO << "Tree is empty. Nothing done.";
        return false;
    }
    if (lexer.HasError()) {
        LOG_WARN << "Lexing error at " << lexer.back().at()
                 << " with message: " << lexer.back().value();
        return false;
    }

    // we will fill this broker with tree elements
    NewickBroker broker;

    // the node that is currently being populated with data
    NewickBrokerElement* node = nullptr;

    // how deep is the current token nested in the tree?
    int depth = 0;

    // acts as pointer to previous token
    Lexer::const_iterator pt = lexer.cend();

    // acts as pointer to current token
    Lexer::const_iterator ct;

    // store error message. also serves as check whether an error occured
    std::string error = "";

    // --------------------------------------------------------------
    //     Loop over lexer tokens and check if it...
    // --------------------------------------------------------------
    for (ct = lexer.cbegin(); ct != lexer.cend(); pt=ct, ++ct) {
        if (ct->IsUnknown()) {
            error = "Invalid characters at " + ct->at() + ": '" + ct->value() + "'.";
            break;
        }

        // ------------------------------------------------------
        //     is bracket '('  ==>  begin of subtree
        // ------------------------------------------------------
        if (ct->IsBracket("(")) {
            if (pt != lexer.cend() && !(
                pt->IsBracket("(")  || pt->IsOperator(",") || pt->IsComment()
            )) {
                error = "Invalid characters at " + ct->at() + ": '" + ct->value() + "'.";
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
        if (ct == lexer.cbegin()) {
            if (ct->IsComment()) {
                continue;
            }
            error = "Tree does not start with '(' at " + ct->at() + ".";
            break;
        }

        // if we reached this point in code, this means that ct != begin, so it is not the first
        // iteration in this loop. this means that pt was already set in the loop header (at least
        // once), which means it now points to a valid token.
        assert(pt != lexer.cend());

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
            Lexer::const_iterator t = pt;
            while (t != lexer.cbegin() && t->IsComment()) {
                --t;
            }
            node->is_leaf = t->IsBracket("(") || t->IsOperator(",");
        }

        // ------------------------------------------------------
        //     is symbol or string  ==>  label
        // ------------------------------------------------------
        if (ct->IsSymbol() || ct->IsString()) {
            if (!(
                pt->IsBracket("(")  || pt->IsBracket(")") ||
                pt->IsOperator(",") || pt->IsComment()
            )) {
                error = "Invalid characters at " + ct->at() + ": '" + ct->value() + "'.";
                break;
            }

            // populate the node
            if (ct->IsSymbol()) {
                // unquoted labels need to turn underscores into space
                node->name = StringReplaceAll(ct->value(), "_", " ");
            } else {
                node->name = ct->value();
            }
            continue;
        }

        // ------------------------------------------------------
        //     is number  ==>  branch length
        // ------------------------------------------------------
        if (ct->IsNumber()) {
            if (!(
                pt->IsBracket("(") || pt->IsBracket(")")  || pt->IsSymbol() || pt->IsString() ||
                pt->IsComment()    || pt->IsOperator(",")
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
        if (ct->IsTag()) {
            // in some newick extensions, a tag has a semantic meaning that belongs to the
            // current node/edge, thus we need to store it

            // populate the node
            node->tags.push_back(ct->value());
            continue;
        }

        // ------------------------------------------------------
        //     is comment []  ==>  comment
        // ------------------------------------------------------
        if (ct->IsComment()) {
            // in some newick extensions, a comment has a semantic meaning that belongs to the
            // current node/edge, thus we need to store it

            // populate the node
            node->comments.push_back(ct->value());
            continue;
        }

        // ------------------------------------------------------
        //     is comma ','  ==>  next subtree
        // ------------------------------------------------------
        if (ct->IsOperator(",")) {
            if (!(
                pt->IsBracket("(") || pt->IsBracket(")") || pt->IsComment() || pt->IsSymbol() ||
                pt->IsString()     || pt->IsNumber()     || pt->IsTag()     || pt->IsOperator(",")
            )) {
                error = "Invalid ',' at " + ct->at() + ": '" + ct->value() + "'.";
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
            broker.PushTop(node);
            node = nullptr;
            continue;
        }

        // ------------------------------------------------------
        //     is bracket ')'  ==>  end of subtree
        // ------------------------------------------------------
        if (ct->IsBracket(")")) {
            if (depth == 0) {
                error = "Too many ')' at " + ct->at() + ".";
                break;
            }
            if (!(
                pt->IsBracket(")") || pt->IsTag()    || pt->IsComment()     || pt->IsSymbol() ||
                pt->IsString()     || pt->IsNumber() || pt->IsOperator(",")
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
            broker.PushTop(node);
            node = nullptr;

            --depth;
            continue;
        }

        // ------------------------------------------------------
        //     is semicolon ';'  ==>  end of tree
        // ------------------------------------------------------
        if (ct->IsOperator(";")) {
            if (!(
                pt->IsBracket(")") || pt->IsSymbol() || pt->IsString() || pt->IsComment() ||
                pt->IsNumber()     || pt->IsTag()
            )) {
                error = "Invalid ';' at " + ct->at() + ": '" + ct->value() + "'.";
                break;
            }

            // populate the node
            if (node->name.empty() && use_default_names) {
                node->name = default_root_name;
            }
            broker.PushTop(node);
            node = nullptr;
            break;
        }

        // if we reach this part of the code, all checkings for token types are done.
        // as we check for every type that NewickLexer yields, and we use a continue or break
        // in each of them, we should never reach this point, unless we forgot a type!
        assert(false);
    }

    if (!error.empty()) {
        LOG_WARN << error;
        return false;
    }

    if (depth != 0) {
        LOG_WARN << "Not enough closing parenthesis.";
        return false;
    }

    if (ct == lexer.cend() || !ct->IsOperator(";")) {
        LOG_WARN << "Tree does not finish with a semicolon.";
        return false;
    }

    // TODO we now stop at the first semicolon. is that good?
    // TODO do we even need to parse the rest as a new tree?

    // skip the semicolon, then see if there is anything other than a comment left
    ++ct;
    while (ct != lexer.cend() && ct->IsComment()) {
        ++ct;
    }
    if (ct != lexer.cend()) {
        LOG_WARN << "Tree contains more data after the semicolon.";
        return false;
    }

    FromBroker(broker, tree);
    return true;
}

/**
 * @brief Create a Tree from a NewickBroker.
 *
 * It does not take the NewickBroker by const, because AssignRanks() has to be called in order to get
 * the nesting right.
 * TODO: this could be changed by not assigning ranks to the broker but a tmp struct.
 */
template <class NDT, class EDT>
void NewickProcessor::FromBroker (NewickBroker& broker, Tree<NDT, EDT>& tree)
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
bool NewickProcessor::ToFile   (const std::string fn, Tree<NDT, EDT>& tree)
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
void NewickProcessor::ToString (std::string& ts, Tree<NDT, EDT>& tree)
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
std::string NewickProcessor::ToString (Tree<NDT, EDT>& tree)
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
void NewickProcessor::ToBroker (NewickBroker& broker, Tree<NDT, EDT>& tree)
{
    // store the distance from each node to the root. this is needed to assign levels of depth
    // to the nodes for the broker.
    std::vector<int> depth = tree.NodeDepthVector();

    // now fill the broker with nodes via postorder traversal, so that the root is put on top last.
    broker.clear();
    for (
        typename Tree<NDT, EDT>::IteratorPostorder it = tree.BeginPostorder();
        it != tree.EndPostorder();
        ++it
    ) {
        NewickBrokerElement* bn = new NewickBrokerElement();
        bn->depth = depth[it.Node()->Index()];

        it.Node()->data.ToNewickBrokerElement(bn);
        // only write edge data to the broker element if it is not the last iteration.
        // the last iteration is the root, which usually does not have edge information in newick.
        // caveat: for the root node, the edge will point to an arbitrary edge away from the root.
        if (!it.IsLastIteration()) {
            it.Edge()->data.ToNewickBrokerElement(bn);
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

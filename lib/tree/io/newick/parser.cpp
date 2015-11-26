/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "tree/io/newick/parser.hpp"

#include "tree/io/newick/broker.hpp"
#include "utils/core/logging.hpp"
#include "utils/core/std.hpp"

#include <assert.h>

namespace genesis {

// =================================================================================================
//     Newick Parser
// =================================================================================================

// TODO turn return code and log warnings into exceptions!

/**
 * @brief Takes a NewickLexer in form of iterators and parses its tokens into a NewickBroker.
 *
 * This function uses a pair of iterators to lexer tokens in order to process the tree, because
 * for files with multiple trees in them (for example, when reading a TreeSet), this function has
 * to be called with a new broker for each tree. This means, we cannot take the lexer as a whole
 * but have to provide more finely grained access to its elements for this funtion.
 */
bool parse_newick_tree (
          NewickLexer::iterator& ct,
    const NewickLexer::iterator& end,
          NewickBroker&          broker
) {
    broker.clear();

    // the node that is currently being populated with data
    std::unique_ptr<NewickBrokerElement> node;

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
            node = make_unique<NewickBrokerElement>();
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

            // store an finish the current node
            broker.push_top(*node);
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

            // store an finish the current node
            broker.push_top(*node);
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

            // store an finish the current node
            broker.push_top(*node);
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

} // namespace genesis

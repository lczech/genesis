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

#include "tree/formats/newick/parser.hpp"

#include "tree/formats/newick/broker.hpp"
#include "utils/core/logging.hpp"
#include "utils/core/std.hpp"

#include <assert.h>
#include <stdexcept>

namespace genesis {
namespace tree {

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
    utils::Lexer::iterator pt = end;

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
            node = utils::make_unique<NewickBrokerElement>();
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
            node->name = ct->value();
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
            node->values.push_back( ct->value() );
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
        throw std::runtime_error( error );
    }

    if (ct == end || !ct->is_operator(";")) {
        throw std::runtime_error( "Tree does not finish with a semicolon." );
    }

    // Move to the token after the closing semicolon. This is needed for the TreeSet parser.
    ++ct;
    return true;
}

// =================================================================================================
//     Newick Generator
// =================================================================================================

/**
 * @brief
 */
std::string element_to_string( NewickBrokerElement const& bn )
{
    std::string res = bn.name;
    for (std::string v : bn.values) {
        res += ":" + v;
    }
    for (std::string c : bn.comments) {
        res += "[" + c + "]";
    }
    for (std::string t : bn.tags) {
        res += "{" + t + "}";
    }
    return res;
}

/**
 * @brief
 */
// TODO this is a quick and dirty (=slow) solution...
std::string to_string_rec( NewickBroker const& broker, size_t pos )
{
    // check if it is a leaf, stop recursion if so.
    if (broker[pos].rank() == 0) {
        return element_to_string(broker[pos]);
    }

    // recurse over all children of the current node. while doing so, build a stack of the resulting
    // substrings in reverse order. this is because newick stores the nodes kind of "backwards",
    // by starting at a leaf node instead of the root.
    std::deque<std::string> children;
    for (size_t i = pos + 1; i < broker.size() && broker[i].depth > broker[pos].depth; ++i) {
        // skip if not immediate children (those will be called in later recursion steps)
        if (broker[i].depth > broker[pos].depth + 1) {
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
std::string generate_newick_tree( NewickBroker const& broker )
{
    return to_string_rec(broker, 0) + ";";
}

} // namespace tree
} // namespace genesis

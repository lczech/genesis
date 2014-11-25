/**
 * @brief Implementation of Newick Parser functions.
 *
 * @file
 * @ingroup tree
 */

#include "tree/newick_parser.hh"

#include <assert.h>
#include <string>

#include "utils/logging.hh"
#include "utils/utils.hh"

namespace genesis {

bool NewickParser::Process (const std::string& tree, TreeBroker& broker)
{
    NewickLexer lexer;
    lexer.Process(tree);
    return Process(lexer, broker);
}

// TODO do a validate brackets first?!
// TODO what happens if a tree's nested brackets fully close to depth 0, then open again
// TODO without semicolon like (...)(...); ? do we need to check for this?

bool NewickParser::Process (const NewickLexer& lexer, TreeBroker& broker)
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

    broker.clear();

    // the node that is currently being populated with data
    TreeBrokerNode* node = nullptr;

    // how deep is the current token nested in the tree?
    int depth = 0;

    // acts as pointer to previous token
    Lexer::const_iterator pt = lexer.end();

    // acts as pointer to current token
    Lexer::const_iterator ct;

    // store error message. also serves as check whether an error occured
    std::string error = "";

    // --------------------------------------------------------------
    //     Loop over lexer tokens and check if it...
    // --------------------------------------------------------------
    for (ct = lexer.begin(); ct != lexer.end(); pt=ct, ++ct) {
        if (ct->IsUnknown()) {
            error = "Invalid characters at " + ct->at() + ": '" + ct->value() + "'.";
            break;
        }

        // ------------------------------------------------------
        //     is bracket '('  ==>  begin of subtree
        // ------------------------------------------------------
        if (ct->IsBracket("(")) {
            if (pt != lexer.end() && !(
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
        if (ct == lexer.begin()) {
            if (ct->IsComment()) {
                continue;
            }
            error = "Tree does not start with '(' at " + ct->at() + ".";
            break;
        }

        // if we reached this point in code, this means that ct != begin, so it is not the first
        // iteration in this loop. this means that pt was already set in the loop header (at least
        // once), which means it now points to a valid token.
        assert(pt != lexer.end());

        // set up the node that will be filled with data now.
        // if it already exists, this means we are adding more information to it, e.g.
        // a branch length or a tag. so we do not need to create it.
        // however, if this node does not exist, this means we saw a token before that finished an
        // node and pushed it to the stack (either closing bracket or comma), so we need to create a
        // new one here.
        if (!node) {
            node = new TreeBrokerNode();
            node->depth = depth;

            // checks if the new node is a leaf.
            // for this, we need to check whether the previous token was an opening brackt or a
            // comma. however, as comments can appear everywhere, we need to check for the first
            // non-comment-token.
            Lexer::const_iterator t = pt;
            while (t != lexer.begin() && t->IsComment()) {
                --t;
            }
            node->is_leaf = t->IsBracket("(") || t->IsOperator(",");
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
            if (node->name.empty()) {
                if (node->is_leaf) {
                    node->name = "Leaf Node";
                } else {
                    node->name = "Internal Node";
                }
            }
            broker.push_back(node);
            node = nullptr;

            --depth;
            continue;
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
            if (!(
                pt->IsBracket(")") || pt->IsSymbol() || pt->IsString() || pt->IsNumber() ||
                pt->IsComment()    || pt->IsTag()
            )) {
                error = "Invalid tag at " + ct->at() + ": '" + ct->value() + "'.";
                break;
            }

            // a tag that comes after one of the tokens: ")", symbol, string, number, comment,
            // tag. in some newick extensions this has a semantic meaning that belongs to the
            // current node/branch, thus we need to store it

            // populate the node
            node->tag += ct->value();
            continue;
        }

        // ------------------------------------------------------
        //     is comment []  ==>  comment
        // ------------------------------------------------------
        if (ct->IsComment()) {
            if (!(
                pt->IsBracket(")") || pt->IsSymbol() || pt->IsString() || pt->IsNumber() ||
                pt->IsComment()    || pt->IsTag()
            )) {
                // just a normal comment without semantics
                continue;
            }

            // a comment that comes after one of the tokens: ")", symbol, string, number, comment,
            // tag. in some newick extensions this has a semantic meaning that belongs to the
            // current node/branch, thus we need to store it

            // populate the node
            node->comment += ct->value();
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
            if (node->name.empty()) {
                if (node->is_leaf) {
                    node->name = "Leaf Node";
                } else {
                    node->name = "Internal Node";
                }
            }
            broker.push_back(node);
            node = nullptr;
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
            if (node->name.empty()) {
                node->name = "Root Node";
            }
            broker.push_back(node);
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

    if (ct == lexer.end() || !ct->IsOperator(";")) {
        LOG_WARN << "Tree does not finish with a semicolon.";
        return false;
    }

    // TODO we now stop at the first semicolon. is that good?
    // TODO do we even need to parse the rest as a new tree?

    // skip the semicolon, then see if there is anything other than a comment left
    ++ct;
    while (ct != lexer.end() && ct->IsComment()) {
        ++ct;
    }
    if (ct != lexer.end()) {
        LOG_WARN << "Tree contains more data after the semicolon.";
        return false;
    }

    return true;
}

}

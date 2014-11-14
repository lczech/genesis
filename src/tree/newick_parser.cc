/**
 * @brief Implementation of Newick Parser functions.
 *
 * @file
 * @ingroup tree
 */

#include "tree/newick_parser.hh"

#include <assert.h>
#include <iomanip>
#include <string>

#include "utils/logging.hh"
#include "utils/utils.hh"

namespace genesis {

bool NewickParser::Process (const NewickLexer& lexer)
{
    // check boundary conditions
    if (lexer.empty()) {
        LOG_INFO << "Tree is empty.";
        return false;
    }
    if (lexer.HasError()) {
        LOG_INFO << "Lexing error at " << lexer.back().at()
                 << " with message: " << lexer.back().value();
        return false;
    }

    Init();

    // the item that is currently being populated with data
    NewickParserItem* item = nullptr;

    // are we currently investigating an item?
    // (this is independent of the value of item, as the root counts as an item, but item will
    // be nullptr at the point we see the root in the newick string)
    bool item_active = false;

    // how deep is the current token nested in the tree?
    int depth = 0;

    // acts as pointer to previous token
    Lexer::const_iterator pt = lexer.end();

    // acts as pointer to current token
    Lexer::const_iterator ct;

    // checks if the current item is a leaf.
    // for this, we need to check whether the previous token was an opening brackt or a comma.
    // however, as comments can appear everywhere, we need to check for the first non-comment-token.
    // it is important to hand over the iterator t per value, in order to not mess with the
    // loop-internal iterator!
    auto IsLeaf = [lexer] (Lexer::const_iterator t)
    {
        while (t != lexer.begin() && t->type() == kComment) {
            --t;
        }
        return t->IsBracket("(") || t->IsOperator(",");
    };

    // --------------------------------------------------------------
    //     Loop over lexer tokens and check if it...
    // --------------------------------------------------------------
    for (ct = lexer.begin(); ct != lexer.end(); pt=ct, ++ct) {
        if (ct->type() == kUnknown) {
            LOG_INFO << "Invalid characters at " << ct->at()
                     << ": '" << std::hex << ct->value() << "'.";
            return false;
        }

        // ------------------------------------------------------
        //     is bracket '('  ==>  begin of subtree
        // ------------------------------------------------------
        if (ct->IsBracket("(")) {
            ++depth;
            continue;
        }

        // if we reach this, the previous condition is not fullfilled (otherwise, continue would
        // have been called). so we have a token other than '(', which means we should already
        // be somewhere in the tree (or a comment). check, if that is true.
        if (ct == lexer.begin()) {
            if (ct->type() == kComment) {
                continue;
            }
            LOG_INFO << "Tree does not start with '(' at " << ct->at() << ".";
            return false;
        }

        // if we reached this point in code, this means that ct != begin, so it is not the first
        // iteration in this loop. this means that pt was already set in the loop header (at least
        // once), which means it now points to a valid token.
        assert(pt != lexer.end());

        // ------------------------------------------------------
        //     is bracket ')'  ==>  end of subtree
        // ------------------------------------------------------
        if (ct->IsBracket(")")) {
            if (depth == 0) {
                LOG_INFO << "Too many ')' at " << ct->at() << ".";
                return false;
            }
            if (!(
                pt->IsBracket(")")    || pt->type() == kTag    || pt->type() == kComment ||
                pt->type() == kSymbol || pt->type() == kString || pt->type() == kNumber
            )) {
                LOG_INFO << "Invalid characters at " << ct->at() << ": '" << ct->value() << "'.";
                return false;
            }

            // populate the item
            if (!item) {
                item = new NewickParserItem();
            }
            if (item->name.empty()) {
                item->name = "Internal Node";
            }
            item->depth = depth;
            items_.push_back(item);
            item = nullptr;

            // set state
            --depth;
            item_active = true;
            continue;
        }

        // ------------------------------------------------------
        //     is symbol or string  ==>  label
        // ------------------------------------------------------
        if (ct->type() == kSymbol || ct->type() == kString) {
            if (!(
                pt->IsBracket("(")  || pt->IsBracket(")") ||
                pt->IsOperator(",") || pt->type() == kComment
            )) {
                LOG_INFO << "Invalid characters at " << ct->at() << ": '" << ct->value() << "'.";
                return false;
            }

            // populate the item
            if (!item) {
                item = new NewickParserItem();
            }
            if (ct->type() == kSymbol) {
                // unquoted labels need to turn underscores into space
                item->name = StringReplaceAll(ct->value(), "_", " ");
            } else {
                item->name = ct->value();
            }
            item->depth    = depth;
            item->is_leaf |= IsLeaf(pt);

            // set state
            item_active = true;
            continue;
        }

        // ------------------------------------------------------
        //     is number  ==>  branch length
        // ------------------------------------------------------
        if (ct->type() == kNumber) {
            if (!(
                pt->IsBracket(")") || pt->type() == kSymbol || pt->type() == kString ||
                pt->type() == kComment
            )) {
                LOG_INFO << "Invalid characters at " << ct->at() << ": '" << ct->value() << "'.";
                return false;
            }

            // populate the item
            if (!item) {
                item = new NewickParserItem();
            }
            item->branch_length  = std::stod(ct->value());
            item->depth          = depth;
            item->is_leaf       |= IsLeaf(pt);

            // set state
            item_active = true;
            continue;
        }

        // ------------------------------------------------------
        //     is tag {}  ==>  tag
        // ------------------------------------------------------
        if (ct->type() == kTag) {
            if (!(
                pt->IsBracket(")")    || pt->type() == kSymbol  || pt->type() == kString ||
                pt->type() == kNumber || pt->type() == kComment || pt->type() == kTag
            )) {
                LOG_INFO << "Invalid tag at " << ct->at() << ": '" << ct->value() << "'.";
                return false;
            }

            // a tag that comes after one of the tokens: ")", symbol, string, number, comment,
            // tag. in some newick extensions this has a semantic meaning that belongs to the
            // current node/branch, thus we need to store it

            // populate the item
            if (!item) {
                item = new NewickParserItem();
            }
            item->depth    = depth;
            item->is_leaf |= IsLeaf(pt);
            item->tag     += ct->value();

            // set state
            item_active = true;
            continue;
        }

        // ------------------------------------------------------
        //     is comment []  ==>  comment
        // ------------------------------------------------------
        if (ct->type() == kComment) {
            if (!(
                pt->IsBracket(")")    || pt->type() == kSymbol  || pt->type() == kString ||
                pt->type() == kNumber || pt->type() == kComment || pt->type() == kTag
            )) {
                // just a normal comment without semantics
                continue;
            }

            // a comment that comes after one of the tokens: ")", symbol, string, number, comment,
            // tag. in some newick extensions this has a semantic meaning that belongs to the
            // current node/branch, thus we need to store it

            // populate the item
            if (!item) {
                item = new NewickParserItem();
            }
            item->depth    = depth;
            item->is_leaf |= IsLeaf(pt);
            item->comment += ct->value();

            // set state
            item_active = true;
            continue;
        }

        // ------------------------------------------------------
        //     is comma ','  ==>  next subtree
        // ------------------------------------------------------
        if (ct->IsOperator(",")) {
            if (!(
                pt->IsBracket(")")    || pt->type() == kSymbol || pt->type() == kComment ||
                pt->type() == kString || pt->type() == kNumber || pt->type() == kTag
            )) {
                LOG_INFO << "Invalid characters at " << ct->at() << ": '" << ct->value() << "'.";
                return false;
            }

            // populate the item
            if (!item) {
                item = new NewickParserItem();
            }
            if (item->name.empty()) {
                item->name = "Internal Node";
            }
            item->depth = depth;
            items_.push_back(item);
            item = nullptr;

            // set state
            item_active = false;
            continue;
        }

        // ------------------------------------------------------
        //     is semicolon ';'  ==>  end of tree
        // ------------------------------------------------------
        if (ct->IsOperator(";")) {
            if (!(
                pt->IsBracket(")")     || pt->type() == kSymbol || pt->type() == kString ||
                pt->type() == kComment || pt->type() == kNumber || pt->type() == kTag
            )) {
                LOG_INFO << "Invalid characters at " << ct->at() << ": '" << ct->value() << "'.";
                return false;
            }

            // populate the item
            if (!item) {
                item = new NewickParserItem();
            }
            if (item->name.empty()) {
                item->name = "Root Node";
            }
            items_.push_back(item);
            item = nullptr;

            // set state
            item_active = false;
            break;
        }

        // if we reach this part of the code, all checkings for token types are done.
        // as we check for every type that NewickLexer yields, and we use a continue or break
        // in each of them, we should never reach this point, unless we forgot a type!
        assert(false);
    }

    if (depth != 0) {
        LOG_INFO << "Not enough closing parenthesis.";
        return false;
    }

    if (item_active) {
        if (!item) {
            item = new NewickParserItem();
        }
        if (item->name.empty()) {
            item->name = "Root Node";
        }
        items_.push_back(item);
    }

    return true;
}

NewickParser::~NewickParser()
{
    clear();
}

std::string NewickParser::Dump()
{
    std::string out;
    for (NewickParserItem* item : items_) {
        for (int i = 0; i < item->depth; ++i) {
            out += "    ";
        }
        out += item->name + ":" + std::to_string(item->branch_length) + " [" + item->comment + "] {"
            +  item->tag + "}";
        out += item->is_leaf ? " (L)\n" : " ( )\n";
    }
    return out;
}

}

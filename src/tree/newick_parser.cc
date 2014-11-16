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
    if (!MakeParseTree(lexer)) {
        return false;
    }

    // TODO newick parsing now does not assign ranks to nodes (how many children they have).
    // TODO this might become imporant in the future, eg to check if it is a binary tree.
    // TODO add AssignRanks() (see PLL newick.c)
    // TODO add Validate() (see PLL newick.c)
    // TODO stops at the first semicolon. is that good? do we need to warn if there is more coming?
    // TODO do we even need to parse the rest as a new tree?

    return true;
}





// TODO make TreeStack class instead of NewickParserItem array, call this func FillTreeStack or so and make it protected





bool NewickParser::MakeParseTree (const NewickLexer& lexer)
{
    // the item that is currently being populated with data
    NewickParserItem* item = nullptr;

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
        if (ct->type() == kUnknown) {
            error = "Invalid characters at " + ct->at() + ": '" + ct->value() + "'.";
            break;
        }

        // ------------------------------------------------------
        //     is bracket '('  ==>  begin of subtree
        // ------------------------------------------------------
        if (ct->IsBracket("(")) {
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
            // we only skip comment in the very beginning, because later it might be used
            // as a field for additional information, that we need to pass on!
            if (ct->type() == kComment) {
                continue;
            }
            error = "Tree does not start with '(' at " + ct->at() + ".";
            break;
        }

        // if we reached this point in code, this means that ct != begin, so it is not the first
        // iteration in this loop. this means that pt was already set in the loop header (at least
        // once), which means it now points to a valid token.
        assert(pt != lexer.end());

        // set up the item that will be filled with data now.
        // if it already exists, this means we are adding more information to it, e.g.
        // a branch length or a tag. so we do not need to create it.
        // however, if this item does not exist, this means we saw a token before that finished an
        // item and pushed it to the stack (either closing bracket or comma), so we need to create a
        // new one here.
        if (!item) {
            item = new NewickParserItem();
            item->depth_ = depth;

            // checks if the new item is a leaf.
            // for this, we need to check whether the previous token was an opening brackt or a
            // comma. however, as comments can appear everywhere, we need to check for the first
            // non-comment-token.
            Lexer::const_iterator t = pt;
            while (t != lexer.begin() && t->type() == kComment) {
                --t;
            }
            item->is_leaf_ = t->IsBracket("(") || t->IsOperator(",");

            // adjust the class-wide counters
            if (item->is_leaf_) {
                ++leaves_;
            }
            ++nodes_;
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
                pt->IsBracket(")")    || pt->type() == kTag    || pt->type() == kComment ||
                pt->type() == kSymbol || pt->type() == kString || pt->type() == kNumber  ||
                pt->IsOperator(",")
            )) {
                error = "Invalid ')' at " + ct->at() + ": '" + ct->value() + "'.";
                break;
            }

            // populate the item
            if (item->name_.empty()) {
                if (item->is_leaf_) {
                    item->name_ = "Leaf Node";
                } else {
                    item->name_ = "Internal Node";
                }
            }
            items_.push_back(item);
            item = nullptr;

            --depth;
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
                error = "Invalid characters at " + ct->at() + ": '" + ct->value() + "'.";
                break;
            }

            // populate the item
            if (ct->type() == kSymbol) {
                // unquoted labels need to turn underscores into space
                item->name_ = StringReplaceAll(ct->value(), "_", " ");
            } else {
                item->name_ = ct->value();
            }
            continue;
        }

        // ------------------------------------------------------
        //     is number  ==>  branch length
        // ------------------------------------------------------
        if (ct->type() == kNumber) {
            if (!(
                pt->IsBracket("(")    || pt->IsBracket(")")    || pt->IsOperator(",")    ||
                pt->type() == kSymbol || pt->type() == kString || pt->type() == kComment
            )) {
                error = "Invalid characters at " + ct->at() + ": '" + ct->value() + "'.";
                break;
            }

            // populate the item
            item->branch_length_ = std::stod(ct->value());
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
                error = "Invalid tag at " + ct->at() + ": '" + ct->value() + "'.";
                break;
            }

            // a tag that comes after one of the tokens: ")", symbol, string, number, comment,
            // tag. in some newick extensions this has a semantic meaning that belongs to the
            // current node/branch, thus we need to store it

            // populate the item
            item->tag_ += ct->value();
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
            item->comment_ += ct->value();
            continue;
        }

        // ------------------------------------------------------
        //     is comma ','  ==>  next subtree
        // ------------------------------------------------------
        if (ct->IsOperator(",")) {
            if (!(
                pt->IsBracket("(")    || pt->IsBracket(")")    || pt->type() == kComment ||
                pt->type() == kSymbol || pt->type() == kString || pt->type() == kNumber  ||
                pt->type() == kTag    || pt->IsOperator(",")
            )) {
                error = "Invalid ',' at " + ct->at() + ": '" + ct->value() + "'.";
                break;
            }

            // populate the item
            if (item->name_.empty()) {
                if (item->is_leaf_) {
                    item->name_ = "Leaf Node";
                } else {
                    item->name_ = "Internal Node";
                }
            }
            items_.push_back(item);
            item = nullptr;
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
                error = "Invalid ';' at " + ct->at() + ": '" + ct->value() + "'.";
                break;
            }

            // populate the item
            if (item->name_.empty()) {
                item->name_ = "Root Node";
            }
            items_.push_back(item);
            item = nullptr;
            break;
        }

        // if we reach this part of the code, all checkings for token types are done.
        // as we check for every type that NewickLexer yields, and we use a continue or break
        // in each of them, we should never reach this point, unless we forgot a type!
        assert(false);
    }

    if (!error.empty()) {
        LOG_INFO << error;
        return false;
    }

    if (depth != 0) {
        LOG_INFO << "Not enough closing parenthesis.";
        return false;
    }

    if (ct == lexer.end() || !ct->IsOperator(";")) {
        LOG_INFO << "Tree does not finish with a semicolon.";
        return false;
    }

    // skip the semicolon, then see if there is anything other than a comment left
    ++ct;
    while (ct != lexer.end() && ct->type() == kComment) {
        ++ct;
    }
    if (ct != lexer.end()) {
        LOG_INFO << "Tree contains more data after the semicolon.";
        return false;
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
        for (int i = 0; i < item->depth_; ++i) {
            out += "    ";
        }
        out += item->name_
            + (item->branch_length_ != 0.0 ? ":" + std::to_string(item->branch_length_) : "")
            + (!item->comment_.empty() ? " [" + item->comment_ + "]" : "")
            + (!item->tag_.empty() ? " {" + item->tag_ + "}" : "")
            + (item->is_leaf_ ? " (Leaf)\n" : "\n");
    }
    return out;
}

}

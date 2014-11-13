/**
 * @brief Implementation of Newick Parser functions.
 *
 * @file
 * @ingroup tree
 */

//~ #include <cstring>
//~ #include <vector>
//~ #include <stack>
#include <assert.h>
#include <iomanip>
#include <string>

#include "newick_parser.hh"
#include "../utils/logging.hh"

namespace genesis {

bool NewickParser::Process (const NewickLexer& lexer)
{
    // check boundary conditions
    if (lexer.empty()) {
        LOG_INFO << "Cannot parse Newick tree, it is empty.";
        return false;
    }
    if (lexer.HasError()) {
        LOG_INFO << "Cannot parse Newick tree. Lexing error at " << lexer.back().at()
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

    // --------------------------------------------------------------
    //     Loop over lexer tokens and check if it...
    // --------------------------------------------------------------
    for (ct = lexer.begin(); ct != lexer.end(); pt=ct, ++ct) {

        // ------------------------------------------------------
        //     is bracket '('
        // ------------------------------------------------------
        if (ct->IsBracket('(')) {
            ++depth;
            continue;
        }

        // if we reach this, the previous condition is not fullfilled (continue was not called).
        if (ct == lexer.begin()) {
            LOG_INFO << "Cannot parse Newick tree, it does not start with '(' at "
                     << ct->at() << ".";
            return false;
        }

        if (ct->type() == kUnknown) {
            LOG_INFO << "Cannot parse Newick tree, invalid characters at " << ct->at()
                     << ": '" << std::hex << ct->value() << "'.";
            return false;
        }

        // if we reached this point in code, this means that ct != begin, so it is not the first
        // iteration in this loop. this means that pt was already set in the loop header (at least
        // once), which means it now points to a valid token. if not, there is something wrong with
        // this code!
        assert(pt != lexer.end());

        // ------------------------------------------------------
        //     is bracket ')'
        // ------------------------------------------------------
        if (ct->IsBracket(')')) {
            if (depth == 0) {
                LOG_INFO << "Cannot parse Newick tree, too many ')' at " << ct->at() << ".";
                return false;
            }
            if (!(
                pt->IsBracket(')')    || pt->IsTag('}') ||
                pt->type() == kSymbol || pt->type() == kString || pt->type() == kNumber
            )) {
                LOG_INFO << "Cannot parse Newick tree, invalid characters at "
                         << ct->at() << ": '" << ct->value() << "'.";
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
        //     is string
        // ------------------------------------------------------
        if (ct->type() == kSymbol || ct->type() == kString) {
            if (!(pt->IsBracket('(') || pt->IsBracket(')') || pt->IsOperator(','))) {
                LOG_INFO << "Cannot parse Newick tree, invalid characters at "
                         << ct->at() << ": '" << ct->value() << "'.";
                return false;
            }

            // populate the item
            if (!item) {
                item = new NewickParserItem();
            }
            // TODO hier += oder nur =? wegen labels = num+string
            item->name = ct->value();
            item->depth = depth;
            if (pt->IsBracket('(') || pt->IsOperator(',')) {
                item->is_leaf = true;
            }

            // set state
            item_active = true;
            continue;
        }

        // ------------------------------------------------------
        //     is number
        // ------------------------------------------------------
        if (ct->type() ==  kNumber) {

        }

        // ------------------------------------------------------
        //     is semicolon ';'
        // ------------------------------------------------------
        if (ct->IsOperator(';')) {
            LOG_INFO << "end;";
        }
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

    return depth == 0;
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
        out += item->name + ":" + std::to_string(item->branch_length) + "\n";
    }
    return out;
}

}

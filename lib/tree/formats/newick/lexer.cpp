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

#include "tree/formats/newick/lexer.hpp"

#include <assert.h>

namespace genesis {
namespace tree {

// =================================================================================================
//     Newick Lexer
// =================================================================================================

NewickLexer::NewickLexer() {
    using namespace utils;

    // set the special chars for newick trees
    set_char_type (LexerTokenType::kComment,  "[]");
    set_char_type (LexerTokenType::kTag,      "{}");
    set_char_type (LexerTokenType::kBracket,  "()");
    set_char_type (LexerTokenType::kOperator, ",;=");

    // we use symbols and strings the same way here: both are labels for nodes, the first begin
    // called unquoted_label, the second quoted_label.
    set_char_type (LexerTokenType::kString,   "'");

    // the only numbers in newick are branch lengths, which are always introduced by a leading
    // colon, so we need only this here as starter for a number.
    set_char_type (LexerTokenType::kNumber,   ":");

    // this also allows (in accordance to the newick standard) to start a label with a digit.
    set_char_type (LexerTokenType::kSymbol,   "0123456789");

    // furthermore, set all remaining graphic chars to symbol so that they can be in a label.
    set_char_type (LexerTokenType::kSymbol,   "!\"#$%&*+-./<>?@\\^_`|~");

    // set the flags as needed
    include_whitespace        = false;
    include_comments          = true;
    glue_sign_to_number       = false;
    trim_quotation_marks      = true;
    use_string_escape         = false;
    use_string_doubled_quotes = true;
}

bool NewickLexer::scan_comment()
{
    using namespace utils;

    if (get_char() == ']') {
        push_token(LexerTokenType::kError, get_position(), "Closing comment without opening it.");
        return false;
    }
    size_t start = get_position();
    bool   found = evaluate_from_to("[", "]");
    if (!found && get_char() == '[') {
        push_token(LexerTokenType::kError, get_position(), "Comment not closed.");
        return false;
    }
    if (found && include_comments) {
        push_token(LexerTokenType::kComment, start+1, get_position()-1);
    }
    return found;
}

bool NewickLexer::scan_number()
{
    // colon is the only char that is set as the beginning char for a number, so when we enter
    // this function, it must be the current char in the text
    assert(get_char() == ':');

    // skip it and continue to scan as a normal number
    next_char();
    return utils::Lexer::scan_number();
}

bool NewickLexer::scan_tag()
{
    using namespace utils;

    if (get_char() == '}') {
        push_token(LexerTokenType::kError, get_position(), "Closing tag without opening tag.");
        return false;
    }

    // curly brackets are the only chars that are used as tag in this lexer. we already checked
    // that the current char is not a closing bracket, so it must be an opening one.
    assert(get_char() == '{');

    size_t start = get_position();
    bool   found = evaluate_from_to("{", "}");
    if (!found) {
        push_token(LexerTokenType::kError, start, "Opening tag without closing tag.");
        return false;
    }
    push_token(LexerTokenType::kTag, start+1, get_position()-1);
    return true;
}

} // namespace tree
} // namespace genesis

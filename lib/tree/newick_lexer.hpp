#ifndef GENESIS_TREE_NEWICK_LEXER_H_
#define GENESIS_TREE_NEWICK_LEXER_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <assert.h>

#include "utils/lexer.hpp"

namespace genesis {

// =============================================================================
//     Newick Lexer
// =============================================================================

class NewickLexer : public Lexer
{
public:
    NewickLexer() {
        // set the special chars for newick trees
        SetCharType (LexerTokenType::kComment,  "[]");
        SetCharType (LexerTokenType::kTag,      "{}");
        SetCharType (LexerTokenType::kBracket,  "()");
        SetCharType (LexerTokenType::kOperator, ",;=");

        // we use symbols and strings the same way here: both are labels for nodes, the first begin
        // called unquoted_label, the second quoted_label.
        SetCharType (LexerTokenType::kString,   "'");

        // the only numbers in newick are branch lengths, which are always introduced by a leading
        // colon, so we need only this here as starter for a number.
        SetCharType (LexerTokenType::kNumber,   ":");

        // this also allows (in accordance to the newick standard) to start a label with a digit.
        SetCharType (LexerTokenType::kSymbol,   "0123456789");

        // furthermore, set all remaining graphic chars to symbol so that they can be in a label.
        SetCharType (LexerTokenType::kSymbol,   "!\"#$%&*+-./<>?@\\^_`|~");

        // set the flags as needed
        include_whitespace        = false;
        include_comments          = true;
        glue_sign_to_number       = false;
        trim_quotation_marks      = true;
        use_string_escape         = false;
        use_string_doubled_quotes = true;
    }

protected:
    inline bool ScanComment()
    {
        if (GetChar() == ']') {
            PushToken(LexerTokenType::kError, GetPosition(), "Closing comment without opening it.");
            return false;
        }
        size_t start = GetPosition();
        bool   found = EvaluateFromTo("[", "]");
        if (!found && GetChar() == '[') {
            PushToken(LexerTokenType::kError, GetPosition(), "Comment not closed.");
            return false;
        }
        if (found && include_comments) {
            PushToken(LexerTokenType::kComment, start+1, GetPosition()-1);
        }
        return found;
    }

    inline bool ScanNumber()
    {
        // colon is the only char that is set as the beginning char for a number, so when we enter
        // this function, it must be the current char in the text
        assert(GetChar() == ':');

        // skip it and continue to scan as a normal number
        NextChar();
        return Lexer::ScanNumber();
    }

    inline bool ScanTag()
    {
        if (GetChar() == '}') {
            PushToken(LexerTokenType::kError, GetPosition(), "Closing tag without opening tag.");
            return false;
        }

        // curly brackets are the only chars that are used as tag in this lexer. we already checked
        // that the current char is not a closing bracket, so it must be an opening one.
        assert(GetChar() == '{');

        size_t start = GetPosition();
        bool   found = EvaluateFromTo("{", "}");
        if (!found) {
            PushToken(LexerTokenType::kError, start, "Opening tag without closing tag.");
            return false;
        }
        PushToken(LexerTokenType::kTag, start+1, GetPosition()-1);
        return true;
    }
};

} // namespace genesis

#endif // include guard

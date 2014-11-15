#ifndef GNS_TREE_NEWICKLEXER_H_
#define GNS_TREE_NEWICKLEXER_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <assert.h>

#include "utils/lexer.hh"

namespace genesis {

class NewickLexer : public Lexer
{
public:
    NewickLexer() {
        // set the special chars for newick trees
        SetCharType (kComment,  "[]");
        SetCharType (kTag,      "{}");
        SetCharType (kBracket,  "()");
        SetCharType (kOperator, ",;");

        // we use symbols and strings the same way here: both are labels for nodes, the first begin
        // called unquoted_label, the second quoted_label.
        SetCharType (kString,   "'");

        // the only numbers in newick are branch lengths, which are always introduced by a leading
        // colon, so we need only this here as starter for a number.
        SetCharType (kNumber,   ":");

        // this also allows (in accordance to the newick standard) to start a label with a digit.
        SetCharType (kSymbol,   "0123456789");

        // furthermore, set all remaining graphic chars to symbol so that they can be in a label.
        SetCharType (kSymbol,   "!\"#$%&*+-./<=>?@\\^_`|~");

        // set the flags as needed
        include_whitespace        = false;
        include_comments          = true;
        glue_sign_to_number       = false;
        trim_quotation_marks      = true;
        use_string_escape         = false;
        use_string_doubled_quotes = true;
    }

    /** @brief Determines whether to include tags in curly brackets. */
    bool include_tags = true;

protected:
    inline bool ScanComment()
    {
        if (GetChar() == ']') {
            PushToken(kError, GetPosition(), "Closing comment without opening it.");
            return false;
        }
        size_t start = GetPosition();
        bool   found = ScanFromTo("[", "]");
        if (!found && GetChar() == '[') {
            PushToken(kError, GetPosition(), "Comment not closed.");
            return false;
        }
        if (found && include_comments) {
            PushToken(kComment, start+1, GetPosition()-1);
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

    inline bool ScanOperator()
    {
        PushToken(kOperator, GetPosition(), GetPosition()+1);
        NextChar();
        return true;
    }

    inline bool ScanTag()
    {
        if (GetChar() == '}') {
            PushToken(kError, GetPosition(), "Closing tag without opening tag.");
            return false;
        }
        size_t start = GetPosition();
        bool   found = ScanFromTo("{", "}");
        if (!found) {
            PushToken(kError, start, "Opening tag without closing tag.");
            return false;
        }
        if (include_tags) {
            PushToken(kTag, start+1, GetPosition()-1);
        }
        return true;
    }
};

} // namespace genesis

#endif // include guard

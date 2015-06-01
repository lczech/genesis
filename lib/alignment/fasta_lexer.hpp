#ifndef GENESIS_ALIGNMENT_FASTA_LEXER_H_
#define GENESIS_ALIGNMENT_FASTA_LEXER_H_

/**
 * @brief
 *
 * @file
 * @ingroup alignment
 */

#include <assert.h>

#include "utils/lexer.hpp"

namespace genesis {

// =============================================================================
//     Fasta Lexer
// =============================================================================

class FastaLexer : public Lexer
{
public:
    FastaLexer() {
        // whitespaces except new lines (CR and LF) are not part of the fasta format.
        set_char_type (LexerTokenType::kUnknown,  "\x09\x0B\x0C");
        set_char_type (LexerTokenType::kUnknown,  " ");

        // we use a tag for marking the label of a sequence.
        set_char_type (LexerTokenType::kTag,      ">");

        // set the two special fasta symbols for gap and sequence end.
        set_char_type (LexerTokenType::kSymbol,   "-*");

        // comments start with ; and continue until the end of the line.
        set_char_type (LexerTokenType::kComment,  ";");

        // digits are not part of fasta sequences.
        set_char_type (LexerTokenType::kUnknown,   "0123456789");

        // set the flags as needed.
        include_whitespace        = false;
        include_comments          = false;
        glue_sign_to_number       = false;
        trim_quotation_marks      = false;
        use_string_escape         = false;
        use_string_doubled_quotes = false;
    }

protected:

    inline bool scan_comment()
    {
        // semicolon is the only char starting a comment. if found, skip it.
        if (get_char() != ';') {
            return false;
        }
        next_char();

        // comments end with a new line. new lines are the only white space, scan until one found.
        size_t start = get_position();
        while (!is_end() && get_char_type() != LexerTokenType::kWhite) {
            next_char();
        }
        if (include_comments) {
            push_token(LexerTokenType::kComment, start, get_position());
        }
        return true;
    }

    inline bool scan_tag()
    {
        // the greater sign is the only one used to start a tag. skip it.
        assert(get_char() == '>');
        next_char();

        if (is_end()) {
            // TODO error
        }

        // scan until we reach a new line (this is the only type of whitespace used in this lexer)
        size_t start = get_position();
        while (!is_end() && get_char_type() != LexerTokenType::kWhite) {
            next_char();
        }
        push_token(LexerTokenType::kTag, start, get_position());
        return true;
    }
};

} // namespace genesis

#endif // include guard

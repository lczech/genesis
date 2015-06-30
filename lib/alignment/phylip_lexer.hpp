#ifndef GENESIS_ALIGNMENT_PHYLIP_LEXER_H_
#define GENESIS_ALIGNMENT_PHYLIP_LEXER_H_

/**
 * @brief Header of PhylipLexer class.
 *
 * @file
 * @ingroup alignment
 */

#include "utils/lexer.hpp"

namespace genesis {

// =================================================================================================
//     PhylipLexer
// =================================================================================================

/**
 * @brief
 */
class PhylipLexer : public Lexer
{
public:
    PhylipLexer() {
        // we want to include all special chars, in order to allow them in labels.
        // those which do not belong into a sequence are filtered out later.
        set_char_type (LexerTokenType::kSymbol, "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~");

        // set the flags as needed.
        include_whitespace        = true;
        include_comments          = false;
        glue_sign_to_number       = false;
        trim_quotation_marks      = false;
        use_string_escape         = false;
        use_string_doubled_quotes = false;
    }
};

} // namespace genesis

#endif // include guard

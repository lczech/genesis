#ifndef GNS_UTILS_JSONLEXER_H_
#define GNS_UTILS_JSONLEXER_H_

/**
 * @brief JSON lexer class. See JsonLexer.
 *
 * @file
 * @ingroup utils
 */

#include <string>

#include "utils/lexer.hpp"

namespace genesis {

/**
 * @brief JSON lexer that takes a JSON document string and turns it into a sequence of tokens.
 *
 * See Lexer class for details on how the lexing process works.
 */
class JsonLexer : public Lexer
{
public:
    /** @brief Constructor that sets the basic JSON character types. */
    JsonLexer() {
        // set the special chars for json files
        SetCharType (LexerType::kBracket,  "[]{}");
        SetCharType (LexerType::kOperator, ",:");
        SetCharType (LexerType::kString,   "\"");
        SetCharType (LexerType::kNumber,   "+-");

        // set the flags as needed
        include_whitespace        = false;
        include_comments          = false;
        glue_sign_to_number       = true;
        trim_quotation_marks      = true;
        use_string_escape         = true;
        use_string_doubled_quotes = false;
    }

protected:
    /**
     * @brief Scans a JSON symbol.
     *
     * JSON only knows three different symbols: `null`, `true` and `false`. Every other sequence
     * of chars must be a string enclosed in double quotation marks, which is covered by the
     * ScanString() function. Thus, this function scans for non-enclosed char sequences and checks
     * if it is one of the three. If so, it is returned as a symbol, oterhwise an error token is
     * produced.
     */
    inline bool ScanSymbol()
    {
        size_t start = GetPosition();
        while (!IsEnd() && GetCharType() == LexerType::kSymbol) {
            NextChar();
        }

        std::string res = GetSubstr(start, GetPosition());
        if (res.compare("null") && res.compare("true") && res.compare("false")) {
            PushToken(LexerType::kError, start, "Invalid symbols: \"" + res + "\"");
            return false;
        }
        PushToken(LexerType::kSymbol, start, res);
        return true;
    }
};

} // namespace genesis

#endif // include guard

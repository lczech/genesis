#ifndef GNS_UTILS_JSONLEXER_H_
#define GNS_UTILS_JSONLEXER_H_

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include <string>

#include "utils/lexer.hh"

namespace genesis {

class JsonLexer : public Lexer
{
public:
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

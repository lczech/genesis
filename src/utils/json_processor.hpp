#ifndef GNS_UTILS_JSONPROCESSOR_H_
#define GNS_UTILS_JSONPROCESSOR_H_

/**
 * @brief Functions for parsing and printing a JSON document.
 *
 * @file
 * @ingroup utils
 */

#include <string>

#include "utils/lexer.hpp"

namespace genesis {

// =============================================================================
//     Forward declarations
// =============================================================================

class JsonDocument;
class JsonValue;
class JsonValueArray;
class JsonValueObject;

// =============================================================================
//     Json Lexer
// =============================================================================

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

// =============================================================================
//     Json Processor
// =============================================================================

/**
 * @brief A JSON parser that filles a JsonDocument with data from different JSON sources.
 *
 * This class provides three functions for parsing JSON data. Those take an input source containing
 * JSON data and parse them into a JsonDocument.
 */
class JsonProcessor
{
    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

public:
    static bool FromFile   (const std::string& fn,    JsonDocument& document);
    static bool FromString (const std::string& json,  JsonDocument& document);
    static bool FromLexer  (const JsonLexer&   lexer, JsonDocument& document);

    // TODO add something like ProcessPartialString that takes any json value and not just a whole doc

protected:
    static bool ParseValue (
        Lexer::const_iterator& ct,
        Lexer::const_iterator& end,
        JsonValue*&            value
    );

    static bool ParseArray (
        Lexer::const_iterator& ct,
        Lexer::const_iterator& end,
        JsonValueArray*        value
    );

    static bool ParseObject (
        Lexer::const_iterator& ct,
        Lexer::const_iterator& end,
        JsonValueObject*       value
    );

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

public:
    static int precision;
    static int indent;

    static bool        ToFile   (const std::string& fn,   const JsonDocument& document);
    static void        ToString (      std::string& json, const JsonDocument& document);
    static std::string ToString (                         const JsonDocument& document);

protected:
    static std::string PrintValue  (const JsonValue*       value);
    static std::string PrintArray  (const JsonValueArray*  value, const int indent_level);
    static std::string PrintObject (const JsonValueObject* value, const int indent_level);
};

} // namespace genesis

#endif // include guard

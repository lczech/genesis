#ifndef GNS_UTILS_JSONPARSER_H_
#define GNS_UTILS_JSONPARSER_H_

/**
 * @brief Functions for parsing a JSON document.
 *
 * @file
 * @ingroup utils
 */

#include <string>

#include "utils/lexer.hh"
#include "utils/json_lexer.hh"
#include "utils/json_document.hh"

namespace genesis {

/**
 * @brief A JSON parser that filles a JsonDocument with data from a JsonLexer or a JSON string.
 *
 * This class provides two functions for parsing JSON data. On of the takes a JsonLexer and puts its
 * contents into a JsonDocument, the other one takes a string containing JSON-formatted data and
 * calls the lexer on this string first, before then calling the first parsing function again (it is
 * a shortcut to avoid instanciating the lexer object first by hand).
 */
class JsonParser
{
public:
    static bool Process (const std::string& json,  JsonDocument& document);
    static bool Process (const JsonLexer&   lexer, JsonDocument& document);

protected:
    static bool ProcessValue (
        Lexer::const_iterator& ct,
        Lexer::const_iterator& end,
        JsonValue*&            value
    );

    static bool ProcessArray (
        Lexer::const_iterator& ct,
        Lexer::const_iterator& end,
        JsonValueArray*        value
    );

    static bool ProcessObject (
        Lexer::const_iterator& ct,
        Lexer::const_iterator& end,
        JsonValueObject*       value
    );
};

} // namespace genesis

#endif // include guard

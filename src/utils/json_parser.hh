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
 * @brief A JSON parser that filles a JsonDocument with data from different JSON sources.
 *
 * This class provides three functions for parsing JSON data. Those take an input source containing
 * JSON data and parse them into a JsonDocument.
 */
class JsonParser
{
public:
    static bool ProcessFile   (const std::string& fn,    JsonDocument& document);
    static bool ProcessString (const std::string& json,  JsonDocument& document);
    static bool ProcessLexer  (const JsonLexer&   lexer, JsonDocument& document);

    // TODO add something like ProcessPartialString that takes any json value and not just a whole doc

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

#ifndef GNS_UTILS_JSONPARSER_H_
#define GNS_UTILS_JSONPARSER_H_

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include <string>

#include "utils/lexer.hh"
#include "utils/json_lexer.hh"
#include "utils/json_document.hh"

namespace genesis {

class JsonParser
{
public:
    static bool Process (const std::string& json,  JsonDocument* document);
    static bool Process (const JsonLexer&   lexer, JsonDocument* document);

protected:
    static bool ProcessValue (
        Lexer::const_iterator& ct,
        Lexer::const_iterator& end,
        JsonValue*             value
    );

    static bool ProcessArray (
        Lexer::const_iterator& ct,
        Lexer::const_iterator& end,
        JsonValue*             value
    );

    static bool ProcessObject (
        Lexer::const_iterator& ct,
        Lexer::const_iterator& end,
        JsonValue*             value
    );
};

} // namespace genesis

#endif // include guard

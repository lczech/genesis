#ifndef GENESIS_UTILS_JSON_PROCESSOR_H_
#define GENESIS_UTILS_JSON_PROCESSOR_H_

/**
 * @brief Functions for parsing and printing a JSON document. See JsonProcessor for more.
 *
 * @file
 * @ingroup utils
 */

#include <string>

#include "utils/json_lexer.hpp"

namespace genesis {

// =============================================================================
//     Forward declarations
// =============================================================================

class JsonDocument;
class JsonValue;
class JsonValueArray;
class JsonValueObject;

// =============================================================================
//     Json Processor
// =============================================================================

/**
 * @brief A JSON parser that fills a JsonDocument with data from different JSON sources.
 *
 * This class provides functions for parsing JSON data. Those take an input source containing
 * JSON data and parse them into a JsonDocument.
 *
 * The parsing works this way:
 *
 * Each JSON document is also a JSON object, and can contain other objects, JSON arrays, or simple
 * value types. The parsing here is thus splitted in those three functions, being recursively called
 * for every level of nesting within objects and arrays.
 *
 * Those three functions (objects, arrays, simple values) take an interator to the current lexer
 * token by reference and advance it until it points to the next token after processing the current
 * object/array/value. To check for the end of the lexer, an intererator to its end is also
 * provided, as well as a pointer to the resulting JSON value, which is filled with data during the
 * execution of the functions.
 */
class JsonProcessor
{
    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

public:
    static bool from_file   (const std::string& fn,    JsonDocument& document);
    static bool from_string (const std::string& json,  JsonDocument& document);

    // TODO add something like ProcessPartialString that takes any json value and not just a whole doc

protected:

    static bool parse_value (
        JsonLexer::iterator& ct,
        JsonLexer::iterator& end,
        JsonValue*&          value
    );

    static bool parse_array (
        JsonLexer::iterator& ct,
        JsonLexer::iterator& end,
        JsonValueArray*      value
    );

    static bool parse_object (
        JsonLexer::iterator& ct,
        JsonLexer::iterator& end,
        JsonValueObject*     value
    );

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

public:
    static int precision;
    static int indent;

    static bool        to_file   (const std::string& fn,   const JsonDocument& document);
    static void        to_string (      std::string& json, const JsonDocument& document);
    static std::string to_string (                         const JsonDocument& document);

protected:
    static std::string print_value  (const JsonValue*       value);
    static std::string print_array  (const JsonValueArray*  value, const int indent_level);
    static std::string print_object (const JsonValueObject* value, const int indent_level);
};

} // namespace genesis

#endif // include guard

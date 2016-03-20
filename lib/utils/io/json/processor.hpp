#ifndef GENESIS_UTILS_IO_JSON_PROCESSOR_H_
#define GENESIS_UTILS_IO_JSON_PROCESSOR_H_

/**
 * @brief Functions for parsing and printing a JSON document. See JsonProcessor for more.
 *
 * @file
 * @ingroup utils
 */

#include <string>

#include "utils/io/json/lexer.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Forward declarations
// =================================================================================================

class JsonDocument;
class JsonValue;
class JsonValueArray;
class JsonValueObject;

// =================================================================================================
//     Json Processor
// =================================================================================================

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
    bool from_file   (const std::string& filename, JsonDocument& document);
    bool from_string (const std::string& json,     JsonDocument& document);

    // TODO add something like ProcessPartialString that takes any json value and not just a whole doc

protected:

    bool parse_value (
        JsonLexer::iterator& ct,
        JsonLexer::iterator& end,
        JsonValue*&          value
    );

    bool parse_array (
        JsonLexer::iterator& ct,
        JsonLexer::iterator& end,
        JsonValueArray*      value
    );

    bool parse_object (
        JsonLexer::iterator& ct,
        JsonLexer::iterator& end,
        JsonValueObject*     value
    );

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

public:

    void        to_file   (const std::string& filename,const JsonDocument& document);
    void        to_string (      std::string& json,    const JsonDocument& document);
    std::string to_string (                            const JsonDocument& document);

    // -----------------------------------------------------
    //     Internal
    // -----------------------------------------------------

protected:
    std::string print_value  (const JsonValue*       value);
    std::string print_array  (const JsonValueArray*  value, const int indent_level);
    std::string print_object (const JsonValueObject* value, const int indent_level);

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

public:

    /**
     * @brief The precision used for printing floating point numbers, particularly Json Value Numbers.
     */
    int precision = 6;

    /**
     * @brief The indent used for printing the elements of Json Arrays and Objects.
     */
    int indent = 4;

};

} // namespace utils
} // namespace genesis

#endif // include guard

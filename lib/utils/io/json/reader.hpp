#ifndef GENESIS_UTILS_IO_JSON_READER_H_
#define GENESIS_UTILS_IO_JSON_READER_H_

/**
 * @brief
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
 * @brief Read `Json` data.
 *
 * This class provides functions for parsing `json` data into a JsonDocument. The parsing works
 * this way:
 *
 * Each JsonDocutment is also a JsonObject, and can contain other objects, JsonArray%s, or simple
 * value types. The parsing here is thus splitted in those three functions, being recursively called
 * for every level of nesting within objects and arrays.
 *
 * Those three functions (objects, arrays, simple values) take an interator to the current lexer
 * token by reference and advance it until it points to the next token after processing the current
 * object/array/value. To check for the end of the lexer, an intererator to its end is also
 * provided, as well as a pointer to the resulting JSON value, which is filled with data during the
 * execution of the functions.
 */
class JsonReader
{
public:

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    JsonReader()  = default;
    ~JsonReader() = default;

    JsonReader( JsonReader const& ) = default;
    JsonReader( JsonReader&& )      = default;

    JsonReader& operator= ( JsonReader const& ) = default;
    JsonReader& operator= ( JsonReader&& )      = default;

    // ---------------------------------------------------------------------
    //     Reading
    // ---------------------------------------------------------------------

    void from_file   (const std::string& filename, JsonDocument& document) const;
    void from_string (const std::string& json,     JsonDocument& document) const;

    // TODO add something like ProcessPartialString that takes any json value and not just a whole doc

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

protected:

    void parse_value (
        JsonLexer::iterator& ct,
        JsonLexer::iterator& end,
        JsonValue*&          value
    ) const;

    void parse_array (
        JsonLexer::iterator& ct,
        JsonLexer::iterator& end,
        JsonValueArray*      value
    ) const;

    void parse_object (
        JsonLexer::iterator& ct,
        JsonLexer::iterator& end,
        JsonValueObject*     value
    ) const;

};

} // namespace utils
} // namespace genesis

#endif // include guard

#ifndef GENESIS_UTILS_FORMATS_JSON_READER_H_
#define GENESIS_UTILS_FORMATS_JSON_READER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include <string>

#include "utils/formats/json/lexer.hpp"

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
//     Json Reader
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

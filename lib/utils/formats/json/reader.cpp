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

#include "utils/formats/json/reader.hpp"

#include <assert.h>
#include <stdexcept>

#include "utils/core/fs.hpp"
#include "utils/formats/json/document.hpp"
#include "utils/text/string.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Reading
// =================================================================================================

/**
 * @brief Take a JSON document file path and parses its contents into a JsonDocument.
 *
 * If the file does not exists, the function throws an `std::runtime_error`.
 */
void JsonReader::from_file (const std::string& filename, JsonDocument& document) const
{
    if( ! utils::file_exists(filename)) {
        throw std::runtime_error( "Json file '" + filename + "' does not exist." );
    }
    return from_string( utils::file_read(filename), document );
}

/**
 * @brief Take a string containing a JSON document and parses its contents into a JsonDocument.
 *
 * Returns true iff successfull.
 */
void JsonReader::from_string (const std::string& json, JsonDocument& document) const
{
    // do stepwise lexing
    JsonLexer lexer;
    lexer.from_string(json);

    if (lexer.empty()) {
        throw std::runtime_error( "Json document is empty." );
    }
    if (lexer.has_error()) {
        throw std::runtime_error(
            "Lexing error at " + lexer.back().at() + " with message: " + lexer.back().value()
        );
    }
    if (!lexer.begin()->is_bracket("{")) {
        throw std::runtime_error( "JSON document does not start with JSON object opener '{'." );
    }

    // A json document is also a json object, so we start parsing the doc as such.
    // The begin iterator will be incremented with every token being processed.
    document.clear();
    JsonLexer::iterator begin = lexer.begin();
    JsonLexer::iterator end   = lexer.end();

    parse_object(begin, end, &document);

    // After processing, the begin iterator will point to the lexer token that comes after
    // the one being processed last. If the document is well-formatted, this token is also
    // the end pointer of the iterator.
    if (begin != end) {
        throw std::runtime_error(
            "JSON document contains more information after the closing bracket."
        );
    }
}

// =================================================================================================
//     Parsing
// =================================================================================================

// -----------------------------------------------------------------------------
//     Parse Value
// -----------------------------------------------------------------------------

/**
 * @brief Parse a JSON value and fills it with data from the lexer.
 *
 * This function is a bit different from the other two process functions parse_array() and
 * parse_object(), because it takes its value parameter by reference. This is because when
 * entering the function it is not clear yet which type of value the current lexer token is, so a
 * new instance has to be created and stored in the pointer.
 */
void JsonReader::parse_value (
    JsonLexer::iterator& ct,
    JsonLexer::iterator& end,
    JsonValue*&          value
) const {
    // Proper usage of this function is to hand over a null pointer to a json value, which will be
    // assigned to a newly created value instance depending on the token type, so check for this
    // here. We don't want to overwrite existing values!
    assert (value == nullptr);

    // Check all possible valid lexer token types and turn them into json values.
    if (ct->is_symbol()) {
        // The lexer only returns null, true or false as symbols, so this is safe.
        if (ct->value().compare("null") == 0) {
            value = new JsonValueNull();
        } else {
            value = new JsonValueBool(ct->value());
        }
        ++ct;
        return;
    }

    if (ct->is_number()) {
        value = new JsonValueNumber(ct->value());
        ++ct;
        return;
    }

    if (ct->is_string()) {
        value = new JsonValueString(ct->value());
        ++ct;
        return;
    }

    if (ct->is_bracket("[")) {
        value = new JsonValueArray();
        parse_array (ct, end, json_value_to_array(value));
        return;
    }

    if (ct->is_bracket("{")) {
        value = new JsonValueObject();
        parse_object (ct, end, json_value_to_object(value));
        return;
    }

    // If the lexer token is not a fitting json value, we have an error.
    throw std::runtime_error(
        "JSON value contains invalid characters at " + ct->at() + ": '" + ct->value() + "'."
    );
}

// -----------------------------------------------------------------------------
//     Parse Array
// -----------------------------------------------------------------------------

/**
 * @brief Parse a JSON array and fill it with data elements from the lexer.
 */
void JsonReader::parse_array (
    JsonLexer::iterator& ct,
    JsonLexer::iterator& end,
    JsonValueArray*        value
) const {
    // Proper usage of this function is to hand over a valid pointer to a json array, so check
    // for this here.
    assert(value);

    if (ct == end || !ct->is_bracket("[")) {
        throw std::runtime_error( "JSON array does not start with '[' at " + ct->at() + "." );
    }

    ++ct;
    while (ct != end) {
        // Proccess the array element.
        JsonValue* element = nullptr;
        parse_value( ct, end, element );
        value->add(element);

        // Check for end of array, leave if found.
        if (ct == end || ct->is_bracket("]")) {
            break;
        }

        // Check for delimiter comma (indicates that there are more elements following).
        if (!ct->is_operator(",")) {
            throw std::runtime_error(
                "JSON array does not contain comma between elements at " + ct->at() + "."
            );
        }
        ++ct;
    }

    // The while loop above only stops when ct points to the end or to a closing bracket. In the
    // first case, we have an error; in the second, we are done with this object and can skip the
    // bracket.
    if (ct == end) {
        throw std::runtime_error( "JSON array ended unexpectedly." );
    }

    assert(ct->is_bracket("]"));
    ++ct;
}

// -----------------------------------------------------------------------------
//     Parse Object
// -----------------------------------------------------------------------------

/**
 * @brief Parse a JSON object and fill it with data members from the lexer.
 */
void JsonReader::parse_object (
    JsonLexer::iterator& ct,
    JsonLexer::iterator& end,
    JsonValueObject*       value
) const {
    // Proper usage of this function is to hand over a valid pointer to a json object, so check
    // for this here.
    assert(value);

    if (ct == end || !ct->is_bracket("{")) {
        throw std::runtime_error( "JSON object does not start with '{' at " + ct->at() + "." );
    }

    ++ct;
    while (ct != end) {
        // Check for name string and store it.
        if (!ct->is_string()) {
            throw std::runtime_error(
                "JSON object member does not start with name string at " + ct->at() + "."
            );
        }
        std::string name = ct->value();
        ++ct;

        // Check for delimiter colon.
        if (ct == end) {
            break;
        }
        if (!ct->is_operator(":")) {
            throw std::runtime_error(
                "JSON object member does not contain colon between name and value at "
                + ct->at() + "."
            );
        }
        ++ct;

        // Check for value and store it.
        if (ct == end) {
            break;
        }

        JsonValue* member = nullptr;
        parse_value( ct, end, member );
        value->set( name, member );

        // Check for end of object, leave if found (either way).
        if (ct == end || ct->is_bracket("}")) {
            break;
        }

        // Check for delimiter comma (indicates that there are more members following).
        if (!ct->is_operator(",")) {
            throw std::runtime_error(
                "JSON object does not contain comma between members at " + ct->at() + "."
            );
        }
        ++ct;
    }

    // The while loop above only stops when ct points to the end or to a closing bracket. In the
    // first case, we have an error; in the second, we are done with this object and can skip the
    // bracket.
    if (ct == end) {
        throw std::runtime_error( "JSON object ended unexpectedly." );
    }
    ++ct;
}

} // namespace utils
} // namespace genesis

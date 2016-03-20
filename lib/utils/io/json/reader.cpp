/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "utils/io/json/reader.hpp"

#include <assert.h>

#include "utils/core/fs.hpp"
#include "utils/core/logging.hpp"
#include "utils/io/json/document.hpp"
#include "utils/text/string.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Reading
// =================================================================================================

/**
 * @brief Take a JSON document file path and parses its contents into a JsonDocument.
 *
 * Returns true iff successfull.
 */
bool JsonReader::from_file (const std::string& filename, JsonDocument& document)
{
    if( ! utils::file_exists(filename)) {
        LOG_WARN << "JSON file '" << filename << "' does not exist.";
        return false;
    }
    return from_string( utils::file_read(filename), document );
}

/**
 * @brief Take a string containing a JSON document and parses its contents into a JsonDocument.
 *
 * Returns true iff successfull.
 */
bool JsonReader::from_string (const std::string& json, JsonDocument& document)
{
    // do stepwise lexing
    JsonLexer lexer;
    lexer.from_string(json);

    if (lexer.empty()) {
        LOG_INFO << "JSON document is empty.";
        return false;
    }
    if (lexer.has_error()) {
        LOG_WARN << "Lexing error at " << lexer.back().at()
                 << " with message: " << lexer.back().value();
        return false;
    }
    if (!lexer.begin()->is_bracket("{")) {
        LOG_WARN << "JSON document does not start with JSON object opener '{'.";
        return false;
    }

    // A json document is also a json object, so we start parsing the doc as such.
    // The begin iterator will be incremented with every token being processed.
    document.clear();
    JsonLexer::iterator begin = lexer.begin();
    JsonLexer::iterator end   = lexer.end();

    if (!parse_object(begin, end, &document)) {
        return false;
    }

    // After processing, the begin iterator will point to the lexer token that comes after
    // the one being processed last. If the document is well-formatted, this token is also
    // the end pointer of the iterator.
    if (begin != end) {
        LOG_WARN << "JSON document contains more information after the closing bracket.";
        return false;
    }
    return true;
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
bool JsonReader::parse_value (
    JsonLexer::iterator& ct,
    JsonLexer::iterator& end,
    JsonValue*&          value
) {
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
        return true;
    }
    if (ct->is_number()) {
        value = new JsonValueNumber(ct->value());
        ++ct;
        return true;
    }
    if (ct->is_string()) {
        value = new JsonValueString(ct->value());
        ++ct;
        return true;
    }
    if (ct->is_bracket("[")) {
        value = new JsonValueArray();
        return parse_array (ct, end, json_value_to_array(value));
    }
    if (ct->is_bracket("{")) {
        value = new JsonValueObject();
        return parse_object (ct, end, json_value_to_object(value));
    }

    // If the lexer token is not a fitting json value, we have an error.
    LOG_WARN << "JSON value contains invalid characters at " + ct->at() + ": '" + ct->value() + "'.";
    return false;
}

// -----------------------------------------------------------------------------
//     Parse Array
// -----------------------------------------------------------------------------

/**
 * @brief Parse a JSON array and fill it with data elements from the lexer.
 */
bool JsonReader::parse_array (
    JsonLexer::iterator& ct,
    JsonLexer::iterator& end,
    JsonValueArray*        value
) {
    // Proper usage of this function is to hand over a valid pointer to a json array, so check
    // for this here.
    assert(value);

    if (ct == end || !ct->is_bracket("[")) {
        LOG_WARN << "JSON array does not start with '[' at " << ct->at() << ".";
        return false;
    }

    ++ct;
    while (ct != end) {
        // Proccess the array element.
        JsonValue* element = nullptr;
        if (!parse_value(ct, end, element)) {
            return false;
        }
        value->add(element);

        // Check for end of array, leave if found.
        if (ct == end || ct->is_bracket("]")) {
            break;
        }

        // Check for delimiter comma (indicates that there are more elements following).
        if (!ct->is_operator(",")) {
            LOG_WARN << "JSON array does not contain comma between elements at " << ct->at() << ".";
            return false;
        }
        ++ct;
    }

    // The while loop above only stops when ct points to the end or to a closing bracket. In the
    // first case, we have an error; in the second, we are done with this object and can skip the
    // bracket.
    if (ct == end) {
        LOG_WARN << "JSON array ended unexpectedly.";
        return false;
    }
    assert(ct->is_bracket("]"));
    ++ct;
    return true;
}

// -----------------------------------------------------------------------------
//     Parse Object
// -----------------------------------------------------------------------------

/**
 * @brief Parse a JSON object and fill it with data members from the lexer.
 */
bool JsonReader::parse_object (
    JsonLexer::iterator& ct,
    JsonLexer::iterator& end,
    JsonValueObject*       value
) {
    // Proper usage of this function is to hand over a valid pointer to a json object, so check
    // for this here.
    assert(value);

    if (ct == end || !ct->is_bracket("{")) {
        LOG_WARN << "JSON object does not start with '{' at " << ct->at() << ".";
        return false;
    }

    ++ct;
    while (ct != end) {
        // Check for name string and store it.
        if (!ct->is_string()) {
            LOG_WARN << "JSON object member does not start with name string at " << ct->at() << ".";
            return false;
        }
        std::string name = ct->value();
        ++ct;

        // Check for delimiter colon.
        if (ct == end) {
            break;
        }
        if (!ct->is_operator(":")) {
            LOG_WARN << "JSON object member does not contain colon between name and value at "
                     << ct->at() << ".";
            return false;
        }
        ++ct;

        // Check for value and store it.
        if (ct == end) {
            break;
        }
        JsonValue* member = nullptr;
        if (!parse_value(ct, end, member)) {
            return false;
        }
        value->set(name, member);

        // Check for end of object, leave if found (either way).
        if (ct == end || ct->is_bracket("}")) {
            break;
        }

        // Check for delimiter comma (indicates that there are more members following).
        if (!ct->is_operator(",")) {
            LOG_WARN << "JSON object does not contain comma between members at " << ct->at() << ".";
            return false;
        }
        ++ct;
    }

    // The while loop above only stops when ct points to the end or to a closing bracket. In the
    // first case, we have an error; in the second, we are done with this object and can skip the
    // bracket.
    if (ct == end) {
        LOG_WARN << "JSON object ended unexpectedly.";
        return false;
    }
    ++ct;
    return true;
}

} // namespace utils
} // namespace genesis

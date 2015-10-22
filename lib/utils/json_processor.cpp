/**
 * @brief Implementation of functions for parsing and printing JSON documents.
 *
 * @file
 * @ingroup utils
 */

#include "utils/json_processor.hpp"

#include <assert.h>

#include "utils/json_document.hpp"
#include "utils/logging.hpp"
#include "utils/string.hpp"
#include "utils/utils.hpp"

namespace genesis {

// =================================================================================================
//     Parsing
// =================================================================================================

/**
 * @brief Takes a JSON document file path and parses its contents into a JsonDocument.
 *
 * Returns true iff successfull.
 */
bool JsonProcessor::from_file (const std::string& fn, JsonDocument& document)
{
    if (!file_exists(fn)) {
        LOG_WARN << "JSON file '" << fn << "' does not exist.";
        return false;
    }
    return from_string(file_read(fn), document);
}

/**
 * @brief Takes a string containing a JSON document and parses its contents into a JsonDocument.
 *
 * Returns true iff successfull.
 */
bool JsonProcessor::from_string (const std::string& json, JsonDocument& document)
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
bool JsonProcessor::parse_value (
    JsonLexer::iterator& ct,
    JsonLexer::iterator& end,
    JsonValue*&            value
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
bool JsonProcessor::parse_array (
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
    ++ct;
    return true;
}

// -----------------------------------------------------------------------------
//     Parse Object
// -----------------------------------------------------------------------------

/**
 * @brief Parse a JSON object and fill it with data members from the lexer.
 */
bool JsonProcessor::parse_object (
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

// =================================================================================================
//     Printing
// =================================================================================================

/**
 * @brief Writes a Json file from a JsonDocument. Returns true iff successful.
 */
bool JsonProcessor::to_file (const std::string& fn, const JsonDocument& document)
{
    if (file_exists(fn)) {
        LOG_WARN << "Json file '" << fn << "' already exist. Will not overwrite it.";
        return false;
    }
    std::string jd;
    to_string(jd, document);
    return file_write(fn, jd);
}

/**
 * @brief Gives the Json string representation of a JsonDocument.
 */
void JsonProcessor::to_string (std::string& json, const JsonDocument& document)
{
    json = to_string(document);
}

/**
 * @brief Returns the Json representation of a JsonDocument.
 */
std::string JsonProcessor::to_string (const JsonDocument& document)
{
    return print_object(&document, 0);
}

/**
 * @brief Returns the Json representation of a Json Value.
 */
std::string JsonProcessor::print_value (const JsonValue* value)
{
    switch(value->type()) {
        case JsonValue::kNull:
        case JsonValue::kBool:
            return value->to_string();
            break;

        case JsonValue::kNumber:
            return to_string_precise(json_value_to_number(value)->value, precision);
            break;

        case JsonValue::kString:
            return "\"" + string_escape(json_value_to_string(value)->value) + "\"";
            break;

        // This function is only called from within print_array() and print_object(), and both of
        // them handle the following two cases separately. So the assertion holds as long as this
        // function is not called illegaly from a different context.
        // Also, add a return to make the compiler happy ;-)
        case JsonValue::kArray:
        case JsonValue::kObject:
        default:
            assert(false);
            return "";
    }
}

/**
 * @brief Returns the Json representation of a Json Array.
 */
std::string JsonProcessor::print_array (const JsonValueArray* value, const int indent_level)
{
    int il = indent_level + 1;
    std::string in (il * indent, ' ');
    std::ostringstream ss;

    // Check if array contains non-simple values. If so, we use better bracket
    // placement to make document look nicer.
    bool has_large = false;
    for (JsonValueArray::const_iterator it = value->cbegin(); it != value->cend(); ++it) {
        JsonValue* v = *it;
        has_large |= (v->is_array() || v->is_object());
    }

    ss << "[ ";
    bool first = true;
    for (JsonValueArray::const_iterator it = value->cbegin(); it != value->cend(); ++it) {
        JsonValue* v = *it;
        if (!first) {
            ss << ", ";
        }
        if (has_large) {
            ss << "\n" << in;
        }
        if (v->is_array()) {
            ss << print_array(json_value_to_array(v), il);
        } else if (v->is_object()) {
            ss << print_object(json_value_to_object(v), il);
        } else {
            ss << print_value(v);
        }
        first = false;
    }

    if (has_large) {
        ss << "\n" << std::string(indent_level * indent, ' ');
    } else {
        ss << " ";
    }
    ss << "]";
    return ss.str();
}

/**
 * @brief Returns the Json representation of a Json Object.
 */
std::string JsonProcessor::print_object (const JsonValueObject* value, const int indent_level)
{
    int il = indent_level + 1;
    std::string in (il * indent, ' ');
    std::stringstream ss;
    ss << "{";

    bool first = true;
    for (JsonValueObject::const_iterator it = value->cbegin(); it != value->cend(); ++it) {
        JsonValueObject::ObjectPair v = *it;
        if (!first) {
            ss << ",";
        }
        ss << "\n" << in << "\"" << v.first << "\": ";
        if (v.second->is_array()) {
            ss << print_array(json_value_to_array(v.second), il);
        } else if (v.second->is_object()) {
            ss << print_object(json_value_to_object(v.second), il);
        } else {
            ss << print_value(v.second);
        }
        first = false;
    }

    ss << "\n" << std::string(indent_level * indent, ' ') << "}";
    return ss.str();
}

} // namespace genesis

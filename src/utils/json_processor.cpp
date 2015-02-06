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
#include "utils/utils.hpp"

namespace genesis {

// =============================================================================
//     Parsing
// =============================================================================

/**
 * @brief Takes a JSON document file path and parses its contents into a JsonDocument.
 *
 * See ParseLexer() for more information.
 * Returns true iff successfull.
 */
bool JsonProcessor::FromFile (const std::string& fn, JsonDocument& document)
{
    if (!FileExists(fn)) {
        LOG_WARN << "JSON file '" << fn << "' does not exist.";
        return false;
    }
    return FromString(FileRead(fn), document);
}

/**
 * @brief Takes a string containing a JSON document and parses its contents into a JsonDocument.
 *
 * See ParseLexer() for more information.
 * Returns true iff successfull.
 */
bool JsonProcessor::FromString (const std::string& json, JsonDocument& document)
{
    JsonLexer lexer;
    lexer.ProcessString(json);
    return FromLexer(lexer, document);
}

/**
 * @brief Takes a JsonLexer and parses its contents into a JsonDocument.
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
 *
 * Returns true iff successfull.
 */
bool JsonProcessor::FromLexer (const JsonLexer& lexer, JsonDocument& document)
{
    if (lexer.empty()) {
        LOG_INFO << "JSON document is empty.";
        return false;
    }
    if (lexer.HasError()) {
        LOG_WARN << "Lexing error at " << lexer.back().at()
                 << " with message: " << lexer.back().value();
        return false;
    }
    if (!lexer.cbegin()->IsBracket("{")) {
        LOG_WARN << "JSON document does not start with JSON object opener '{'.";
        return false;
    }

    // a json document is also a json object, so we start parsing the doc as such.
    // the begin iterator will be incremented with every token being processed.
    document.clear();
    Lexer::const_iterator begin = lexer.cbegin();
    Lexer::const_iterator end   = lexer.cend();
    if (!ParseObject(begin, end, &document)) {
        return false;
    }

    // after processing, the begin iterator will point to the lexer token that comes after
    // the one being processed last. if the document is well-formatted, this token is also
    // the end pointer of the iterator.
    if (begin != end) {
        LOG_WARN << "JSON document contains more information after the closing bracket.";
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------
//     Parse Value
// ---------------------------------------------------------------------

/**
 * @brief Parse a JSON value and fills it with data from the lexer.
 *
 * This function is a bit different from the other two process functions ParseArray() and
 * ParseObject(), because it takes its value parameter by reference. This is because when
 * entering the function it is not clear yet which type of value the current lexer token is, so a
 * new instance has to be created and stored in the pointer.
 */
bool JsonProcessor::ParseValue (
    Lexer::const_iterator& ct,
    Lexer::const_iterator& end,
    JsonValue*&            value
) {
    // proper usage of this function is to hand over a null pointer to a json value, which will be
    // assigned to a newly created value instance depending on the token type, so check for this
    // here. we don't want to overwrite existing values!
    assert (value == nullptr);

    // check all possible valid lexer token types and turn them into json values
    if (ct->IsSymbol()) {
        // the lexer only returns null, true or false as symbols, so this is safe
        if (ct->value().compare("null") == 0) {
            value = new JsonValueNull();
        } else {
            value = new JsonValueBool(ct->value());
        }
        ++ct;
        return true;
    }
    if (ct->IsNumber()) {
        value = new JsonValueNumber(ct->value());
        ++ct;
        return true;
    }
    if (ct->IsString()) {
        value = new JsonValueString(ct->value());
        ++ct;
        return true;
    }
    if (ct->IsBracket("[")) {
        value = new JsonValueArray();
        return ParseArray (ct, end, static_cast<JsonValueArray*>(value));
    }
    if (ct->IsBracket("{")) {
        value = new JsonValueObject();
        return ParseObject (ct, end, static_cast<JsonValueObject*>(value));
    }

    // if the lexer token is not a fitting json value, we have an error
    LOG_WARN << "JSON value contains invalid characters at " + ct->at() + ": '" + ct->value() + "'.";
    return false;
}

// ---------------------------------------------------------------------
//     Parse Array
// ---------------------------------------------------------------------

/**
 * @brief Parse a JSON array and fill it with data elements from the lexer.
 */
bool JsonProcessor::ParseArray (
    Lexer::const_iterator& ct,
    Lexer::const_iterator& end,
    JsonValueArray*        value
) {
    // proper usage of this function is to hand over a valid pointer to a json array, so check
    // for this here.
    assert(value);

    if (ct == end || !ct->IsBracket("[")) {
        LOG_WARN << "JSON array does not start with '[' at " << ct->at() << ".";
        return false;
    }

    ++ct;
    while (ct != end) {
        // proccess the array element
        JsonValue* element = nullptr;
        if (!ParseValue(ct, end, element)) {
            return false;
        }
        value->Add(element);

        // check for end of array, leave if found
        if (ct == end || ct->IsBracket("]")) {
            break;
        }

        // check for delimiter comma (indicates that there are more elements following)
        if (!ct->IsOperator(",")) {
            LOG_WARN << "JSON array does not contain comma between elements at " << ct->at() << ".";
            return false;
        }
        ++ct;
    }

    // the while loop above only stops when ct points to the end or to a closing bracket. in the
    // first case, we have an error; in the second, we are done with this object and can skip the
    // bracket.
    if (ct == end) {
        LOG_WARN << "JSON array ended unexpectedly.";
        return false;
    }
    ++ct;
    return true;
}

// ---------------------------------------------------------------------
//     Parse Object
// ---------------------------------------------------------------------

/**
 * @brief Parse a JSON object and fill it with data members from the lexer.
 */
bool JsonProcessor::ParseObject (
    Lexer::const_iterator& ct,
    Lexer::const_iterator& end,
    JsonValueObject*       value
) {
    // proper usage of this function is to hand over a valid pointer to a json object, so check
    // for this here.
    assert(value);

    if (ct == end || !ct->IsBracket("{")) {
        LOG_WARN << "JSON object does not start with '{' at " << ct->at() << ".";
        return false;
    }

    ++ct;
    while (ct != end) {
        // check for name string and store it
        if (!ct->IsString()) {
            LOG_WARN << "JSON object member does not start with name string at " << ct->at() << ".";
            return false;
        }
        std::string name = ct->value();
        ++ct;

        // check for delimiter colon
        if (ct == end) {
            break;
        }
        if (!ct->IsOperator(":")) {
            LOG_WARN << "JSON object member does not contain colon between name and value at "
                     << ct->at() << ".";
            return false;
        }
        ++ct;

        // check for value and store it
        if (ct == end) {
            break;
        }
        JsonValue* member = nullptr;
        if (!ParseValue(ct, end, member)) {
            return false;
        }
        value->Set(name, member);

        // check for end of object, leave if found (either way)
        if (ct == end || ct->IsBracket("}")) {
            break;
        }

        // check for delimiter comma (indicates that there are more members following)
        if (!ct->IsOperator(",")) {
            LOG_WARN << "JSON object does not contain comma between members at " << ct->at() << ".";
            return false;
        }
        ++ct;
    }

    // the while loop above only stops when ct points to the end or to a closing bracket. in the
    // first case, we have an error; in the second, we are done with this object and can skip the
    // bracket.
    if (ct == end) {
        LOG_WARN << "JSON object ended unexpectedly.";
        return false;
    }
    ++ct;
    return true;
}

// =============================================================================
//     Printing
// =============================================================================

/**
 * @brief The precision used for printing floating point numbers, particularly Json Value Numbers.
 */
int JsonProcessor::precision = 6;

/**
 * @brief The indent used for printing the elements of Json Arrays and Objects.
 */
int JsonProcessor::indent = 4;

/**
 * @brief Writes a Json file from a JsonDocument. Returns true iff successful.
 */
bool JsonProcessor::ToFile (const std::string& fn, const JsonDocument& document)
{
    if (FileExists(fn)) {
        LOG_WARN << "Json file '" << fn << "' already exist. Will not overwrite it.";
        return false;
    }
    std::string jd;
    ToString(jd, document);
    return FileWrite(fn, jd);
}

/**
 * @brief Gives the Json string representation of a JsonDocument.
 */
void JsonProcessor::ToString (std::string& json, const JsonDocument& document)
{
    json = PrintObject(&document, 0);
}

/**
 * @brief Returns the Json representation of a JsonDocument.
 */
std::string JsonProcessor::ToString (const JsonDocument& document)
{
    std::string jd;
    ToString(jd, document);
    return jd;
}

/**
 * @brief Returns the Json representation of a Json Value.
 */
std::string JsonProcessor::PrintValue (const JsonValue* value)
{
    switch(value->type()) {
        case JsonValue::kNull:
        case JsonValue::kBool:
            return value->ToString();
            break;

        case JsonValue::kNumber:
            return ToStringPrecise(JsonValueToNumber(const_cast<JsonValue*>(value))->value, precision);
            break;

        case JsonValue::kString:
            return "\"" + StringEscape(JsonValueToString(const_cast<JsonValue*>(value))->value) + "\"";
            break;

        // this function is only called from within PrintArray() and PrintObject(), and both of them
        // handle those two cases separately. so the assertion holds as long as this function
        // is not called illegaly from a different context.
        case JsonValue::kArray:
        case JsonValue::kObject:
        default:
            assert(false);
    }
}

/**
 * @brief Returns the Json representation of a Json Array.
 */
std::string JsonProcessor::PrintArray (const JsonValueArray* value, const int indent_level)
{
    int il = indent_level + 1;
    std::string in (il * indent, ' ');
    std::ostringstream ss;

    // check if array contains non-simple values. if so, we use better bracket
    // placement to make document look nicer
    bool has_large = false;
    for (JsonValueArray::const_iterator it = value->cbegin(); it != value->cend(); ++it) {
        JsonValue* v = *it;
        has_large |= (v->IsArray() || v->IsObject());
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
        if (v->IsArray()) {
            ss << PrintArray(static_cast<JsonValueArray*>(v), il);
        } else if (v->IsObject()) {
            ss << PrintObject(static_cast<JsonValueObject*>(v), il);
        } else {
            ss << PrintValue(v);
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
std::string JsonProcessor::PrintObject (const JsonValueObject* value, const int indent_level)
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
        if (v.second->IsArray()) {
            ss << PrintArray(static_cast<JsonValueArray*>(v.second), il);
        } else if (v.second->IsObject()) {
            ss << PrintObject(static_cast<JsonValueObject*>(v.second), il);
        } else {
            ss << PrintValue(v.second);
        }
        first = false;
    }

    ss << "\n" << std::string(indent_level * indent, ' ') << "}";
    return ss.str();
}

} // namespace genesis

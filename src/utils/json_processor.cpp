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
bool JsonProcessor::FromLexer (JsonLexer& lexer, JsonDocument& document)
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
    if (!lexer.Peek().IsBracket("{")) {
        LOG_WARN << "JSON document does not start with JSON object opener '{'.";
        return false;
    }

    // a json document is also a json object, so we start parsing the doc as such.
    // the lexer tokens will be consumed with every token being processed.
    document.clear();
    if (!ParseObject(lexer, &document)) {
        return false;
    }

    // reaching this poing means that we saw the closing curly bracket of the document.
    // if we are however not yet done with everything, we have an error.
    if (!lexer.Finished()) {
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
bool JsonProcessor::ParseValue (JsonLexer& lexer, JsonValue*& value)
{
    // we cannot consume the token yet, because we might need it for one of the other functions.
    LexerToken ct = lexer.Peek();

    // proper usage of this function is to hand over a null pointer to a json value, which will be
    // assigned to a newly created value instance depending on the token type, so check for this
    // here. we don't want to overwrite existing values!
    assert (value == nullptr);

    // check all possible valid lexer token types and turn them into json values
    if (ct.IsSymbol()) {
        // the lexer only returns null, true or false as symbols, so this is safe
        if (ct.value().compare("null") == 0) {
            value = new JsonValueNull();
        } else {
            value = new JsonValueBool(ct.value());
        }
        lexer.ConsumeToken();
        return true;
    }
    if (ct.IsNumber()) {
        value = new JsonValueNumber(ct.value());
        lexer.ConsumeToken();
        return true;
    }
    if (ct.IsString()) {
        value = new JsonValueString(ct.value());
        lexer.ConsumeToken();
        return true;
    }
    if (ct.IsBracket("[")) {
        value = new JsonValueArray();
        return ParseArray (lexer, JsonValueToArray(value));
    }
    if (ct.IsBracket("{")) {
        value = new JsonValueObject();
        return ParseObject (lexer, JsonValueToObject(value));
    }

    // if the lexer token is not a fitting json value, we have an error
    LOG_WARN << "JSON value contains invalid characters at " + ct.at() + ": '" + ct.value() + "'.";
    return false;
}

// ---------------------------------------------------------------------
//     Parse Array
// ---------------------------------------------------------------------

/**
 * @brief Parse a JSON array and fill it with data elements from the lexer.
 */
bool JsonProcessor::ParseArray (JsonLexer& lexer, JsonValueArray* value)
{
    // proper usage of this function is to hand over a valid pointer to a json array, so check
    // for this here.
    assert(value);

    // we are expecting an opening square bracket here.
    if (lexer.Finished() || !lexer.Peek().IsBracket("[")) {
        LOG_WARN << "JSON array does not start with '[' at " << lexer.Peek().at() << ".";
        return false;
    }

    // now we are sure that we found the bracket. consume it, then go into the array.
    lexer.ConsumeToken();
    while (!lexer.Finished()) {
        // proccess the array element
        JsonValue* element = nullptr;
        if (!ParseValue(lexer, element)) {
            return false;
        }
        value->Add(element);

        // check for end of array, leave if found
        if (lexer.Finished() || lexer.Peek().IsBracket("]")) {
            break;
        }

        // check for delimiter comma (indicates that there are more elements following)
        if (!lexer.Peek().IsOperator(",")) {
            LOG_WARN << "JSON array does not contain comma between elements at " << lexer.Peek().at() << ".";
            return false;
        }
        lexer.ConsumeToken();
    }

    // the while loop above only stops at the end or at a closing bracket. in the
    // first case, we have an error; in the second, we are done with this object and can skip the
    // bracket.
    if (lexer.Finished()) {
        LOG_WARN << "JSON array ended unexpectedly.";
        return false;
    }
    lexer.ConsumeToken();
    return true;
}

// ---------------------------------------------------------------------
//     Parse Object
// ---------------------------------------------------------------------

/**
 * @brief Parse a JSON object and fill it with data members from the lexer.
 */
bool JsonProcessor::ParseObject (JsonLexer& lexer, JsonValueObject* value)
{
    // proper usage of this function is to hand over a valid pointer to a json object, so check
    // for this here.
    assert(value);

    if (lexer.Finished() || !lexer.Peek().IsBracket("{")) {
        LOG_WARN << "JSON object does not start with '{' at " << lexer.Peek().at() << ".";
        return false;
    }

    lexer.ConsumeToken();
    LexerToken ct = lexer.ConsumeToken();
    while (!lexer.Finished()) {
        // check for name string and store it
        if (!ct.IsString()) {
            LOG_WARN << "JSON object member does not start with name string at " << ct.at() << ".";
            return false;
        }
        std::string name = ct.value();

        // check for delimiter colon
        ct = lexer.ConsumeToken();
        if (!ct.IsOperator(":")) {
            LOG_WARN << "JSON object member does not contain colon between name and value at "
                     << ct.at() << ".";
            return false;
        }

        if (lexer.Finished()) {
            break;
        }

        // check for value and store it
        JsonValue* member = nullptr;
        if (!ParseValue(lexer, member)) {
            return false;
        }
        value->Set(name, member);

        if (lexer.Finished()) {
            break;
        }

        // leave if end of object detected
        ct = lexer.ConsumeToken();
        if (ct.IsBracket("}")) {
            break;
        }

        // check for delimiter comma (indicates that there are more members following)
        if (!ct.IsOperator(",")) {
            LOG_WARN << "JSON object does not contain comma between members at " << ct.at() << ".";
            return false;
        }

        if (lexer.Finished()) {
            break;
        }
        ct = lexer.ConsumeToken();
    }

    // the while loop above only stops when ct points to the end or to a closing bracket. in the
    // first case, we have an error; in the second, we are done with this object and can skip the
    // bracket.
    if (lexer.Finished()) {
        LOG_WARN << "JSON object ended unexpectedly.";
        return false;
    }
    lexer.ConsumeToken();
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
    json = ToString(document);
}

/**
 * @brief Returns the Json representation of a JsonDocument.
 */
std::string JsonProcessor::ToString (const JsonDocument& document)
{
    return PrintObject(&document, 0);
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
            return ToStringPrecise(JsonValueToNumber(value)->value, precision);
            break;

        case JsonValue::kString:
            return "\"" + StringEscape(JsonValueToString(value)->value) + "\"";
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
            ss << PrintArray(JsonValueToArray(v), il);
        } else if (v->IsObject()) {
            ss << PrintObject(JsonValueToObject(v), il);
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
            ss << PrintArray(JsonValueToArray(v.second), il);
        } else if (v.second->IsObject()) {
            ss << PrintObject(JsonValueToObject(v.second), il);
        } else {
            ss << PrintValue(v.second);
        }
        first = false;
    }

    ss << "\n" << std::string(indent_level * indent, ' ') << "}";
    return ss.str();
}

} // namespace genesis

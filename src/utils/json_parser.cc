/**
 * @brief Implementation of functions for parsing JSON documents.
 *
 * @file
 * @ingroup utils
 */

#include "utils/json_parser.hh"

#include <assert.h>

#include "utils/logging.hh"
#include "utils/utils.hh"

namespace genesis {

// =============================================================================
//     Process
// =============================================================================

/**
 * @brief Takes a JSON document file path and parses its contents into a JsonDocument.
 *
 * See ProcessLexer() for more information.
 * Returns true iff successfull.
 */
bool JsonParser::ProcessFile (const std::string& fn, JsonDocument& document)
{
    if (!FileExists(fn)) {
        LOG_WARN << "JSON file '" << fn << "' does not exist.";
        return false;
    }
    return ProcessString(FileRead(fn), document);
}

/**
 * @brief Takes a string containing a JSON document and parses its contents into a JsonDocument.
 *
 * See ProcessLexer() for more information.
 * Returns true iff successfull.
 */
bool JsonParser::ProcessString (const std::string& json, JsonDocument& document)
{
    JsonLexer lexer;
    lexer.ProcessString(json);
    return ProcessLexer(lexer, document);
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
bool JsonParser::ProcessLexer (const JsonLexer& lexer, JsonDocument& document)
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
    document.clear();
    Lexer::const_iterator begin = lexer.cbegin();
    Lexer::const_iterator end   = lexer.cend();
    if (!ProcessObject(begin, end, &document)) {
        return false;
    }
    if (begin != end) {
        LOG_WARN << "JSON document contains more information after the closing bracket.";
        return false;
    }
    return true;
}

// =============================================================================
//     ProcessValue
// =============================================================================

/**
 * @brief Processes a JSON value and fills it with data from the lexer.
 *
 * This function is a bit different from the other two process functions ProcessArray() and
 * ProcessObject(), because it takes its value parameter by reference. This is because when
 * entering the function it is not clear yet which type of value the current lexer token is, so a
 * new instance has to be created and stored in the pointer.
 */
bool JsonParser::ProcessValue (
    Lexer::const_iterator& ct,
    Lexer::const_iterator& end,
    JsonValue*&            value
) {
    // proper usage of this function is to hand over a null pointer to a json value, which will be
    // assigned to a newly created value instance depending on the token type, so check for this
    // here.
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
        return ProcessArray (ct, end, static_cast<JsonValueArray*>(value));
    }
    if (ct->IsBracket("{")) {
        value = new JsonValueObject();
        return ProcessObject (ct, end, static_cast<JsonValueObject*>(value));
    }

    // if the lexer token is not a fitting json value, we have an error
    LOG_WARN << "JSON value contains invalid characters at " + ct->at() + ": '" + ct->value() + "'.";
    return false;
}

// =============================================================================
//     ProcessArray
// =============================================================================

/**
 * @brief Process a JSON array and fill it with data elements from the lexer.
 */
bool JsonParser::ProcessArray (
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
        if (!ProcessValue(ct, end, element)) {
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

// =============================================================================
//     ProcessObject
// =============================================================================

/**
 * @brief Process a JSON object and fill it with data members from the lexer.
 */
bool JsonParser::ProcessObject (
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
        if (!ProcessValue(ct, end, member)) {
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

}

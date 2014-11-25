/**
 * @brief Implementation of functions for parsing JSON documents.
 *
 * @file
 * @ingroup utils
 */

#include "utils/json_parser.hh"

#include <assert.h>

#include "utils/logging.hh"

namespace genesis {

// =============================================================================
//     Process
// =============================================================================

/**
 * @brief Shortcut for parsing a JSON string and putting its information into a JSON document.
 *
 * This shortcut includes the lexing step and then calls the other Process() function for the actual
 * parsing.
 */
bool JsonParser::Process (const std::string& json, JsonDocument& document)
{
    JsonLexer lexer;
    lexer.Process(json);
    return Process(lexer, document);
}

/**
 * @brief Takes a JsonLexer and parses its contents into a JsonDocument.
 *
 * In order to parse a JSON string directly into a JSON document, use the other Process() function.
 *
 * Each JSON document is also a JSON object, and can contain other objects, JSON arrays, or simple
 * value types. The parsing here is thus splitted in those three functions, being recursively called
 * for every level of nesting within objects and arrays.
 *
 * Those three functions take an interator to the current lexer token by reference and advance it
 * until it points to the next token after processing the current value. To check for the end of
 * the lexer, an intererator to its end is also provided, as well as a pointer to the resulting
 * JSON value.
 */
bool JsonParser::Process (const JsonLexer& lexer, JsonDocument& document)
{
    if (lexer.empty()) {
        LOG_INFO << "JSON file is empty. Nothing done.";
        return false;
    }
    if (lexer.HasError()) {
        LOG_WARN << "Lexing error at " << lexer.back().at()
                 << " with message: " << lexer.back().value();
        return false;
    }
    if (!lexer.begin()->IsBracket("{")) {
        LOG_WARN << "JSON document does not start with JSON object opener '{'.";
        return false;
    }

    // a json document is also a json object, so we start parsing the doc as such.
    document.clear();
    Lexer::const_iterator begin = lexer.begin();
    Lexer::const_iterator end   = lexer.end();
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
    // this function is protected in this class, so it should only be called from within or its
    // derivatives. the proper usage is to hand over a valid pointer to a json value, so check
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
        value->data.push_back(element);

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
    // this function is protected in this class, so it should only be called from within or its
    // derivatives. the proper usage is to hand over a valid pointer to a json value, so check
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

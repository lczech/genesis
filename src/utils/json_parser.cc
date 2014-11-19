/**
 * @brief Implementation of JSON Parser functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/json_parser.hh"

#include "utils/logging.hh"

namespace genesis {

// =============================================================================
//     Process
// =============================================================================

bool JsonParser::Process (const std::string& json, JsonDocument* document)
{
    JsonLexer lexer;
    lexer.Process(json);
    return Process(lexer, document);
}

bool JsonParser::Process (const JsonLexer& lexer, JsonDocument* document)
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

    document->clear();
    Lexer::const_iterator begin = lexer.begin();
    Lexer::const_iterator end   = lexer.end();
    return ProcessObject(begin, end, document);
}

// =============================================================================
//     ProcessValue
// =============================================================================

bool JsonParser::ProcessValue (
    Lexer::const_iterator& begin,
    Lexer::const_iterator& end,
    JsonValue*             value
) {
    // acts as pointer to previous token
    Lexer::const_iterator pt = end;

    // acts as pointer to current token
    Lexer::const_iterator ct;

    // store error message. also serves as check whether an error occured
    std::string error = "";

    // --------------------------------------------------------------
    //     Loop over lexer tokens and check if it...
    // --------------------------------------------------------------
    for (ct = begin; ct != end; pt=ct, ++ct) {
        if (ct->type() == LexerType::kUnknown) {
            error = "Invalid characters at " + ct->at() + ": '" + ct->value() + "'.";
            break;
        }

        // ------------------------------------------------------
        //     is bracket '{'  ==>  begin of object
        // ------------------------------------------------------
        if (ct->IsBracket("{")) {
            if (pt != end && pt->type() != LexerType::kOperator) {
                error = "Invalid characters at " + ct->at() + ": '" + ct->value() + "'.";
                break;
            }
            continue;
        }

        // ------------------------------------------------------
        //     is bracket '}'  ==>  end of object
        // ------------------------------------------------------
        if (ct->IsBracket("}")) {
            if (pt != end && pt->type() != LexerType::kOperator) {
                error = "Invalid characters at " + ct->at() + ": '" + ct->value() + "'.";
                break;
            }
            continue;
        }

        // TODO remove
        value = nullptr;
    }

    if (!error.empty()) {
        LOG_WARN << error;
        return false;
    }

    return true;
}

// =============================================================================
//     ProcessArray
// =============================================================================

bool JsonParser::ProcessArray (
    Lexer::const_iterator& begin,
    Lexer::const_iterator& end,
    JsonValueArray*        array
) {
    array->clear();

    if (!begin->IsBracket("[")) {
        LOG_WARN << "JSON array does not start with '['.";
        return false;
    }

    while (begin != end) {

        ++begin;
    }
    return true;
}

// =============================================================================
//     ProcessObject
// =============================================================================

bool JsonParser::ProcessObject (
    Lexer::const_iterator& begin,
    Lexer::const_iterator& end,
    JsonValueObject*       object
) {
    object->clear();

    if (!begin->IsBracket("{")) {
        LOG_WARN << "JSON object does not start with '{'.";
        return false;
    }

    while (begin != end) {

        ++begin;
    }
    return true;
}

}

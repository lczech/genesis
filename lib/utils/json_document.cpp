/**
 * @brief Implementation of JSON document functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/json_document.hpp"

#include "utils/logging.hpp"

namespace genesis {

// =============================================================================
//     JsonDocument
// =============================================================================

// TODO write a validate json doc function that checks if there are no recurrencies in the values,
// TODO meaning that the pointers in an object or array need to point to unique values, and not for
// TODO example to themselves or their parent objects.
/*
bool JsonDocument::Validate()
{
    std::deque<JsonValue*> pointers;
    std::stack<JsonValue*> callstack;

    for (ObjectData::value_type v : value) {
        callstack.push(v.second);
    }

    while (!callstack.empty()) {
        break;
    }
    return true;
}
*/

// =============================================================================
//     Converter Functions
// =============================================================================

/**
 * @brief Converts a pointer to a JsonValue to JsonValueNull if appropriate.
 *
 * Triggers a warning and returns a nullptr if the dynamic type of the object is not actually
 * a JsonValueNull object.
 */
JsonValueNull* JsonValueToNull (const JsonValue* v)
{
    if (v->type() != JsonValue::kNull) {
        LOG_WARN << "Invalid conversion from JsonValue::" << v->type_to_string()
                 << " to JsonValue::Null.";
        return nullptr;
    }
    return static_cast<JsonValueNull*> (const_cast<JsonValue*> (v));
}

/**
 * @brief Converts a pointer to a JsonValue to JsonValueBool if appropriate.
 *
 * Triggers a warning and returns a nullptr if the dynamic type of the object is not actually
 * a JsonValueBool object.
 */
JsonValueBool* JsonValueToBool (const JsonValue* v)
{
    if (v->type() != JsonValue::kBool) {
        LOG_WARN << "Invalid conversion from JsonValue::" << v->type_to_string()
                 << " to JsonValue::Bool.";
        return nullptr;
    }
    return static_cast<JsonValueBool*> (const_cast<JsonValue*> (v));
}

/**
 * @brief Converts a pointer to a JsonValue to JsonValueNumber if appropriate.
 *
 * Triggers a warning and returns a nullptr if the dynamic type of the object is not actually
 * a JsonValueNumber object.
 */
JsonValueNumber* JsonValueToNumber (const JsonValue* v)
{
    if (v->type() != JsonValue::kNumber) {
        LOG_WARN << "Invalid conversion from JsonValue::" << v->type_to_string()
                 << " to JsonValue::Number.";
        return nullptr;
    }
    return static_cast<JsonValueNumber*> (const_cast<JsonValue*> (v));
}

/**
 * @brief Converts a pointer to a JsonValue to JsonValueString if appropriate.
 *
 * Triggers a warning and returns a nullptr if the dynamic type of the object is not actually
 * a JsonValueString object.
 */
JsonValueString* JsonValueToString (const JsonValue* v)
{
    if (v->type() != JsonValue::kString) {
        LOG_WARN << "Invalid conversion from JsonValue::" << v->type_to_string()
                 << " to JsonValue::String.";
        return nullptr;
    }
    return static_cast<JsonValueString*> (const_cast<JsonValue*> (v));
}

/**
 * @brief Converts a pointer to a JsonValue to JsonValueArray if appropriate.
 *
 * Triggers a warning and returns a nullptr if the dynamic type of the object is not actually
 * a JsonValueArray object.
 */
JsonValueArray* JsonValueToArray (const JsonValue* v)
{
    if (v->type() != JsonValue::kArray) {
        LOG_WARN << "Invalid conversion from JsonValue::" << v->type_to_string()
                 << " to JsonValue::Array.";
        return nullptr;
    }
    return static_cast<JsonValueArray*> (const_cast<JsonValue*> (v));
}

/**
 * @brief Converts a pointer to a JsonValue to JsonValueObject if appropriate.
 *
 * Triggers a warning and returns a nullptr if the dynamic type of the object is not actually
 * a JsonValueObject object.
 */
JsonValueObject* JsonValueToObject (const JsonValue* v)
{
    if (v->type() != JsonValue::kObject) {
        LOG_WARN << "Invalid conversion from JsonValue::" << v->type_to_string()
                 << " to JsonValue::Object.";
        return nullptr;
    }
    return static_cast<JsonValueObject*> (const_cast<JsonValue*> (v));
}

} // namespace genesis

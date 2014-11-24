/**
 * @brief Implementation of JSON document functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/json_document.hh"

#include <stack>

#include "utils/logging.hh"

namespace genesis {

// =============================================================================
//     JsonValueNumber
// =============================================================================

int JsonValueNumber::precision = 6;

// =============================================================================
//     JsonValueArray
// =============================================================================

JsonValueArray::~JsonValueArray ()
{
    clear();
}

void JsonValueArray::clear()
{
    for (JsonValue* v : data) {
        delete v;
    }
    ArrayData().swap(data);
}

std::string JsonValueArray::ToString() const
{
    std::stringstream ss;
    ss << "[ ";

    bool first = true;
    for (JsonValue* v : data) {
        if (!first) {
            ss << ", ";
        }
        ss << v->ToString();
        first = false;
    }

    ss << " ]";
    return ss.str();
}

// =============================================================================
//     JsonValueObject
// =============================================================================

JsonValueObject::~JsonValueObject()
{
    clear();
}

void JsonValueObject::clear()
{
    for (ObjectPair v : data) {
        delete v.second;
    }
    ObjectData().swap(data);
}

std::string JsonValueObject::ToString() const
{
    std::stringstream ss;
    ss << "{ ";

    bool first = true;
    for (ObjectPair v : data) {
        if (!first) {
            ss << ", ";
        }
        ss << "\"" << v.first << "\": " << v.second->ToString();
        first = false;
    }

    ss << " }";
    return ss.str();
}

void JsonValueObject::Set (const std::string &name, JsonValue* value)
{
    data[name] = value;
}

JsonValue* JsonValueObject::Get (const std::string &name)
{
    if (Has(name)) {
        return data[name];
    } else {
        return nullptr;
    }
}

// =============================================================================
//     JsonDocument
// =============================================================================

JsonDocument::~JsonDocument ()
{
    clear();
}

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
JsonValueNull* JsonValueToNull (JsonValue* v)
{
    if (v->type() != JsonValue::kNull) {
        LOG_WARN << "Invalid conversion from JsonValue::" << v->TypeToString()
                 << " to JsonValue::kNull.";
        return nullptr;
    }
    return static_cast<JsonValueNull*> (v);
}

/**
 * @brief Converts a pointer to a JsonValue to JsonValueBool if appropriate.
 *
 * Triggers a warning and returns a nullptr if the dynamic type of the object is not actually
 * a JsonValueBool object.
 */
JsonValueBool* JsonValueToBool (JsonValue* v)
{
    if (v->type() != JsonValue::kBool) {
        LOG_WARN << "Invalid conversion from JsonValue::" << v->TypeToString()
                 << " to JsonValue::kBool.";
        return nullptr;
    }
    return static_cast<JsonValueBool*> (v);
}

/**
 * @brief Converts a pointer to a JsonValue to JsonValueNumber if appropriate.
 *
 * Triggers a warning and returns a nullptr if the dynamic type of the object is not actually
 * a JsonValueNumber object.
 */
JsonValueNumber* JsonValueToNumber (JsonValue* v)
{
    if (v->type() != JsonValue::kNumber) {
        LOG_WARN << "Invalid conversion from JsonValue::" << v->TypeToString()
                 << " to JsonValue::kNumber.";
        return nullptr;
    }
    return static_cast<JsonValueNumber*> (v);
}

/**
 * @brief Converts a pointer to a JsonValue to JsonValueString if appropriate.
 *
 * Triggers a warning and returns a nullptr if the dynamic type of the object is not actually
 * a JsonValueString object.
 */
JsonValueString* JsonValueToString (JsonValue* v)
{
    if (v->type() != JsonValue::kString) {
        LOG_WARN << "Invalid conversion from JsonValue::" << v->TypeToString()
                 << " to JsonValue::kString.";
        return nullptr;
    }
    return static_cast<JsonValueString*> (v);
}

/**
 * @brief Converts a pointer to a JsonValue to JsonValueArray if appropriate.
 *
 * Triggers a warning and returns a nullptr if the dynamic type of the object is not actually
 * a JsonValueArray object.
 */
JsonValueArray* JsonValueToArray (JsonValue* v)
{
    if (v->type() != JsonValue::kArray) {
        LOG_WARN << "Invalid conversion from JsonValue::" << v->TypeToString()
                 << " to JsonValue::kArray.";
        return nullptr;
    }
    return static_cast<JsonValueArray*> (v);
}

/**
 * @brief Converts a pointer to a JsonValue to JsonValueObject if appropriate.
 *
 * Triggers a warning and returns a nullptr if the dynamic type of the object is not actually
 * a JsonValueObject object.
 */
JsonValueObject* JsonValueToObject (JsonValue* v)
{
    if (v->type() != JsonValue::kObject) {
        LOG_WARN << "Invalid conversion from JsonValue::" << v->TypeToString()
                 << " to JsonValue::kObject.";
        return nullptr;
    }
    return static_cast<JsonValueObject*> (v);
}

} // namespace genesis

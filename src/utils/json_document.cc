/**
 * @brief Implementation of JSON document functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/json_document.hh"

//~ #include <stack>

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

std::string JsonValueArray::ToString(const int indent_level) const
{
    std::string in (indent_level * JsonDocument::indent, ' ');
    std::stringstream ss;
    ss << in << "[";

    bool first = true;
    for (JsonValue* v : data) {
        if (!first) {
            ss << ", ";
        }
        ss << "\n";
        if (v->type() == JsonValue::kArray) {
            ss << static_cast<JsonValueArray*>(v)->ToString(indent_level + 1);
        } else if (v->type() == JsonValue::kObject) {
            ss << static_cast<JsonValueObject*>(v)->ToString(indent_level + 1);
        } else {
            ss << in << std::string(JsonDocument::indent, ' ') << v->ToString();
        }
        first = false;
    }

    ss << "\n" << in << "]";
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

std::string JsonValueObject::ToString(const int indent_level) const
{
    std::string in (indent_level * JsonDocument::indent, ' ');
    std::stringstream ss;
    ss << in << "{";

    bool first = true;
    for (ObjectPair v : data) {
        if (!first) {
            ss << ",";
        }
        ss << "\n" << in << std::string(JsonDocument::indent, ' ') << "\"" << v.first << "\":\n";
        if (v.second->type() == JsonValue::kArray) {
            ss << static_cast<JsonValueArray*>(v.second)->ToString(indent_level + 1);
        } else if (v.second->type() == JsonValue::kObject) {
            ss << static_cast<JsonValueObject*>(v.second)->ToString(indent_level + 1);
        } else {
            ss << in << std::string(2*JsonDocument::indent, ' ') << v.second->ToString();
        }
        first = false;
    }

    ss << "\n" << in << "}";
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

int JsonDocument::indent = 4;

JsonDocument::~JsonDocument ()
{
    clear();
}

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

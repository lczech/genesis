/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief Implementation of JSON document functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/formats/json/document.hpp"

#include "utils/core/fs.hpp"

namespace genesis {
namespace utils {

// =============================================================================
//     JsonDocument
// =============================================================================

// TODO write a validate json doc function that checks if there are no recurrencies in the values,
// TODO meaning that the pointers in an object or array need to point to unique values, and not for
// TODO example to themselves or their parent objects.
/*
bool JsonDocument::validate()
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
 * Throws if the dynamic type of the object is not actually a JsonValueNull object.
 */
JsonValueNull* json_value_to_null (const JsonValue* v)
{
    if (v->type() != JsonValue::kNull) {
        throw std::domain_error(
            "Invalid conversion from JsonValue::" + v->type_to_string() + " to JsonValue::Null."
        );
    }
    return static_cast<JsonValueNull*> (const_cast<JsonValue*> (v));
}

/**
 * @brief Converts a pointer to a JsonValue to JsonValueBool if appropriate.
 *
 * Throws if the dynamic type of the object is not actually a JsonValueBool object.
 */
JsonValueBool* json_value_to_bool (const JsonValue* v)
{
    if (v->type() != JsonValue::kBool) {
        throw std::domain_error(
            "Invalid conversion from JsonValue::" + v->type_to_string() + " to JsonValue::Bool."
        );
    }
    return static_cast<JsonValueBool*> (const_cast<JsonValue*> (v));
}

/**
 * @brief Converts a pointer to a JsonValue to JsonValueNumber if appropriate.
 *
 * Throws if the dynamic type of the object is not actually a JsonValueNumber object.
 */
JsonValueNumber* json_value_to_number (const JsonValue* v)
{
    if (v->type() != JsonValue::kNumber) {
        throw std::domain_error(
            "Invalid conversion from JsonValue::" + v->type_to_string() + " to JsonValue::Number."
        );
    }
    return static_cast<JsonValueNumber*> (const_cast<JsonValue*> (v));
}

/**
 * @brief Converts a pointer to a JsonValue to JsonValueString if appropriate.
 *
 * Throws if the dynamic type of the object is not actually a JsonValueString object.
 */
JsonValueString* json_value_to_string (const JsonValue* v)
{
    if (v->type() != JsonValue::kString) {
        throw std::domain_error(
            "Invalid conversion from JsonValue::" + v->type_to_string() + " to JsonValue::String."
        );
    }
    return static_cast<JsonValueString*> (const_cast<JsonValue*> (v));
}

/**
 * @brief Converts a pointer to a JsonValue to JsonValueArray if appropriate.
 *
 * Throws if the dynamic type of the object is not actually a JsonValueArray object.
 */
JsonValueArray* json_value_to_array (const JsonValue* v)
{
    if (v->type() != JsonValue::kArray) {
        throw std::domain_error(
            "Invalid conversion from JsonValue::" + v->type_to_string() + " to JsonValue::Array."
        );
    }
    return static_cast<JsonValueArray*> (const_cast<JsonValue*> (v));
}

/**
 * @brief Converts a pointer to a JsonValue to JsonValueObject if appropriate.
 *
 * Throws if the dynamic type of the object is not actually a JsonValueObject object.
 */
JsonValueObject* json_value_to_object (const JsonValue* v)
{
    if (v->type() != JsonValue::kObject) {
        throw std::domain_error(
            "Invalid conversion from JsonValue::" + v->type_to_string() + " to JsonValue::Object."
        );
    }
    return static_cast<JsonValueObject*> (const_cast<JsonValue*> (v));
}

} // namespace utils
} // namespace genesis

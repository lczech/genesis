/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "utils/io/json/writer.hpp"

#include <assert.h>
#include <stdexcept>

#include "utils/core/fs.hpp"
#include "utils/io/json/document.hpp"
#include "utils/text/string.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Writing
// =================================================================================================

/**
 * @brief Writes a Json file from a JsonDocument. Returns true iff successful.
 *
 * If the file already exists, the function throws `std::runtime_error`.
 * The function uses utils::file_write. See there for other exceptions that can be thrown.
 */
void JsonWriter::to_file( JsonDocument const& document, std::string const& filename )
{
    if( utils::file_exists(filename) ) {
        throw std::runtime_error( "Json file '" + filename + "' already exist." );
    }
    std::string jd;
    to_string( document, jd );
    utils::file_write( jd, filename );
}

/**
 * @brief Gives the Json string representation of a JsonDocument.
 */
void JsonWriter::to_string( JsonDocument const& document, std::string& output )
{
    output = to_string(document);
}

/**
 * @brief Returns the Json representation of a JsonDocument.
 */
std::string JsonWriter::to_string( JsonDocument const& document )
{
    return print_object(&document, 0);
}

// =================================================================================================
//     Printing
// =================================================================================================

/**
 * @brief Returns the Json representation of a Json Value.
 */
std::string JsonWriter::print_value( JsonValue const* value )
{
    switch(value->type()) {
        case JsonValue::kNull:
        case JsonValue::kBool:
            return value->to_string();
            break;

        case JsonValue::kNumber:
            return utils::to_string_precise(json_value_to_number(value)->value, precision);
            break;

        case JsonValue::kString:
            return "\"" + utils::escape(json_value_to_string(value)->value) + "\"";
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
std::string JsonWriter::print_array( JsonValueArray const* value, int indent_level )
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
std::string JsonWriter::print_object( JsonValueObject const* value, int indent_level )
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

} // namespace utils
} // namespace genesis

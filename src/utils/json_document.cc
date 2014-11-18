/**
 * @brief Implementation of JSON document functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/json_document.hh"

namespace genesis {

// =============================================================================
//     JsonValueNumber
// =============================================================================

int JsonValueNumber::precision = 6;

// =============================================================================
//     JsonValueArray
// =============================================================================

std::string JsonValueArray::ToString()
{
    std::stringstream ss;
    ss << "[ ";

    bool first = true;
    for (JsonValue* v : value) {
        if (!first) {
            ss << ", ";
        }
        ss << v->ToString();
        first = false;
    }

    ss << " ]";
    return ss.str();
}

} // namespace genesis

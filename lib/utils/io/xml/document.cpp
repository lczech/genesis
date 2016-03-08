/**
 * @brief Implementation of XML document functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/io/xml/document.hpp"

#include "utils/core/logging.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Converter Functions
// =================================================================================================

/**
 * @brief Converts a pointer to an XmlValue to XmlComment if appropriate.
 *
 * Triggers a warning and returns a nullptr if the dynamic type of the object is not actually
 * an XmlComment object.
 */
const XmlComment* xml_value_to_comment (const XmlValue* v)
{
    if (v->type() != XmlValue::kComment) {
        LOG_WARN << "Invalid conversion from XmlValue::" << v->type_to_string()
                 << " to XmlValue::Comment.";
        return nullptr;
    }
    return static_cast<XmlComment*> (const_cast<XmlValue*> (v));
}

/**
 * @brief Converts a pointer to an XmlValue to XmlMarkup if appropriate.
 *
 * Triggers a warning and returns a nullptr if the dynamic type of the object is not actually
 * an XmlMarkup object.
 */
const XmlMarkup* xml_value_to_markup (const XmlValue* v)
{
    if (v->type() != XmlValue::kMarkup) {
        LOG_WARN << "Invalid conversion from XmlValue::" << v->type_to_string()
                 << " to XmlValue::Markup.";
        return nullptr;
    }
    return static_cast<XmlMarkup*> (const_cast<XmlValue*> (v));
}

/**
 * @brief Converts a pointer to an XmlValue to XmlElement if appropriate.
 *
 * Triggers a warning and returns a nullptr if the dynamic type of the object is not actually
 * an XmlElement object.
 */
const XmlElement* xml_value_to_element (const XmlValue* v)
{
    if (v->type() != XmlValue::kElement) {
        LOG_WARN << "Invalid conversion from XmlValue::" << v->type_to_string()
                 << " to XmlValue::Element.";
        return nullptr;
    }
    return static_cast<XmlElement*> (const_cast<XmlValue*> (v));
}

} // namespace utils
} // namespace genesis

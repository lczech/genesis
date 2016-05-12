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
 * @brief Implementation of XML document functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/formats/xml/document.hpp"

#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Converter Functions
// =================================================================================================

/**
 * @brief Converts a pointer to an XmlValue to XmlComment if appropriate.
 *
 * Throws if the dynamic type of the object is not actually an XmlComment object.
 */
const XmlComment* xml_value_to_comment (const XmlValue* v)
{
    if (v->type() != XmlValue::kComment) {
        throw std::domain_error(
            "Invalid conversion from XmlValue::" + v->type_to_string() + " to XmlValue::Comment."
        );
    }
    return static_cast<XmlComment*> (const_cast<XmlValue*> (v));
}

/**
 * @brief Converts a pointer to an XmlValue to XmlMarkup if appropriate.
 *
 * Throws if the dynamic type of the object is not actually an XmlMarkup object.
 */
const XmlMarkup* xml_value_to_markup (const XmlValue* v)
{
    if (v->type() != XmlValue::kMarkup) {
        throw std::domain_error(
            "Invalid conversion from XmlValue::" + v->type_to_string() + " to XmlValue::Markup."
        );
    }
    return static_cast<XmlMarkup*> (const_cast<XmlValue*> (v));
}

/**
 * @brief Converts a pointer to an XmlValue to XmlElement if appropriate.
 *
 * Throws if the dynamic type of the object is not actually an XmlElement object.
 */
const XmlElement* xml_value_to_element (const XmlValue* v)
{
    if (v->type() != XmlValue::kElement) {
        throw std::domain_error(
            "Invalid conversion from XmlValue::" + v->type_to_string() + " to XmlValue::Element."
        );
    }
    return static_cast<XmlElement*> (const_cast<XmlValue*> (v));
}

} // namespace utils
} // namespace genesis

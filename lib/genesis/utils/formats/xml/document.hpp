#ifndef GENESIS_UTILS_FORMATS_XML_DOCUMENT_H_
#define GENESIS_UTILS_FORMATS_XML_DOCUMENT_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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
 * @brief A collection of classes for working with XML documents. See XmlDocument for more.
 *
 * @file
 * @ingroup utils
 */

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "genesis/utils/core/std.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Xml Value
// =================================================================================================

/**
 *
 */
class XmlValue
{
public:
    enum Type {
        kComment,
        kMarkup,
        kElement
    };

    inline static std::string type_to_string (const Type t)
    {
        switch (t) {
            case kComment : return "Comment";
            case kMarkup  : return "Markup";
            case kElement : return "Element";
            default       : return "Unknown";
        }
    }

    inline std::string type_to_string () const
    {
        return type_to_string(type_);
    }

    inline Type type() const
    {
        return type_;
    }

    inline bool is_comment()
    {
        return type_ == kComment;
    }

    inline bool is_markup()
    {
        return type_ == kMarkup;
    }

    inline bool is_element()
    {
        return type_ == kElement;
    }

    virtual ~XmlValue() {};

protected:
    /**
     * @brief Protected constructor that allows derived classes to set their value type.
     *
     * Storing the type of each object as a special member saves the issue of using run time type
     * inference, making it faster and easier to work with objects of different types.
     */
    XmlValue (const Type type) : type_(type) {};

    /** @brief Stores the type of an object. Set by the constructor. */
    const Type type_;
};

// =================================================================================================
//     Xml Comment
// =================================================================================================

/**
 *
 */
class XmlComment : public XmlValue
{
public:
    XmlComment()                           : XmlValue(kComment)                   {};
    XmlComment(const std::string& comment) : XmlValue(kComment), content(comment) {};

    std::string content;
};

// =================================================================================================
//     Xml Markup
// =================================================================================================

/**
 *
 */
class XmlMarkup : public XmlValue
{
public:
    XmlMarkup()                           : XmlValue(kMarkup)                   {};
    XmlMarkup(const std::string& content) : XmlValue(kMarkup), content(content) {};

    std::string content;
};

// =================================================================================================
//     Xml Element
// =================================================================================================

/**
 *
 */
class XmlElement : public XmlValue
{
public:
    typedef std::unordered_map<std::string, std::string> StringMapType;

    XmlElement()                       : XmlValue(kElement)           {};
    XmlElement(const std::string& tag) : XmlValue(kElement), tag(tag) {};

    virtual ~XmlElement() override
    {
        clear();
    }

    inline void clear()
    {
        tag = "";
        attributes.clear();
        content.clear();
    }

    inline XmlComment* append_comment (const std::string& comment)
    {
        auto elm = utils::make_unique<XmlComment>(comment);
        auto ptr = elm.get();
        content.push_back(std::move(elm));
        return ptr;
    }

    inline XmlMarkup* append_markup (const std::string& text)
    {
        auto elm = utils::make_unique<XmlMarkup>(text);
        auto ptr = elm.get();
        content.push_back(std::move(elm));
        return ptr;
    }

    inline XmlElement* append_element (const std::string& tag)
    {
        auto elm = utils::make_unique<XmlElement>(tag);
        auto ptr = elm.get();
        content.push_back(std::move(elm));
        return ptr;
    }

    std::string                            tag;
    StringMapType                          attributes;
    std::vector<std::unique_ptr<XmlValue>> content;
};

// =================================================================================================
//     Xml Document
// =================================================================================================

/**
 *
 */
class XmlDocument : public XmlElement
{
public:
    virtual ~XmlDocument() override
    {
        clear();
    }

    inline void clear()
    {
        XmlElement::clear();
        xml_tag = "";
        declarations.clear();
    }

    std::string   xml_tag;
    StringMapType declarations;
};

// =================================================================================================
//     Converter Functions
// =================================================================================================

const XmlComment* xml_value_to_comment (const XmlValue* v);
const XmlMarkup*  xml_value_to_markup  (const XmlValue* v);
const XmlElement* xml_value_to_element (const XmlValue* v);

} // namespace utils
} // namespace genesis

#endif // include guard

#ifndef GENESIS_UTILS_IO_XML_DOCUMENT_H_
#define GENESIS_UTILS_IO_XML_DOCUMENT_H_

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

#include "utils/core/std.hpp"

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
        auto elm = make_unique<XmlComment>(comment);
        auto ptr = elm.get();
        content.push_back(std::move(elm));
        return ptr;
    }

    inline XmlMarkup* append_markup (const std::string& text)
    {
        auto elm = make_unique<XmlMarkup>(text);
        auto ptr = elm.get();
        content.push_back(std::move(elm));
        return ptr;
    }

    inline XmlElement* append_element (const std::string& tag)
    {
        auto elm = make_unique<XmlElement>(tag);
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

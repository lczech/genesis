#ifndef GNS_UTILS_XMLDOCUMENT_H_
#define GNS_UTILS_XMLDOCUMENT_H_

/**
 * @brief A collection of classes for working with XML documents. See XmlDocument for more.
 *
 * @file
 * @ingroup utils
 */

#include <string>
#include <unordered_map>
#include <vector>

//~ #include "utils/logging.hpp"
//~ #include "utils/utils.hpp"

namespace genesis {

// =============================================================================
//     Xml Value
// =============================================================================

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

    static std::string TypeToString (const Type t)
    {
        switch (t) {
            case kComment : return "Comment";
            case kMarkup  : return "Markup";
            case kElement : return "Element";
            default       : return "Unknown";
        }
    }

    inline std::string TypeToString () const
    {
        return TypeToString(type_);
    }

    inline Type type() const
    {
        return type_;
    }

    inline bool IsComment()
    {
        return type_ == kComment;
    }

    inline bool IsMarkup()
    {
        return type_ == kMarkup;
    }

    inline bool IsElement()
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

// =============================================================================
//     Xml Comment
// =============================================================================

/**
 *
 */
class XmlComment : public XmlValue
{
public:
    XmlComment() : XmlValue(kComment) {};

    XmlComment(const std::string& v) : XmlValue(kComment), content(v) {};

    std::string content;
};

// =============================================================================
//     Xml Markup
// =============================================================================

/**
 *
 */
class XmlMarkup : public XmlValue
{
public:
    XmlMarkup() : XmlValue(kMarkup) {};

    XmlMarkup(const std::string& v) : XmlValue(kMarkup), content(v) {};

    std::string content;
};

// =============================================================================
//     Xml Element
// =============================================================================

/**
 *
 */
class XmlElement : public XmlValue
{
public:
    typedef std::unordered_map<std::string, std::string> StringMapType;

    XmlElement() : XmlValue(kElement) {};

    virtual ~XmlElement() override
    {
        clear();
    }

    void clear()
    {
        for (XmlValue* v : content) {
            delete v;
        }
        tag = "";
        attributes.clear();
        content.clear();
    }

    std::string           tag;
    StringMapType         attributes;
    std::vector<XmlValue*> content;
};

// =============================================================================
//     Xml Document
// =============================================================================

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

    void clear()
    {
        XmlElement::clear();
        xml_tag = "";
        declarations.clear();
    }

    std::string   xml_tag;
    StringMapType declarations;
};

// =============================================================================
//     Converter Functions
// =============================================================================

const XmlComment* XmlValueToComment (const XmlValue* v);
const XmlMarkup*  XmlValueToMarkup  (const XmlValue* v);
const XmlElement* XmlValueToElement (const XmlValue* v);

} // namespace genesis

#endif // include guard

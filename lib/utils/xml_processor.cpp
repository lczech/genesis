/**
 * @brief Implementation of functions for parsing and printing XML documents.
 *
 * @file
 * @ingroup utils
 */

#include "utils/xml_processor.hpp"

#include "assert.h"

#include "utils/xml_document.hpp"
#include "utils/logging.hpp"
#include "utils/utils.hpp"

namespace genesis {

// =============================================================================
//     Parsing
// =============================================================================

/*
std::string XmlProcessor::XmlDescape (std::string& xml)
{
    std::string res;
    res = StringReplaceAll(xml, "&lt;",   "<");
    res = StringReplaceAll(res, "&gt;",   ">");
    res = StringReplaceAll(res, "&amp;",  "&");
    res = StringReplaceAll(res, "&apos;", "'");
    res = StringReplaceAll(res, "&quot;", "\"");
    return res;
}
*/

// =============================================================================
//     Printing
// =============================================================================

/**
 * @brief The indent used for printing the XML elements.
 */
int XmlProcessor::indent = 4;

/**
 * @brief Writes an XML file from an XmlDocument. Returns true iff successful.
 */
bool XmlProcessor::to_file (const std::string& fn, const XmlDocument& document)
{
    if (FileExists(fn)) {
        LOG_WARN << "XML file '" << fn << "' already exist. Will not overwrite it.";
        return false;
    }
    std::string xml;
    to_string(xml, document);
    return FileWrite(fn, xml);
}

/**
 * @brief Gives the XML string representation of a XmlDocument.
 */
void XmlProcessor::to_string (std::string& xml, const XmlDocument& document)
{
    xml = to_string(document);
}

/**
 * @brief Returns the XML representation of a XmlDocument.
 */
std::string XmlProcessor::to_string (const XmlDocument& document)
{
    std::string res = "";
    if (!document.xml_tag.empty() || !document.declarations.empty()) {
        res = "<?" + document.xml_tag + PrintAttributesList(document.declarations) + "?>\n";
    }
    PrintElement(res, &document, 0);
    return res + "\n";
}

/**
 * @brief Prints an XML comment.
 */
void XmlProcessor::PrintComment (std::string& xml, const XmlComment* value)
{
    xml += "<!--" + value->content + "-->";
}

/**
 * @brief Prints an XML markup (simple text).
 */
void XmlProcessor::PrintMarkup  (std::string& xml, const XmlMarkup*  value)
{
    xml += XmlEscape(value->content);
}

/**
 * @brief Prints an XML element.
 */
void XmlProcessor::PrintElement (std::string& xml, const XmlElement* value, const int indent_level)
{
    // prepare indention and open tag
    std::string in0 (indent_level * indent, ' ');
    xml += in0 + "<" + value->tag + PrintAttributesList(value->attributes);

    // if it's an empty element, close it, and we are done
    if (value->content.size() == 0) {
        xml += " />";
        return;
    }

    // if the element only contains a single markup, don't add new lines. however, if it contains
    // more data, put each of them in a new line
    xml += ">";
    if (value->content.size() == 1 && value->content[0]->IsMarkup()) {
        PrintMarkup(xml, XmlValueToMarkup(value->content[0]));
    } else {
        std::string in1 ((indent_level + 1) * indent, ' ');
        xml += "\n";

        for (XmlValue* v : value->content) {
            if (v->IsComment()) {
                xml += in1;
                PrintComment(xml, XmlValueToComment(v));
            } else if (v->IsMarkup()) {
                xml += in1;
                PrintMarkup(xml, XmlValueToMarkup(v));
            } else if (v->IsElement()) {
                PrintElement(xml, XmlValueToElement(v), indent_level + 1);
            } else {
                // there are no other cases
                assert(0);
            }
            xml += "\n";
        }
        xml += in0;
    }

    xml += "</" + value->tag + ">";
}

/**
 * @brief Prints a list of XML attributes.
 */
std::string XmlProcessor::PrintAttributesList (StringMapType attr)
{
    std::string xml;
    for (auto pair : attr) {
        xml += " " + pair.first + "=\"" + pair.second + "\"";
    }
    return xml;
}

/**
 * @brief Escape special XML characters.
 */
std::string XmlProcessor::XmlEscape (const std::string& txt)
{
    std::string res;
    res = StringReplaceAll(txt, "<",  "&lt;");
    res = StringReplaceAll(res, ">",  "&gt;");
    res = StringReplaceAll(res, "&",  "&amp;");
    res = StringReplaceAll(res, "'",  "&apos;");
    res = StringReplaceAll(res, "\"", "&quot;");
    return res;
}

} // namespace genesis

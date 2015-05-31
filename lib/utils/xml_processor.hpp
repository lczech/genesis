#ifndef GENESIS_UTILS_XML_PROCESSOR_H_
#define GENESIS_UTILS_XML_PROCESSOR_H_

/**
 * @brief Functions for parsing and printing an XML document. See XmlProcessor for more.
 *
 * @file
 * @ingroup utils
 */

#include <string>
#include <unordered_map>

//~ #include "utils/lexer.hpp"

namespace genesis {

// =============================================================================
//     Forward declarations
// =============================================================================

class XmlComment;
class XmlMarkup;
class XmlElement;
class XmlDocument;

// =============================================================================
//     Xml Lexer
// =============================================================================

// =============================================================================
//     Xml Processor
// =============================================================================

/**
 * @brief
 */
class XmlProcessor
{
    typedef std::unordered_map<std::string, std::string> StringMapType;

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

public:
    static int indent;

    static bool        to_file   (const std::string& fn,  const XmlDocument& document);
    static void        to_string (      std::string& xml, const XmlDocument& document);
    static std::string to_string (                        const XmlDocument& document);

protected:
    static void PrintComment (std::string& xml, const XmlComment* value);
    static void PrintMarkup  (std::string& xml, const XmlMarkup*  value);
    static void PrintElement (std::string& xml, const XmlElement* value, const int indent_level);

    static std::string PrintAttributesList (StringMapType attr);
    static std::string XmlEscape (const std::string& txt);
};

} // namespace genesis

#endif // include guard

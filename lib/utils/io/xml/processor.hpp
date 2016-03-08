#ifndef GENESIS_UTILS_IO_XML_PROCESSOR_H_
#define GENESIS_UTILS_IO_XML_PROCESSOR_H_

/**
 * @brief Functions for parsing and printing an XML document. See XmlProcessor for more.
 *
 * @file
 * @ingroup utils
 */

#include <string>
#include <unordered_map>

namespace genesis {
namespace utils {

// =================================================================================================
//     Forward declarations
// =================================================================================================

class XmlComment;
class XmlMarkup;
class XmlElement;
class XmlDocument;

// =================================================================================================
//     Xml Lexer
// =================================================================================================

// =================================================================================================
//     Xml Processor
// =================================================================================================

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
    bool        to_file   (const std::string& fn,  const XmlDocument& document);
    void        to_string (      std::string& xml, const XmlDocument& document);
    std::string to_string (                        const XmlDocument& document);

    // -----------------------------------------------------
    //     Internal
    // -----------------------------------------------------

protected:
    void print_comment (std::string& xml, const XmlComment* value);
    void print_markup  (std::string& xml, const XmlMarkup*  value);
    void print_element (std::string& xml, const XmlElement* value, const int indent_level);

    std::string print_attributes_list (StringMapType attr);
    std::string xml_escape (const std::string& txt);

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

public:

    /**
     * @brief The indent used for printing the XML elements.
     */
    int indent = 4;
};

} // namespace utils
} // namespace genesis

#endif // include guard

#ifndef GENESIS_UTILS_IO_XML_WRITER_H_
#define GENESIS_UTILS_IO_XML_WRITER_H_

/**
 * @brief
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
\
// =================================================================================================
//     Xml Writer
// =================================================================================================

/**
 * @brief Write Xml data.
 */
class XmlWriter
{
    typedef std::unordered_map<std::string, std::string> StringMapType;

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

public:
    void        to_file   ( const XmlDocument& document, const std::string& filename);
    void        to_string ( const XmlDocument& document,       std::string& output);
    std::string to_string ( const XmlDocument& document);

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
     * @brief Indent used for printing the XML elements.
     */
    int indent = 4;
};

} // namespace utils
} // namespace genesis

#endif // include guard

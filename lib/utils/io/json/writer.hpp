#ifndef GENESIS_UTILS_IO_JSON_WRITER_H_
#define GENESIS_UTILS_IO_JSON_WRITER_H_

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include <string>

#include "utils/io/json/lexer.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Forward declarations
// =================================================================================================

class JsonDocument;
class JsonValue;
class JsonValueArray;
class JsonValueObject;

// =================================================================================================
//     Json Writer
// =================================================================================================

/**
 * @brief Write Json data.
 */
class JsonWriter
{
public:

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    JsonWriter()  = default;
    ~JsonWriter() = default;

    JsonWriter( JsonWriter const& ) = default;
    JsonWriter( JsonWriter&& )      = default;

    JsonWriter& operator= ( JsonWriter const& ) = default;
    JsonWriter& operator= ( JsonWriter&& )      = default;

    // ---------------------------------------------------------------------
    //     Writing
    // ---------------------------------------------------------------------

public:

    void        to_file   ( JsonDocument const& document, std::string const& filename);
    void        to_string ( JsonDocument const& document, std::string& output);
    std::string to_string ( JsonDocument const& document);

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

protected:

    std::string print_value  ( JsonValue       const* value);
    std::string print_array  ( JsonValueArray  const* value, int indent_level);
    std::string print_object ( JsonValueObject const* value, int indent_level);

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

public:

    /**
     * @brief Precision used for printing floating point numbers, particularly Json Value Numbers.
     */
    int precision = 6;

    /**
     * @brief Indent used for printing the elements of Json Arrays and Objects.
     */
    int indent = 4;

};

} // namespace utils
} // namespace genesis

#endif // include guard

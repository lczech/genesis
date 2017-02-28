#ifndef GENESIS_UTILS_FORMATS_JSON_READER_H_
#define GENESIS_UTILS_FORMATS_JSON_READER_H_

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
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include <iosfwd>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Forward declarations
// =================================================================================================

class InputStream;
class JsonDocument;

// =================================================================================================
//     Json Reader
// =================================================================================================

/**
 * @brief Read `Json` data into a JsonDocument.
 *
 * This class provides functions for parsing `json` data into a JsonDocument. The parsing works
 * this way: Each JsonDocument is also a Json object, and can contain other objects, arrays, or
 * simple value types. The parsing here is thus splitted in different functions, being recursively
 * called for every level of nesting within objects and arrays.
 */
class JsonReader
{
public:

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    JsonReader()  = default;
    ~JsonReader() = default;

    JsonReader( JsonReader const& ) = default;
    JsonReader( JsonReader&& )      = default;

    JsonReader& operator= ( JsonReader const& ) = default;
    JsonReader& operator= ( JsonReader&& )      = default;

    // ---------------------------------------------------------------------
    //     Reading
    // ---------------------------------------------------------------------

    /**
     * @brief Read from a stream containing a JSON document and parse its contents into a JsonDocument.
     */
    JsonDocument from_stream( std::istream& input_stream  ) const;

    /**
     * @brief Take a JSON document file path and parse its contents into a JsonDocument.
     *
     * If the file does not exists, the function throws.
     */
    JsonDocument from_file   (const std::string& filename ) const;

    /**
     * @brief Take a string containing a JSON document and parse its contents into a JsonDocument.
     */
    JsonDocument from_string (const std::string& json     ) const;

    // ---------------------------------------------------------------------
    //     Parsing Functions
    // ---------------------------------------------------------------------

    JsonDocument parse( InputStream& input_stream ) const;
    JsonDocument parse_value( InputStream& input_stream ) const;
    JsonDocument parse_array(  InputStream& input_stream ) const;
    JsonDocument parse_object( InputStream& input_stream ) const;
    JsonDocument parse_number( InputStream& input_stream ) const;

};

} // namespace utils
} // namespace genesis

#endif // include guard

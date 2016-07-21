#ifndef GENESIS_UTILS_FORMATS_JSON_WRITER_H_
#define GENESIS_UTILS_FORMATS_JSON_WRITER_H_

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
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "utils/formats/json/lexer.hpp"

#include <iosfwd>
#include <string>

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

    void        to_stream ( JsonDocument const& document, std::ostream& out ) const;
    void        to_file   ( JsonDocument const& document, std::string const& filename) const;
    void        to_string ( JsonDocument const& document, std::string& output) const;
    std::string to_string ( JsonDocument const& document) const;

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

protected:

    void print_value  ( JsonValue       const* value, std::ostream& out) const;
    void print_array  ( JsonValueArray  const* value, std::ostream& out, int indent_level) const;
    void print_object ( JsonValueObject const* value, std::ostream& out, int indent_level) const;

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

public:

    // /* *
    //  * @brief Precision used for printing floating point numbers, particularly Json Value Numbers.
    //  */
    // int precision = 6;

    /**
     * @brief Indent used for printing the elements of Json Arrays and Objects.
     */
    int indent = 4;

};

} // namespace utils
} // namespace genesis

#endif // include guard

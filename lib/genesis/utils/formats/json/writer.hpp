#ifndef GENESIS_UTILS_FORMATS_JSON_WRITER_H_
#define GENESIS_UTILS_FORMATS_JSON_WRITER_H_

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

class JsonDocument;

// =================================================================================================
//     Json Writer
// =================================================================================================

/**
 * @brief Write Json data from a JsonDocument.
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

    /**
     * @brief Write a JsonDocument to a stream.
     */
    void        to_stream ( JsonDocument const& document, std::ostream& out ) const;

    /**
     * @brief Write a JsonDocument to a file.
     *
     * If the file cannot be written to, the function throws an exception. Also, by default, if the
     * file already exists, an exception is thrown.
     * See @link Options::allow_file_overwriting( bool ) Options::allow_file_overwriting()@endlink
     * to change this behaviour.
     */
    void        to_file   ( JsonDocument const& document, std::string const& filename) const;

    /**
     * @brief Give the Json string representation of a JsonDocument.
     */
    void        to_string ( JsonDocument const& document, std::string& output) const;

    /**
     * @brief Return the Json representation of a JsonDocument.
     */
    std::string to_string ( JsonDocument const& document) const;

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

private:

    /**
     * @brief Write the Json representation of any Json value to a stream.
     */
    void print_value  ( JsonDocument const& value, std::ostream& out) const;

    /**
     * @brief Write the Json representation of a Json array to a stream.
     */
    void print_array  ( JsonDocument const& value, std::ostream& out, int indent_level) const;

    /**
     * @brief Write the Json representation of a Json object to a stream.
     */
    void print_object ( JsonDocument const& value, std::ostream& out, int indent_level) const;

    // ---------------------------------------------------------------------
    //     Settings
    // ---------------------------------------------------------------------

public:

    /**
     * @brief Get the precision used for printing floating point numbers.
     */
    size_t precision() const
    {
        return precision_;
    }

    /**
     * @brief Set the precision used for printing floating point numbers.
     *
     * The function returns a reference to the JsoNWriter to allow for fluent interfaces.
     */
    JsonWriter& precision( size_t value )
    {
        precision_ = value;
        return *this;
    }

    /**
     * @brief Get the indent used for printing the elements of Json arrays and objects.
     */
    size_t indent() const
    {
        return indent_;
    }

    /**
     * @brief Set the indent used for printing the elements of Json arrays and objects.
     *
     * The function returns a reference to the JsoNWriter to allow for fluent interfaces.
     */
    JsonWriter& indent( size_t value )
    {
        indent_ = value;
        return *this;
    }

    // ---------------------------------------------------------------------
    //     Data Members
    // ---------------------------------------------------------------------

private:

    size_t precision_ = 6;
    size_t indent_    = 4;

};

} // namespace utils
} // namespace genesis

#endif // include guard

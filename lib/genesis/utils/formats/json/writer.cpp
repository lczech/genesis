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

#include "genesis/utils/formats/json/writer.hpp"

#include <cassert>
#include <fstream>
#include <ostream>
#include <stdexcept>

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/formats/json/document.hpp"
#include "genesis/utils/formats/json/iterator.hpp"
#include "genesis/utils/text/string.hpp"
#include "genesis/utils/io/output_stream.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Writing
// =================================================================================================

void JsonWriter::to_stream( JsonDocument const& document, std::ostream& out ) const
{
    print_value( document, out );
}

void JsonWriter::to_file( JsonDocument const& document, std::string const& filename ) const
{
    std::ofstream ofs;
    utils::file_output_stream( filename, ofs );
    print_value( document, ofs );
}

void JsonWriter::to_string( JsonDocument const& document, std::string& output ) const
{
    output = to_string(document);
}

std::string JsonWriter::to_string( JsonDocument const& document ) const
{
    std::stringstream sstr;
    print_value( document, sstr );
    return sstr.str();
}

// =================================================================================================
//     Printing
// =================================================================================================

void JsonWriter::print_value(
    JsonDocument const& value,
    std::ostream& out
) const {
    switch(value.type()) {
        case JsonDocument::ValueType::kNull: {
            out << "null";
            break;
        }
        case JsonDocument::ValueType::kBoolean: {
            out << ( value.get_boolean() ? "true" : "false" );
            break;
        }
        case JsonDocument::ValueType::kNumberFloat: {
            out << to_string_precise( value.get_number_float(), precision_ );
            break;
        }
        case JsonDocument::ValueType::kNumberSigned: {
            out << value.get_number_signed();
            break;
        }
        case JsonDocument::ValueType::kNumberUnsigned: {
            out << value.get_number_unsigned();
            break;
        }
        case JsonDocument::ValueType::kString: {
            out << "\"" + utils::escape( value.get_string() ) + "\"";
            break;
        }
        case JsonDocument::ValueType::kArray: {
            print_array( value, out, 0 );
            break;
        }
        case JsonDocument::ValueType::kObject: {
            print_object( value, out, 0 );
            break;
        }
        default: {
            assert( false );
        }
    }
}

void JsonWriter::print_array(
    JsonDocument const& value,
    std::ostream& out,
    int indent_level
) const {
    int il = indent_level + 1;
    std::string in (il * indent_, ' ');

    // Check if array contains non-primitive values.
    // If so, we use better bracket placement to make document look nicer.
    bool has_large = false;
    for( auto const& elem : value ) {
        has_large |= ( elem.is_array() || elem.is_object());
    }

    out << "[ ";
    bool first = true;
    for( auto const& elem : value ) {
        if (!first) {
            out << ", ";
        }
        if (has_large) {
            out << "\n" << in;
        }
        if (elem.is_array()) {
            print_array( elem, out, il );
        } else if ( elem.is_object() ) {
            print_object( elem, out, il );
        } else {
            print_value( elem, out );
        }
        first = false;
    }

    if (has_large) {
        out << "\n" << std::string(indent_level * indent_, ' ');
    } else {
        out << " ";
    }
    out << "]";
}

void JsonWriter::print_object(
    JsonDocument const& value,
    std::ostream& out,
    int indent_level
) const {
    int il = indent_level + 1;
    std::string in (il * indent_, ' ');
    out << "{";

    bool first = true;
    for( auto it = value.begin(); it != value.end(); ++it ) {
        if (!first) {
            out << ",";
        }
        out << "\n" << in << "\"" << it.key() << "\": ";
        if ( it.value().is_array() ) {
            print_array( it.value(), out, il );
        } else if( it.value().is_object() ) {
            print_object( it.value(), out, il );
        } else {
            print_value( it.value(), out );
        }
        first = false;
    }

    out << "\n" << std::string(indent_level * indent_, ' ') << "}";
}

} // namespace utils
} // namespace genesis

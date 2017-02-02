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

#include "utils/formats/json/reader.hpp"

#include <cassert>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "utils/core/fs.hpp"
#include "utils/core/std.hpp"
#include "utils/formats/json/document.hpp"
#include "utils/io/input_stream.hpp"
#include "utils/io/parser.hpp"
#include "utils/io/scanner.hpp"
#include "utils/text/char.hpp"
#include "utils/text/string.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Reading
// =================================================================================================

JsonDocument JsonReader::from_stream( std::istream& input_stream ) const
{
    utils::InputStream is( utils::make_unique< utils::StreamInputSource >( input_stream ));
    return parse_object( is );
}

JsonDocument JsonReader::from_file (const std::string& filename ) const
{
    utils::InputStream is( utils::make_unique< utils::FileInputSource >( filename ));
    return parse_object( is );
}

JsonDocument JsonReader::from_string (const std::string& json ) const
{
    utils::InputStream is( utils::make_unique< utils::StringInputSource >( json ));
    return parse_object( is );
}

// =================================================================================================
//     Parsing
// =================================================================================================

// -----------------------------------------------------------------------------
//     Parse Value
// -----------------------------------------------------------------------------

JsonDocument JsonReader::parse_value( InputStream& input_stream ) const
{
    auto& it = input_stream;

    // Go to first non-white char.
    skip_while( it, isspace );
    // while( it && lookup_[ *it ] == CharTypes::kSpace ) {
    //     ++it;
    // }
    // skip_while( it, [&]( char c ){ return lookup_[c] == CharTypes::kSpace; } );

    if( !it ) {
        // If there is no content, return an empty Json doc.
        return nullptr;

    } else if( *it == '[' ) {
        // Parse an array.
        return parse_array( it );

    } else if( *it == '{' ) {
        // Parse an object.
        return parse_object( it );

    } else if( *it == '"' ) {
        // Parse a string.
        return parse_quoted_string( it );

    } else if( isalpha( *it ) ) {

        // Either null or boolean.
        auto value = to_lower( read_while( it, isalpha ));
        if(  value == "null" ) {
            // If it is a null token, return an empty Json doc.
            return nullptr;

            // Otherwise return a boolean value.
        } else if( value == "true" ) {
            return JsonDocument::boolean( true );
        } else if( value == "false" ) {
            return JsonDocument::boolean( false );
        } else {
            throw std::runtime_error(
                "Unexpected Json input string: '" + value + "' at " + it.at() + "."
            );
        }

    } else if( isdigit( *it ) or char_is_sign( *it ) or *it == '.' ) {
        // Parse a number.
        return parse_number( it );

    } else {
        throw std::runtime_error(
            "Unexpected Json input char: '" + std::string( 1, *it ) + "' at " + it.at() + "."
        );
    }
}

// -----------------------------------------------------------------------------
//     Parse Array
// -----------------------------------------------------------------------------

JsonDocument JsonReader::parse_array( InputStream& input_stream ) const
{
    JsonDocument doc = JsonDocument::array();
    auto& it = input_stream;

    // Initial check whether this actually is an array.
    skip_while( it, isspace );
    read_char_or_throw( it, '[' );

    while( it ) {
        // Get the element.
        skip_while( it, isspace );
        doc.emplace_back( parse_value( it ));

        // Check for end of array, leave if found.
        skip_while( it, isspace );
        if( ! it ) {
            throw std::runtime_error( "Unexpected end of Json Document at " + it.at() );
        }
        if( *it == ']' ) {
            break;
        }

        // We expect more. Or throw, if we unexpectedly are at the end or have an illegal char.
        read_char_or_throw( it, ',' );
    }

    // We are at the end of the array. Move to next char.
    assert( it && *it == ']' );
    ++it;

    return doc;
}

// -----------------------------------------------------------------------------
//     Parse Object
// -----------------------------------------------------------------------------

JsonDocument JsonReader::parse_object( InputStream& input_stream ) const
{
    JsonDocument doc = JsonDocument::object();
    auto& it = input_stream;

    // Initial check whether this actually is an object.
    skip_while( it, isspace );
    read_char_or_throw( it, '{' );

    while( it ) {
        // Get the key.
        skip_while( it, isspace );
        affirm_char_or_throw( it, '"' );
        auto key = parse_quoted_string( it );

        // Find the colon and skip it.
        skip_while( it, isspace );
        read_char_or_throw( it, ':' );

        // Get the value and insert into object.
        skip_while( it, isspace );
        doc[ key ] = parse_value( it );

        // Check for end of object, leave if found.
        skip_while( it, isspace );
        if( ! it ) {
            throw std::runtime_error( "Unexpected end of Json Document at " + it.at() );
        }
        if( *it == '}' ) {
            break;
        }

        // We expect more. Or throw, if we unexpectedly are at the end or have an illegal char.
        read_char_or_throw( it, ',' );
    }

    // We are at the end of the object. Move to next char.
    assert( it && *it == '}' );
    ++it;

    return doc;
}

// -----------------------------------------------------------------------------
//     Parse Number
// -----------------------------------------------------------------------------

JsonDocument JsonReader::parse_number( InputStream& input_stream ) const
{
    auto& it = input_stream;
    skip_while( it, isspace );
    if( !it ) {
        return 0;
    }

    // Sign
    bool is_neg = false;
    if( *it == '-' ){
        is_neg = true;
        ++it;
    } else if( *it == '+' ) {
        ++it;
    }

    if( ! it || ! isdigit( *it ) ) {
        throw std::runtime_error( "Invalid Json number at " + it.at() );
    }

    // Integer Part
    JsonDocument::NumberUnsignedType ix = 0;
    while( it && isdigit( *it )) {
        int y = *it - '0';
        ix *= 10;
        ix += y;
        ++it;
    }

    // If not float
    if( !it || !( *it == '.' || tolower(*it) == 'e' ) ) {
        if( is_neg ) {
            return JsonDocument::number_signed( -ix );
        } else {
            return JsonDocument::number_unsigned( ix );
        }
    }

    // Decimal part
    JsonDocument::NumberFloatType dx = ix;
    if( it && *it == '.' ) {
        ++it;

        JsonDocument::NumberFloatType pos = 1.0;
        while( it && isdigit( *it )) {
            pos /= 10.0;
            int y = *it - '0';
            dx += y * pos;
            ++it;
        }
    }

    // Exponential part
    if( it && tolower(*it) == 'e' ) {
        ++it;

        int e = parse_signed_integer<int>( it );
        if( e != 0 ) {
            JsonDocument::NumberFloatType base;
            if( e < 0 ) {
                base = 0.1;
                e = -e;
            } else {
                base = 10;
            }

            while( e != 1 ) {
                if( ( e & 1 ) == 0 ) {
                    base = base * base;
                    e >>= 1;
                } else {
                    dx *= base;
                    --e;
                }
            }
            dx *= base;
        }
    }

    // Sign
    if (is_neg) {
        dx = -dx;
    }

    return JsonDocument::number_float( dx );
}

} // namespace utils
} // namespace genesis

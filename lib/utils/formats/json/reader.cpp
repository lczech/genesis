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

#include <assert.h>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "utils/core/fs.hpp"
#include "utils/core/std.hpp"
#include "utils/formats/json/document.hpp"
#include "utils/io/input_stream.hpp"
#include "utils/io/scanner.hpp"
#include "utils/text/string.hpp"

#include "utils/formats/json/better.hpp"
#include "utils/text/char.hpp"
#include "utils/io/parser.hpp"
#include "utils/io/scanner.hpp"

// #include "test/src/utils/formats/json.hpp"


namespace genesis {
namespace utils {

// =================================================================================================
//      Constructor and Rule of Five
// =================================================================================================

JsonReader::JsonReader()
{
    lookup_.set_all( CharTypes::kInvalid );

    lookup_.set_if( [](char c){ return isspace(c); }, CharTypes::kSpace );
    lookup_.set_if( [](char c){ return isalpha(c); }, CharTypes::kAlpha );
    lookup_.set_if( [](char c){ return isdigit( c ) or char_is_sign( c ) or c == '.'; }, CharTypes::kNumber );

    lookup_.set_char( '[', CharTypes::kBracketOpen );
    lookup_.set_char( ']', CharTypes::kBracketClose );
    lookup_.set_char( '{', CharTypes::kBraceOpen );
    lookup_.set_char( ']', CharTypes::kBraceClose );
    lookup_.set_char( '"', CharTypes::kQuotation );
}

// =================================================================================================
//     Reading
// =================================================================================================

JsonDocument JsonReader::from_stream( std::istream& input_stream ) const
{
    // Prepare doc, enable copy elision.
    JsonDocument doc;

    // Prepare stream.
    utils::InputStream is( utils::make_unique< utils::StreamInputSource >( input_stream ));

    affirm_char_or_throw( is, '{', SkipWhitespace::kLeading );

    return doc;
}

/**
 * @brief Take a JSON document file path and parses its contents into a JsonDocument.
 *
 * If the file does not exists, the function throws an `std::runtime_error`.
 */
void JsonReader::from_file (const std::string& filename, JsonDocument& document) const
{
    if( ! utils::file_exists(filename)) {
        throw std::runtime_error( "Json file '" + filename + "' does not exist." );
    }
    return from_string( utils::file_read(filename), document );
}

/**
 * @brief Take a string containing a JSON document and parses its contents into a JsonDocument.
 *
 * Returns true iff successfull.
 */
void JsonReader::from_string (const std::string& json, JsonDocument& document) const
{
    // do stepwise lexing
    JsonLexer lexer;
    lexer.from_string(json);

    if (lexer.empty()) {
        throw std::runtime_error( "Json document is empty." );
    }
    if (lexer.has_error()) {
        throw std::runtime_error(
            "Lexing error at " + lexer.back().at() + " with message: " + lexer.back().value()
        );
    }
    if (!lexer.begin()->is_bracket("{")) {
        throw std::runtime_error( "JSON document does not start with JSON object opener '{'." );
    }

    // A json document is also a json object, so we start parsing the doc as such.
    // The begin iterator will be incremented with every token being processed.
    document.clear();
    JsonLexer::iterator begin = lexer.begin();
    JsonLexer::iterator end   = lexer.end();

    parse_object(begin, end, &document);

    // After processing, the begin iterator will point to the lexer token that comes after
    // the one being processed last. If the document is well-formatted, this token is also
    // the end pointer of the iterator.
    if (begin != end) {
        throw std::runtime_error(
            "JSON document contains more information after the closing bracket."
        );
    }
}

// =================================================================================================
//     Parsing
// =================================================================================================

// -----------------------------------------------------------------------------
//     Parse Value
// -----------------------------------------------------------------------------

/**
 * @brief Parse a JSON value and fills it with data from the lexer.
 *
 * This function is a bit different from the other two process functions parse_array() and
 * parse_object(), because it takes its value parameter by reference. This is because when
 * entering the function it is not clear yet which type of value the current lexer token is, so a
 * new instance has to be created and stored in the pointer.
 */
void JsonReader::parse_value (
    JsonLexer::iterator& ct,
    JsonLexer::iterator& end,
    JsonValue*&          value
) const {
    // Proper usage of this function is to hand over a null pointer to a json value, which will be
    // assigned to a newly created value instance depending on the token type, so check for this
    // here. We don't want to overwrite existing values!
    assert (value == nullptr);

    // Check all possible valid lexer token types and turn them into json values.
    if (ct->is_symbol()) {
        // The lexer only returns null, true or false as symbols, so this is safe.
        if (ct->value().compare("null") == 0) {
            value = new JsonValueNull();
        } else {
            value = new JsonValueBool(ct->value());
        }
        ++ct;
        return;
    }

    if (ct->is_number()) {
        value = new JsonValueNumber(ct->value());
        ++ct;
        return;
    }

    if (ct->is_string()) {
        value = new JsonValueString(ct->value());
        ++ct;
        return;
    }

    if (ct->is_bracket("[")) {
        value = new JsonValueArray();
        parse_array (ct, end, json_value_to_array(value));
        return;
    }

    if (ct->is_bracket("{")) {
        value = new JsonValueObject();
        parse_object (ct, end, json_value_to_object(value));
        return;
    }

    // If the lexer token is not a fitting json value, we have an error.
    throw std::runtime_error(
        "JSON value contains invalid characters at " + ct->at() + ": '" + ct->value() + "'."
    );
}

// -----------------------------------------------------------------------------
//     Parse Array
// -----------------------------------------------------------------------------

/**
 * @brief Parse a JSON array and fill it with data elements from the lexer.
 */
void JsonReader::parse_array (
    JsonLexer::iterator& ct,
    JsonLexer::iterator& end,
    JsonValueArray*        value
) const {
    // Proper usage of this function is to hand over a valid pointer to a json array, so check
    // for this here.
    assert(value);

    if (ct == end || !ct->is_bracket("[")) {
        throw std::runtime_error( "JSON array does not start with '[' at " + ct->at() + "." );
    }

    ++ct;
    while (ct != end) {
        // Proccess the array element.
        JsonValue* element = nullptr;
        parse_value( ct, end, element );
        value->add(element);

        // Check for end of array, leave if found.
        if (ct == end || ct->is_bracket("]")) {
            break;
        }

        // Check for delimiter comma (indicates that there are more elements following).
        if (!ct->is_operator(",")) {
            throw std::runtime_error(
                "JSON array does not contain comma between elements at " + ct->at() + "."
            );
        }
        ++ct;
    }

    // The while loop above only stops when ct points to the end or to a closing bracket. In the
    // first case, we have an error; in the second, we are done with this object and can skip the
    // bracket.
    if (ct == end) {
        throw std::runtime_error( "JSON array ended unexpectedly." );
    }

    assert(ct->is_bracket("]"));
    ++ct;
}

// -----------------------------------------------------------------------------
//     Parse Object
// -----------------------------------------------------------------------------

/**
 * @brief Parse a JSON object and fill it with data members from the lexer.
 */
void JsonReader::parse_object (
    JsonLexer::iterator& ct,
    JsonLexer::iterator& end,
    JsonValueObject*       value
) const {
    // Proper usage of this function is to hand over a valid pointer to a json object, so check
    // for this here.
    assert(value);

    if (ct == end || !ct->is_bracket("{")) {
        throw std::runtime_error( "JSON object does not start with '{' at " + ct->at() + "." );
    }

    ++ct;
    while (ct != end) {
        // Check for name string and store it.
        if (!ct->is_string()) {
            throw std::runtime_error(
                "JSON object member does not start with name string at " + ct->at() + "."
            );
        }
        std::string name = ct->value();
        ++ct;

        // Check for delimiter colon.
        if (ct == end) {
            break;
        }
        if (!ct->is_operator(":")) {
            throw std::runtime_error(
                "JSON object member does not contain colon between name and value at "
                + ct->at() + "."
            );
        }
        ++ct;

        // Check for value and store it.
        if (ct == end) {
            break;
        }

        JsonValue* member = nullptr;
        parse_value( ct, end, member );
        value->set( name, member );

        // Check for end of object, leave if found (either way).
        if (ct == end || ct->is_bracket("}")) {
            break;
        }

        // Check for delimiter comma (indicates that there are more members following).
        if (!ct->is_operator(",")) {
            throw std::runtime_error(
                "JSON object does not contain comma between members at " + ct->at() + "."
            );
        }
        ++ct;
    }

    // The while loop above only stops when ct points to the end or to a closing bracket. In the
    // first case, we have an error; in the second, we are done with this object and can skip the
    // bracket.
    if (ct == end) {
        throw std::runtime_error( "JSON object ended unexpectedly." );
    }
    ++ct;
}

// =================================================================================================
//     Parsing
// =================================================================================================

JsonBetter JsonReader::parse( InputStream& input_stream ) const
{
    // JsonBetter doc = nullptr;
    auto& it = input_stream;

    // Go to first non-white char.
    skip_while( it, isspace );


    // while( it && lookup_[ *it ] == CharTypes::kSpace ) {
    //     ++it;
    // }

    // skip_while( it, [&]( char c ){ return lookup_[c] == CharTypes::kSpace; } );


    // // If there is no content, return an empty Json doc. Nothing to do here.
    // if( !it ) {
    //     return nullptr;
    // }
    // switch( lookup_[ *it ] ) {
    //     case 1: {
    //         return parse_array( it, doc );
    //         break;
    //     }
    //     case 2: {
    //         return parse_object( it, doc );
    //         break;
    //     }
    //     case 3: {
    //         return parse_quoted_string( it );
    //         break;
    //     }
    //     case 4: {
    //         // Either null or boolean.
    //         auto value = to_lower( read_while( it, isalpha ));
    //         if(  value == "null" ) {
    //             return nullptr;
    //             // Json doc is already null. Nothing to do here.
    //         } else if( value == "true" ) {
    //             return true;
    //         } else if( value == "false" ) {
    //             return false;
    //         } else {
    //             throw std::runtime_error(
    //                 "Unexpected Json input string: '" + value + "' at " + it.at() + "."
    //             );
    //         }
    //         break;
    //     }
    //     case 5: {
    //         auto num = parse_number_string( it );
    //         return JsonBetter::number_float( 0.0 );
    //         break;
    //     }
    //     default: {
    //         assert( lookup_[ *it ] == 0 );
    //         throw std::runtime_error(
    //             "Unexpected Json input char: '" + std::string( 1, *it ) + "' at " + it.at() + "."
    //         );
    //     }
    // }

    if( !it ) {
        // If there is no content, return an empty Json doc. Nothing to do here.
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
            // Json doc is already null. Nothing to do here.
            return nullptr;

        } else if( value == "true" ) {
            return true;
        } else if( value == "false" ) {
            return false;
        } else {
            throw std::runtime_error(
                "Unexpected Json input string: '" + value + "' at " + it.at() + "."
            );
        }

    } else if( isdigit( *it ) or char_is_sign( *it ) or *it == '.' ) {
        // // If it is a number, read it first, then convert, in order to get its type right.
        // auto num = parse_number_string( it );
        //
        // // double num = parse_float<double>( it );
        // // return JsonBetter::number_float( num );
        //
        // if( num.find('.') != std::string::npos ) {
        //     return JsonBetter::number_float( std::stod( num ) );
        //     // doc = std::stod( num );
        // } else if( num[0] == '-' ) {
        //     return JsonBetter::number_signed( std::stoll( num ) );
        //     // doc = std::stoll( num );
        // } else {
        //     return JsonBetter::number_unsigned( std::stoull( num ) );
        //     // doc = std::stoull( num );
        // }

        return parse_number( it );

    } else {
        throw std::runtime_error(
            "Unexpected Json input char: '" + std::string( 1, *it ) + "' at " + it.at() + "."
        );
    }
}

// JsonBetter JsonReader::parse_array( InputStream& input_stream, JsonBetter& doc ) const
JsonBetter JsonReader::parse_array( InputStream& input_stream ) const
{
    JsonBetter doc = JsonBetter::array();
    auto& it = input_stream;

    // Initial check whether this actually is an array.
    read_char_or_throw( it, '[' );

    while( it ) {
        // Get the element. If the doc is not an array, the push back throws.
        skip_while( it, isspace );
        // doc.push_back( parse( it ));
        doc.emplace_back( parse( it ));

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

// JsonBetter JsonReader::parse_object( InputStream& input_stream, JsonBetter& doc ) const
JsonBetter JsonReader::parse_object( InputStream& input_stream ) const
{
    JsonBetter doc = JsonBetter::object();
    auto& it = input_stream;

    // Initial check whether this actually is an object.
    read_char_or_throw( it, '{' );

    while( it ) {
        // Get the key.
        skip_while( it, isspace );
        affirm_char_or_throw( it, '"' );
        auto key = parse_quoted_string( it );

        // Find the colon and skip it.
        skip_while( it, isspace );
        read_char_or_throw( it, ':' );

        // Get the value.
        skip_while( it, isspace );
        // auto value = parse( it );

        // Insert into object (if the doc is not an object, this throws).
        // doc[ key ] = value;
        doc[ key ] = parse( it );

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

JsonBetter JsonReader::parse_number( InputStream& input_stream ) const
{
    auto& it = input_stream;
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

    // Integer Part
    JsonBetter::NumberUnsignedType ix = 0;
    while( it && isdigit( *it )) {
        int y = *it - '0';
        ix *= 10;
        ix += y;
        ++it;
    }

    // If not float
    if( !it || !( *it == '.' || *it == ',' || tolower(*it) == 'e' ) ) {
        if( is_neg ) {
            return JsonBetter::number_signed( -ix );
        } else {
            return JsonBetter::number_unsigned( ix );
        }
    }

    // Decimal part
    JsonBetter::NumberFloatType dx = ix;
    if( it && ( *it == '.' || *it == ',' )) {
        ++it;

        JsonBetter::NumberFloatType pos = 1.0;
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
            JsonBetter::NumberFloatType base;
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

    return JsonBetter::number_float( dx );
}

} // namespace utils
} // namespace genesis

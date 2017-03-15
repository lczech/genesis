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

#include "genesis/utils/io/parser.hpp"
#include "genesis/utils/io/scanner.hpp"
#include "genesis/utils/text/char.hpp"
#include "genesis/utils/text/string.hpp"

#include <assert.h>
#include <cctype>
#include <limits>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     General Number String
// =================================================================================================

std::string parse_number_string(
    utils::InputStream& source
) {
    // Parse the format [+-][123][.456][eE[+-]789]
    std::string result;

    // Need to keep track whether we found a number.
    bool found_digits = false;

    // Sign.
    if( source && char_is_sign( *source )) {
        result += *source;
        ++source;
    }

    // Integer part. Read while char is digit.
    while( source && char_is_digit( *source )) {
        result += *source;
        ++source;
        found_digits = true;
    }

    // Decimal dot?
    if( source && *source == '.' ) {
        result += '.';
        ++source;
    }

    // Decimal part. Read while char is digit.
    while( source && char_is_digit( *source )) {
        result += *source;
        ++source;
        found_digits = true;
    }

    // If there was no match so far, stop here.
    // Otherwise, a string starting with "E" will be read as a number...
    if( ! found_digits ) {
        return result;
    }

    // Is there an exponent? If not, we are done.
    if( source && char_match_ci( *source, 'e' ) ) {
        result += *source;
        ++source;
    } else {
        return result;
    }

    // Sign.
    if( source && char_is_sign( *source )) {
        result += *source;
        ++source;
    }

    // Exponent. Read while char is digit.
    while( source && char_is_digit( *source )) {
        result += *source;
        ++source;
    }

    return result;
}

// =================================================================================================
//     String
// =================================================================================================

std::string parse_quoted_string(
    utils::InputStream& source,
    bool use_escapes,
    bool use_twin_quotes,
    bool include_qmarks
) {
    // Prepare the return value.
    std::string value = "";

    // Nothing to do.
    if( !source ) {
        return value;
    }

    // Read the introductory quotation mark. We will read until it appears again.
    char qmark = *source;
    ++source;

    // Include the quotation mark if needed.
    if( include_qmarks ) {
        value += qmark;
    }

    bool found_closing_qmark = false;
    while( source ) {

        // Treat quotation marks.
        if( *source == qmark ) {
            ++source;

            // This is the end if we are not looking for double qmarks.
            if( ! use_twin_quotes ) {
                found_closing_qmark = true;
                break;
            }

            // If we are here, this is potentially a double qmark.
            // If so, it belongs to the result string. If not, this is the end.
            if( source && *source == qmark ) {
                value += qmark;
            } else {
                found_closing_qmark = true;
                break;
            }

        // Treat escape sequences.
        } else if( *source == '\\' && use_escapes ) {

            // Skip the backslash.
            ++source;

            // We found an escaping backslash. This cannot be the end of the stream.
            if( !source ) {
                throw std::runtime_error(
                    "Unexpected end of " + source.source_name() + " at " + source.at()
                    + ". Expecting escape sequence."
                );
            }

            // Turn the char after the backslash into its correct de-escaped char.
            value += deescape( *source );

        // Treat normal (non-escape) chars.
        } else {
            value += *source;
        }

        // Next char.
        ++source;
    }

    // We need to find the closing qmark, otherwise it's an error.
    // This case only occurs if the stream ends before the qmark is found, so assert this.
    // (This is not true the other way round: the stream can have reached its end right after
    // the closing qmark!)
    if( ! found_closing_qmark ) {
        assert( ! source );
        throw std::runtime_error(
            "Unexpected end of " + source.source_name() + " at " + source.at()
            + ". Expected closing quotation mark."
        );
    }

    // Finish the return value.
    if( include_qmarks ) {
        value += qmark;
    }
    return value;
}

} // namespace utils
} // namespace genesis

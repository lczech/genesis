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

#include "utils/io/parser.hpp"
#include "utils/text/string.hpp"

#include <assert.h>
#include <cctype>
#include <limits>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     String
// =================================================================================================

/**
 * @brief Read a string in quotation marks from a stream and return it.
 *
 * The first char of the stream is considered to be the opening quotation mark. Everything up until
 * the closing quotation mark (the same char again, whatever it is) is then read. The stream is
 * then pointing to the char right after the closing quotation mark.
 *
 * If the string ends prematurely, i.e., without the closing quotation mark, or right after a
 * backslash if `use_escapes` is used, the function throws an `std::runtime_error`.
 *
 * @param source Stream to read from.
 *
 * @param use_escapes If this optional parameter is set to `true`, chars preceeded by
 *        a backslash `\` are considered to be "escaped". If the char following the backslash is any
 *        of `r`, `n` or `t`, it is turned into its respective white space equivalent, while all
 *        other chars are copied verbatim to the return string. Thus, by escaping it, a the string
 *        can also include the quotation mark itself. Default of this parameter is `true`.
 *
 * @param use_twin_quotes If this optional parameter is set to `true`, the quotation mark itself
 *        can be escaped using two consecutive quotation marks. This works in addition to escaping
 *        it with a backslash (when `use_escapes` is used).
 *
 * @param include_qmarks The optional parameter `include_qmarks` demtermines whether the quotation
 *        marks are included in the output or not. Default is `false`.
 */
std::string parse_quoted_string(
    utils::InputStream& source,
    bool use_escapes,
    bool use_twin_quotes,
    bool include_qmarks
) {
    if( !source ) {
        return "";
    }

    // Read the introductory quotation mark. We will read until it appears again.
    char qmark = *source;
    ++source;

    // Prepare the return value.
    std::string value;
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

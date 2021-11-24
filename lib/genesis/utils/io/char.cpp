/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief Implementation of string utility functions.
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/io/char.hpp"

#include <array>
#include <cassert>
#include <cctype>
#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Chars Functions
// =================================================================================================

/**
 * @brief List of all ASCII symbols.
 *
 * Actually, we only need that for the non-printable characters. All printable ones are prepresented
 * by their ASCII code directly. But as there is the "DEL" symbol at the end, using this list is
 * easier than introducing a special case for that.
 */
static const std::array<std::string, 128> ascii_symbols_ = {{
    "NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL", "BS", "HT", "LF", "VT", "FF", "CR", "SO", "SI",
    "DLE", "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB", "CAN", "EM", "SUB", "ESC", "FS", "GS", "RS", "US",
    " ", "!", "\"", "#", "$", "%", "&", "'", "(", ")", "*", "+", ",", "-", ".", "/",
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "<", "=", ">", "?",
    "@", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "[", "\\", "]", "^", "_",
    "`", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o",
    "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "{", "|", "}", "~", "DEL"
}};

/**
 * @brief List of all ASCII names.
 *
 * We use this for extra clear user output, in order to make sure that the symbols can be
 * understood when a parser finds an unexpected char.
 *
 * List from "ASCII / ISO 8859-1 (Latin-1) Table with HTML Entity Names",
 * see https://cs.stanford.edu/people/miles/iso8859.html
 */
static const std::array<std::string, 128> ascii_names_ = {{
    "control: null", "control: start of heading", "control: start of text", "control: end of text",
    "control: end of transmission", "control: enquiry", "control: acknowledge", "control: bell",
    "control: backspace", "control: horizontal tab", "control: NL line feed, new line",
    "control: vertical tab", "control: NP form feed, new page", "control: carriage return",
    "control: shift out", "control: shift in", "control: data link escape",
    "control: device control 1", "control: device control 2", "control: device control 3",
    "control: device control 4", "control: negative acknowledge", "control: synchronous idle",
    "control: end of trans. block", "control: cancel", "control: end of medium",
    "control: substitute", "control: escape", "control: file separator",
    "control: group separator", "control: record separator", "control: unit separator",
    "symbol: space", "symbol: exclamation mark", "symbol: double quotation mark",
    "symbol: number sign, pound", "symbol: dollar sign", "symbol: percent sign",
    "symbol: ampersand", "symbol: apostrophe, single quote mark", "symbol: left parenthesis",
    "symbol: right parenthesis", "symbol: asterisk", "symbol: plus sign", "symbol: comma",
    "symbol: minus sign, hyphen", "symbol: period, decimal point, full stop",
    "symbol: slash, virgule, solidus",
    "digit: 0", "digit: 1", "digit: 2", "digit: 3", "digit: 4",
    "digit: 5", "digit: 6", "digit: 7", "digit: 8", "digit: 9",
    "symbol: colon", "symbol: semicolon", "symbol: less-than sign", "symbol: equal sign",
    "symbol: greater-than sign", "symbol: question mark", "symbol: commercial at sign",
    "letter: capital A", "letter: capital B", "letter: capital C", "letter: capital D",
    "letter: capital E", "letter: capital F", "letter: capital G", "letter: capital H",
    "letter: capital I", "letter: capital J", "letter: capital K", "letter: capital L",
    "letter: capital M", "letter: capital N", "letter: capital O", "letter: capital P",
    "letter: capital Q", "letter: capital R", "letter: capital S", "letter: capital T",
    "letter: capital U", "letter: capital V", "letter: capital W", "letter: capital X",
    "letter: capital Y", "letter: capital Z",
    "symbol: left square bracket", "symbol: backslash, reverse solidus",
    "symbol: right square bracket", "symbol: spacing circumflex accent, caret",
    "symbol: spacing underscore, low line, horizontal bar",
    "symbol: spacing grave accent, back apostrophe",
    "letter: small a", "letter: small b", "letter: small c", "letter: small d", "letter: small e",
    "letter: small f", "letter: small g", "letter: small h", "letter: small i", "letter: small j",
    "letter: small k", "letter: small l", "letter: small m", "letter: small n", "letter: small o",
    "letter: small p", "letter: small q", "letter: small r", "letter: small s", "letter: small t",
    "letter: small u", "letter: small v", "letter: small w", "letter: small x", "letter: small y",
    "letter: small z",
    "symbol: left brace, left curly bracket", "symbol: vertical bar",
    "symbol: right brace, right curly bracket", "symbol: tilde accent", "control: delete"
}};

std::string char_to_hex( char c, bool full )
{
    // By conversion to unsigned char, we transform potential negative numbers to their mod 256
    // equivalent. This helps to mitigate the irritation that is caused by allowing char to be
    // signed or unsigned in the standard.
    return char_to_hex( static_cast<unsigned char>( c ), full );

    // The following check is obsolte now, as we accept the whole byte range.

    // Check that we are in the valid ascii range. If not, outputting an ascii char does not make
    // sense anyway. We cast to int here, because char can either be signed or unsigned, and hence
    // different compilers will warn that one of the comparisons is always false. But we need both,
    // because of compilers differ...
    // if( static_cast<int>( c ) < 0 || static_cast<int>( c ) > 127 ) {
    //     throw std::runtime_error( "Invalid ASCII char " + std::to_string( static_cast<int>( c )));
    // }
}

std::string char_to_hex( unsigned char c, bool full )
{
    std::stringstream ss;
    if( full ) {
        if( c < 128 ) {
            if( std::isprint(c) ) {
                assert( std::string(1, c) == ascii_symbols_[c] );
                ss << "'" << std::string( 1, c ) << "'";
            } else {
                ss << ascii_symbols_[c];
            }
        } else {
            ss << "non-ASCII char";
        }

        ss << " (0x";
    }
    ss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>( c );
    if( full ) {
        if( c < 128 ) {
            ss << "; " << ascii_names_[c] << ")";
        } else {
            ss << ")";
        }
    }
    return ss.str();
}

} // namespace utils
} // namespace genesis

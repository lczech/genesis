/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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
        ss << ")";
    }
    return ss.str();
}

} // namespace utils
} // namespace genesis

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
 * @brief Testing string functions.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/io/char.hpp"

#include <cctype>

using namespace genesis::utils;

TEST( Char, AsciiFunctions )
{
    // Compare our custom ascii char functions to the build-in versions
    // See http://www.cplusplus.com/reference/cctype/
    for( int c = 0; c < 128; ++c ) {
        EXPECT_TRUE( is_ascii(c) );

        EXPECT_EQ( static_cast<bool>( ::iscntrl( c )), is_cntrl( c ));
        EXPECT_EQ( static_cast<bool>( ::isblank( c )), is_blank( c ));
        EXPECT_EQ( static_cast<bool>( ::isspace( c )), is_space( c ));
        EXPECT_EQ( static_cast<bool>( ::isupper( c )), is_upper( c ));
        EXPECT_EQ( static_cast<bool>( ::islower( c )), is_lower( c ));
        EXPECT_EQ( static_cast<bool>( ::isalpha( c )), is_alpha( c ));
        EXPECT_EQ( static_cast<bool>( ::isdigit( c )), is_digit( c ));
        EXPECT_EQ( static_cast<bool>( ::isxdigit( c )), is_xdigit( c ));
        EXPECT_EQ( static_cast<bool>( ::isalnum( c )), is_alnum( c ));
        EXPECT_EQ( static_cast<bool>( ::ispunct( c )), is_punct( c ));
        EXPECT_EQ( static_cast<bool>( ::isgraph( c )), is_graph( c ));
        EXPECT_EQ( static_cast<bool>( ::isprint( c )), is_print( c ));

        EXPECT_EQ( ::tolower( c ), static_cast<int>( to_lower( c )));
        EXPECT_EQ( ::toupper( c ), static_cast<int>( to_upper( c )));
    }
}

TEST( Char, CharToHex )
{
    EXPECT_EQ( "LF (0x0A; control: NL line feed, new line)", char_to_hex( '\n', true ));
    EXPECT_EQ( "'N' (0x4E; letter: capital N)", char_to_hex( 'N', true ));
    EXPECT_EQ( "non-ASCII char (0xF7)", char_to_hex( static_cast<char>( 0xF7 ), true ));

    EXPECT_EQ(
        "LF (0x0A; control: NL line feed, new line)",
        char_to_hex( static_cast<unsigned char>( 0x0A ), true )
    );
    EXPECT_EQ(
        "'N' (0x4E; letter: capital N)",
        char_to_hex( static_cast<unsigned char>( 0x4E ), true )
    );
    EXPECT_EQ( "non-ASCII char (0xF7)", char_to_hex( static_cast<unsigned char>( 0xF7 ), true ));
}

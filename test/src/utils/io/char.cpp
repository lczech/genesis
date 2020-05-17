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
 * @brief Testing string functions.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/io/char.hpp"

using namespace genesis::utils;

TEST(Char, Ascii)
{
    EXPECT_TRUE( is_ascii('a') );
    EXPECT_TRUE( is_lower('a') );
    EXPECT_FALSE( is_upper('a') );

    EXPECT_TRUE( is_punct('.') );
    EXPECT_FALSE( is_punct('x') );

}

TEST( Text, CharToHex )
{
    EXPECT_EQ( "LF (0x0A)", char_to_hex( '\n', true ));
    EXPECT_EQ( "'N' (0x4E)", char_to_hex( 'N', true ));
    EXPECT_EQ( "non-ASCII char (0xF7)", char_to_hex( static_cast<char>( 0xF7 ), true ));

    EXPECT_EQ( "LF (0x0A)", char_to_hex( static_cast<unsigned char>( 0x0A ), true ));
    EXPECT_EQ( "'N' (0x4E)", char_to_hex( static_cast<unsigned char>( 0x4E ), true ));
    EXPECT_EQ( "non-ASCII char (0xF7)", char_to_hex( static_cast<unsigned char>( 0xF7 ), true ));
}

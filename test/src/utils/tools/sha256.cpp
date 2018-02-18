/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/tools/sha256.hpp"

#include <string>
#include <iostream>

using namespace genesis;
using namespace utils;

TEST( Utils, SHA256 )
{
    SHA256 checksum;

    // abc
    checksum.update("abc");
    EXPECT_EQ( "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad", checksum.final_hex() );

    // abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq
    checksum.update("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");
    EXPECT_EQ( "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1", checksum.final_hex() );

    // A million repetitions of 'a'.
    for (int i = 0; i < 1000000/200; ++i)
    {
        checksum.update(
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        );
    }
    EXPECT_EQ( "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0", checksum.final_hex() );

    // No string
    EXPECT_EQ( "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", checksum.final_hex());

    // Empty string
    checksum.update("");
    EXPECT_EQ( "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", checksum.final_hex());

    // abcde
    checksum.update("abcde");
    EXPECT_EQ( "36bbe50ed96841d10443bcb670d6554f0a34b761be67ec9c4a8ad2c0c44ca42c", checksum.final_hex());

    // Two concurrent checksum calculations
    SHA256 checksum1, checksum2;
    checksum1.update("abc");
    EXPECT_EQ( "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad", checksum1.final_hex());
    EXPECT_EQ( "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", checksum2.final_hex());
}

TEST( Utils, SHA256Files )
{
    // Skip test if no data directory availabe.
    NEEDS_TEST_DATA;

    std::string empty_file = environment->data_dir + "utils/hash/empty.txt";
    EXPECT_EQ( "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", SHA256::from_file_hex( empty_file ));

    std::string abc_file = environment->data_dir + "utils/hash/abc.txt";
    EXPECT_EQ( "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad", SHA256::from_file_hex( abc_file ));

}

TEST( Utils, SHA256Convert )
{
    // empty string
    auto const empty_d = SHA256::from_string_digest( "" );
    auto const empty_h = SHA256::from_string_hex( "" );
    EXPECT_EQ( "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", empty_h );
    EXPECT_EQ( empty_d, SHA256::hex_to_digest( empty_h ));
    EXPECT_EQ( empty_h, SHA256::digest_to_hex( empty_d ));

    // abc
    auto const abc_d = SHA256::from_string_digest( "abc" );
    auto const abc_h = SHA256::from_string_hex( "abc" );
    EXPECT_EQ( "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad", abc_h );
    EXPECT_EQ( abc_d, SHA256::hex_to_digest( abc_h ));
    EXPECT_EQ( abc_h, SHA256::digest_to_hex( abc_d ));
}

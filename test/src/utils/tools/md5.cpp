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

#include "genesis/utils/tools/md5.hpp"

#include <string>
#include <iostream>

using namespace genesis;
using namespace utils;

TEST( Utils, MD5 )
{
    MD5 checksum;

    // abc
    checksum.update("abc");
    EXPECT_EQ( "900150983cd24fb0d6963f7d28e17f72", checksum.final_hex() );

    // abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq
    checksum.update("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");
    EXPECT_EQ( "8215ef0796a20bcaaae116d3876c664a", checksum.final_hex() );

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
    EXPECT_EQ( "7707d6ae4e027c70eea2a935c2296f21", checksum.final_hex() );

    // No string
    EXPECT_EQ( "d41d8cd98f00b204e9800998ecf8427e", checksum.final_hex());

    // Empty string
    checksum.update("");
    EXPECT_EQ( "d41d8cd98f00b204e9800998ecf8427e", checksum.final_hex());

    // Two concurrent checksum calculations
    MD5 checksum1, checksum2;
    checksum1.update("abc");
    EXPECT_EQ( "900150983cd24fb0d6963f7d28e17f72", checksum1.final_hex());
    EXPECT_EQ( "d41d8cd98f00b204e9800998ecf8427e", checksum2.final_hex());
}

TEST( Utils, MD5Files )
{
    // Skip test if no data directory availabe.
    NEEDS_TEST_DATA;

    std::string empty_file = environment->data_dir + "utils/hash/empty.txt";
    EXPECT_EQ( "d41d8cd98f00b204e9800998ecf8427e", MD5::from_file_hex( empty_file ));

    std::string abc_file = environment->data_dir + "utils/hash/abc.txt";
    EXPECT_EQ( "900150983cd24fb0d6963f7d28e17f72", MD5::from_file_hex( abc_file ));

}

TEST( Utils, MD5Convert )
{
    // empty string
    auto const empty_d = MD5::from_string_digest( "" );
    auto const empty_h = MD5::from_string_hex( "" );
    EXPECT_EQ( "d41d8cd98f00b204e9800998ecf8427e", empty_h );
    EXPECT_EQ( empty_d, MD5::hex_to_digest( empty_h ));
    EXPECT_EQ( empty_h, MD5::digest_to_hex( empty_d ));

    // abc
    auto const abc_d = MD5::from_string_digest( "abc" );
    auto const abc_h = MD5::from_string_hex( "abc" );
    EXPECT_EQ( "900150983cd24fb0d6963f7d28e17f72", abc_h );
    EXPECT_EQ( abc_d, MD5::hex_to_digest( abc_h ));
    EXPECT_EQ( abc_h, MD5::digest_to_hex( abc_d ));
}

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
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/math/sha1.hpp"

#include <string>
#include <iostream>

using namespace genesis;
using namespace utils;

TEST( Math, SHA1 )
{
    SHA1 checksum;

    // abc
    checksum.update("abc");
    EXPECT_EQ( "a9993e364706816aba3e25717850c26c9cd0d89d", checksum.final_hex() );

    // abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq
    checksum.update("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");
    EXPECT_EQ( "84983e441c3bd26ebaae4aa1f95129e5e54670f1", checksum.final_hex() );

    // A million repetitions of 'a'.
    for (int i = 0; i < 1000000/200; ++i)
    {
        checksum.update("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                       );
    }
    EXPECT_EQ( "34aa973cd4c4daa4f61eeb2bdbad27316534016f", checksum.final_hex() );

    // No string
    EXPECT_EQ( "da39a3ee5e6b4b0d3255bfef95601890afd80709", checksum.final_hex());

    // Empty string
    checksum.update("");
    EXPECT_EQ( "da39a3ee5e6b4b0d3255bfef95601890afd80709", checksum.final_hex());

    // abcde
    checksum.update("abcde");
    EXPECT_EQ( "03de6c570bfe24bfc328ccd7ca46b76eadaf4334", checksum.final_hex());

    // Two concurrent checksum calculations
    SHA1 checksum1, checksum2;
    checksum1.update("abc");
    EXPECT_EQ( "a9993e364706816aba3e25717850c26c9cd0d89d", checksum1.final_hex());
    EXPECT_EQ( "da39a3ee5e6b4b0d3255bfef95601890afd80709", checksum2.final_hex());
}

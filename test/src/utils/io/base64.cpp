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

#include "genesis/utils/io/base64.hpp"

#include <cctype>
#include <ctime>
#include <string>
#include <unistd.h>

using namespace genesis::utils;

// From https://en.wikipedia.org/wiki/Base64 - quote from Thomas Hobbes's Leviathan:

std::string base64_input_
{
    "Man is distinguished, not only by his reason, but by this singular passion from other animals, "
    "which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable "
    "generation of knowledge, exceeds the short vehemence of any carnal pleasure."
};
std::string base64_reference_
{
    "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlz\n"
    "IHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2Yg\n"
    "dGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGlu\n"
    "dWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRo\n"
    "ZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4="
};

TEST( Base64, EncodeDecode )
{
    // Test all our functions.

    auto const from_string = base64_encode( base64_input_ );
    EXPECT_EQ( base64_reference_, from_string );

    auto const to_uint8  = base64_decode_uint8(  from_string );
    auto const to_string = base64_decode_string( from_string );
    EXPECT_EQ( base64_input_, to_string );

    auto const from_uint8   = base64_encode( to_uint8 );
    auto const from_string2 = base64_encode( to_string );
    EXPECT_EQ( base64_reference_, from_uint8 );
    EXPECT_EQ( base64_reference_, from_string2 );
}

std::string base64_generate_random_string_( size_t len )
{
    static const char alphanum[] =
        "`~!@#$%^&*()-_=+[{]}\\|;:'\",<.>/?"
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
    ;

    std::string tmp_s;
    for( size_t i = 0; i < len; ++i ) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return tmp_s;
}

TEST( Base64, RandomStrings )
{
    // Random init
    srand( (unsigned) time(NULL) * getpid());


    // Some random trials
    for( size_t i = 0; i < 1000; ++i ) {
        std::string const truth = base64_generate_random_string_( i );
        auto const converted = base64_encode( truth );
        auto const back = base64_decode_string( converted );
        EXPECT_EQ( truth, back );
    }
}

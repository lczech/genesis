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
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/io/char.hpp"
#include "genesis/utils/text/string.hpp"

#include <cstdlib>
#include <string>

using namespace genesis::utils;

TEST( Text, HeadTail )
{
    std::string large = "hello\nworld.\nthis\nis\na\ntest\nwith\nsome\ntext.";

    // Check length.
    auto vec = split( large, "\n" );
    EXPECT_EQ( 9, vec.size() );

    // Get first and last 3 words.
    EXPECT_EQ( "hello\nworld.\nthis", head( large, 3 ) );
    EXPECT_EQ( "with\nsome\ntext.",   tail( large, 3 ) );

    // Get all words.
    EXPECT_EQ( large, head( large, 10 ) );
    EXPECT_EQ( large, tail( large, 10 ) );
}

TEST( Text, Indent )
{
    std::string lines_orig_1 = "this is\na test with normal\nlines  ";
    std::string lines_orig_2 = "this is  \n   a test with weird\nlines \n  ";

    std::string lines_idnt_1 = "    this is\n    a test with normal\n    lines";
    std::string lines_idnt_2 = "    this is  \n       a test with weird\n    lines \n";

    EXPECT_EQ( lines_idnt_1, indent( lines_orig_1 ));
    EXPECT_EQ( lines_idnt_2, indent( lines_orig_2 ));
}

TEST(Text, SplitDelim)
{
    auto no_delim = split("one:two:three:four", ",");
    EXPECT_EQ(1, no_delim.size());

    auto simple = split("one:two:three:four", ":");
    EXPECT_EQ(4, simple.size());

    auto mulit_delim = split("one:two three-four", ": -");
    EXPECT_EQ(4, mulit_delim.size());

    auto with_empty = split("::one:two:three::four:", ":");
    EXPECT_EQ(4, with_empty.size());

    auto non_empty = split("::one:two:three::four:", ":", false);
    EXPECT_EQ(8, non_empty.size());
}

TEST(Text, SplitPredicate)
{
    auto no_delim = split( "one:two:three:four", isblank );
    EXPECT_EQ(1, no_delim.size());

    auto simple = split( "one two\tthree four", isblank );
    EXPECT_EQ(4, simple.size());

    auto with_empty = split( "one   two\t three\t four", isblank );
    EXPECT_EQ(4, with_empty.size());

    auto non_empty = split( "one   two\t three\t four", isblank, false );
    EXPECT_EQ(8, non_empty.size());
}

TEST(Text, SplitAt)
{
    auto no_delim = split_at( "one:two:three:four", " " );
    EXPECT_EQ(1, no_delim.size());

    auto simple = split_at( "one:two:three:four", ":" );
    EXPECT_EQ(4, simple.size());

    auto with_empty = split_at("::one:two:three::four:", ":");
    EXPECT_EQ(4, with_empty.size());

    auto non_empty = split_at("::one:two:three::four:", ":", false);
    EXPECT_EQ(8, non_empty.size());

    auto no_delim2 = split_at( "one:two:three:four", "foo" );
    EXPECT_EQ(1, no_delim2.size());

    auto simple2 = split_at( "onefootwofoothreefoofour", "foo" );
    EXPECT_EQ(4, simple2.size());

    auto with_empty2 = split_at("foofooonefootwofoothreefoofoofourfoo", "foo");
    EXPECT_EQ(4, with_empty2.size());

    auto non_empty2 = split_at("foofooonefootwofoothreefoofoofourfoo", "foo", false);
    EXPECT_EQ(8, non_empty2.size());
}

TEST( Text, SplitRangeList )
{
    auto const empty = split_range_list( " " );
    EXPECT_EQ( std::vector<size_t>{}, empty );

    auto const single = split_range_list( "5" );
    EXPECT_EQ( (std::vector<size_t>{ 5 }), single );

    auto const list = split_range_list( " 5, 3, 6,  7 " );
    EXPECT_EQ( (std::vector<size_t>{ 3, 5, 6, 7 }), list );

    auto const range = split_range_list( " 5 - 8 " );
    EXPECT_EQ( (std::vector<size_t>{ 5, 6, 7, 8 }), range );

    auto const combined = split_range_list( "1,  5 - 8, 10-11 " );
    EXPECT_EQ( (std::vector<size_t>{ 1, 5, 6, 7, 8, 10, 11 }), combined );

    EXPECT_THROW( split_range_list( "1,  5 - 8, 10-a " ), std::runtime_error );
    EXPECT_THROW( split_range_list( "x" ), std::runtime_error );
}

TEST( Text, ToString )
{
    double pi    = 3.14159267535;
    double zeros = 42.4200000000;

    EXPECT_EQ( "3.14159", to_string_nice( pi ) );
    EXPECT_EQ( "42.42",   to_string_nice( zeros ) );

    EXPECT_EQ( "3",       to_string_precise( pi, 0 ) );
    EXPECT_EQ( "3.1",     to_string_precise( pi, 1 ) );
    EXPECT_EQ( "3.1416",  to_string_precise( pi, 4 ) );

    EXPECT_EQ( "42",      to_string_precise( zeros, 0 ) );
    EXPECT_EQ( "42.4",    to_string_precise( zeros, 1 ) );
    EXPECT_EQ( "42.4200", to_string_precise( zeros, 4 ) );

    EXPECT_EQ( "3",       to_string_rounded( pi, 0 ) );
    EXPECT_EQ( "3.1",     to_string_rounded( pi, 1 ) );
    EXPECT_EQ( "3.1416",  to_string_rounded( pi, 4 ) );

    EXPECT_EQ( "42",      to_string_rounded( zeros, 0 ) );
    EXPECT_EQ( "42.4",    to_string_rounded( zeros, 1 ) );
    EXPECT_EQ( "42.42",   to_string_rounded( zeros, 4 ) );
}

std::string test_get_random_string( size_t len )
{
    std::string str;
    for( size_t i = 0; i < len; ++i ) {
        // Valid ASCII range: 32 - 126, inclusive. That's 94 valid chars.
        str += static_cast<char>( 32 + rand() % 95 );
    }
    return str;
}

TEST( Text, ToLower )
{
    // Need strings of sufficient length, to trigger the AVX2 part.
    std::string const all = "\n\tABCDEFGHIJKLMNOPQRSTUVWXYZ_1234567890_"
    "!@#$%^&*()_+=[]{};:'\",.<>?\\|ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz_"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n\tABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string const aim = "\n\tabcdefghijklmnopqrstuvwxyz_1234567890_"
    "!@#$%^&*()_+=[]{};:'\",.<>?\\|abcdefghijklmnopqrstuvwxyz_abcdefghijklmnopqrstuvwxyz_"
    "abcdefghijklmnopqrstuvwxyz\n\tabcdefghijklmnopqrstuvwxyz";

    EXPECT_EQ( aim, to_lower_ascii( all ));

    // Run some random tests as well, with increasing length, so that all parts
    // of the functions, including the AVX2 part, get triggered sufficienty.
    for( size_t i = 0; i < 100; ++i ) {
        auto text = test_get_random_string( i );
        auto test = to_lower_ascii( text );

        // Convert manually, using the char based function.
        for( auto& c : text ){
            c = to_lower(c);
        }

        EXPECT_EQ( text, test );
    }
}

TEST( Text, ToUpper )
{
    // Need strings of sufficient length, to trigger the AVX2 part.
    std::string const all = "\n\tabcdefghijklmnopqrstuvwxyz_1234567890_"
    "!@#$%^&*()_+=[]{};:'\",.<>?\\|abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ_"
    "abcdefghijklmnopqrstuvwxyz\n\tabcdefghijklmnopqrstuvwxyz";
    std::string const aim = "\n\tABCDEFGHIJKLMNOPQRSTUVWXYZ_1234567890_"
    "!@#$%^&*()_+=[]{};:'\",.<>?\\|ABCDEFGHIJKLMNOPQRSTUVWXYZ_ABCDEFGHIJKLMNOPQRSTUVWXYZ_"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n\tABCDEFGHIJKLMNOPQRSTUVWXYZ";

    EXPECT_EQ( aim, to_upper_ascii( all ));

    // Run some random tests as well, with increasing length, so that all parts
    // of the functions, including the AVX2 part, get triggered sufficienty.
    for( size_t i = 0; i < 100; ++i ) {
        auto text = test_get_random_string( i );
        auto test = to_upper_ascii( text );

        // Convert manually, using the char based function.
        for( auto& c : text ){
            c = to_upper(c);
        }

        EXPECT_EQ( text, test );
    }
}

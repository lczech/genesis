/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/text/light_string.hpp"

#include <sstream>
#include <stdexcept>
#include <string>

using namespace genesis::utils;

TEST( LightString, Construction )
{
    // Test default constructor: creates an empty string.
    {
        LightString ls;
        EXPECT_STREQ(ls.c_str(), "");
        EXPECT_EQ(ls.size(), 0u);
    }

    // Test construction from a C-style null-terminated string.
    {
        LightString ls("Hello, world!");
        EXPECT_STREQ(ls.c_str(), "Hello, world!");
        EXPECT_EQ(ls.size(), 13u);
    }

    // Test construction from a std::string.
    {
        std::string str = "Test String";
        LightString ls(str);
        EXPECT_STREQ(ls.c_str(), "Test String");
        EXPECT_EQ(ls.size(), str.size());
    }

    // Test construction from a non-const char*.
    {
        char arr[] = "Mutable";
        LightString ls(arr);
        EXPECT_STREQ(ls.c_str(), "Mutable");
        EXPECT_EQ(ls.size(), 7u);
    }

    // Test copy constructor.
    {
        LightString ls1("Copy me");
        LightString ls2(ls1);
        EXPECT_STREQ(ls2.c_str(), "Copy me");
    }

    // Test move constructor.
    {
        LightString ls1("Move me");
        LightString ls2(std::move(ls1));
        EXPECT_STREQ(ls2.c_str(), "Move me");
        // Note: ls1 is left in a valid but unspecified state.
    }

    // Test copy assignment operator.
    {
        LightString ls1("Original");
        LightString ls2;
        ls2 = ls1;
        EXPECT_STREQ(ls2.c_str(), "Original");
    }

    // Test move assignment operator.
    {
        LightString ls1("Move assign");
        LightString ls2;
        ls2 = std::move(ls1);
        EXPECT_STREQ(ls2.c_str(), "Move assign");
    }
}

TEST( LightString, Misc )
{
    // Test element access via operator[] and at().
    {
        LightString ls("Hello");
        EXPECT_EQ(ls[1], 'e'); // Verify element at index 1.
        ls[0] = 'h';
        EXPECT_STREQ(ls.c_str(), "hello");

        EXPECT_EQ(ls.at(1), 'e');
        EXPECT_THROW(ls.at(10), std::out_of_range);
    }

    // Test data() function.
    {
        LightString ls("DataTest");
        EXPECT_STREQ(ls.data(), "DataTest");
    }

    // Test empty() function.
    {
        LightString ls1;
        LightString ls2("");
        LightString ls3("NotEmpty");

        EXPECT_TRUE(ls1.empty());
        EXPECT_TRUE(ls2.empty());
        EXPECT_FALSE(ls3.empty());
    }

    // Test stream output operator<<.
    {
        LightString ls("Stream");
        std::stringstream ss;
        ss << ls;
        EXPECT_STREQ(ss.str().c_str(), "Stream");
    }

    // Test swap() member function.
    {
        LightString ls1("One");
        LightString ls2("Two");

        ls1.swap(ls2);
        EXPECT_STREQ(ls1.c_str(), "Two");
        EXPECT_STREQ(ls2.c_str(), "One");
    }
}

TEST( LightString, Operators )
{
    // Test equality and inequality operators with LightString, std::string, and C-strings.
    {
        LightString ls1("Equal");
        LightString ls2("Equal");
        LightString ls3("Not Equal");
        std::string str("Equal");
        const char* cstr = "Equal";

        EXPECT_TRUE(ls1 == ls2);
        EXPECT_FALSE(ls1 == ls3);
        EXPECT_TRUE(ls1 != ls3);
        EXPECT_TRUE(ls1 == str);
        EXPECT_TRUE(str == ls1);
        EXPECT_TRUE(ls1 == cstr);
        EXPECT_TRUE(cstr == ls1);
    }

    // Test relational operators (<, >, <=, >=) with LightString and std::string.
    {
        LightString ls1("Apple");
        LightString ls2("Banana");

        EXPECT_TRUE(ls1 < ls2);
        EXPECT_TRUE(ls2 > ls1);
        EXPECT_TRUE(ls1 <= ls2);
        EXPECT_TRUE(ls2 >= ls1);

        std::string str1("Apple");
        std::string str2("Banana");
        EXPECT_TRUE(ls1 < str2);
        EXPECT_TRUE(str1 < ls2);
    }

    // Test compare() member function.
    {
        LightString ls1("abc");
        LightString ls2("abc");
        LightString ls3("abd");

        EXPECT_EQ(ls1.compare(ls2), 0);
        EXPECT_LT(ls1.compare(ls3), 0);
        EXPECT_GT(ls3.compare(ls1), 0);
    }
}

TEST( LightString, Concatenation )
{
    // Test concatenation operator+ (optimized to avoid extra copies).
    {
        LightString ls1("Hello");
        LightString ls2(", World!");
        LightString ls3 = ls1 + ls2;
        EXPECT_STREQ(ls3.c_str(), "Hello, World!");
        EXPECT_EQ(ls3.size(), 13u);
    }

    // Test concatenation with mixed types returning a std::string.
    {
        LightString ls("Hello");
        std::string str = "World";
        std::string result1 = ls + str;
        EXPECT_EQ(result1, "HelloWorld");

        std::string result2 = std::string("Hi ") + ls;
        EXPECT_EQ(result2, "Hi Hello");

        auto result3 = ls + " Everyone";
        EXPECT_EQ(result3, "Hello Everyone");

        auto result4 = "Greeting: " + ls;
        EXPECT_EQ(result4, "Greeting: Hello");
    }
}

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
 * @brief Testing Serializer class.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include <cstdio>
#include <string>
#include <vector>

#include "genesis/utils/io/serializer.hpp"
#include "genesis/utils/io/deserializer.hpp"

using namespace genesis;
using namespace utils;

struct SerializerTestData
{
    char          m[8];
    char          a;
    signed int    b;
    unsigned long c;
    double        d;
    int           e;
    std::string   s;

    std::vector<int> vi;
    std::vector<std::string> vs;
    std::vector<std::vector<int>> vvi;
};

void init_test_data( SerializerTestData& data )
{
    strncpy(data.m, "bytes\0\0\0", 8);
    data.a = 8;
    data.b = -1;
    data.c = 42;
    data.d = 3.1415;
    data.e = 125;
    data.s = "Hello World!";

    data.vi.push_back( 42 );
    data.vi.push_back( 0 );
    data.vs.push_back( "hello" );
    data.vs.push_back( "world" );
    data.vvi.push_back( data.vi );
    data.vvi.push_back( data.vi );
}

void apply_serializer( Serializer& serial, SerializerTestData& data )
{
    serial.put(data.m, 8);
    serial.put(data.a);
    serial.put(data.b);
    serial.put(data.c);
    serial.put(data.d);
    serial.put(data.e);
    serial.put(data.s);

    serial.put( data.vi );
    serial.put( data.vs );
    serial.put( data.vvi );
}

void apply_deserializer_args( Deserializer& deser, SerializerTestData& data )
{
    deser.get(data.m, 8);
    deser.get(data.a);
    deser.get(data.b);
    deser.get(data.c);
    deser.get(data.d);
    deser.get(data.e);
    deser.get(data.s);

    deser.get( data.vi );
    deser.get( data.vs );
    deser.get( data.vvi );
}

void apply_deserializer_return( Deserializer& deser, SerializerTestData& data )
{
    // No return variant for char array
    deser.get(data.m, 8);

    // All other types have overloads to return the value directly
    data.a = deser.get<decltype(data.a)>();
    data.b = deser.get<decltype(data.b)>();
    data.c = deser.get<decltype(data.c)>();
    data.d = deser.get<decltype(data.d)>();
    data.e = deser.get<decltype(data.e)>();
    data.s = deser.get<decltype(data.s)>();

    data.vi = deser.get<decltype(data.vi)>();
    data.vs = deser.get<decltype(data.vs)>();
    data.vvi = deser.get<decltype(data.vvi)>();
}

void compare_data( SerializerTestData const& data_a, SerializerTestData const& data_b )
{
    for (size_t i = 0; i < 8; i++) {
        EXPECT_EQ (data_a.m[i], data_b.m[i]);
    }
    EXPECT_EQ (data_a.a, data_b.a);
    EXPECT_EQ (data_a.b, data_b.b);
    EXPECT_EQ (data_a.c, data_b.c);
    EXPECT_DOUBLE_EQ (data_a.d, data_b.d);
    EXPECT_EQ (data_a.e, data_b.e);
    EXPECT_EQ (data_a.s, data_b.s);

    EXPECT_EQ (data_a.vi, data_b.vi);
    EXPECT_EQ (data_a.vs, data_b.vs);
    EXPECT_EQ (data_a.vvi, data_b.vvi);
}

TEST( Serializer, ToAndFromStream )
{
    std::ostringstream out;
    Serializer serial( to_stream( out ));

    SerializerTestData input;
    init_test_data(input);
    apply_serializer(serial, input);

    // Test with arguments
    {
        std::istringstream in(out.str());
        Deserializer deser( from_stream( in ));
        SerializerTestData output;
        apply_deserializer_args(deser, output);
        compare_data(input, output);
        EXPECT_TRUE( deser.finished() );
    }

    // Test with return
    {
        std::istringstream in(out.str());
        Deserializer deser( from_stream( in ));
        SerializerTestData output;
        apply_deserializer_return(deser, output);
        compare_data(input, output);
        EXPECT_TRUE( deser.finished() );
    }
}

TEST( Serializer, ToAndFromFile )
{
    // Skip test if no data directory availabe.
    NEEDS_TEST_DATA;

    // Temp file. We first try to remove it as well,
    // in case it accidentally exists from a run before.
    std::string file_name = environment->data_dir + "Serializer.ToAndFromFile.bin";
    std::remove(file_name.c_str());

    // Write serialized data to file.
    Serializer serial( to_file( file_name ));
    SerializerTestData input;
    init_test_data(input);
    apply_serializer(serial, input);
    serial.flush();

    // Prepare to read from file.
    Deserializer deser( from_file( file_name ));
    if (!deser) {
        FAIL() << "Deserializer not ok.";
    }

    // Get deserialized data from file.
    SerializerTestData output;
    apply_deserializer_args(deser, output);
    EXPECT_TRUE( deser.finished() );

    // Test whether both processes worked.
    compare_data(input, output);

    // Make sure file is deleted.
    ASSERT_EQ (0, std::remove(file_name.c_str()));
}

TEST( Deserializer, MoveAssignment )
{
    // Write data to stream.
    std::ostringstream out;
    Serializer serial( to_stream( out ));
    SerializerTestData input;
    init_test_data(input);
    apply_serializer(serial, input);

    // Read data from stream.
    std::istringstream in(out.str());
    Deserializer deser( from_stream( in ));
    SerializerTestData output;
    apply_deserializer_args(deser, output);
    compare_data(input, output);
    EXPECT_TRUE( deser.finished() );

    // Move assign and repeat.
    std::istringstream in2(out.str());
    deser = Deserializer( from_stream( in2 ));
    SerializerTestData output2;
    apply_deserializer_return(deser, output2);
    compare_data(input, output2);
    EXPECT_TRUE( deser.finished() );
}

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

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/formats/json/document.hpp"
#include "genesis/utils/formats/json/iterator.hpp"
#include "genesis/utils/formats/json/reader.hpp"
#include "genesis/utils/formats/json/writer.hpp"
#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/text/string.hpp"

#include <fstream>
#include <string>
#include <vector>

using namespace genesis;
using namespace genesis::utils;

// -------------------------------------------------------------------------
//     Json
// -------------------------------------------------------------------------

/**
 * @brief Get the total number of Json objects that a JsonDocument contains, recursively.
 */
size_t json_size( JsonDocument const& obj )
{
    size_t sum = 1;
    if( obj.is_array() or obj.is_object() ) {
        for( auto const& elem : obj ) {
            sum += json_size( elem );
        }
    }
    return sum;
}

TEST( Json, Constructors )
{
    // Test if the type deduction works in c'tors. Unsinged is tricky, but not needed right now,
    // so we leave it out for the moment.
    auto json_null1    = JsonDocument();
    auto json_null2    = JsonDocument( nullptr );
    auto json_float    = JsonDocument( 3.14 );
    auto json_signed   = JsonDocument( -2 );
    // auto json_unsigned = JsonDocument( 3 );
    auto json_string   = JsonDocument( "bla" );
    auto json_array    = JsonDocument( { true, false, "happy?" } );
    auto json_object   = JsonDocument( { {"key", "value"}, {"more", 100} } );

    EXPECT_EQ( json_null1.type(),    JsonDocument::ValueType::kNull );
    EXPECT_EQ( json_null2.type(),    JsonDocument::ValueType::kNull );
    EXPECT_EQ( json_float.type(),    JsonDocument::ValueType::kNumberFloat );
    EXPECT_EQ( json_signed.type(),   JsonDocument::ValueType::kNumberSigned );
    // EXPECT_EQ( json_unsigned.type(), JsonDocument::ValueType::kNumberUnsigned );
    EXPECT_EQ( json_string.type(),   JsonDocument::ValueType::kString );
    EXPECT_EQ( json_array.type(),    JsonDocument::ValueType::kArray );
    EXPECT_EQ( json_object.type(),   JsonDocument::ValueType::kObject );
}

TEST( Json, ReadWrite )
{
    // Just see if this compiles
    JsonDocument object = {
        {"nothing", nullptr},
        {"zero", 0},
        {"pi", 3.141},
        {"happy", true},
        {"name", "Einstein"},
        {"list", {0, 1, 2}},
        {"answer", {
            {"everything", 42}
        }},
        {"more complex", {
            {"something", "is"},
            {"wicked", true}
        }}
    };

    auto serialized   = JsonWriter().to_string( object );
    auto deserialized = JsonReader().from_string( serialized );

    // use Json internal comparison
    EXPECT_EQ( object, deserialized );
}

TEST( Json, ParsingFails )
{
    NEEDS_TEST_DATA;

    auto reader = JsonReader();
    std::string data_dir = environment->data_dir + "utils/json/";
    auto fail_files = dir_list_files( data_dir, "fail.*.jtest" );
    ASSERT_EQ( 24, fail_files.size() );

    for( auto const& fail_file : fail_files ) {
        // LOG_DBG << fail_file;
        EXPECT_ANY_THROW( reader.from_file( data_dir + fail_file ));
    }
}

TEST( Json, ParsingPasses )
{
    NEEDS_TEST_DATA;

    auto reader = JsonReader();
    std::string data_dir = environment->data_dir + "utils/json/";
    auto pass_files = dir_list_files( data_dir, "pass.*.jtest" );
    ASSERT_EQ( 3, pass_files.size() );

    for( auto const& pass_file : pass_files ) {
        // LOG_DBG << pass_file;
        EXPECT_NO_THROW( reader.from_file( data_dir + pass_file ));
        // reader.from_file( data_dir + pass_file );
    }
}

// TEST( Json, Speed )
// {
//     std::string inputfile = "/home/lucas/Projects/data/for_testing/jplace/sample_0_all_big.jplace";
//     utils::InputStream is( utils::make_unique< utils::FileInputSource >( inputfile ));
//
//     auto reader = JsonReader();
//     auto json = reader.parse_value( is );
//     LOG_DBG << "size  " << json.size();
// }

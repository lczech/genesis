/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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

#include "genesis/utils/containers/dataframe.hpp"
#include "genesis/utils/containers/dataframe/operators.hpp"
#include "genesis/utils/containers/dataframe/reader.hpp"

using namespace genesis::utils;

TEST( Containers, DataframeMatrix )
{
    NEEDS_TEST_DATA;

    auto const infile = environment->data_dir + "utils/matrix/headers.mat";

    auto reader = DataframeReader<double>();
    reader.csv_reader().separator_chars( "\t" );
    auto df = reader.read( from_file( infile ));

    EXPECT_EQ(  3, df.cols() );
    EXPECT_EQ( 10, df.rows() );

    EXPECT_TRUE( df[0].is<double>() );
    EXPECT_FALSE( df[0].is<int>() );

    // Index access
    EXPECT_EQ( 4.5, dynamic_cast<Dataframe::Column<double>&>(df[2])[0]);
    EXPECT_EQ( 1.0, dynamic_cast<Dataframe::Column<double>&>(df[0])[8]);

    EXPECT_EQ( 4.5, df[2].as<double>()[0]);
    EXPECT_EQ( 1.0, df[0].as<double>()[8]);

    EXPECT_EQ( 4.5, df[2].get<double>(0));
    EXPECT_EQ( 1.0, df[0].get<double>(8));

    // Name acces
    EXPECT_EQ( 4.7, df["Third"].as<double>()["gamma"] );
    EXPECT_EQ( 0.0, df["First"].as<double>()["kappa"] );

    EXPECT_TRUE( df.has_row_name( "gamma" ) );
    EXPECT_TRUE( df.has_col_name( "Second" ) );
    EXPECT_FALSE( df.has_row_name( "noop" ) );
    EXPECT_FALSE( df.has_col_name( "noop" ) );
    EXPECT_ANY_THROW( df.has_row_name( "" ) );
    EXPECT_ANY_THROW( df.has_col_name( "" ) );

    // Invalid types and indices.
    EXPECT_ANY_THROW( df[2].as<std::string>()[0] );
    EXPECT_ANY_THROW( df.at(42).as<double>()[0] );
    EXPECT_ANY_THROW( df[0].as<double>().at(13) );
    EXPECT_ANY_THROW( df["omega"].as<double>().at(0) );
    EXPECT_ANY_THROW( df.at("omega").as<double>().at(0) );
    EXPECT_ANY_THROW( df[0].as<double>()["Ninetinineth"] );
    EXPECT_ANY_THROW( df[0].as<double>().at("Ninetinineth") );

    EXPECT_TRUE( validate(df) );
}

TEST( Containers, DataframeCsv )
{
    NEEDS_TEST_DATA;
    auto const infile = environment->data_dir + "utils/csv/table.csv";

    auto reader = DataframeReader<double>();
    auto df = reader.read( from_file( infile ));

    EXPECT_EQ(  3, df.cols() );
    EXPECT_EQ( 10, df.rows() );

    // Index access
    EXPECT_EQ( 4.5, df[2].as<double>()[0]);
    EXPECT_EQ( 1.0, df[0].as<double>()[8]);

    // Name acces
    EXPECT_EQ( 4.7, df["Third"].as<double>()["gamma"]);
    EXPECT_EQ( 0.0, df["First"].as<double>()["kappa"]);

    // Remove some things.
    EXPECT_NO_THROW( df.remove_col( "Second" ));
    EXPECT_EQ(  2, df.cols() );
    EXPECT_NO_THROW( df.remove_row( 3 ));
    EXPECT_EQ( 9, df.rows() );
    EXPECT_EQ( 5.5, df["Third"].as<double>()[3]);

    // Make a copy.
    auto df2 = df;
    EXPECT_EQ( 5.5, df2["Third"].as<double>()[3]);

    for( size_t c = 0; c < df2.cols(); ++c ) {
        EXPECT_EQ( c, df2.col_index( df2.col_name( c )));
        EXPECT_EQ( df2.col_names()[c], df2.col_name( c ) );

        EXPECT_EQ( df2.rows(), df2[c].size() );
        EXPECT_EQ( df2.col_name(c), df2[c].name() );
        EXPECT_EQ( df2.col_names()[c], df2[c].name() );
        EXPECT_EQ( c, df2[c].index() );
        EXPECT_EQ( c, df2.col_index( df2[c].name() ));
    }

    for( size_t r = 0; r < df2.rows(); ++r ) {
        EXPECT_EQ( r, df2.row_index( df2.row_name(r) ));
        EXPECT_EQ( df2.row_names()[r], df2.row_name(r) );
    }

    EXPECT_TRUE( validate(df) );
    EXPECT_TRUE( validate(df2) );
}

TEST( Containers, DataframeReplaceCol )
{
    // Get data as before.
    NEEDS_TEST_DATA;
    auto const infile = environment->data_dir + "utils/csv/table.csv";
    auto reader = DataframeReader<double>();
    auto df = reader.read( from_file( infile ));
    EXPECT_EQ(  3, df.cols() );
    EXPECT_EQ( 10, df.rows() );

    auto const strvec = std::vector<std::string>{
        "a", "b", "c", "d", "e", "f", "g", "h", "i", "j"
    };

    df.replace_col<std::string>( "Second", strvec );
    EXPECT_EQ(  3, df.cols() );
    EXPECT_EQ( 10, df.rows() );
    EXPECT_TRUE( validate(df) );

    EXPECT_EQ( "c", df[1].as<std::string>()[2] );
}

/**
 * @brief Read and return directly.
 */
Dataframe read_dataframe_file_simple( std::string const& file )
{
    auto reader = DataframeReader<double>();
    reader.csv_reader().separator_chars( "\t" );
    return reader.read( from_file( file ));
}

/**
* @brief Read, manipulate, and return from value.
*/
Dataframe read_dataframe_file_complex( std::string const& file )
{
    auto reader = DataframeReader<double>();
    reader.csv_reader().separator_chars( "\t" );
    auto df = reader.read( from_file( file ));
    df.add_col( "Fourth", 1.0 );
    df.add_row( "lambda" );
    df[0].as<double>()[10] = 100.0;
    return df;
}

TEST( Containers, DataframeExtra )
{
    NEEDS_TEST_DATA;
    auto const infile = environment->data_dir + "utils/matrix/headers.mat";

    auto const df1 = read_dataframe_file_simple( infile );
    EXPECT_EQ(  3, df1.cols() );
    EXPECT_EQ( 10, df1.rows() );
    EXPECT_EQ( 4.7, df1["Third"].as<double>()["gamma"] );

    auto df2 = read_dataframe_file_complex( infile );
    EXPECT_EQ(  4, df2.cols() );
    EXPECT_EQ( 11, df2.rows() );
    EXPECT_EQ( 4.7, df2["Third"].as<double>()["gamma"] );
    EXPECT_EQ( 1.0, df2["Fourth"].as<double>()["gamma"] );
    EXPECT_EQ( 100.0, df2["First"].as<double>()["lambda"] );
    EXPECT_NE( 100.0, df2["First"].as<double>()["delta"] );

    EXPECT_TRUE( validate(df1) );
    EXPECT_TRUE( validate(df2) );
}

TEST( Containers, DataframeConvertString )
{
    // Read stuff.
    NEEDS_TEST_DATA;
    auto const infile = environment->data_dir + "utils/csv/table.csv";
    auto reader = DataframeReader<std::string>();
    auto df = reader.read( from_file( infile ));
    EXPECT_EQ(  3, df.cols() );
    EXPECT_EQ( 10, df.rows() );

    EXPECT_TRUE( is_convertible_to_bool( df, "First" ));
    EXPECT_TRUE( is_convertible_to_double( df, "First" ));
    EXPECT_FALSE( is_convertible_to_bool( df, "Second" ));
    EXPECT_TRUE( is_convertible_to_double( df, "Second" ));
    EXPECT_FALSE( is_convertible_to_bool( df, "Third" ));
    EXPECT_TRUE( is_convertible_to_double( df, "Third" ));

    // First column only contains 1 and 0.
    convert_to_bool( df, "First" );
    EXPECT_EQ( 0, df["First"].as<signed char>()[0] );
    EXPECT_EQ( 1, df["First"].as<signed char>()[1] );

    // Second column contains integers.
    convert_to_double( df, "Second" );
    EXPECT_EQ( 0, df["Second"].get<double>(0) );
    EXPECT_EQ( 8, df["Second"].get<double>(1) );

    EXPECT_TRUE( validate(df) );
}

TEST( Containers, DataframeAddReplaceCols )
{
    // Read stuff.
    NEEDS_TEST_DATA;
    auto const infile = environment->data_dir + "utils/csv/table.csv";
    auto reader = DataframeReader<std::string>();
    auto df = reader.read( from_file( infile ));
    EXPECT_EQ(  3, df.cols() );
    EXPECT_EQ( 10, df.rows() );

    std::vector<std::string> insert1 = {{ "A", "B", "C", "D", "E", "F", "G", "H", "I", "J" }};
    std::vector<std::string> insert2 = {{ "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T" }};

    // Copy unnamed col.
    df.add_unnamed_col( insert1 );

    // Move unnamed col.
    df.add_unnamed_col( std::vector<std::string>{{ "A", "B", "C", "D", "E", "F", "G", "H", "I", "J" }} );

    // Copy named col.
    df.add_col( "Sixth", insert1 );

    // Move named col.
    df.add_col( "Seventh", std::vector<std::string>{{ "A", "B", "C", "D", "E", "F", "G", "H", "I", "J" }} );

    // Copy replacement.
    df.replace_col( "Sixth", insert2 );

    // Move replacement.
    df.replace_col( "Seventh", std::vector<std::string>{{ "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T" }} );

    EXPECT_TRUE( validate(df) );
}

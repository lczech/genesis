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

#include "genesis/utils/formats/csv/reader.hpp"
#include "genesis/utils/formats/csv/input_iterator.hpp"

#include <string>
#include <vector>

using namespace genesis::utils;

// -------------------------------------------------------------------------
//     CSV Reader
// -------------------------------------------------------------------------

void test_csv_table(
    std::string const&      filename,
    CsvReader::Table const& actual,
    CsvReader::Table const& expected
) {
    // Print the table (for manual testing).
    // std::cout << "File: " << filename;
    // for( auto const& line : table ) {
    //     std::cout << "|";
    //     for( auto const& field : line ) {
    //         std::cout << field << "|";
    //     }
    //     std::cout << "\n";
    // }
    // std::cout << "\n";

    // Assert the vector sizes (aborts if not - otherwise we'd end of with segfaults).
    ASSERT_EQ( expected.size(), actual.size() )
        << "File: " << filename;

    for( size_t i = 0; i < expected.size(); ++i ) {
        ASSERT_EQ( expected[i].size(), actual[i].size() )
            << "Line: " << i << ", File: " << filename;

        for( size_t j = 0; j < expected[i].size(); ++j ) {
            EXPECT_EQ( expected[i][j], actual[i][j] )
                << "Column: " << j << ", Line: " << i << ", File: " << filename;
        }
    }
}

TEST( Csv, ReaderDefault )
{
    NEEDS_TEST_DATA;

    // Preparation.
    auto reader = CsvReader();
    std::string infile;
    CsvReader::Table table;

    // Simple table with default settings.
    infile = environment->data_dir + "utils/csv/simple.csv";
    table  = reader.read( from_file( infile ));

    test_csv_table( infile, table, {
        { "1", "Bran",     "Stark" },
        { "2", "Petyr",    "Baelish" },
        { "3", "Samwell",  "Tarly" },
        { "4", "Davos",    "Seaworth" },
        { "5", "Margaery", "Tyrell" }
    });

    // Complex(ish) table with default settings.
    infile = environment->data_dir + "utils/csv/complex.csv";
    table  = reader.read( from_file( infile ));

    test_csv_table( infile, table, {
        { "1",   "Bran", "" },
        { "2",   "", "Bolton" },
        { "",    "Tormund", "Giantsbane" },
        { "4",   "", "" },
        { "",    "Joffrey", "" },
        { "",    "", "Stark" },
        { "",    "", "" },
        { "8  ", "Jaime \"The Kingslayer\"", "Lannister" },
        { "9",   "  Sandor, called The Hound", "\"" },
        { "10",  "Daenerys \"Mother of Dragons\"", " \"" }
    });
}

TEST( Csv, ReaderTabulatedEscaped )
{
    NEEDS_TEST_DATA;

    // Preparation.
    auto reader = CsvReader();
    std::string infile;
    CsvReader::Table table;

    // Change Reader settings.
    reader.separator_chars( " \t" );
    reader.merge_separators( true );
    reader.use_escapes( true );
    reader.use_twin_quotes( false );

    // Read table that uses tabs as separators and escape sequences.
    infile = environment->data_dir + "utils/csv/tab_esc.csv";
    table  = reader.read( from_file( infile ));

    test_csv_table( infile, table, {
        { "1",  "Jon", "Snow" },
        { "2",  "Tommen", "Baratheon\nThe King" },
        { "3",  "Jorah with spaces", "Mormont\twith\ttabs" },
        { "4,", "Tyrion,", "Lannister\ncontinued: The Imp" },
        { "5",  "Jaqen\nnew line", "\"H'ghar\"" },
        { "6",  "Arya \"with quotes\"", "Stark" }
    });
}

TEST( Csv, ReaderCommentEmpty )
{
    NEEDS_TEST_DATA;

    // Preparation.
    auto reader = CsvReader();
    std::string infile;
    CsvReader::Table table;

    // Change Reader settings.
    reader.comment_chars( "#" );
    reader.merge_separators( true );
    reader.skip_empty_lines( true );

    // Read table that has comments and empty lines and stuff.
    infile = environment->data_dir + "utils/csv/comment_empty.csv";
    table  = reader.read( from_file( infile ));

    test_csv_table( infile, table, {
        { "1",  "Cersei", "Lannister" },
        { "2",  "Ned", "Stark" },
        { "3",  "Khal", "Drogo" },
        { "" },
        { " # this is not", " but", " no!" },
        { "4",  "Daario", "Naharis" }
    });
}

TEST( Csv, InputIterator )
{
    NEEDS_TEST_DATA;

    // while loop
    {
        size_t count = 0;
        auto line_it = CsvInputIterator( from_file( environment->data_dir + "utils/csv/complex.csv" ));
        while( line_it ) {
            EXPECT_EQ( 3, line_it->size() );
            ++count;
            ++line_it;
        }
        EXPECT_EQ( 10, count );
    }

    // range based for loop
    {
        size_t count = 0;
        auto line_it = CsvInputIterator( from_file( environment->data_dir + "utils/csv/complex.csv" ));
        for( auto const& line : line_it ) {
            EXPECT_EQ( 3, line.size() );
            ++count;
        }
        EXPECT_EQ( 10, count );
    }
}

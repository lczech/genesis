/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

#include "common.hpp"

#include "lib/utils/formats/csv/reader.hpp"

#include <string>
#include <vector>

using namespace genesis::utils;

// -------------------------------------------------------------------------
//     CSV Reader
// -------------------------------------------------------------------------

void test_csv_table(
    std::string const&      filename,
    CsvReader::table const& actual,
    CsvReader::table const& expected
) {
    EXPECT_EQ( expected.size(), actual.size() )
        << "File: " << filename;

    for( size_t i = 0; i < expected.size(); ++i ) {
        EXPECT_EQ( expected[i].size(), actual[i].size() )
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
    CsvReader::table table;

    // Simple table with default settings.

    infile = environment->data_dir + "utils/simple.csv";
    table  = reader.from_file( infile );

    test_csv_table( infile, table, {
        { "1", "Brandon", "Butler" },
        { "2", "Gerald",  "Gilbert" },
        { "3", "Anna",    "Edwards" },
        { "4", "Randy",   "Anderson" },
        { "5", "Heather", "Richards" }
    });

    // Complex(ish) table with default settings.

    infile = environment->data_dir + "utils/complex.csv";
    table  = reader.from_file( infile );

    test_csv_table( infile, table, {
        { "1",   "Brandon", "" },
        { "2",   "", "Gilbert" },
        { "",    "Anna", "Edwards" },
        { "4",   "", "" },
        { "",    "Heather", "" },
        { "",    "", "Cox" },
        { "",    "", "" },
        { "8  ", "Jeremy \"The trick\"", "Campbell" },
        { "9",   "  John, or was it Steve", "\"" },
        { "10",  "Raymond with \"quotes\"", " \"" }
    });
}

TEST( Csv, ReaderTabulatedEscaped )
{
    NEEDS_TEST_DATA;

    // Preparation.
    auto reader = CsvReader();
    std::string infile;
    CsvReader::table table;

    // Change Reader settings.

    reader.separator_chars( " \t" );
    reader.merge_separators( true );
    reader.use_escapes( true );
    reader.use_twin_quotes( false );

    // Read table that uses tabs as separators and escape sequences.

    infile = environment->data_dir + "utils/tab_esc.csv";
    table  = reader.from_file( infile );

    test_csv_table( infile, table, {
        { "1",  "Brandon", "Butler" },
        { "2",  "Gerald", "Gilbert\nBold" },
        { "3",  "Anna with spaces", "Edwards\twith\ttabs" },
        { "4,", "Randy,", "Anderson\ncontinued" },
        { "5",  "Heather\nnew line", "\"Richards\"" },
        { "6",  "Man \"with quotes\"", "Wins" }
    });

}

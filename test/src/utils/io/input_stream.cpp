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

#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/io/parser.hpp"
#include "genesis/utils/math/common.hpp"
#include "genesis/utils/math/random.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <limits>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using namespace genesis;
using namespace genesis::utils;

static void input_stream_test_input_specs_( InputStream& instr, size_t lines, size_t columns )
{
    size_t max_col = 0;
    size_t max_lin = 0;

    while( instr ) {
        max_col = std::max( max_col, instr.column() );
        max_lin = std::max( max_lin, instr.line() );

        ++instr;
    }

    EXPECT_EQ( columns, max_col );
    EXPECT_EQ( lines,   max_lin );
}

static void input_stream_test_string_ ( std::string str, size_t lines, size_t columns )
{
    InputStream instr( utils::make_unique< StringInputSource >( str ));
    input_stream_test_input_specs_( instr, lines, columns );
}

TEST(InputStream, Strings)
{
    input_stream_test_string_ ("",              0, 0);
    input_stream_test_string_ ("\n",            1, 1);
    input_stream_test_string_ ("\n\n\n",        3, 1);
    input_stream_test_string_ ("x",             1, 2);
    input_stream_test_string_ ("xyz",           1, 4);
    input_stream_test_string_ ("xyz\n",         1, 4);
    input_stream_test_string_ ("xyz\nxy\nx\nx", 4, 4);
}

TEST(InputStream, FileReading)
{
    NEEDS_TEST_DATA;
    SCOPED_TRACE("InputStream.FileReading");

    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    InputStream instr( utils::make_unique< FileInputSource >( infile ));

    input_stream_test_input_specs_( instr, 110, 51 );
}

TEST(InputStream, NewLines)
{
    // Just \n.
    input_stream_test_string_( "a\nb",   2, 2);
    input_stream_test_string_( "a\nb\n", 2, 2);

    // Just \r.
    input_stream_test_string_( "a\rb",   2, 2);
    input_stream_test_string_( "a\rb\r", 2, 2);

    // Both.
    input_stream_test_string_( "a\r\nb",     2, 2);
    input_stream_test_string_( "a\r\nb\r\n", 2, 2);

    // Go crazy.
    input_stream_test_string_( "\r\r\n\r\n\n", 4, 1);
}

// =================================================================================================
//     Random Fuzzy
// =================================================================================================

struct RandomFuzzyLines
{
    std::vector<size_t> line_lengths;
    std::string text;
};

RandomFuzzyLines make_random_fuzzy_lines_( size_t const n_lines )
{
    RandomFuzzyLines result;
    std::stringstream ss;
    for( size_t i = 0; i < n_lines; ++i ) {
        auto const len = permuted_congruential_generator( 0, 100 );
        ss << std::string( len, 'x' ) << "\n";
        result.line_lengths.push_back( len );
    }
    // LOG_DBG << utils::join( result.line_lengths );
    result.text = ss.str();
    return result;
}

template<bool View>
void test_input_stream_fuzzy_()
{
    // Make a string with random line length.
    // We explicitly want to have tests that generate more than one block length of text.
    // The function creates lines of 1-100 chars, so 50 on average, meaning that 4MB / 50 ~ 80k
    // lines are one buffer block. So, we just do some tests with more than double.
    auto const n_lines = permuted_congruential_generator( 1, 200000 );
    auto const lines_data = make_random_fuzzy_lines_( n_lines );

    // Now read it again and expect the correct line length.
    auto it = InputStream( from_string( lines_data.text ));
    size_t cnt = 0;
    while( it ) {

        EXPECT_EQ( cnt + 1, it.line() );
        EXPECT_EQ( 1, it.column() );

        std::string line;
        if( View ) {
            #if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
                // To keep the below test the same for either get_line function, we convert to string.
                line = std::string( it.get_line_view() );
            #else
                // Without C++17, we just do the line get again, to satisfy the compiler.
                line = it.get_line();
            #endif
        } else {
            line = it.get_line();
        }

        EXPECT_EQ( lines_data.line_lengths[cnt], line.size() );
        EXPECT_EQ( cnt + 2, it.line() );
        EXPECT_EQ( 1, it.column() );

        ++cnt;
    }
}

template<bool View>
void run_test_input_stream_fuzzy_()
{
    // Random seed. Report it, so that in an error case, we can reproduce.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;

    // For the duration of the test, we deactivate debug logging.
    // But if needed, comment this line out, and each test will report its input.
    LOG_SCOPE_LEVEL( genesis::utils::Logging::kInfo );

    size_t num_tests = 50;
    for( size_t i = 0; i < num_tests; ++i ) {
        LOG_DBG << "=================================";
        LOG_DBG << "Test " << i;
        test_input_stream_fuzzy_<View>();
    }
}

TEST( InputStream, GetLineFuzzy )
{
    run_test_input_stream_fuzzy_<false>();
}

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)

TEST( InputStream, GetLineViewFuzzy )
{
    run_test_input_stream_fuzzy_<true>();
}

// =================================================================================================
//     Random Fuzzy Views
// =================================================================================================

void run_test_input_stream_fuzzy_views_()
{
    // How many lines do we want to read at a time?
    auto const line_block_size = 4;

    // Make a string with random line length.
    // We explicitly want to have tests that generate more than one block length of text.
    // The function creates lines of 1-100 chars, so 50 on average, meaning that 4MB / 50 ~ 80k
    // lines are one buffer block. So, we just do some tests with more than double.
    // We round to a multiple of the line_block_size, as the reader expects an exact fit.
    auto n_lines = permuted_congruential_generator( 1, 200000 );
    n_lines += line_block_size - ( n_lines % line_block_size );
    auto const lines_data = make_random_fuzzy_lines_( n_lines );

    // Now read it again and expect the correct line length.
    auto it = InputStream( from_string( lines_data.text ));
    size_t cnt = 0;
    while( it ) {

        EXPECT_EQ( cnt + 1, it.line() );
        EXPECT_EQ( 1, it.column() );

        // We get four lines at a time.
        auto const views = it.get_line_views<line_block_size>();
        for( size_t i = 0; i < line_block_size; ++i ) {
            if( cnt < lines_data.line_lengths.size() ) {
                EXPECT_EQ( lines_data.line_lengths[cnt], views[i].size() );
                ++cnt;
            }
        }

        EXPECT_EQ( cnt + 1, it.line() );
        EXPECT_EQ( 1, it.column() );
    }
}

TEST( InputStream, GetLineViewsFuzzy )
{
    // Random seed. Report it, so that in an error case, we can reproduce.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;

    // For the duration of the test, we deactivate debug logging.
    // But if needed, comment this line out, and each test will report its input.
    LOG_SCOPE_LEVEL( genesis::utils::Logging::kInfo );

    size_t num_tests = 50;
    for( size_t i = 0; i < num_tests; ++i ) {
        LOG_DBG << "=================================";
        LOG_DBG << "Test " << i;
        run_test_input_stream_fuzzy_views_();
    }
}

#endif

// =================================================================================================
//     Get Lines
// =================================================================================================

TEST( InputStream, GetLineLargeFile )
{
    // Skip test if no data directory availabe.
    NEEDS_TEST_DATA;

    // Create a large file with a known number and length of lines.
    std::string tmpfile = environment->data_dir + "utils/large_file.txt";
    std::ofstream out{ tmpfile };
    ASSERT_TRUE( out );

    // We use a quarter of the block len as our text block,
    // and then construct a file with multiples of this for the lines.
    // That means, at some point we will have lines that are exact multiples of the block size.
    // This is good for testing boundary conditions of exact matches of lengths,
    // so that we can check some of the assertions of the input stream!
    auto const block_len = InputStream::BlockLength;
    auto const text = std::string( block_len / 4, 'x' );

    // Make a file with the form
    //
    // x
    // xx
    // xxx
    // xxxx
    // ...
    for( size_t i = 0; i < 16; ++i ) {
        for( size_t j = 0; j < i; ++j ) {
            out << text;
        }
        out << "\n";
    }
    out.close();

    // Now read it again and expect the correct line length.
    auto it = InputStream( from_file( tmpfile ));
    size_t cnt = 0;
    while( it ) {

        EXPECT_EQ( cnt + 1, it.line() );
        EXPECT_EQ( 1, it.column() );

        auto const line = it.get_line();

        EXPECT_EQ( cnt * text.size(), line.size() );
        EXPECT_EQ( cnt + 2, it.line() );
        EXPECT_EQ( 1, it.column() );

        ++cnt;
    }

    // Make sure the file is deleted.
    ASSERT_EQ( 0, std::remove(tmpfile.c_str()) );
}

// =================================================================================================
//     Large Fuzzy
// =================================================================================================

void test_input_stream_large_fuzzy_()
{
    // We write to a string buffer here, for speed, instead of files.
    std::stringstream ss;

    // We use the block len as our basic line length, but vary this by plus or minus one char
    // each time, so that we constantly hit block boundaries in the reading.
    // That means, we will like have lines that are exact multiples of the block size,
    // and are aligned at the block ends.
    // This is good for testing boundary conditions of exact matches of lengths,
    // so that we can check some of the assertions of the input stream!
    auto const block_len = InputStream::BlockLength;
    auto const base_text = std::string( block_len - 1, '-' );

    // Make a string with random line length
    auto const lines = permuted_congruential_generator( 1, 10 );
    std::vector<size_t> line_lengths;
    for( size_t i = 0; i < lines; ++i ) {

        // The base text above is one character short. Decide if we want to keep it that way,
        // hit the exact block size, or overshoot by one.
        auto const extra = permuted_congruential_generator( 0, 2 );

        line_lengths.push_back( block_len - 1 + extra );
        ss << base_text;
        for( size_t j = 0; j < extra; ++j ) {
            ss << "x";
        }
        ss << "\n";
    }
    auto const large = ss.str();
    // LOG_DBG << utils::join( line_lengths );

    // Now read it again and expect the correct line length.
    auto it = InputStream( from_string( large ));
    size_t cnt = 0;
    while( it ) {

        EXPECT_EQ( cnt + 1, it.line() );
        EXPECT_EQ( 1, it.column() );

        auto const line = it.get_line();

        EXPECT_EQ( line_lengths[cnt], line.size() );
        EXPECT_EQ( cnt + 2, it.line() );
        EXPECT_EQ( 1, it.column() );

        ++cnt;
    }
}

TEST( InputStream, GetLineLargeFuzzy )
{
    // Random seed. Report it, so that in an error case, we can reproduce.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;

    // For the duration of the test, we deactivate debug logging.
    // But if needed, comment this line out, and each test will report its input.
    LOG_SCOPE_LEVEL( genesis::utils::Logging::kInfo );

    size_t num_tests = 50;
    for( size_t i = 0; i < num_tests; ++i ) {
        LOG_DBG << "=================================";
        LOG_DBG << "Test " << i;
        test_input_stream_large_fuzzy_();
    }
}

// =================================================================================================
//     Parsing
// =================================================================================================

TEST( InputStream, ParseInt )
{
    // Skip test if no data directory availabe.
    NEEDS_TEST_DATA;

    // Create a file with some test data in it.
    std::string tmpfile = environment->data_dir + "utils/ints.txt";
    std::ofstream out{ tmpfile };
    ASSERT_TRUE( out );

    // Prepare random numbers of different lengths. We need to test length of > 8,
    // for the naive algorithm to kick in.
    // First, we use a distrib of how many digits we want, and then distribs for each of them,
    // in order to guarantee that we see enough short numbers.
    auto const max = 10;
    auto const seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine engine(seed);
    std::uniform_int_distribution<> separator(0,1);
    std::uniform_int_distribution<> negative(0,1);
    std::uniform_int_distribution<size_t> digits( 1, max );
    std::vector<std::uniform_int_distribution<int64_t>> distribs;

    // Dumy entry to get the indexing in the vector to match the number of digits that it is
    // going to produce. Then, fill with actual distribs
    distribs.emplace_back( 0, 0 );
    for( size_t i = 0; i < max; ++i ) {
        auto const min = ( i == 0 ? 0 : int_pow( 10, i ));
        auto const max = int_pow( 10, i+1 ) - 1;
        distribs.emplace_back( min, max );
    }

    // We write random numbers, along with either new lines or colon characters.
    // We use these two, as new lines are below and colons are above digits in the ascii table,
    // so that we test both conditions for where a number stops.
    // We keep track of lines and line lengths here as well, for testing the column counter.

    auto const n = 1000000;
    int64_t sum = 0;
    auto line_lengths = std::vector<size_t>( 1, 0 );
    size_t line = 0;
    for( size_t i = 0; i < n; ++i ) {
        auto const d = digits( engine );
        auto const sign = negative(engine) ? -1 : +1;
        auto const r = sign * distribs[d]( engine );

        sum += r;
        line_lengths[line] += std::to_string(r).size();
        out << r;

        // Extra test case: Stream that does not end in a new line!
        // Let's test this, and make sure that it works.
        if( i == n - 1 ) {
            break;
        }

        // Decide randomly whether we make a new line or a colon, except for the last number,
        // where we always end with a new line, as otherwise the below simply parsing function
        // will expect another int after the colon.
        auto const make_new_line = separator( engine );
        if( make_new_line || i == n - 1 ) {
            out << '\n';
            ++line;
            line_lengths.emplace_back( 0 );
        } else {
            out << ':';
            ++line_lengths[line];
        }
    }
    out.close();

    // Now read through and check that we got all the same again.
    InputStream input_stream( from_file( tmpfile ));
    int64_t target_sum = 0;
    size_t target_line = 0;
    size_t target_length = 0;
    while( input_stream ) {

        auto const n = parse_signed_integer<int64_t>( input_stream );
        target_sum += n;
        target_length += std::to_string(n).size();

        if( *input_stream == '\n' ) {
            EXPECT_EQ( line_lengths[target_line], target_length );

            // Check that we are correctly counting lines and columns.
            // We need to offset by one, due to lines and columns starting their count at 1.
            EXPECT_EQ( target_line + 1, input_stream.line() );
            EXPECT_EQ( target_length + 1, input_stream.column() );

            ++target_line;
            target_length = 0;
        } else if( *input_stream == ':' ) {
            ++target_length;
        } else {
            EXPECT_TRUE(false);
        }
        ++input_stream;
    }

    // Test that we end up where we want, with the correct sum of numbers, and the correct line.
    // When creating the file above, we omitted the closing new line from the file, which is added
    // by the parser automatically. So here we expect one additional line to be found.
    EXPECT_EQ( line + 1, target_line );
    EXPECT_EQ( sum, target_sum );

    // Make sure the file is deleted.
    ASSERT_EQ( 0, std::remove(tmpfile.c_str()) );
}

/**
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include "lib/utils/io/counting_istream_iterator.hpp"

#include <algorithm>
#include <iterator>
#include <fstream>
#include <string>
#include <sstream>

using namespace genesis::utils;

void test_input_specs( CountingIstreamIterator& iit, size_t lines, size_t columns )
{
    CountingIstreamIterator eos;

    size_t max_col = 0;
    size_t max_lin = 0;

    while( iit != eos ) {
        max_col = std::max( max_col, iit.column() );
        max_lin = std::max( max_lin, iit.line() );

        ++iit;
    }

    EXPECT_EQ( columns, max_col );
    EXPECT_EQ( lines,   max_lin );
}

void test_string ( std::string str, size_t lines, size_t columns )
{
    std::istringstream iss ( str );
    CountingIstreamIterator iit (iss);
    test_input_specs( iit, lines, columns );
}

TEST(CountingIstreamIterator, Strings)
{
    test_string ("",              0, 0);
    test_string ("\n",            1, 1);
    test_string ("\n\n\n",        3, 1);
    test_string ("x",             1, 1);
    test_string ("xyz",           1, 3);
    test_string ("xyz\n",         1, 4);
    test_string ("xyz\nxy\nx\nx", 4, 4);
}

TEST(CountingIstreamIterator, LargeFile)
{
    NEEDS_TEST_DATA;

    std::string infile = environment->data_dir + "sequence/dna_354.fasta";

    std::ifstream ifs (infile);
    CountingIstreamIterator iit (ifs);

    test_input_specs( iit, 3894, 51 );
}

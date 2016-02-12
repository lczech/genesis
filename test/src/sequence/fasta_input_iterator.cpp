/**
 * @brief Testing Sequence Set.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include "lib/sequence/sequence.hpp"
#include "lib/sequence/io/fasta_input_iterator.hpp"

#include <fstream>

TEST( FastaInputIterator, Reading )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    std::string infile = environment->data_dir + "sequence/dna_354.fasta";
    std::ifstream ifs (infile);

    auto it = genesis::sequence::FastaInputIterator(ifs);

    size_t len = 0;
    size_t cnt = 0;
    while( it.extract_sequence() ) {
        len = std::max( len, it->length() );
        ++cnt;
    }

    EXPECT_EQ( 460, len );
    EXPECT_EQ( 354, cnt );
}

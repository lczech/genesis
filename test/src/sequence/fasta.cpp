/**
 * @brief Testing Sequence Set.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include "lib/sequence/codes.hpp"
#include "lib/sequence/functions.hpp"
#include "lib/sequence/sequence_set.hpp"
#include "lib/sequence/io/fasta_reader.hpp"

#include <string>

using namespace genesis::sequence;

TEST( Sequence, FastaReaderSimple )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/dna_354.fasta";
    SequenceSet sset;

    FastaReader()
        .validate_chars( nucleic_acid_codes_all() )
        .from_file( infile, sset );

    // Check data.
    EXPECT_EQ( 354, sset.size() );
    EXPECT_EQ( 460,                    sset[0].length() );
    EXPECT_EQ( "Di106BGTue",           sset[0].label() );
    EXPECT_EQ( "",                     sset[0].metadata() );
    EXPECT_EQ( "TCGAAACCTGC------CTA", sset[0].sites().substr( 0, 20 ) );
}

TEST( Sequence, FastaReaderValidating )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/dna_354.fasta";
    SequenceSet sset;
    FastaReader().validate_chars( nucleic_acid_codes_all() ).from_file(infile, sset);

    // Check data.
    EXPECT_EQ( 354, sset.size() );
    EXPECT_EQ( 460,                    sset[0].length() );
    EXPECT_EQ( "Di106BGTue",           sset[0].label() );
    EXPECT_EQ( "",                     sset[0].metadata() );
    EXPECT_EQ( "TCGAAACCTGC------CTA", sset[0].sites().substr( 0, 20 ) );
}

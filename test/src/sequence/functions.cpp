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
#include "lib/sequence/io/fasta_processor.hpp"

#include <string>

using namespace genesis::sequence;

TEST( SequenceSet, Characteristics )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/dna_354.fasta";
    SequenceSet sset;
    FastaProcessor().from_file(infile, sset);

    EXPECT_TRUE( validate_chars( sset, nucleic_acid_codes_all() ));

    // Some basic assertments.
    EXPECT_EQ( 162840, total_length( sset ));
    EXPECT_TRUE( is_alignment( sset ));
    EXPECT_FLOAT_EQ( 0.1470646, gapyness( sset, nucleic_acid_codes_undetermined() ));

    // Check counting.
    auto const sh = site_histogram( sset );
    auto const bf = base_frequencies( sset, nucleic_acid_codes_plain() );

    // Site histogram.
    EXPECT_EQ( 23948, sh.at('-') );
    EXPECT_EQ( 26648, sh.at('A') );
    EXPECT_EQ( 43878, sh.at('C') );
    EXPECT_EQ( 40133, sh.at('G') );
    EXPECT_EQ( 28220, sh.at('T') );

    // Site histogram: Noise in the dataset.
    EXPECT_EQ( 3, sh.at('M') );
    EXPECT_EQ( 3, sh.at('R') );
    EXPECT_EQ( 1, sh.at('S') );
    EXPECT_EQ( 6, sh.at('Y') );

    // Base frequencies.
    EXPECT_DOUBLE_EQ( 0.19187926180344039, bf.at('A') );
    EXPECT_DOUBLE_EQ( 0.31594409521957961, bf.at('C') );
    EXPECT_DOUBLE_EQ( 0.28897817524607750, bf.at('G') );
    EXPECT_DOUBLE_EQ( 0.20319846773090244, bf.at('T') );
}

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

    std::string infile = environment->data_dir + "sequence/dna_354.fasta";

    SequenceSet sset;
    FastaProcessor().from_file(infile, sset);

    EXPECT_TRUE( validate_chars( sset, nucleic_acid_codes_all() ));

    EXPECT_EQ( 162840, total_length( sset ));
    EXPECT_TRUE( is_alignment( sset ));
    EXPECT_FLOAT_EQ( 0.1470646, gapyness( sset, nucleic_acid_codes_undetermined() ));
}

/**
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include "lib/sequence/codes.hpp"
#include "lib/sequence/functions.hpp"
#include "lib/sequence/sequence_set.hpp"
#include "lib/sequence/io/phylip_reader.hpp"

#include <string>

using namespace genesis::sequence;

// =================================================================================================
//     Read
// =================================================================================================

// ---------------------------------------------------------------------
//     Interleaved
// ---------------------------------------------------------------------

TEST( Sequence, PhylipReaderAaInterleaved )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/aa_3_384_i.phylip";
    SequenceSet sset;
    PhylipReader()
        .label_length( 10 )
        .mode( PhylipReader::Mode::kInterleaved )
        .validate_chars( amino_acid_codes_all() )
        .from_file(infile, sset);

    // Check data.
    EXPECT_EQ( 3, sset.size() );
    EXPECT_EQ( 384,          sset[0].length() );
    EXPECT_EQ( "CATH_HUMAN", sset[2].label() );
    EXPECT_EQ( "",           sset[0].metadata() );
    EXPECT_EQ( "G-AVTPVKNQ", sset[0].sites().substr( 160, 10 ) );
}

TEST( Sequence, PhylipReaderDnaInterleaved )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/dna_5_42_i.phylip";
    SequenceSet sset;
    PhylipReader()
        .label_length( 10 )
        .mode( PhylipReader::Mode::kInterleaved )
        .validate_chars( nucleic_acid_codes_all() )
        .from_file(infile, sset);

    // Check data.
    EXPECT_EQ( 5, sset.size() );
    EXPECT_EQ( 42,           sset[0].length() );
    EXPECT_EQ( "Turkey",     sset[0].label() );
    EXPECT_EQ( "",           sset[0].metadata() );
    EXPECT_EQ( "AAGCTNGGGC", sset[0].sites().substr( 0, 10 ) );
}

// ---------------------------------------------------------------------
//     Sequential
// ---------------------------------------------------------------------

TEST( Sequence, PhylipReaderAaSequential )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/aa_3_384_s.phylip";
    SequenceSet sset;
    PhylipReader()
        .label_length( 10 )
        .validate_chars( amino_acid_codes_all() )
        .from_file(infile, sset);

    // Check data.
    EXPECT_EQ( 3, sset.size() );
    EXPECT_EQ( 384,          sset[0].length() );
    EXPECT_EQ( "CATH_HUMAN", sset[2].label() );
    EXPECT_EQ( "",           sset[0].metadata() );
    EXPECT_EQ( "G-AVTPVKNQ", sset[0].sites().substr( 160, 10 ) );
}

TEST( Sequence, PhylipReaderDnaSequential )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/dna_5_42_s.phylip";
    SequenceSet sset;
    PhylipReader()
        .label_length( 10 )
        .validate_chars( nucleic_acid_codes_all() )
        .from_file(infile, sset);

    // Check data.
    EXPECT_EQ( 5, sset.size() );
    EXPECT_EQ( 42,           sset[0].length() );
    EXPECT_EQ( "Salmo gair", sset[1].label() );
    EXPECT_EQ( "",           sset[0].metadata() );
    EXPECT_EQ( "CGGGCACGGT", sset[1].sites().substr( 30, 10 ) );
}

// ---------------------------------------------------------------------
//     Automatic (Interleaved)
// ---------------------------------------------------------------------

TEST( Sequence, PhylipReaderAaInterleavedAuto )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/aa_3_384_i.phylip";
    SequenceSet sset;
    PhylipReader()
        .label_length( 10 )
        .mode( PhylipReader::Mode::kAutomatic )
        .validate_chars( amino_acid_codes_all() )
        .from_file(infile, sset);

    // Check data.
    EXPECT_EQ( 3, sset.size() );
    EXPECT_EQ( 384,          sset[0].length() );
    EXPECT_EQ( "CATH_HUMAN", sset[2].label() );
    EXPECT_EQ( "",           sset[0].metadata() );
    EXPECT_EQ( "G-AVTPVKNQ", sset[0].sites().substr( 160, 10 ) );
}

TEST( Sequence, PhylipReaderDnaInterleavedAuto )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/dna_5_42_i.phylip";
    SequenceSet sset;
    PhylipReader()
        .label_length( 10 )
        .mode( PhylipReader::Mode::kAutomatic )
        .validate_chars( nucleic_acid_codes_all() )
        .from_file(infile, sset);

    // Check data.
    EXPECT_EQ( 5, sset.size() );
    EXPECT_EQ( 42,           sset[0].length() );
    EXPECT_EQ( "Turkey",     sset[0].label() );
    EXPECT_EQ( "",           sset[0].metadata() );
    EXPECT_EQ( "AAGCTNGGGC", sset[0].sites().substr( 0, 10 ) );
}

// ---------------------------------------------------------------------
//     Automatic (Sequential)
// ---------------------------------------------------------------------

TEST( Sequence, PhylipReaderAaSequentialAuto )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/aa_3_384_s.phylip";
    SequenceSet sset;
    PhylipReader()
        .label_length( 10 )
        .mode( PhylipReader::Mode::kAutomatic )
        .validate_chars( amino_acid_codes_all() )
        .from_file(infile, sset);

    // Check data.
    EXPECT_EQ( 3, sset.size() );
    EXPECT_EQ( 384,          sset[0].length() );
    EXPECT_EQ( "CATH_HUMAN", sset[2].label() );
    EXPECT_EQ( "",           sset[0].metadata() );
    EXPECT_EQ( "G-AVTPVKNQ", sset[0].sites().substr( 160, 10 ) );
}

TEST( Sequence, PhylipReaderDnaSequentialAuto )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/dna_5_42_s.phylip";
    SequenceSet sset;
    PhylipReader()
        .label_length( 10 )
        .mode( PhylipReader::Mode::kAutomatic )
        .validate_chars( nucleic_acid_codes_all() )
        .from_file(infile, sset);

    // Check data.
    EXPECT_EQ( 5, sset.size() );
    EXPECT_EQ( 42,           sset[0].length() );
    EXPECT_EQ( "Salmo gair", sset[1].label() );
    EXPECT_EQ( "",           sset[0].metadata() );
    EXPECT_EQ( "CGGGCACGGT", sset[1].sites().substr( 30, 10 ) );
}

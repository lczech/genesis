/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include "genesis/sequence/functions/codes.hpp"
#include "genesis/sequence/functions/functions.hpp"
#include "genesis/sequence/sequence_set.hpp"
#include "genesis/sequence/formats/phylip_reader.hpp"

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
        .valid_chars( amino_acid_codes_all() )
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
        .valid_chars( nucleic_acid_codes_all() )
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
        .valid_chars( amino_acid_codes_all() )
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
        .valid_chars( nucleic_acid_codes_all() )
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
        .valid_chars( amino_acid_codes_all() )
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
        .valid_chars( nucleic_acid_codes_all() )
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
        .valid_chars( amino_acid_codes_all() )
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
        .valid_chars( nucleic_acid_codes_all() )
        .from_file(infile, sset);

    // Check data.
    EXPECT_EQ( 5, sset.size() );
    EXPECT_EQ( 42,           sset[0].length() );
    EXPECT_EQ( "Salmo gair", sset[1].label() );
    EXPECT_EQ( "",           sset[0].metadata() );
    EXPECT_EQ( "CGGGCACGGT", sset[1].sites().substr( 30, 10 ) );
}

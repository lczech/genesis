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
#include "lib/sequence/io/fasta_reader.hpp"

#include <string>

using namespace genesis::sequence;

TEST( Sequence, Print )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/dna_354.fasta";
    SequenceSet sset;
    FastaReader().from_file(infile, sset);

    // Print Sequence.
    EXPECT_EQ(
        "Di106BGTue: TCGAA ...\n",
        print( sset[0], true, 5 )
    );
    EXPECT_EQ(
        "TCGAA ...\n",
        print( sset[0], false, 5 )
    );

    // Print Sequence Set
    EXPECT_EQ(
        "Di106BGTue: TCGAA ...\nDi145BGTue: TCGAA ...\n...\n",
        print( sset, true, 5, 2 )
    );
    EXPECT_EQ(
        "TCGAA ...\nTCGAA ...\n...\n",
        print( sset, false, 5, 2 )
    );

    // Print Color Sequence
    EXPECT_EQ(
        "Di106BGTue: \x1B[30;44mT\x1B[0m\x1B[30;42mC\x1B[0m\x1B[30;43mG\x1B[0m ...\n",
        print_color( sset[0], nucleic_acid_text_colors(), true, 3, true )
    );
    EXPECT_EQ(
        "\x1B[30;44mT\x1B[0m\x1B[30;42mC\x1B[0m\x1B[30;43mG\x1B[0m ...\n",
        print_color( sset[0], nucleic_acid_text_colors(), false, 3, true )
    );
    EXPECT_EQ(
        "Di106BGTue: \x1B[34mT\x1B[0m\x1B[32mC\x1B[0m\x1B[33mG\x1B[0m ...\n",
        print_color( sset[0], nucleic_acid_text_colors(), true, 3, false )
    );
    EXPECT_EQ(
        "\x1B[34mT\x1B[0m\x1B[32mC\x1B[0m\x1B[33mG\x1B[0m ...\n",
        print_color( sset[0], nucleic_acid_text_colors(), false, 3, false )
    );

    // Print Color Sequence Set
    EXPECT_EQ(
        "Di106BGTue: \x1B[30;44mT\x1B[0m\x1B[30;42mC\x1B[0m\x1B[30;43mG\x1B[0m ...\n"
        "Di145BGTue: \x1B[30;44mT\x1B[0m\x1B[30;42mC\x1B[0m\x1B[30;43mG\x1B[0m ...\n...\n",
        print_color( sset, nucleic_acid_text_colors(), true, 3, 2, true )
    );
    EXPECT_EQ(
        "\x1B[30;44mT\x1B[0m\x1B[30;42mC\x1B[0m\x1B[30;43mG\x1B[0m ...\n"
        "\x1B[30;44mT\x1B[0m\x1B[30;42mC\x1B[0m\x1B[30;43mG\x1B[0m ...\n...\n",
        print_color( sset, nucleic_acid_text_colors(), false, 3, 2, true )
    );

    /*
    std::cout << "Simple:\n";
    std::cout << sset[0] << "\n";
    std::cout << sset << "\n";

    std::cout << "\nPrint Sequence:\n";
    std::cout << print( sset[0], true, 100 ) << "\n";
    std::cout << print( sset[0], false, 100 ) << "\n";
    std::cout << print( sset[0], false, 10 ) << "\n";

    std::cout << "\nPrint Set:\n";
    std::cout << print( sset, true, 100, 5 ) << "\n";
    std::cout << print( sset, false, 80, 5 ) << "\n";

    std::cout << "\nPrint Color Sequence:\n";
    std::cout << print_color( sset[0], nucleic_acid_text_colors(), true, 100, true ) << "\n";
    std::cout << print_color( sset[0], nucleic_acid_text_colors(), true, 100, false ) << "\n";
    std::cout << print_color( sset[0], nucleic_acid_text_colors(), false, 100, true ) << "\n";
    std::cout << print_color( sset[0], nucleic_acid_text_colors(), false, 100, false ) << "\n";

    std::cout << "\nPrint Color Set:\n";
    std::cout << print_color( sset, nucleic_acid_text_colors(), true, 100, 6, true ) << "\n";
    std::cout << print_color( sset, nucleic_acid_text_colors(), true, 100, 6, false ) << "\n";
    std::cout << print_color( sset, nucleic_acid_text_colors(), false, 100, 6, true ) << "\n";
    std::cout << print_color( sset, nucleic_acid_text_colors(), false, 100, 6, false ) << "\n";
    */
}

TEST( SequenceSet, Characteristics )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/dna_354.fasta";
    SequenceSet sset;
    FastaReader().from_file(infile, sset);

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

    // Site histogram: Degenerated noise in the dataset.
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

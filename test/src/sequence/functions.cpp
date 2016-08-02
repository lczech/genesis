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

#include "lib/sequence/functions/codes.hpp"
#include "lib/sequence/functions/functions.hpp"
#include "lib/sequence/sequence_set.hpp"
#include "lib/sequence/formats/fasta_reader.hpp"

#include <string>

using namespace genesis::sequence;

TEST( Sequence, Print )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
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
    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    SequenceSet sset;
    FastaReader().from_file(infile, sset);

    EXPECT_TRUE( validate_chars( sset, nucleic_acid_codes_all() ));

    // Some basic assertments.
    EXPECT_EQ( 4600, total_length( sset ));
    EXPECT_TRUE( is_alignment( sset ));
    EXPECT_FLOAT_EQ( 0.15086956, gapyness( sset, nucleic_acid_codes_undetermined() ));

    // Check counting.
    auto const sh = site_histogram( sset );
    auto const bf = base_frequencies( sset, nucleic_acid_codes_plain() );

    // Site histogram.
    EXPECT_EQ( 694,  sh.at('-') );
    EXPECT_EQ( 786,  sh.at('A') );
    EXPECT_EQ( 1175, sh.at('C') );
    EXPECT_EQ( 1100, sh.at('G') );
    EXPECT_EQ( 845,  sh.at('T') );

    // Base frequencies.
    EXPECT_DOUBLE_EQ( 0.20122887864823349, bf.at('A') );
    EXPECT_DOUBLE_EQ( 0.30081925243215568, bf.at('C') );
    EXPECT_DOUBLE_EQ( 0.2816180235535074,  bf.at('G') );
    EXPECT_DOUBLE_EQ( 0.21633384536610342, bf.at('T') );
}

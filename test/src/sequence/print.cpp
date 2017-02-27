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
#include "genesis/sequence/printers/bitmap.hpp"
#include "genesis/sequence/printers/simple.hpp"
#include "genesis/sequence/sequence_set.hpp"
#include "genesis/sequence/formats/fasta_reader.hpp"
#include "genesis/sequence/formats/phylip_reader.hpp"

#include <string>

using namespace genesis::sequence;

TEST( Sequence, PrinterSimple )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    SequenceSet sset;
    FastaReader().from_file(infile, sset);

    // Printer.
    auto printer = PrinterSimple();

    // Print Sequence.
    printer.label_mode( genesis::sequence::PrinterSimple::LabelMode::kSameLine );
    printer.length_limit(5);
    EXPECT_EQ(
        "Di106BGTue: TCGAA ...\n",
        printer( sset[0] )
    );

    printer.label_mode( genesis::sequence::PrinterSimple::LabelMode::kNone );
    EXPECT_EQ(
        "TCGAA ...\n",
        printer( sset[0] )
    );

    // Print Sequence Set
    printer.label_mode( genesis::sequence::PrinterSimple::LabelMode::kSameLine );
    printer.sequence_limit( 2 );
    EXPECT_EQ(
        "Di106BGTue: TCGAA ...\nDi145BGTue: TCGAA ...\n...\n",
        printer( sset )
    );
    printer.label_mode( genesis::sequence::PrinterSimple::LabelMode::kNone );
    EXPECT_EQ(
        "TCGAA ...\nTCGAA ...\n...\n",
        printer( sset )
    );

    // Print Color Sequence
    printer.label_mode( genesis::sequence::PrinterSimple::LabelMode::kSameLine );
    printer.color_mode (genesis::sequence::PrinterSimple::ColorMode::kBackground );
    printer.color_map( nucleic_acid_text_colors() );
    printer.length_limit( 3 );
    EXPECT_EQ(
        "Di106BGTue: \x1B[30;44mT\x1B[0m\x1B[30;42mC\x1B[0m\x1B[30;43mG\x1B[0m ...\n",
        printer( sset[0] )
    );

    printer.label_mode( genesis::sequence::PrinterSimple::LabelMode::kNone );
    EXPECT_EQ(
        "\x1B[30;44mT\x1B[0m\x1B[30;42mC\x1B[0m\x1B[30;43mG\x1B[0m ...\n",
        printer( sset[0] )
    );

    printer.label_mode( genesis::sequence::PrinterSimple::LabelMode::kSameLine );
    printer.color_mode (genesis::sequence::PrinterSimple::ColorMode::kForeground );
    EXPECT_EQ(
        "Di106BGTue: \x1B[34mT\x1B[0m\x1B[32mC\x1B[0m\x1B[33mG\x1B[0m ...\n",
        printer( sset[0] )
    );

    printer.label_mode( genesis::sequence::PrinterSimple::LabelMode::kNone );
    EXPECT_EQ(
        "\x1B[34mT\x1B[0m\x1B[32mC\x1B[0m\x1B[33mG\x1B[0m ...\n",
        printer( sset[0] )
    );

    // Print Color Sequence Set
    printer.label_mode( genesis::sequence::PrinterSimple::LabelMode::kSameLine );
    printer.color_mode (genesis::sequence::PrinterSimple::ColorMode::kBackground );
    EXPECT_EQ(
        "Di106BGTue: \x1B[30;44mT\x1B[0m\x1B[30;42mC\x1B[0m\x1B[30;43mG\x1B[0m ...\n"
        "Di145BGTue: \x1B[30;44mT\x1B[0m\x1B[30;42mC\x1B[0m\x1B[30;43mG\x1B[0m ...\n...\n",
        printer( sset )
    );

    printer.label_mode( genesis::sequence::PrinterSimple::LabelMode::kNone );
    EXPECT_EQ(
        "\x1B[30;44mT\x1B[0m\x1B[30;42mC\x1B[0m\x1B[30;43mG\x1B[0m ...\n"
        "\x1B[30;44mT\x1B[0m\x1B[30;42mC\x1B[0m\x1B[30;43mG\x1B[0m ...\n...\n",
        printer( sset )
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

TEST( Sequence, PrinterBitmap )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    // std::string infile = "/home/lucas/tmp/batch_aa/papara_alignment.batch_aa_0";
    SequenceSet sset;
    FastaReader().from_file(infile, sset);
    // PhylipReader().from_file(infile, sset);

    // Printer.
    std::ostringstream ost;
    auto printer = PrinterBitmap();
    printer.color_map( nucleic_acid_colors() );
    // printer.to_file( sset, "/home/lucas/dna.bmp" );
    printer.to_stream( sset, ost );
}

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

#include "genesis/genesis.hpp"

int main()
{
    using namespace genesis::sequence;

    // Read a fasta file into a SequenceSet object.
    SequenceSet sequences_a = FastaReader().from_file( "path/to/file_a.fasta" );

    // Read a phylip file into a SequenceSet object.
    SequenceSet sequences_b = PhylipReader().from_file( "path/to/file_b.phylip" );

    // Read more sequences into an existing SequenceSet object.
    FastaReader().from_file( "path/to/file_c.fasta", sequences_b );
    PhylipReader().from_file( "path/to/file_d.phylip", sequences_a );

    // Write data from a SequenceSet object to a fasta file.
    FastaWriter().to_file( sequences_b, "path/to/file_e.fasta" );

    // Write data from a SequenceSet object to a phylip file.
    PhylipWriter().to_file( sequences_a, "path/to/file_f.phylip" );

    // Instantiate objects and change some exemplary settings.
    auto fasta_reader = FastaReader();
    fasta_reader.valid_chars( "ACGT-" );
    auto phylip_writer = PhylipWriter();
    phylip_writer.line_length( 100 );

    // Iterate all Sequences in a SequenceSet and all their sites and print them.
    for( Sequence const& sequence : sequences_a ) {

        // Print the Sequnce label.
        std::cout << sequence.label() << ": ";

        // Iterate and print all sites of the Sequence.
        for( char const& site : sequence ) {
            std::cout << site;
        }
        // Alternatively, instead of the loop, you can use
        std::cout << sequence.sites();

        // Finish the print line.
        std::cout << std::endl;
    }

    // Print a SequenceSet as characters to the standard  output.
    std::cout << PrinterSimple().print( sequences_a );

    // Print a SequenceSet as pixels to a bitmap file.
    PrinterBitmap()
        .color_map( nucleic_acid_colors() )
        .to_file( sequences_a, "path/to/sits.bmp");
}

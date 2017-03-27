Sequence {#tutorials_sequence}
===========

# Sequences {#tutorials_sequence_sequences}

The basic class to represent a genetic sequence is called @ref genesis::sequence::Sequence "Sequence"
(quelle surprise). It stores a @ref genesis::sequence::Sequence::label() "label", i.e., a name for
the sequence, and its @ref genesis::sequence::Sequence::sites() "sites". The Sequence class itself
is agnostic of the format/encoding of its content, that is, whether it stores nucleotide or amino
acid or any other form of data. This offers flexibility when working with sequence data.
There are however some functions that are specialized for e.g., nucleotide sequences; that is, they
work with sequences of the characters `ACGT-`.

A sequence comes rarely alone. A collection of sequences is stored in a
@ref genesis::sequence::SequenceSet "SequenceSet". The sequences in such a set do not have to have
the same length, i.e., it is not an alignment.

The code examples in this tutorial assume that you use

~~~{.cpp}
#include "genesis/genesis.hpp"
using namespace genesis::sequence;
~~~

at the beginning of your code.

# Reading, Writing and Converting {#tutorials_sequence_read_write_convert}

Reading is done using reader classes like @ref genesis::sequence::FastaReader "FastaReader" and
@ref genesis::sequence::PhylipReader "PhylipReader". See there for details. Basic usage:

~~~{.cpp}
// Read a fasta file into a SequenceSet object.
SequenceSet sequences_a = FastaReader().from_file( "path/to/file_a.fasta" );

// Read a phylip file into a SequenceSet object.
SequenceSet sequences_b = PhylipReader().from_file( "path/to/file_b.phylip" );

// Read more sequences into an existing SequenceSet object.
FastaReader().from_file( "path/to/file_c.fasta", sequences_b );
PhylipReader().from_file( "path/to/file_d.phylip", sequences_a );
~~~

Writing is done the other way round, using e.g., @ref genesis::sequence::FastaWriter "FastaWriter"
or @ref genesis::sequence::PhylipWriter "PhylipWriter":

~~~{.cpp}
// Write data from a SequenceSet object to a fasta file.
FastaWriter().to_file( sequences_b, "path/to/file_e.fasta" );

// Write data from a SequenceSet object to a phylip file.
PhylipWriter().to_file( sequences_a, "path/to/file_f.phylip" );
~~~

All the readers and writers can also be normally stored in a variable, for example
in order to change their settings and then use them multiple times:

~~~{.cpp}
// Instantiate objects and change some exemplary settings.
auto fasta_reader = FastaReader();
fasta_reader.valid_chars( "ACGT-" );
auto phylip_writer = PhylipWriter();
phylip_writer.line_length( 100 );

// ... continue using fasta_reader and phylip_writer
~~~

Lastly, conversion between different sequence file formats is of course easily done by reading
it in one format, and writing it in another.

# Inspecting Sites and Printing {#tutorials_sequence_inspect_sites_and_print}

Access to the sites of a @ref genesis::sequence::Sequence "Sequence" is given via its member
function @ref genesis::sequence::Sequence::sites() "sites()".

It is also possible to directly iterate the Sequences in a SequenceSet and the single sites of a
Sequence:

~~~{.cpp}
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
~~~

As printing a @ref genesis::sequence::Sequence "Sequence" or a whole
@ref genesis::sequence::SequenceSet "SequenceSet" is common in order to inspect the sites, we offer a
class @ref genesis::sequence::PrinterSimple "PrinterSimple" that does this more easily and with
various settings:

~~~{.cpp}
// Print a SequenceSet as characters to the standard output.
std::cout << PrinterSimple().print( sequences_a );
~~~

It also offers printing using colors for the different sites (i.e., color each of the nucleotides
differently). See the class description of @ref genesis::sequence::PrinterSimple "PrinterSimple"
for details.

Furthermore, when dealing with many sequences, printing each character might be to much. For such
large datasets, we offer @ref genesis::sequence::PrinterBitmap "PrinterBitmap", which prints the
sites as pixels in a bitmap, each Sequence on a separate line, thus offering a more dense
representation of the data:

~~~{.cpp}
// Print a SequenceSet as pixels to a bitmap file.
PrinterBitmap()
    .color_map( nucleic_acid_colors() )
    .to_file( sequences_a, "path/to/sits.bmp");
~~~

# Consensus Sequences {#tutorials_sequence_consensus_sequences}

Often, it is desired to summarize a collection of Sequence%s into a consensus sequence. For this,
Genesis offers a couple of different algorithms:

  * @ref genesis::sequence::consensus_sequence_with_threshold( SequenceSet const&, double, bool, bool ) "consensus_sequence_with_threshold()",
    for nucleotide sequences (`ACGT`), that uses a threshold for the character frequency to determine
    the consensus at each site.
  * @ref genesis::sequence::consensus_sequence_with_ambiguities( SequenceSet const&, double, bool ) "consensus_sequence_with_ambiguities()",
    also for nucleotide sequences (`ACGT`), that uses a `similarity_factor` to calculate consensus with
    ambiguity characters.
  * @ref genesis::sequence::consensus_sequence_with_majorities( SequenceSet const&, std::string const&, bool, char ) "consensus_sequence_with_majorities()",
    for all kinds of site characters, that uses a majority rule to find the consensus at each site.

See the documentation of those functions (and their variants) for details.

Related to the calculation of consensus sequences is the calculation of the entropy of a collection
of Sequences. The entropy is a measure of information contained in the sites of such a collection.

We offer two modes of calculating the Sequence entropy:

  * @ref genesis::sequence::absolute_entropy() "absolute_entropy()"
  * @ref genesis::sequence::averaged_entropy() "averaged_entropy()"

as well as the single-site functions @ref genesis::sequence::site_entropy() "site_entropy()"
and @ref genesis::sequence::site_information() "site_information()".

Instead of a @ref genesis::sequence::SequenceSet "SequenceSet", they take a
@ref genesis::sequence::SequenceCounts "SequenceCounts" object as input, which is a summarization
of the occurence frequency of the sites in a SequenceSet. See there for details.

# Further Functions {#tutorials_sequence_further_functions}

Finally, we want to point out some other interesting functions:

  * @ref genesis::sequence::filter_min_max_sequence_length() "filter_min_max_sequence_length()"
    removes Sequences from a @ref genesis::sequence::SequenceSet "SequenceSet" that are either too
    short or too long.
  * @ref genesis::sequence::remove_characters( SequenceSet&, std::string const& ) "remove_characters()"
    allows to remove certain characters by deleting sites in Sequences.
  * @ref genesis::sequence::relabel_sha1( SequenceSet& ) "relabel_sha1()"
    sets new labels for all Sequences according to the @ref genesis::utils::SHA1 "SHA1" hash of the
    sequence sites.

There are more classes and functions to work with @ref genesis::sequence::Sequence "Sequences", see
@ref genesis::sequence "namespace sequence" for the full list.

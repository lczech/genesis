/**
* @brief Documentation strings for the Python module.
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include <map>
#include <string>

static std::map<std::string, std::string> doc_strings_ = {
    // Class JplaceReader
    {"static bool ::genesis::placement::JplaceReader::check_version (size_t version)", "Checks whether the version of the jplace format works with this parser.\n\nThis parser is intended for jplace versions 2 and 3. If while reading a different version tag is found, the reader will trigger a warning and try to continue anyway."},
    {"static bool ::genesis::placement::JplaceReader::check_version (std::string const & version)", "Checks whether the version of the jplace format works with this parser.\n\nThis parser is intended for jplace versions 2 and 3. If while reading a different version tag is found, the reader will trigger a warning and try to continue anyway."},
    {"Sample ::genesis::placement::JplaceReader::from_document (utils::JsonDocument & doc) const", "Take a JsonDocument and parse it as a Jplace document into a Sample."},
    {"Sample ::genesis::placement::JplaceReader::from_file (std::string const & fn) const", "Read a file and parse it as a Jplace document into a Sample."},
    {"SampleSet ::genesis::placement::JplaceReader::from_files (std::vector< std::string > const & fns) const", "Read a list of files and parse them as a Jplace document into a SampleSet."},
    {"void ::genesis::placement::JplaceReader::from_files (std::vector< std::string > const & fns, SampleSet & set) const", "Read a list of files and parse them as a Jplace document into a SampleSet.\n\nThe Samples are added to the SampleSet, so that existing Samples in the SampleSet are kept."},
    {"Sample ::genesis::placement::JplaceReader::from_stream (std::istream & is) const", "Read jplace data from a stream into a Sample."},
    {"Sample ::genesis::placement::JplaceReader::from_string (std::string const & jplace) const", "Parse a string as a Jplace document into a Sample."},
    {"SampleSet ::genesis::placement::JplaceReader::from_strings (std::vector< std::string > const & jps) const", "Parse a list of strings as a Jplace document into a SampleSet."},
    {"void ::genesis::placement::JplaceReader::from_strings (std::vector< std::string > const & jps, SampleSet & set) const", "Parse a list of strings as a Jplace document into a SampleSet.\n\nThe Samples are added to the SampleSet, so that existing Samples in the SampleSet are kept."},
    {"InvalidNumberBehaviour ::genesis::placement::JplaceReader::invalid_number_behaviour () const", "Return the currenlty set InvalidNumberBehaviour."},
    {"JplaceReader & ::genesis::placement::JplaceReader::invalid_number_behaviour (InvalidNumberBehaviour val)", "Set the InvalidNumberBehaviour.\n\nThis setter controls the InvalidNumberBehaviour of the JplaceReader. The default value is InvalidNumberBehaviour::kIgnore. See InvalidNumberBehaviour for the valid options.The function returns the JplaceReader object to allow for a fluent interface."},
    {"static std::string ::genesis::placement::JplaceReader::version ()", "Returns the version number that this class is written for. Currently, this is \"3\"."},

    // Class JplaceWriter
    {"size_t ::genesis::placement::JplaceWriter::branch_length_precision () const", "Get the precision used for printing the branch_length floating point numbers of the reference Tree of the Sample."},
    {"JplaceWriter & ::genesis::placement::JplaceWriter::branch_length_precision (size_t value)", "Set the precision used for printing the branch_length floating point numbers of the reference Tree of the Sample."},

    // Class PlacementEdgeData
    {"std::unique_ptr< BaseEdgeData > ::genesis::placement::PlacementEdgeData::clone () const", "Polymorphically copy an instance of this class. Use instead of copy constructor."},

    // Class PlacementNodeData
    {"std::unique_ptr< BaseNodeData > ::genesis::placement::PlacementNodeData::clone () const", "Polymorphically copy an instance of this class. Use instead of copy constructor."},

    // Class PlacementTreeNewickReaderMixin

    // Class PlacementTreeNewickWriterMixin

    // Class Pquery

    // Class PqueryName

    // Class PqueryPlacement

    // Class PqueryPlacementPlain

    // Class PqueryPlain

    // Class Sample
    {"Pquery & ::genesis::placement::Sample::add ()", "Create an empty Pquery, add it to the Sample and return it.\n\nThe returned reference can then be used to add PqueryPlacements and PqueryNames to the Pquery.As this function might reallocate the memory where Pqueries are stored, all iterators and pointers to the Pqueries of this Sample are invalidated."},
    {"Pquery & ::genesis::placement::Sample::add (Pquery const & other)", "Create a Pquery as a copy of the provided one, add it to the sample and return it.\n\nIt is possible to provide a Pquery from a different Sample, as long as the tree topology of both Samples is identical, including identical edge indices  and edge_nums. For that purpose, this function automatically adjusts the internal pointers of the Pquery and its PqueryPlacements accordingly. Furthermore, the proximal_length is also adjusted so that the relative position on the edge maintained.However, if the trees are incompatible (i.e., have a different topology, indices or edge nums), the PqueryPlacements will either point to different edges or the function might throw an exception, in cases where the tree does not have a corresponding edge at all. To further ensure correct behaviour, the function also checks whether the edge_num  is the same for the edge of the original PqueryPlacement and the new one, and throws an std::runtime_error if not.As this function might reallocate the memory where Pqueries are stored, all iterators and pointers to the Pqueries of this Sample are invalidated."},
    {"Pquery & ::genesis::placement::Sample::at (size_t index)", "Return the Pquery at a certain index."},
    {"Pquery const & ::genesis::placement::Sample::at (size_t index) const", "Return the Pquery at a certain index."},
    {"void ::genesis::placement::Sample::clear ()", "Clears all data of this object.\n\nThe Pqueries , the PlacementTree and the metadata are deleted."},
    {"void ::genesis::placement::Sample::clear_pqueries ()", "Clear all Pqueries  of this Sample.\n\nAll Pqueries are deleted. However, the PlacementTree and the metadata are left as they are. Thus this is a useful method for e.g., simulating placements: Take a copy of a given sample, clear its Pqueries, then generate new ones using the Simulator."},
    {"bool ::genesis::placement::Sample::empty () const", "Return whether the tree or the set of Pqueries is empty."},
    {"utils::Range< iterator_pqueries > ::genesis::placement::Sample::pqueries ()", "Return a Range iterator to the Pqueries .\n\nThis makes iterating Pqueries via a range based for loop easy."},
    {"utils::Range< const_iterator_pqueries > ::genesis::placement::Sample::pqueries () const", "Return a const Range iterator to the Pqueries .\n\nThis makes iterating Pqueries via a range based for loop easy."},
    {"void ::genesis::placement::Sample::remove (size_t index)", "Remove the Pquery at a given index from the Sample.\n\nAs this function might reallocate the memory where Pqueries are stored, all iterators and pointer to the Pqueries of this Sample are invalidated."},
    {"void ::genesis::placement::Sample::remove (size_t first_index, size_t last_index)", "Remove the Pqueries  between the first_index (inclusive) and the last_index (exclusive) from the Sample."},
    {"void ::genesis::placement::Sample::remove (iterator_pqueries position)", "Remove the Pquery at a given iterator position from the Sample."},
    {"void ::genesis::placement::Sample::remove (iterator_pqueries first, iterator_pqueries last)", "Remove the Pqueries  between the first (inclusive) and the last (exclusive) iterator position from the Sample."},
    {"size_t ::genesis::placement::Sample::size () const", "Return the number of Pqueries  that are stored in this Sample."},
    {"void ::genesis::placement::Sample::swap (Sample & other)", "Swap the contents of this Sample with the contents of another Sample."},
    {"PlacementTree & ::genesis::placement::Sample::tree ()", "Get the PlacementTree of this Sample."},
    {"PlacementTree const & ::genesis::placement::Sample::tree () const", "Get the PlacementTree of this Sample."},

    // Class SampleSerializer

    // Class SampleSet

    // Class Simulator

    // Class SimulatorEdgeDistribution
    {"size_t ::genesis::placement::SimulatorEdgeDistribution::generate ()", "Return a randomly chosen edge index, according to the distribution."},
    {"void ::genesis::placement::SimulatorEdgeDistribution::prepare (Sample const & sample)", "Prepare the distribution for usage. Needs to be called before generate()."},

    // Class SimulatorExtraPlacementDistribution

    // Class SimulatorLikeWeightRatioDistribution
    {"double ::genesis::placement::SimulatorLikeWeightRatioDistribution::generate ()", "Return a randomly chosen like weight ratio."},
    {"void ::genesis::placement::SimulatorLikeWeightRatioDistribution::prepare (Sample const & sample)", "Prepare the distribution for usage. Needs to be called before generate()."},

    // Class SimulatorPendantLengthDistribution
    {"double ::genesis::placement::SimulatorPendantLengthDistribution::generate (PlacementTreeEdge const & edge)", "Return a randomly chosen position on an edge."},
    {"void ::genesis::placement::SimulatorPendantLengthDistribution::prepare (Sample const & sample)", "Prepare the distribution for usage. Needs to be called before generate()."},

    // Class SimulatorProximalLengthDistribution
    {"double ::genesis::placement::SimulatorProximalLengthDistribution::generate (PlacementTreeEdge const & edge)", "Return a randomly chosen position on an edge."},
    {"void ::genesis::placement::SimulatorProximalLengthDistribution::prepare (Sample const & sample)", "Prepare the distribution for usage. Needs to be called before generate()."},

    // Class NamedSample

    // Class EdgeProximity

    // Class FastaInputIterator
    {"FastaReader & ::genesis::sequence::FastaInputIterator::reader ()", "Return the FastaReader used for this iterator.\n\nUse this to change the reading behaviour of the iterator. See FastaReader for details."},

    // Class FastaOutputIterator
    {"FastaWriter & ::genesis::sequence::FastaOutputIterator::writer ()", "Return the FastaWrtier used for this iterator.\n\nUse this to change the writing behaviour of the iterator. See FastaWriter for details."},

    // Class FastaReader
    {"void ::genesis::sequence::FastaReader::from_file (std::string const & file_name, SequenceSet & sequence_set) const", "Read all Sequences from a file in Fasta format into a SequenceSet.\n\nThe Sequences are added to the SequenceSet, whose existing Sequences are kept. Thus, by repeatedly calling this or similar read functions, multiple input files can easily be read into one SequenceSet."},
    {"SequenceSet ::genesis::sequence::FastaReader::from_file (std::string const & file_name) const", "Read all Sequences from a file in Fasta format and return them as a SequenceSet."},
    {"void ::genesis::sequence::FastaReader::from_stream (std::istream & input_stream, SequenceSet & sequence_set) const", "Read all Sequences from a std::istream in Fasta format into a SequenceSet.\n\nThe Sequences are added to the SequenceSet, whose existing Sequences are kept. Thus, by repeatedly calling this or similar read functions, multiple input files can easily be read into one SequenceSet."},
    {"SequenceSet ::genesis::sequence::FastaReader::from_stream (std::istream & input_stream) const", "Read all Sequences from a std::istream in Fasta format and return them as a SequenceSet."},
    {"void ::genesis::sequence::FastaReader::from_string (std::string const & input_string, SequenceSet & sequence_set) const", "Read all Sequences from a std::string in Fasta format into a SequenceSet.\n\nThe Sequences are added to the SequenceSet, whose existing Sequences are kept. Thus, by repeatedly calling this or similar read functions, multiple input files can easily be read into one SequenceSet."},
    {"SequenceSet ::genesis::sequence::FastaReader::from_string (std::string const & input_string) const", "Read all Sequences from a std::string in Fasta format and return them as a SequenceSet."},
    {"void ::genesis::sequence::FastaReader::parse_document (utils::InputStream & input_stream, SequenceSet & sequence_set) const", "Parse a whole fasta document into a SequenceSet.\n\nThis function is mainly used internally by the reading functions from_...(). It uses the currently set parsing_method() for parsing the data."},
    {"bool ::genesis::sequence::FastaReader::parse_sequence (utils::InputStream & input_stream, Sequence & sequence) const", "Parse a Sequence in Fasta format.\n\nThis function takes an InputStream and interprets it as a Fasta formatted sequence. It extracts the data and writes it into the given Sequence object. See the class description of FastaReader for the expected data format.The function stops after parsing one such sequence. It returns true if a sequence was extracted and false if the stream is empty. If the input is not in the correct format, an std::runtime_error exception is thrown indicating the malicious position in the input stream.This method has a maximum line length of utils::InputStream::BlockLength and reports errors only on the line where the sequence starts. If you have files with longer lines or want error reporting at the exact line and column where the error occurs, use ParsingMethod::kPedantic instead. See FastaReader::ParsingMethod for details."},
    {"bool ::genesis::sequence::FastaReader::parse_sequence_pedantic (utils::InputStream & input_stream, Sequence & sequence) const", "Parse a Sequence in Fasta format.\n\nThis function takes an InputStream and interprets it as a Fasta formatted sequence. It extracts the data and writes it into the given Sequence object. See the class description of FastaReader for the expected data format.The function stops after parsing one such sequence. It returns true if a sequence was extracted and false if the stream is empty. If the input is not in the correct format, an std::runtime_error exception is thrown indicating the malicious position in the input stream.Compared to parse_sequence(), this function allows for arbitrarily long lines and reports errors at the exact line and column where they occur. It is however slower. Apart from that, there are no differences. See FastaReader::ParsingMethod for details."},
    {"FastaReader & ::genesis::sequence::FastaReader::parsing_method (ParsingMethod value)", "Set the parsing method.\n\nThe parsing method is used for all the reader functions and parse_document(). See the ParsingMethod enum for details."},
    {"ParsingMethod ::genesis::sequence::FastaReader::parsing_method () const", "Return the currently set parsing method.\n\nSee the ParsingMethod enum for details."},
    {"FastaReader & ::genesis::sequence::FastaReader::to_upper (bool value)", "Set whether Sequence sites are automatically turned into upper case.\n\nIf set to true (default), all sites of the read Sequences are turned into upper case letters automatically. This is demanded by the Fasta standard. The function returns the FastaReader object to allow for fluent interfaces."},
    {"bool ::genesis::sequence::FastaReader::to_upper () const", "Return whether Sequence sites are automatically turned into upper case."},
    {"utils::CharLookup< bool > & ::genesis::sequence::FastaReader::valid_char_lookup ()", "Return the internal CharLookup that is used for validating the Sequence sites.\n\nThis function is provided in case direct access to the lookup is needed. Usually, the valid_chars() function should suffice. See there for details."},
    {"FastaReader & ::genesis::sequence::FastaReader::valid_chars (std::string const & chars)", "Set the chars that are used for validating Sequence sites when reading them.\n\nWhen this function is called with a string of chars, those chars are used to validate the sites when reading them. That is, only sequences consisting of the given chars are valid.If set to an empty string, this check is deactivated. This is also the default, meaning that no checking is done.In case that to_upper() is set to true: The validation is done after making the char upper case, so that only capital letters have to be provided for validation. In case that to_upper() is set to false: All chars that are to be considered valid have to be provided for validation.See nucleic_acid_codes...() and amino_acid_codes...() functions for presettings of chars that can be used for validation here."},
    {"std::string ::genesis::sequence::FastaReader::valid_chars () const", "Return the currently set chars used for validating Sequence sites.\n\nAn empty string means that no validation is done."},

    // Class FastaWriter

    // Class PhylipReader
    {"void ::genesis::sequence::PhylipReader::from_file (std::string const & file_name, SequenceSet & sequence_set) const", "Read all Sequences from a file in Phylip format into a SequenceSet.\n\nThe Sequences are added to the SequenceSet, whose existing Sequences are kept. Thus, by repeatedly calling this or similar read functions, multiple input files can easily be read into one SequenceSet."},
    {"SequenceSet ::genesis::sequence::PhylipReader::from_file (std::string const & file_name) const", "Read all Sequences from a file in Phylip format and return them as a SequenceSet."},
    {"void ::genesis::sequence::PhylipReader::from_stream (std::istream & input_stream, SequenceSet & sequence_set) const", "Read all Sequences from a std::istream in Phylip format into a SequenceSet.\n\nThe Sequences are added to the SequenceSet, whose existing Sequences are kept. Thus, by repeatedly calling this or similar read functions, multiple input files can easily be read into one SequenceSet.This function is only allowed for Mode::kSequential and Mode::kInterleaved. Automatic mode does not work, as the stream might need to be reset, which is not possible. See mode(Mode) for details."},
    {"SequenceSet ::genesis::sequence::PhylipReader::from_stream (std::istream & input_stream) const", "Read all Sequences from a std::istream in Phylip format and return them as a SequenceSet.\n\nThis function is only allowed for Mode::kSequential and Mode::kInterleaved. Automatic mode does not work, as the stream might need to be reset, which is not possible. See mode(Mode) for details."},
    {"void ::genesis::sequence::PhylipReader::from_string (std::string const & input_string, SequenceSet & sequence_set) const", "Read all Sequences from a std::string in Phylip format into a SequenceSet.\n\nThe Sequences are added to the SequenceSet, whose existing Sequences are kept. Thus, by repeatedly calling this or similar read functions, multiple input files can easily be read into one SequenceSet."},
    {"SequenceSet ::genesis::sequence::PhylipReader::from_string (std::string const & input_string) const", "Read all Sequences from a std::string in Phylip format and return them as a SequenceSet."},
    {"PhylipReader & ::genesis::sequence::PhylipReader::label_length (size_t value)", "Set the length of the label in front of the sequences.\n\nPhylip has the weird property that labels are written in front of sequences and do not need to have a delimiter, but instead are simply the first n characters of the string. This value determines after how many chars the label ends and the actual sequence begins.If set to a value greater than 0, exaclty this many characters are read as label. Thus, they can also contain spaces. Spaces at the beginning or end of a label are stripped. The length that is dictated by the Phylip standard is 10, but any other length can also be used.If set to 0 (default), a relaxed version of Phylip is used instead, where the sequence begin is automatically detected. Labels can then be of arbitrary lengths, as long as they do not contain white spaces. However, in this case, there has to be at least one space or tab character between the label and the sequence. After the whitespace(s), the rest of the line is then treated as sequence data.The function returns the PhylipReader object to allow for fluent interfaces."},
    {"size_t ::genesis::sequence::PhylipReader::label_length () const", "Return the currently set label length.\n\nSee the setter label_length( size_t ) for details."},
    {"PhylipReader & ::genesis::sequence::PhylipReader::mode (Mode value)", "Set the mode for reading sequences.\n\nPhylip offers two variants for storing the sequences: sequential and interleaved. As there is no option or flag in the file itself, there is no chance of knowing the variant without trying to parse it. If one fails but not the other, it is proabably the latter variant. However, there are instances where both variants are valid at the same time, but yield different sequences. So, in general detecting the correct variant is undecidable, making Phylip a non-well-defined format.In order to avoid those problems, this function explicitly sets the variant being used for parsing. By default, it is set to Mode::kSequential. Use Mode::kInterleaved for the other variant.We also offer a Mode::kAutomatic. It first tries to parse in sequential mode, and, if this fails, in interleaved mode. However, as this might involve starting from the beginning of the data, this is only possible with the from_file() and from_string() readers and does not work when using the from_stream() reader. Also, be aware that using automatic mode is slower because of implementation details induced by those limitations. Try to avoid automatic mode. If possible, try to avoid Phylip at all."},
    {"Mode ::genesis::sequence::PhylipReader::mode () const", "Return the currently set mode for parsing Phylip.See the setter mode( Mode ) for details."},
    {"Header ::genesis::sequence::PhylipReader::parse_phylip_header (utils::InputStream & it) const", "Parse a Phylip header and return the contained sequence count and length.\n\nThis helper function expects to find a Phylip header line in the form x y, which describes the number of sequences x in the Phylip data and their length y. The remainder of the header line is interpreted as Phylip options. See Header struct for more information.The function then advances the stream and skips potential empty lines after the header. It thus leaves the stream at the beginning of the first sequence line."},
    {"void ::genesis::sequence::PhylipReader::parse_phylip_interleaved (utils::InputStream & it, SequenceSet & sset) const", "Parse a whole Phylip file using the sequential variant (Mode::kSequential)."},
    {"std::string ::genesis::sequence::PhylipReader::parse_phylip_label (utils::InputStream & it) const", "Parse and return a Phylip label.\n\nThis helper functions either takes the first label_length chars as a label or, if label_length == 0 takes all chars until the first blank as label. It returns the trimmed label and leaves the stream at the next char after the label (and after subsequent blanks)."},
    {"std::string ::genesis::sequence::PhylipReader::parse_phylip_sequence_line (utils::InputStream & it) const", "Parse one sequence line.\n\nThe line (which can also start after a label) is parsed until the first '\\n' char. While parsing, the options to_upper() and valid_chars() are applied according to their settings. The stream is left at the beginning of the next line."},
    {"void ::genesis::sequence::PhylipReader::parse_phylip_sequential (utils::InputStream & it, SequenceSet & sset) const", "Parse a whole Phylip file using the interleaved variant (Mode::kInterleaved)."},
    {"PhylipReader & ::genesis::sequence::PhylipReader::to_upper (bool value)", "Set whether Sequence sites are automatically turned into upper case.\n\nIf set to true (default), all sites of the read Sequences are turned into upper case letters automatically. This is demanded by the Phylip standard.The function returns the PhylipReader object to allow for fluent interfaces."},
    {"bool ::genesis::sequence::PhylipReader::to_upper () const", "Return whether Sequence sites are automatically turned into upper case."},
    {"utils::CharLookup< bool > & ::genesis::sequence::PhylipReader::valid_char_lookup ()", "Return the internal CharLookup that is used for validating the Sequence sites.\n\nThis function is provided in case direct access to the lookup is needed. Usually, the valid_chars() function should suffice. See there for details."},
    {"PhylipReader & ::genesis::sequence::PhylipReader::valid_chars (std::string const & chars)", "Set the chars that are used for validating Sequence sites when reading them.\n\nWhen this function is called with a string of chars, those chars are used to validate the sites when reading them. That is, only sequences consisting of the given chars are valid.If set to an empty string, this check is deactivated. This is also the default, meaning that no checking is done.In case that to_upper() is set to true: The validation is done after making the char upper case, so that only capital letters have to be provided for validation. In case that to_upper() is set to false: All chars that are to be considered valid have to be provided for validation.See nucleic_acid_codes...() and amino_acid_codes...() functions for presettings of chars that can be used for validation here."},
    {"std::string ::genesis::sequence::PhylipReader::valid_chars () const", "Return the currently set chars used for validating Sequence sites.\n\nAn empty string means that no validation is done."},

    // Class PhylipWriter

    // Class PrinterBitmap
    {"PrinterBitmap & ::genesis::sequence::PrinterBitmap::color_map (std::map< char, utils::Color > value)", "Set the list of Colors to use for the Sequence charaters.\n\nThe provided map sets the colors for each Sequence character. The presettings nucleic_acid_colors() and amino_acid_colors() for default sequence types can be used as input for this parameter. If the colors map does not contain a key for one of the chars in the sequence, the char is printed in black."},
    {"std::map< char, utils::Color > const & ::genesis::sequence::PrinterBitmap::color_map () const", "Get the currently set list of Colors for each Sequence character.\n\nSee the setter for details."},

    // Class PrinterSimple

    // Class Sequence

    // Class SequenceCounts

    // Class SequenceSet
    {"reference ::genesis::sequence::SequenceSet::add (Sequence const & s)", "Add a Sequence to the SequenceSet by copying it, and return a reference to it."},
    {"reference ::genesis::sequence::SequenceSet::add (Sequence && s)", "Add a Sequence to the SequenceSet by moving it, and return a reference to it."},
    {"void ::genesis::sequence::SequenceSet::clear ()", "Remove all Sequences from the SequenceSet, leaving it with a size() of 0."},
    {"bool ::genesis::sequence::SequenceSet::empty () const", "Return whether the SequenceSet is empty, i.e. whether its size() is 0."},
    {"void ::genesis::sequence::SequenceSet::remove (size_t index)", "Remove the Sequence at a given index from the SequenceSet."},
    {"void ::genesis::sequence::SequenceSet::remove (size_t first_index, size_t last_index)", "Remove the Sequences between the first_index (inclusive) and the last_index (exclusive) from the SequenceSet."},
    {"void ::genesis::sequence::SequenceSet::remove (iterator position)", "Remove the Sequence at a given iterator position from the SequenceSet."},
    {"void ::genesis::sequence::SequenceSet::remove (iterator first, iterator last)", "Remove the Sequences between the first (inclusive) and the last (exclusive) iterator position from the SequenceSet."},
    {"size_t ::genesis::sequence::SequenceSet::size () const", "Return the number of Sequences in the SequenceSet."},

    // Class Header

    // Class BaseTaxonData
    {"std::unique_ptr< BaseTaxonData > ::genesis::taxonomy::BaseTaxonData::clone () const", "Polymorphically copy an instance of this class. Use instead of copy constructor."},
    {"static std::unique_ptr< BaseTaxonData > ::genesis::taxonomy::BaseTaxonData::create ()", "Create a new instance of this class. Use instead of default constructor."},

    // Class EntropyTaxonData
    {"std::unique_ptr< BaseTaxonData > ::genesis::taxonomy::EntropyTaxonData::clone () const", "Polymorphically copy an instance of this class. Use instead of copy constructor."},

    // Class IteratorPreorder

    // Class PrinterNested

    // Class PruneByEntropySettings

    // Class Taxon

    // Class Taxonomy

    // Class TaxonomyReader

    // Class Taxopath

    // Class TaxopathGenerator

    // Class TaxopathParser

    // Class Line

    // Class BaseEdgeData
    {"std::unique_ptr< BaseEdgeData > ::genesis::tree::BaseEdgeData::clone () const", "Polymorphically copy an instance of this class. Use instead of copy constructor."},
    {"static std::unique_ptr< BaseEdgeData > ::genesis::tree::BaseEdgeData::create ()", "Create a new instance of this class. Use instead of default constructor."},

    // Class BaseNodeData
    {"std::unique_ptr< BaseNodeData > ::genesis::tree::BaseNodeData::clone () const", "Polymorphically copy an instance of this class. Use instead of copy constructor."},
    {"static std::unique_ptr< BaseNodeData > ::genesis::tree::BaseNodeData::create ()", "Create a new instance of this class. Use instead of default constructor."},

    // Class Bipartition

    // Class BipartitionSet

    // Class CircularEdgeData
    {"std::unique_ptr< BaseEdgeData > ::genesis::tree::CircularEdgeData::clone () const", "Polymorphically copy an instance of this class. Use instead of copy constructor."},

    // Class CircularLayout

    // Class CircularNodeData
    {"std::unique_ptr< BaseNodeData > ::genesis::tree::CircularNodeData::clone () const", "Polymorphically copy an instance of this class. Use instead of copy constructor."},

    // Class ColorWriterMixin
    {"void ::genesis::tree::ColorWriterMixin::edge_colors (std::vector< utils::Color > const & color_vector)", "Set the edge colors that shall be written to the output.\n\nIf this function is called with a vector of size > 0, the edges in the output will be colored according to the values given as a parameter. The vector then needs to contain as many elements as the tree has edges. The elements need to be indexed using the edge.index() value.If this function is called with an empty vector, the color printing is reset to not print the edge colors that might have been set before."},
    {"std::vector< utils::Color > ::genesis::tree::ColorWriterMixin::edge_colors ()", "Return the edge colors that are currently set."},
    {"void ::genesis::tree::ColorWriterMixin::enable_color (bool value)", "Set whether colors tags are written to the output."},
    {"bool ::genesis::tree::ColorWriterMixin::enable_color () const", "Returns whether colors tags are written to the output."},
    {"void ::genesis::tree::ColorWriterMixin::ignored_color (utils::Color value)", "Set a color that is used as marker for partially disabling the output of color tags.\n\nAll edges that have set a color equal to the ignored color will produce no color tag output. This is thus something like \"magic pink\", where all parts of an image are rendered transparent when originially colored in pink.By default, we do not use an ignored color. If this option is activated, also use_ignored_color() is set to true."},
    {"utils::Color ::genesis::tree::ColorWriterMixin::ignored_color () const", "Return the currently set ignored color. See the setter for more information."},
    {"void ::genesis::tree::ColorWriterMixin::use_ignored_color (bool value)", "Set whether to use the ignored_color().\n\nIf this option is set to true, the color that is set via ignored_color() is not written as a color attribute to the output format."},
    {"bool ::genesis::tree::ColorWriterMixin::use_ignored_color () const", "Return whether currently an ignored color is used."},

    // Class DefaultEdgeData
    {"std::unique_ptr< BaseEdgeData > ::genesis::tree::DefaultEdgeData::clone () const", "Polymorphically copy an instance of this class. Use instead of copy constructor."},

    // Class DefaultNodeData
    {"std::unique_ptr< BaseNodeData > ::genesis::tree::DefaultNodeData::clone () const", "Polymorphically copy an instance of this class. Use instead of copy constructor."},

    // Class DefaultTreeNewickReaderMixin

    // Class DefaultTreeNewickWriterMixin

    // Class DefaultTreePhyloxmlWriterMixin

    // Class EmdEdgeData
    {"std::unique_ptr< BaseEdgeData > ::genesis::tree::EmdEdgeData::clone () const", "Polymorphically copy an instance of this class. Use instead of copy constructor."},

    // Class EmdNodeData
    {"std::unique_ptr< BaseNodeData > ::genesis::tree::EmdNodeData::clone () const", "Polymorphically copy an instance of this class. Use instead of copy constructor."},

    // Class IteratorEulertour

    // Class IteratorLevelorder

    // Class IteratorNodeLinks

    // Class IteratorPath
    {"bool ::genesis::tree::IteratorPath::is_last_common_ancestor () const", "Return whether the current iterator position (node) is the last common ancestor of the two start and finish nodes.\n\nThis is useful in many cases:\nFind the LCA (obviously).Check when the path is moving away from the root again.Iterating edges instead of nodes.\nThe last bullet point may need some explanation:The iterator visits all nodes between the start and the finish (both included). On the path between them, there is however one edge fewer than the number of visited nodes. That means, if you want to visit each edge on the path between two nodes (instead of each node), you need a way to spot this superflous edge. This function indicates the edge that needs to be skipped in this case."},
    {"bool ::genesis::tree::IteratorPath::is_lca () const", "Alias for is_last_common_ancestor(). See there for more information."},

    // Class IteratorPathSet

    // Class IteratorPostorder

    // Class IteratorPreorder

    // Class LayoutEdgeData
    {"std::unique_ptr< BaseEdgeData > ::genesis::tree::LayoutEdgeData::clone () const", "Polymorphically copy an instance of this class. Use instead of copy constructor."},

    // Class LayoutNodeData
    {"std::unique_ptr< BaseNodeData > ::genesis::tree::LayoutNodeData::clone () const", "Polymorphically copy an instance of this class. Use instead of copy constructor."},

    // Class NewickBroker

    // Class NewickBrokerElement
    {"int ::genesis::tree::NewickBrokerElement::rank () const", "Returns the rank (number of immediate children) of this node.\n\nNewickBroker::assign_ranks() has to be called before using this function. Otherwise, this function will throw an std::logic_error."},

    // Class NewickColorWriterMixin
    {"void ::genesis::tree::NewickColorWriterMixin::color_tag_prefix (std::string prefix)", "Set the prefix string that is used within the Newick comment before the actual color value.\n\nBy default, this string is set to &!color=, which is for example used by FigTree to mark color in trees. This will result in a Newick comment like [&!color=#c0ffee]."},
    {"std::string ::genesis::tree::NewickColorWriterMixin::color_tag_prefix ()", "Get the currently set prefix string. See the setter for more information."},
    {"void ::genesis::tree::NewickColorWriterMixin::color_tag_suffix (std::string suffix)", "Set the suffix string that is used within the Newick comment after the actual color value.\n\nBy default, this string is empty. See the setter color_tag_prefix() for more information."},
    {"std::string ::genesis::tree::NewickColorWriterMixin::color_tag_suffix ()", "Get the currently set suffix string. See the setter for more information."},

    // Class NewickLexer

    // Class NewickReader

    // Class NewickWriter

    // Class PhyloxmlColorWriterMixin

    // Class PhyloxmlWriter

    // Class PrinterCompact

    // Class PrinterDetailed

    // Class PrinterTable

    // Class RectangularEdgeData
    {"std::unique_ptr< BaseEdgeData > ::genesis::tree::RectangularEdgeData::clone () const", "Polymorphically copy an instance of this class. Use instead of copy constructor."},

    // Class RectangularLayout

    // Class RectangularNodeData
    {"std::unique_ptr< BaseNodeData > ::genesis::tree::RectangularNodeData::clone () const", "Polymorphically copy an instance of this class. Use instead of copy constructor."},

    // Class Tree

    // Class TreeEdge

    // Class TreeLink

    // Class TreeNode

    // Class TreeSet

    // Class StackElement

    // Class NamedTree

    // Class BaseInputSource
    {"size_t ::genesis::utils::BaseInputSource::read (char * buffer, size_t size)", "Read size many bytes into the char buffer."},
    {"std::string ::genesis::utils::BaseInputSource::source_name () const", "Get a name of the input source. Mainly interesting for user output."},

    // Class Bitvector
    {"void ::genesis::utils::Bitvector::flip (size_t index)", "Flips (inverts) the value of a single bit, with boundary check."},
    {"bool ::genesis::utils::Bitvector::get (size_t index) const", "Returns the value of a single bit, with boundary check."},
    {"void ::genesis::utils::Bitvector::set (size_t index)", "Sets the value of a single bit to true, with boundary check."},
    {"void ::genesis::utils::Bitvector::set (size_t index, bool value)", "Sets the value of a single bit to a given bool value, with boundary check."},
    {"size_t ::genesis::utils::Bitvector::size () const", "Returns the size (number of total bits) of this Bitvector."},
    {"void ::genesis::utils::Bitvector::unset (size_t index)", "Sets the value of a single bit to false, with boundary check."},

    // Class BmpWriter
    {"void ::genesis::utils::BmpWriter::to_file (Matrix< Color > const & image, std::string const & filename) const", "Write a full 24bit RGB Color image to a file."},
    {"void ::genesis::utils::BmpWriter::to_file (Matrix< unsigned char > const & image, std::string const & filename) const", "Write an 8 bit grayscale image to a file.\n\nThe entries of the given Matrix are simply translated into a grayscale image, where 0 gives black and 255 gives white pixels."},
    {"void ::genesis::utils::BmpWriter::to_file (Matrix< unsigned char > const & image, std::vector< Color > palette, std::string const & filename) const", "Write an 8 bit image with a Color palette to a file.\n\nThe given palette needs to contain exaclty 256 Color entries. The values of the given image Matrix are then mapped to the palette entries, e.g., the value at image( 2, 3 ) == 5 maps to the Color palette[ 5 ]."},
    {"void ::genesis::utils::BmpWriter::to_stream (Matrix< Color > const & image, std::ostream & outstream) const", "Write a full 24bit RGB Color image to a stream."},
    {"void ::genesis::utils::BmpWriter::to_stream (Matrix< unsigned char > const & image, std::ostream & outstream) const", "Write an 8 bit grayscale image to a stream.\n\nThe entries of the given Matrix are simply translated into a grayscale image, where 0 gives black and 255 gives white pixels."},
    {"void ::genesis::utils::BmpWriter::to_stream (Matrix< unsigned char > const & image, std::vector< Color > palette, std::ostream & outstream) const", "Write an 8 bit image with a Color palette to a stream.\n\nThe given palette needs to contain exaclty 256 Color entries. The values of the given image Matrix are then mapped to the palette entries, e.g., the value at image( 2, 3 ) == 5 maps to the Color palette[ 5 ]."},

    // Class CharLookup
    {"bool ::genesis::utils::CharLookup::all_equal_to (T comp_value) const", "Return whether all chars compare equal to a given value."},
    {"T ::genesis::utils::CharLookup::get (char c) const", "Return the lookup status for a given char."},
    {"std::string ::genesis::utils::CharLookup::get_chars_equal_to (T comp_value) const", "Return a std::string containg all chars which have lookup status equal to a given value."},
    {"void ::genesis::utils::CharLookup::set_all (T value)", "Set the lookup status for all chars at once."},
    {"void ::genesis::utils::CharLookup::set_char (char c, T value)", "Set the lookup status for a given char."},
    {"void ::genesis::utils::CharLookup::set_char_upper_lower (char c, T value)", "Set the lookup status for both the upper and lower case of a given char."},
    {"void ::genesis::utils::CharLookup::set_if (std::function< bool(char)> predicate, T value)", "Set the lookup status for all chars that fulfill a given predicate."},
    {"void ::genesis::utils::CharLookup::set_range (char first, char last, T value)", "Set the lookup status for all chars in an inlcuding range between two chars."},
    {"void ::genesis::utils::CharLookup::set_selection (std::string const & chars, T value)", "Set the lookup status for all chars that are contained in a given std::string."},
    {"void ::genesis::utils::CharLookup::set_selection_upper_lower (std::string const & chars, T value)", "Set the lookup status for both the upper and lower case of all chars that are contained in a given std::string."},

    // Class Color

    // Class ColorNames

    // Class CsvReader

    // Class Deserializer
    {"T ::genesis::utils::Deserializer::get_float ()", "Read a floating point number from the stream and return it."},
    {"void ::genesis::utils::Deserializer::get_float (T & res)", "Read an floating point number from the stream and store it in the result."},
    {"T ::genesis::utils::Deserializer::get_int ()", "Read an integer number from the stream and return it."},
    {"void ::genesis::utils::Deserializer::get_int (T & res)", "Read an integer number from the stream and store it in the result."},
    {"bool ::genesis::utils::Deserializer::get_null (size_t n)", "Reads n bytes from the stream and returns whether all of them are \\0 bytes."},
    {"T ::genesis::utils::Deserializer::get_plain ()", "Read as many bytes from the stream as the type T holds, and return them in form of a value of type T."},
    {"void ::genesis::utils::Deserializer::get_plain (T & res)", "Read as many bytes from the stream as the type T holds, and put them in the result value of type T."},
    {"void ::genesis::utils::Deserializer::get_raw (char * buffer, size_t n)", "Read n bytes from the stream and store them in the buffer.\n\nThe buffer needs to be big enough to hold n bytes."},
    {"std::string ::genesis::utils::Deserializer::get_raw_string (size_t n)", "Read n bytes from the stream and return them as a string."},
    {"std::string ::genesis::utils::Deserializer::get_string ()", "Read a string from the stream, provided that its length it written preceding it, as done by put_string()."},

    // Class FileInputSource
    {"void ::genesis::utils::FileInputSource::rewind ()", "Rewind the source to its start, so that it can be re-read."},

    // Class Histogram

    // Class HistogramAccumulator

    // Class IndexedList

    // Class IndexedListIterator

    // Class InputStream
    {"self_type & ::genesis::utils::InputStream::advance ()", "Move to the next char in the stream and advance the counters."},
    {"std::string ::genesis::utils::InputStream::at () const", "Return a textual representation of the current input position in the form \"line:column\"."},
    {"size_t ::genesis::utils::InputStream::column () const", "Return the current column of the input stream.\n\nThe counter starts with column 1 for each line of the input stream. New line characters \\n are included in counting and count as the last character of a line."},
    {"char ::genesis::utils::InputStream::current () const", "Return the current char, with some checks.\n\nThis function is similar to the dereference operator, but additionally performs two checks:\nEnd of input: If this function is called when there is no more data left in the input, it throws an runtime_error.Current char: This iterator is meant for ASCII (or similar) text format encodings with single bytes, and its output should be usable for lookup tables etc. Thus, this function ensures that the char is in the range [0, 127]. If not, an std::domain_error is thrown.\nUsually, those two conditions are checked in the parser anyway, so in most cases it is preferred to use the dereference operator instead."},
    {"bool ::genesis::utils::InputStream::eof () const", "Return true iff the input reached its end."},
    {"char ::genesis::utils::InputStream::get_char ()", "Extract a single char from the input.\n\nReturn the current char and move to the next one."},
    {"std::pair< char *, size_t > ::genesis::utils::InputStream::get_line ()", "Return the current line and move to the beginning of the next.\n\nThe function finds the end of the current line, starting from the current position. It returns a pointer to the current position and the length of the line. Furthermore, a null char is set at the end of the line, replacing the new line char. This allows downstream parses to directly use the returned pointer as a c-string.The stream is left at the first char of the next line."},
    {"bool ::genesis::utils::InputStream::good () const", "Return true iff the input is good (not end of data) and can be read from."},
    {"size_t ::genesis::utils::InputStream::line () const", "Return the current line of the input stream.\n\nThe counter starts with line 1 for input stream."},
    {"std::string ::genesis::utils::InputStream::source_name () const", "Get the input source name where this stream reads from.\n\nDepending on the type of input, this is either\n\"input string\",\"input stream\" or\"input file <filename>\"\nThis is mainly useful for user output like log and error messages."},

    // Class IteratorDeletions
    {"TwobitVector::WordType ::genesis::utils::IteratorDeletions::hash () const", "Get the hash value of the current vector."},
    {"size_t ::genesis::utils::IteratorDeletions::position () const", "Get the position that is currently deleted."},
    {"TwobitVector const & ::genesis::utils::IteratorDeletions::vector () const", "Get the current vector."},

    // Class IteratorInsertions
    {"TwobitVector::WordType ::genesis::utils::IteratorInsertions::hash () const", "Get the hash value of the current vector."},
    {"size_t ::genesis::utils::IteratorInsertions::position () const", "Get the position that is currently deleted."},
    {"TwobitVector const & ::genesis::utils::IteratorInsertions::vector () const", "Get the current vector."},

    // Class IteratorSubstitutions
    {"TwobitVector::WordType ::genesis::utils::IteratorSubstitutions::hash () const", "Get the hash value of the current vector."},
    {"size_t ::genesis::utils::IteratorSubstitutions::position () const", "Get the position that is currently deleted."},
    {"TwobitVector const & ::genesis::utils::IteratorSubstitutions::vector () const", "Get the current vector."},

    // Class JsonDocument
    {"static JsonDocument ::genesis::utils::JsonDocument::array (std::initializer_list< JsonDocument > init=std::initializer_list< )", "Explicitly create an array from an initializer list."},
    {"static JsonDocument ::genesis::utils::JsonDocument::boolean (BooleanType value)", "Explicitly create a boolean."},
    {"const_iterator ::genesis::utils::JsonDocument::cbegin () const", "Return a const iterator to the first element."},
    {"const_iterator ::genesis::utils::JsonDocument::cend () const", "Return a const iterator to one past the last element."},
    {"void ::genesis::utils::JsonDocument::clear ()", "Clears the content of a JSON value and resets it to the default value as if JsonDocument(ValueType) would have been called.\n\nThe values are cleared as follows:\nValue type initial value  \n\nnull null \n\nboolean false \n\nstring \"\" \n\nnumber 0 \n\nobject {} \n\narray [] \n\nIn order to complete eliminate a Json value, set it to nullptr instead."},
    {"size_type ::genesis::utils::JsonDocument::count (typename ObjectType::key_type key) const", "Return the number of occurrences of a key in a JSON object.\n\nAs we use a std::map for objects, this value is either 0 (key not found) or 1 (key found). For non-object types, the return value is always 0."},
    {"iterator ::genesis::utils::JsonDocument::find (typename ObjectType::key_type key)", "Find an element in a JSON object.\n\nFinds an element in a JSON object with key equivalent to key. If the element is not found or the JSON value is not an object, end() is returned."},
    {"const_iterator ::genesis::utils::JsonDocument::find (typename ObjectType::key_type key) const", "Find an element in a JSON object.\n\nFinds an element in a JSON object with key equivalent to key. If the element is not found or the JSON value is not an object, end() is returned."},
    {"bool ::genesis::utils::JsonDocument::is_array () const", "Return true iff the JSON value is an array."},
    {"bool ::genesis::utils::JsonDocument::is_boolean () const", "Return true iff the JSON value is a boolean."},
    {"bool ::genesis::utils::JsonDocument::is_null () const", "Return true iff the JSON value is null."},
    {"bool ::genesis::utils::JsonDocument::is_number () const", "Return true iff the JSON value is a number, i.e., a float or signed or unsigned integer."},
    {"bool ::genesis::utils::JsonDocument::is_number_float () const", "Return true iff the JSON value is a float number."},
    {"bool ::genesis::utils::JsonDocument::is_number_integer () const", "Return true iff the JSON value is a signed or unsigned integer number."},
    {"bool ::genesis::utils::JsonDocument::is_number_signed () const", "Return true iff the JSON value is a signed integer number."},
    {"bool ::genesis::utils::JsonDocument::is_number_unsigned () const", "Return true iff the JSON value is an unsigned integer number."},
    {"bool ::genesis::utils::JsonDocument::is_object () const", "Return true iff the JSON value is an object."},
    {"bool ::genesis::utils::JsonDocument::is_primitive () const", "Return true iff the JSON type is primitive (string, number, boolean, or null)."},
    {"bool ::genesis::utils::JsonDocument::is_string () const", "Return true iff the JSON value is a string."},
    {"bool ::genesis::utils::JsonDocument::is_structured () const", "Return true iff the JSON type is structured (array or object)."},
    {"static JsonDocument ::genesis::utils::JsonDocument::number_float (NumberFloatType value)", "Explicitly create a float number."},
    {"static JsonDocument ::genesis::utils::JsonDocument::number_signed (NumberSignedType value)", "Explicitly create a signed number."},
    {"static JsonDocument ::genesis::utils::JsonDocument::number_unsigned (NumberUnsignedType value)", "Explicitly create an unsigned number."},
    {"static JsonDocument ::genesis::utils::JsonDocument::object (std::initializer_list< JsonDocument > init=std::initializer_list< )", "Explicitly create an object from an initializer list."},
    {"void ::genesis::utils::JsonDocument::push_back (JsonDocument && val)", "Add a Json value to an array.\n\nAppends the given element val to the end of the JSON value. If the function is called on a JSON null value, an empty array is created before appending val.void push_back( JsonDocument&& val );"},
    {"void ::genesis::utils::JsonDocument::push_back (JsonDocument const & val)", "Add a Json value to an array.\n\nAppends the given element val to the end of the JSON value. If the function is called on a JSON null value, an empty array is created before appending val.void push_back( JsonDocument&& val );"},
    {"void ::genesis::utils::JsonDocument::push_back (typename ObjectType::value_type const & val)", "Add an object to an object.\n\nInserts the given element val to the JSON object. If the function is called on a JSON null value, an empty object is created before inserting val."},
    {"static JsonDocument ::genesis::utils::JsonDocument::string (std::string const & init)", "Explicitly create a string."},
    {"ValueType ::genesis::utils::JsonDocument::type () const", "Return the type of the JSON value."},

    // Class JsonIterator
    {"JsonDocument::ObjectType::key_type ::genesis::utils::JsonIterator::key () const", "return the key of an object iterator\n\nThe iterator is initialized; i.e. object_ != nullptr."},
    {"reference ::genesis::utils::JsonIterator::value () const", "return the value of an iterator\n\nThe iterator is initialized; i.e. object_ != nullptr."},

    // Class JsonReader
    {"JsonDocument ::genesis::utils::JsonReader::from_file (const std::string & filename) const", "Take a JSON document file path and parse its contents into a JsonDocument.\n\nIf the file does not exists, the function throws."},
    {"JsonDocument ::genesis::utils::JsonReader::from_stream (std::istream & input_stream) const", "Read from a stream containing a JSON document and parse its contents into a JsonDocument."},
    {"JsonDocument ::genesis::utils::JsonReader::from_string (const std::string & json) const", "Take a string containing a JSON document and parse its contents into a JsonDocument."},

    // Class JsonWriter
    {"size_t ::genesis::utils::JsonWriter::indent () const", "Get the indent used for printing the elements of Json arrays and objects."},
    {"JsonWriter & ::genesis::utils::JsonWriter::indent (size_t value)", "Set the indent used for printing the elements of Json arrays and objects.\n\nThe function returns a reference to the JsoNWriter to allow for fluent interfaces."},
    {"size_t ::genesis::utils::JsonWriter::precision () const", "Get the precision used for printing floating point numbers."},
    {"JsonWriter & ::genesis::utils::JsonWriter::precision (size_t value)", "Set the precision used for printing floating point numbers.\n\nThe function returns a reference to the JsoNWriter to allow for fluent interfaces."},
    {"void ::genesis::utils::JsonWriter::to_file (JsonDocument const & document, std::string const & filename) const", "Write a JsonDocument to a file.\n\nIf the file already exists or cannot be written to, the function throws std::runtime_error."},
    {"void ::genesis::utils::JsonWriter::to_stream (JsonDocument const & document, std::ostream & out) const", "Write a JsonDocument to a stream."},
    {"void ::genesis::utils::JsonWriter::to_string (JsonDocument const & document, std::string & output) const", "Give the Json string representation of a JsonDocument."},
    {"std::string ::genesis::utils::JsonWriter::to_string (JsonDocument const & document) const", "Return the Json representation of a JsonDocument."},

    // Class Lexer
    {"LexerToken ::genesis::utils::Lexer::back () const", "Returns a reference to the last token.\n\nCalling this function on an empty() lexer causes undefined behavior."},
    {"void ::genesis::utils::Lexer::clear ()", "Clears all tokens, as if the object was newly created.\n\nThe options of the lexer are however not changed."},
    {"bool ::genesis::utils::Lexer::empty () const", "Returns whether the list of tokens is empty.\n\nThis is usually the case before ProcessString() was run."},
    {"bool ::genesis::utils::Lexer::has_error () const", "Returns whether there appeared an error while lexing."},
    {"size_t ::genesis::utils::Lexer::size () const", "Returns the number of tokens produced during the analysis process."},

    // Class LexerIterator
    {"void ::genesis::utils::LexerIterator::consume_head ()", "Removes all tokens up to (but excluding) the current one.\n\nThis is a slow operation, thus should not be called in the inner parsing loop. It is meant to save memory when reading and parsing large files."},
    {"bool ::genesis::utils::LexerIterator::has_error () const", "Returns whether there appeared an error while lexing."},

    // Class LexerToken
    {"std::string ::genesis::utils::LexerToken::at () const", "Shortcut that returns \"line:column\" (e.g., for logging)."},
    {"size_t ::genesis::utils::LexerToken::column () const", "Getter for the column where this token occured."},
    {"size_t ::genesis::utils::LexerToken::has_new_lines () const", "Returns the number of new line characters, if this token is a whitespace."},
    {"bool ::genesis::utils::LexerToken::is_bracket () const", "Shortcut to check if this is a bracket token.\n\nThere is an overload of this function that also checks if the token is a specific type of bracket."},
    {"bool ::genesis::utils::LexerToken::is_bracket (const std::string & br) const", "Returns whether this token is a given type of bracket.\n\nUsage: token.is_bracket(\")\") will return true if this token is of LexerTokenType kBracket and if it is the closing parenthesis. This is a shortcut for testing type and value at the same time."},
    {"bool ::genesis::utils::LexerToken::is_comment () const", "Shortcut to check if this is a comment token."},
    {"bool ::genesis::utils::LexerToken::is_error () const", "Shortcut to check if this is an error token."},
    {"bool ::genesis::utils::LexerToken::is_number () const", "Shortcut to check if this is a number token."},
    {"bool ::genesis::utils::LexerToken::is_operator () const", "Shortcut to check if this is an operator token.\n\nThere is an overload of this function that also checks if the token is a specific type of operator."},
    {"bool ::genesis::utils::LexerToken::is_operator (const std::string & op) const", "Returns whether this token is a given type of operator.\n\nUsage: token.is_operator(\"%\") will return true if this token is of LexerTokenType kOperator and if it is the modulo operator. This is a shortcut for testing type and value at the same time."},
    {"bool ::genesis::utils::LexerToken::is_string () const", "Shortcut to check if this is a string token."},
    {"bool ::genesis::utils::LexerToken::is_symbol () const", "Shortcut to check if this is a symbol token."},
    {"bool ::genesis::utils::LexerToken::is_tag () const", "Shortcut to check if this is a tag token."},
    {"bool ::genesis::utils::LexerToken::is_unknown () const", "Shortcut to check if this is an unknown token."},
    {"bool ::genesis::utils::LexerToken::is_white () const", "Shortcut to check if this is a whitespace token."},
    {"size_t ::genesis::utils::LexerToken::line () const", "Getter for the line where this token occured."},
    {"LexerTokenType ::genesis::utils::LexerToken::type () const", "Getter for the LexerTokenType of this token."},
    {"std::string ::genesis::utils::LexerToken::type_to_string () const", "Returns the string representation for the LexerTokenType of this token."},
    {"std::string ::genesis::utils::LexerToken::value () const", "Getter for the string value of this token."},

    // Class Logging
    {"static LoggingLevel ::genesis::utils::Logging::max_level ()", "Get the highest log level that is reported."},
    {"static int ::genesis::utils::Logging::report_percentage ()", "Get the current percentage for reporting LOG_PROG messages."},

    // Class LoggingDetails

    // Class LoggingScopeLevel

    // Class Matrix

    // Class MeanStddevPair

    // Class MinMaxPair

    // Class NexusBlock

    // Class NexusDocument

    // Class NexusTaxa

    // Class NexusTrees

    // Class NexusWriter

    // Class Options
    {"static std::string ::genesis::utils::Options::build_type ()", "Return the build type that was used to compile the binary, i.e., \"debug\" or \"release\"."},
    {"std::vector< std::string > ::genesis::utils::Options::command_line () const", "Returns an array of strings containing the program's command line arguments."},
    {"void ::genesis::utils::Options::command_line (int const argc, char const *const * argv)", "Set arguments to the program's command line options.\n\nIf the program is run from the command line, this method has to be used to properly propagate the command line options to this options class."},
    {"std::string ::genesis::utils::Options::command_line_string () const", "Returns a string containing the program's command line arguments."},
    {"static std::string ::genesis::utils::Options::compiler_family ()", "Return the compiler family (name) that was used to compile genesis.\n\nSee compiler_version() to get the version of the compiler."},
    {"static std::string ::genesis::utils::Options::compiler_version ()", "Return the compiler version that was used to compile genesis.\n\nSee compiler_family() to get the corresponding compiler name."},
    {"static std::string ::genesis::utils::Options::cpp_version ()", "Return the CPP version that was used to compile genesis."},
    {"static Options & ::genesis::utils::Options::get ()", "Returns a single instance of this class."},
    {"std::string ::genesis::utils::Options::info () const", "Return a list with compile time and run time options with their values."},
    {"static bool ::genesis::utils::Options::is_big_endian ()", "Return whether the system uses big endian memory."},
    {"static bool ::genesis::utils::Options::is_debug ()", "Return whether the binary was compiled with build type DEBUG."},
    {"static bool ::genesis::utils::Options::is_little_endian ()", "Return whether the system uses little endian memory."},
    {"static bool ::genesis::utils::Options::is_release ()", "Return whether the binary was compiled with build type RELEASE."},
    {"unsigned int ::genesis::utils::Options::number_of_threads () const", "Returns the number of threads."},
    {"void ::genesis::utils::Options::number_of_threads (const unsigned int number)", "Overwrite the system given number of threads.\n\nOn startup, the value is initialized with the actual number of cores available in the system using std::thread::hardware_concurrency(). This method overwrites this value."},
    {"static std::string ::genesis::utils::Options::platform ()", "Return the platform under which genesis was compiled.\n\nThis can be either \"Win32\", \"Linux\", \"Apple\", \"Unix\" or \"Unknown\"."},
    {"std::default_random_engine & ::genesis::utils::Options::random_engine ()", "Returns the default engine for random number generation.\n\nCaveat: This is not intended for the use in more than one thread. As the order of execution in threads is not deterministic, results would not be reproducible, even when using a fixed seed. Furthermore, it might be a speed bottleneck to add a mutex to this method.If in the future there is need for multi-threaded random engines, they needed to be outfitted with separate seeds each (otherwise they would all produce the same results). Thus, for now we simply assume single-threaded use."},
    {"unsigned ::genesis::utils::Options::random_seed () const", "Returns the random seed that was used to initialize the engine."},
    {"void ::genesis::utils::Options::random_seed (const unsigned seed)", "Set a specific seed for the random engine.\n\nOn startup, the random engine is initialized using the current system time. This value can be overwritten using this method."},
    {"bool ::genesis::utils::Options::using_openmp () const", "Return whether the binary was compiled using OpenMP."},
    {"bool ::genesis::utils::Options::using_pthreads () const", "Return whether the binary was compiled using Pthreads."},

    // Class PcaData

    // Class Range

    // Class RangeMinimumQuery

    // Class SHA1

    // Class Serializer
    {"void ::genesis::utils::Serializer::put_float (const T v)", "Write a floating point number to the stream.\n\nCurrently, this simply uses put_plain(), but future versions might change this behaviour and convert it to some machine-independent format."},
    {"void ::genesis::utils::Serializer::put_int (const T v)", "Write an integer number to the stream.\n\nCurrently, this simply uses put_plain(), but future versions might change this behaviour and use specific conversions (litte/big endianness, signed/unsigned) before writing."},
    {"void ::genesis::utils::Serializer::put_null (const size_t n)", "Write n zero bytes (\\0) to the stream."},
    {"void ::genesis::utils::Serializer::put_plain (const T v)", "Write plain data to the stream, by casting it to a char array."},
    {"void ::genesis::utils::Serializer::put_raw (char * data, size_t n)", "Write raw data, provided as a char array of length n, to the stream."},
    {"void ::genesis::utils::Serializer::put_raw_string (const std::string & v)", "Write raw data, provided as a string, to the stream, without writing its length."},
    {"void ::genesis::utils::Serializer::put_string (const std::string & v)", "Write a string, preceded by its length, to the stream. Use get_string() to read it."},

    // Class SortedVector
    {"bool ::genesis::utils::SortedVector::contains (const_reference value) const", "Return whether a certain value is present in the container."},
    {"size_type ::genesis::utils::SortedVector::index_of (const_reference value) const", "Return the index at which a certain value is stored, or size(), if it is not present in the container."},
    {"void ::genesis::utils::SortedVector::insert (const_reference value)", "Insert a value into the container by copying it.\n\nIf a value comparing equal to the new one is already stored in the container, nothing happens."},
    {"void ::genesis::utils::SortedVector::insert (value_type && value)", "Insert a value into the container by moving it.\n\nIf a value comparing equal to the new one is already stored in the container, nothing happens."},
    {"void ::genesis::utils::SortedVector::insert (InputIterator first, InputIterator last)", "Insert values into the container by copying from an InputIterator range.\n\nThe function copies all values in the range [ first, last ) into the container."},
    {"void ::genesis::utils::SortedVector::remove (const_reference value)", "Remove a certain value from the container.\n\nIf the value is not stored in the container, nothing happens."},
    {"void ::genesis::utils::SortedVector::reserve (size_t n)", "Reserve space in the unterlying vector."},

    // Class StreamInputSource

    // Class StringInputSource
    {"void ::genesis::utils::StringInputSource::rewind ()", "Rewind the source to its start, so that it can be re-read."},

    // Class Style

    // Class SvgBox

    // Class SvgCircle

    // Class SvgDefinitions

    // Class SvgDocument
    {"self_type & ::genesis::utils::SvgDocument::add (SvgObject const & object)", "Add an SvgObject to the document.\n\nReturns the SvgDocument in order to allow for a fluent interface."},

    // Class SvgDrawingOptions

    // Class SvgEllipse

    // Class SvgFill

    // Class SvgFont

    // Class SvgGradientLinear

    // Class SvgGradientStop

    // Class SvgGroup
    {"self_type & ::genesis::utils::SvgGroup::add (SvgObject const & object)", "Add an SvgObject to the document.\n\nReturns the SvgDocument in order to allow for a fluent interface."},

    // Class SvgLine

    // Class SvgMargin

    // Class SvgObject

    // Class SvgPath

    // Class SvgPoint

    // Class SvgPolygon

    // Class SvgPolyline

    // Class SvgRect

    // Class SvgSize

    // Class SvgStroke

    // Class SvgText

    // Class SvgTransform

    // Class SvgUse

    // Class SynchronousReader

    // Class Table

    // Class TableLayout

    // Class Tickmarks
    {"std::vector< LabeledTick > ::genesis::utils::Tickmarks::linear_labels (double min, double max, size_t target_steps)", "Return a set of labels with relative positions between min and max, where the labels correspond to the linear_ticks() numbers.\n\nThis function is a convenience wrapper for linear_ticks(), that turns the tick positions into relative values, while keeping the actual tick value as a label. Relative position 0.0 corresponds to min, relative position 1.0 to max."},
    {"std::vector< double > ::genesis::utils::Tickmarks::linear_ticks (double min, double max, size_t target_steps)", "Calculate a set of ticks that linearily span from min to max in approximately target_steps many steps, where each tick sits on a \"nice\" number.\n\nSee step_size() for what is meant by nice numbers."},
    {"std::vector< LabeledTick > ::genesis::utils::Tickmarks::logarithmic_labels (double max)", "Return a set of labels with relative positions between min and max, where the labels correspond to logarithmic ticks.\n\nThis function is the equivalent of linear_labels(), but instead uses logarithmic scaling. Also, it always starts at a minimum of 1 and inserts as many ticks as are needed to go up to max in powers of ten.Example: Given max == 350, with overshoot_at_max == false and include_max == true, the resulting vector contains entries with labels 1, 10, 100, 350.Remark: There is no logarithmic equivalent for linear_ticks(), as those would simply be powers of 10 up to max."},
    {"static double ::genesis::utils::Tickmarks::step_size (double interval_size, size_t target_steps)", "Calculate a step size that fills the interval_size in approximately target_steps many steps.\n\nThe resulting step size is calculated to be a \"nice\" size, i.e. fits [ 1 | 2 | 5 ] * 10^n. Thus, exemplary nice step sizes are 0.01, 20 and 500."},

    // Class TridiagonalDecompositionData

    // Class TwobitVector

    // Class XmlComment

    // Class XmlDocument

    // Class XmlElement

    // Class XmlMarkup

    // Class XmlValue

    // Class XmlWriter

    // Class BitmapFileheader

    // Class BitmapInfo

    // Class BitmapInfoheader

    // Class RgbQuad

    // Class InternalIterator

    // Class PrimitiveIterator

    // Class Entry

    // Class Concept

    // Class Model

    // Class Concept

    // Class Model

    // Class Matrix

    // Class Rotate

    // Class Scale

    // Class Skew

    // Class Transformation

    // Class Translate

    // Class Concept

    // Class Model

    // Class Column

    // Class Binder

    // Class Line

    // Class LabeledTick


    // Functions
    {"std::string ::genesis::utils::join (T const & v, std::string const & delimiter)", "Return a string where the elements of a container v are joined using the string delimiter in between them.\n\nThe container is iterated via its range based for loop, thus it needs to have begin() and end() functions.For turning the elements of the container into a string, their operator << is used. Thus, this function can used with all types that support this operator."},
    {"std::string ::genesis::utils::lexer_token_type_to_string (const LexerTokenType t)", "Converts a LexerTokenType into its string representation."},
    {"long ::genesis::utils::logging_progress_value (long value=-1)", "Hack function to make sure that the value arugment in LOG_PROG is only evaluated once.\n\nWithout this function, LOG_PROG would include two appearances of its variable value, which means that a statement like LOG_PROG(++i, n) << \"of progress.\";\nwould lead to a double evaluation of the increment statement ++i. That is not intended, thus we need this hack function."},
    {"std::unique_ptr< T > ::genesis::utils::make_unique (Args &&... args)", "Returns a std::unique_ptr for a given type.\n\nSince this is not available in C++11, we need our own implementation. It is following http://herbsutter.com/gotw/_102/"},
    {"std::vector< MeanStddevPair > ::genesis::utils::matrix_col_mean_stddev (Matrix< double > const & data)", "Calcualte the column-wise mean and standard deviation of a Matrix.\n\nSee also matrix_row_mean_stddev()."},
    {"std::vector< MinMaxPair< double > > ::genesis::utils::matrix_col_minmax (Matrix< double > const & data)", "Calculate the column-wise min and max values of a Matrix.\n\nSee also matrix_row_minmax()."},
    {"Matrix< double > ::genesis::utils::matrix_multiplication (Matrix< double > const & a, Matrix< double > const & b)", "Calculate the product of two Matrices.\n\nThe two matrices need to have fitting sized, i.e., a[ l, m ] x b[ m, n ]."},
    {"std::vector< MeanStddevPair > ::genesis::utils::matrix_row_mean_stddev (Matrix< double > const & data)", "Calcualte the row-wise mean and standard deviation of a Matrix.\n\nSee also matrix_col_mean_stddev()."},
    {"std::vector< MinMaxPair< double > > ::genesis::utils::matrix_row_minmax (Matrix< double > const & data)", "Calculate the row-wise min and max values of a Matrix.\n\nSee also matrix_col_minmax()."},
    {"double ::genesis::utils::mean (const Histogram & h)", "Compute the bin-weighted arithmetic mean.\n\nThe histogram is regarded as a probability distribution. Negative bin values are ignored for the purposes of this calculation. The accuracy of the result is limited by the bin width."},
    {"std::vector< MinMaxPair< double > > ::genesis::utils::normalize_cols (Matrix< double > & data)", "Normalize the columns of a Matrix so that all values are in the range [ 0.0, 1.0 ].\n\nThe Matrix is manipulated inline. For each column, the new values of the Matrix are calculated as $ x_{new} = \\frac{ x - x_{min} }{ x_{max} - x_{min} } $.The function returns a vector containing the min and max values of the columns before normalization, see matrix_col_minmax().\n\ndata\n\n\nInput data Matrix; normalization is done inline. \n\n\nVector containing the min and max values for each column before normalization."},
    {"std::vector< MinMaxPair< double > > ::genesis::utils::normalize_rows (Matrix< double > & data)", "Normalize the rows of a Matrix so that all values are in the range [ 0.0, 1.0 ].\n\nThe Matrix is manipulated inline. For each row, the new values of the Matrix are calculated as $ x_{new} = \\frac{ x - x_{min} }{ x_{max} - x_{min} } $.The function returns a vector containing the min and max values of the rows before normalization, see matrix_row_minmax().\n\ndata\n\n\nInput data Matrix; normalization is done inline. \n\n\nVector containing the min and max values for each row before normalization."},
    {"bool ::genesis::utils::operator& (SkipWhitespace lhs, SkipWhitespace rhs)", "And-operator to check whether a SkipWhitespace is set.\n\nTypical usage: SkipWhitespace option;\nif( option & SkipWhitespace::kTrailing ) {\n    // Do stuff...\n}\nSee SkipWhitespace for more information."},
    {"T ::genesis::utils::parse_float (utils::InputStream & source)", "Read a floating point number from a stream and return it.\n\nThe number is expected to be in the following format: [+-][123][.456][eE[+-]789]\nThe function stops reading at the first non-fitting digit. It throws an std::overflow_error or underflow_error in case that the exponent (the part after the 'E') does not fit into integer value range."},
    {"T ::genesis::utils::parse_integer (utils::InputStream & source)", "Alias for parse_signed_integer()."},
    {"std::string ::genesis::utils::parse_number_string (utils::InputStream & source)", "Read a general number string from an input stream.\n\nThe function reads from the input as long as it is a valid number of the format [+-][123][.456][eE[+-]789]\nand returns this string. This is useful for intermediately parsing a number and then checking whether it is valid. Use parse_float() to turn such a number string into an actual float/double."},
    {"std::string ::genesis::utils::parse_quoted_string (utils::InputStream & source, bool use_escapes=true, bool use_twin_quotes=false, bool include_qmarks=false)", "Read a string in quotation marks from a stream and return it.\n\nThe first char of the stream is considered to be the opening quotation mark. Everything up until the closing quotation mark (the same char again, whatever it is) is then read. The stream is then pointing to the char right after the closing quotation mark.If the string ends prematurely, i.e., without the closing quotation mark, or right after a backslash if use_escapes is used, the function throws an std::runtime_error.\n\nsource\n\n\nStream to read from.\n\n\n\nuse_escapes\n\n\nIf this optional parameter is set to true, chars preceeded by a backslash \\ are considered to be \"escaped\". If the char following the backslash is any of r, n or t, it is turned into its respective white space equivalent, while all other chars are copied verbatim to the return string. Thus, by escaping it, a the string can also include the quotation mark itself. Default of this parameter is true.\n\n\n\nuse_twin_quotes\n\n\nIf this optional parameter is set to true, the quotation mark itself can be escaped using two consecutive quotation marks. This works in addition to escaping it with a backslash (when use_escapes is used).\n\n\n\ninclude_qmarks\n\n\nThe optional parameter include_qmarks demtermines whether the quotation marks are included in the output or not. Default is false."},
    {"T ::genesis::utils::parse_signed_integer (utils::InputStream & source)", "Read a signed integer from a stream and return it.\n\nThe function expects a sequence of digits, possibly with a leading + or -. The first char after that has to be a digit, otherwise the function throws std::runtime_error. It stops reading at the first non-digit. In case the value range is too small, the function throws std::overflow_error, or underflow_error, respectively."},
    {"T ::genesis::utils::parse_unsigned_integer (utils::InputStream & source)", "Read an unsigned integer from a stream and return it.\n\nThe function expects a sequence of digits. The current char in the stream has to be a digit, otherwise the function throws std::runtime_error. It stops reading at the first non-digit. In case the value range is too small, the function throws std::overflow_error."},
    {"PcaData ::genesis::utils::principal_component_analysis (Matrix< double > const & data, size_t components=0, PcaStandardization standardization)", "Perfom a Principal Component Analysis on a given data Matrix.\n\nPrincipal Components Analysis or the Karhunen-Loeve expansion is a classical method for dimensionality reduction or exploratory data analysis. One reference among many is: F. Murtagh and A. Heck, Multivariate Data Analysis, Kluwer Academic, Dordrecht, 1987.\n\ndata\n\n\nMatrix with the data, samples in rows, features in columns. \n\n\n\ncomponents\n\n\nIntended number of PCA components to calculate. If 0 (default), all PCA components are calculated, i.e., as many components as data has columns. \n\n\n\nstandardization\n\n\nIndicate the standardization algorithm to perfom on the data before calculating the PCA components, see PcaStandardization. \n\n\nA struct that contains the eigenvalues and corresponding eigenvectors (i.e., the PCA components), and a Matrix with the projected data. See PcaData for details."},
    {"char ::genesis::utils::read_char_or_throw (InputStream & source, char criterion, SkipWhitespace skip_ws)", "Lexing function that reads a single char from the stream and checks whether it equals the provided one.\n\nIf not, the function throws std::runtime_error. The stream is advanced by one position and the char is returned. For a similar function that checks the value of the current char but does not advance in the stream, see affirm_char_or_throw().Using the parameter skip_ws, it is possible to skip leading and/or trailing whitespaces before/after treating the criterion char. See SkipWhitespace for more information."},
    {"char ::genesis::utils::read_char_or_throw (InputStream & source, std::function< bool(char)> criterion, SkipWhitespace skip_ws)", "Lexing function that reads a single char from the stream and checks whether it fulfills the provided criterion.\n\nIf not, the function throws std::runtime_error. The stream is advanced by one position and the char is returned. For a similar function that checks the value of the current char but does not advance in the stream, see affirm_char_or_throw().Using the parameter skip_ws, it is possible to skip leading and/or trailing whitespaces before/after treating the criterion char. See SkipWhitespace for more information."},
    {"std::string ::genesis::utils::read_to_end_of_line (InputStream & source)", "Lexing function that reads until the end of the line (i.e., to the new line char), and returns the read chars (excluding the new line char).\n\nThe stream is left at the new line char. If the stream is already at its last line, this function reads to the end of the stream."},
    {"std::string ::genesis::utils::read_until (InputStream & source, char criterion)", "Lexing function that reads from the stream until its current char equals the provided one. The read chars are returned."},
    {"std::string ::genesis::utils::read_until (InputStream & source, std::function< bool(char)> criterion)", "Lexing function that reads from the stream until its current char fulfills the provided criterion. The read chars are returned."},
    {"std::string ::genesis::utils::read_while (InputStream & source, char criterion)", "Lexing function that reads from the stream while its current char equals the provided one. The read chars are returned."},
    {"std::string ::genesis::utils::read_while (InputStream & source, std::function< bool(char)> criterion)", "Lexing function that reads from the stream while its current char fulfills the provided criterion. The read chars are returned."},
    {"TridiagonalDecompositionData ::genesis::utils::reduce_to_tridiagonal_matrix (Matrix< double > & data)", "Triangular decomposition of a symmetric matrix.\n\nThis function performs an intermediate step that is usually followed by an execution of the tridiagonal_ql_algorithm().It uses the Householder reduction of matrix a to tridiagonal form, according to Martin et al., Num. Math. 11, 181-195, 1968.\nRef: Smith et al., Matrix Eigensystem Routines -- EISPACK Guide\nSpringer-Verlag, 1976, pp. 489-494.\nW H Press et al., Numerical Recipes in C, Cambridge U P, 1988, pp. 373-374.\nThere, the function is called tred2().The implementation of this function is largely borrowed from http://www.classification-society.org/csna/mda-sw/pca.c published as Public Domain by Fionn Murtagh.\n\ndata\n\n\nInput data matrix. Operations are performed inline, that is, the matrix is changed during execution of the function."},
    {"double ::genesis::utils::round_to (double x, size_t accuracy_order)", "Retun the value of x, rounded to the decimal digit given by accuracy_order."},
    {"double ::genesis::utils::sigma (const Histogram & h)", "Compute the bin-weighted standard deviation.\n\nThe histogram is regarded as a probability distribution. Negative bin values are ignored for the purposes of this calculation. The accuracy of the result is limited by the bin width."},
    {"constexpr int ::genesis::utils::signum (T x, std::false_type )", "Implementation of signum(T x) for unsigned types. See there for details."},
    {"constexpr int ::genesis::utils::signum (T x, std::true_type )", "Implementation of signum(T x) for signed types. See there for details."},
    {"constexpr int ::genesis::utils::signum (T x)", "Get the sign of a value, which is either -1, 0, or 1.\n\nWorks for all numerical types. We use two tagged implementations for signed and unsigned types in order to avoid compiler warnings. See http://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c for details."},
    {"void ::genesis::utils::skip_to_end_of_line (InputStream & source)", "Lexing function that advances the stream to the end of the line, i.e., to the new line char.\n\nIf the stream is already at its last line, this function moves to the end of the stream."},
    {"void ::genesis::utils::skip_until (InputStream & source, char criterion)", "Lexing function that advances the stream until its current char equals the provided one."},
    {"void ::genesis::utils::skip_until (InputStream & source, std::function< bool(char)> criterion)", "Lexing function that advances the stream until its current char fulfills the provided criterion."},
    {"void ::genesis::utils::skip_while (InputStream & source, char criterion)", "Lexing function that advances the stream while its current char equals the provided one."},
    {"void ::genesis::utils::skip_while (InputStream & source, std::function< bool(char)> criterion)", "Lexing function that advances the stream while its current char fulfills the provided criterion."},
    {"std::vector< size_t > ::genesis::utils::sort_indices (RandomAccessIterator first, RandomAccessIterator last, Comparator comparator)", "Get the indices to the sorted order of the given range.\n\nThis function returns a list of indices into the given range, so that their order gives the sorted content of the elements in [ first, last ), using std::less for comparisons.For example, for( auto i: sort_indices(v) ) {\n    cout << *( first + i ) << endl;\n}\noutputs the elements in the range in sorted order, without actually moving any elements in the range. This is useful if the same sorting order needs to be applied to some other container.There is also an overload of this function that takes an additional parameter comparator. It can be used to specify the function for comparing two values of the range.Furthermore, there are variants that use std::stable_sort instead, see stable_sort_indices()."},
    {"std::vector< size_t > ::genesis::utils::sort_indices (RandomAccessIterator first, RandomAccessIterator last)", "Get the indices to the sorted order of the given range.\n\nThis function returns a list of indices into the given range, so that their order gives the sorted content of the elements in [ first, last ), using std::less for comparisons.For example, for( auto i: sort_indices(v) ) {\n    cout << *( first + i ) << endl;\n}\noutputs the elements in the range in sorted order, without actually moving any elements in the range. This is useful if the same sorting order needs to be applied to some other container.There is also an overload of this function that takes an additional parameter comparator. It can be used to specify the function for comparing two values of the range.Furthermore, there are variants that use std::stable_sort instead, see stable_sort_indices()."},
    {"std::vector< size_t > ::genesis::utils::stable_sort_indices (RandomAccessIterator first, RandomAccessIterator last, Comparator comparator)", "Get the indices to the stable sorted order of the given range.\n\nThis function returns a list of indices into the given range, so that their order gives the stable sorted content of the elements in [ first, last ), using std::less for comparisons.For example, for( auto i: stable_sort_indices(v) ) {\n    cout << *( first + i ) << endl;\n}\noutputs the elements in the range in stable sorted order, without actually moving any elements in the range. This is useful if the same sorting order needs to be applied to some other container.There is also an overload of this function that takes an additional parameter comparator. It can be used to specify the function for comparing two values of the range.Furthermore, there are variants that use normal std::sort instead, see sort_indices()."},
    {"std::vector< size_t > ::genesis::utils::stable_sort_indices (RandomAccessIterator first, RandomAccessIterator last)", "Get the indices to the stable sorted order of the given range.\n\nThis function returns a list of indices into the given range, so that their order gives the stable sorted content of the elements in [ first, last ), using std::less for comparisons.For example, for( auto i: stable_sort_indices(v) ) {\n    cout << *( first + i ) << endl;\n}\noutputs the elements in the range in stable sorted order, without actually moving any elements in the range. This is useful if the same sorting order needs to be applied to some other container.There is also an overload of this function that takes an additional parameter comparator. It can be used to specify the function for comparing two values of the range.Furthermore, there are variants that use normal std::sort instead, see sort_indices()."},
    {"std::vector< MeanStddevPair > ::genesis::utils::standardize_cols (Matrix< double > & data, bool scale_means=true, bool scale_std=true)", "Standardize the columns of a Matrix by subtracting the mean and scaling to unit variance.\n\nThe function performs a column-wise z-transformation on the given data.The function returns a vector containing the mean and standard deviation of the columns before standardization, see MeanStddevPair. If both scale_means and scale_std are set to false, the original data is not manipulated, while the returned vector still contains the means and standard deviations. See also matrix_col_mean_stddev().\n\ndata\n\n\nInput data Matrix; standardizing is done inline. \n\n\n\nscale_means\n\n\nIf true (default), center the data by subtracting the mean per column. \n\n\n\nscale_std\n\n\nIf true (default), scale the data to unit variance (or equivalently, unit standard deviation) per column. \n\n\nVector containing the mean and standard deviation for each column before standardization."},
    {"std::vector< MeanStddevPair > ::genesis::utils::standardize_rows (Matrix< double > & data, bool scale_means=true, bool scale_std=true)", "Standardize the rows of a Matrix by subtracting the mean and scaling to unit variance.\n\nThe function performs a row-wise z-transformation on the given data.The function returns a vector containing the mean and standard deviation of the rows before standardization, see MeanStddevPair. If both scale_means and scale_std are set to false, the original data is not manipulated, while the returned vector still contains the means and standard deviations. See also matrix_row_mean_stddev().\n\ndata\n\n\nInput data Matrix; standardizing is done inline. \n\n\n\nscale_means\n\n\nIf true (default), center the data by subtracting the mean per column. \n\n\n\nscale_std\n\n\nIf true (default), scale the data to unit variance (or equivalently, unit standard deviation) per column. \n\n\nVector containing the mean and standard deviation for each column before standardization."},
    {"Matrix< double > ::genesis::utils::sums_of_squares_and_cross_products_matrix (Matrix< double > const & data)", "Calculate the Sums of Squares and Cross Products Matrix (SSCP Matrix)."},
    {"void ::genesis::utils::swap_cols (Matrix< T > & data, size_t col_a, size_t col_b)", "Swap (interchange) two columns of a Matrix, given their indices."},
    {"void ::genesis::utils::swap_rows (Matrix< T > & data, size_t row_a, size_t row_b)", "Swap (interchange) two rows of a Matrix, given their indices."},
    {"std::string ::genesis::utils::to_string (T const & v)", "Return a string representation of a given value.\n\nThis function template is a drop-in replacement for std::to_string, with the difference that it treats floating point numbers more nicely: Instead of printing a fixed amount of digits, it only prints digits without trailing zeros.If you also want to round the value, see to_string_precise() and to_string_rounded().As it uses operator << on the given value, it is suitable for any class or value for which this stream operator is available. Thus, this function can also be used for conveniently returning a string where otherwise some stream operations would have been necessary."},
    {"Matrix< T > ::genesis::utils::transpose (Matrix< T > const & data)", "Transpose a Matrix."},
    {"void ::genesis::utils::tridiagonal_ql_algorithm (Matrix< double > & data, TridiagonalDecompositionData & tri, size_t max_iterations=1000)", "Reduce a symmetric matrix to a symmetric tridiagonal matrix.\n\nUses the data and the intermediate result after executing reduce_to_tridiagonal_matrix() in order to run the algorithm. See there for details.Both parameters are modified during the algorithm. After it is finished, tri.eigenvalues contains the eigenvalus and data the associated eigenvectors of the original data.The implementation of this function is largely borrowed from http://www.classification-society.org/csna/mda-sw/pca.c published as Public Domain by Fionn Murtagh.\n\ndata\n\n\nInput data matrix. Operations are performed inline, that is, the matrix is changed during execution of the function. \n\n\n\ntri\n\n\nTridiagonal matrix coming from reduce_to_tridiagonal_matrix(). \n\n\n\nmax_iterations\n\n\nMaximum number of iterations to perform, default is 1000. If set to 0, the algorithm continues until convergence. As convergence is not given for all data, this might result in an infinite loop."},
    {"constexpr T ::genesis::utils::abs_diff (T const & lhs, T const & rhs)", "Calculate the absolute differenence between two values.\n\nThis function is particularly useful for unsigned types, as subtracting them and then using std::abs() does not work for them."},
    {"void ::genesis::utils::affirm_char_or_throw (InputStream & source, char criterion, SkipWhitespace skip_ws)", "Lexing function that checks whether the current char from the stream equals the provided one.\n\nIf not, the function throws std::runtime_error. The stream is not advanced (i.e., it stays at its current position). For a similar function that reads (i.e., also advances) the current char from the stream, see read_char_or_throw().Using the parameter skip_ws, it is possible to skip leading and/or trailing whitespaces before/after treating the criterion char. See SkipWhitespace for more information."},
    {"void ::genesis::utils::affirm_char_or_throw (InputStream & source, std::function< bool(char)> criterion, SkipWhitespace skip_ws)", "Lexing function that checks whether the current char from the stream fulfills the provided criterion.\n\nIf not, the function throws std::runtime_error. The stream is not advanced (i.e., it stays at its current position). For a similar function that reads (i.e., also advances) the current char from the stream, see read_char_or_throw().Using the parameter skip_ws, it is possible to skip leading and/or trailing whitespaces before/after treating the criterion char. See SkipWhitespace for more information."},
    {"bool ::genesis::utils::almost_equal_relative (double lhs, double rhs, double max_rel_diff=std::numeric_limits< double >::epsilon())", "Check whether two doubles are almost equal, using a relative epsilon to compare them."},
    {"bool ::genesis::utils::char_is_digit (const char c)", "Return whether a char is a digit (0-9)."},
    {"bool ::genesis::utils::char_is_number_part (char const c)", "Return whether a char is valid in a number, i.e., a digit, a dot, plus, minus, or 'e'/'E'."},
    {"bool ::genesis::utils::char_is_sign (const char c)", "Return whether a char is a sign (+-)."},
    {"bool ::genesis::utils::char_match_ci (const char c1, const char c2)", "Return whether two chars are the same, case insensitive."},
    {"bool ::genesis::utils::contains (const C & v, const T & x)", "Returns whether a container object has a certain element.\n\nThe usage of std::find just to check for presence of a certain item is a bit cumbersome. This template simply takes any container and a value and returns true iff the value is present in the container."},
    {"Matrix< double > ::genesis::utils::correlation_matrix (Matrix< double > const & data)", "Calculate the correlation Matrix of a given data Matrix.\n\nThis is done by standardizing the mean and variance of the given data and then calculating the sums_of_squares_and_cross_products_matrix()."},
    {"Matrix< double > ::genesis::utils::covariance_matrix (Matrix< double > const & data)", "Calculate the covariance Matrix of a given data Matrix.\n\nThis is done by standardizing the mean of the given data and then calculating the sums_of_squares_and_cross_products_matrix()."},
    {"std::string ::genesis::utils::current_date ()", "Returns the current date as a string in the format \"2014-12-31\"."},
    {"std::string ::genesis::utils::current_time ()", "Returns the current time as a string in the format \"13:37:42\"."},
    {"void ::genesis::utils::erase_if (Container & c, UnaryPredicate p)", "Erases all elements from the container that satisfy a given predicate. An element is erased, if the predicate evaluates to true for it. The predicate needs to have a signature similar to (T const&)->bool.\n\nc\n\n\nThe container to modify. \n\n\n\np\n\n\nThe predicate to satisfy."},
    {"double ::genesis::placement::node_histogram_distance (Sample const & sample_a, Sample const & sample_b, size_t histogram_bins=25)", "Calculate the Node Histogram Distance of two Samples."},
    {"utils::Matrix< double > ::genesis::placement::node_histogram_distance (SampleSet const & sample_set, size_t histogram_bins=25)", "Calculate the node_histogram_distance() for every pair of Samples in the SampleSet."},
    {"void ::genesis::placement::normalize_weight_ratios (Pquery & pquery)", "Recalculate the like_weight_ratio of the PqueryPlacement&s of a Pquery, so that their sum is 1.0, while maintaining their ratio to each other."},
    {"void ::genesis::placement::normalize_weight_ratios (Sample & smp)", "Recalculate the like_weight_ratio of the PqueryPlacement&s of each Pquery in the Sample, so that their sum is 1.0, while maintaining their ratio to each other.\n\nThis function simply calls normalize_weight_ratios( Pquery& pquery ) for all Pqueries of the Sample."},
    {"double ::genesis::placement::pairwise_distance (const Sample & smp_a, const Sample & smp_b, bool with_pendant_length=false)", "Calculate the normalized pairwise distance between all placements of the two Samples.\n\nThis method calculates the distance between two Samples as the normalized sum of the distances between all pairs of Pqueries  in the Sample. It is similar to the variance() calculation, which calculates this sum for the squared distances between all Pqueries of one Sample.\n\nsmp_a\n\n\nFirst Sample to which the distances shall be calculated to. \n\n\n\nsmp_b\n\n\nSecond Sample to which the distances shall be calculated to. \n\n\n\nwith_pendant_length\n\n\nWhether or not to include all pendant lengths in the calculation.\n\n\nDistance value."},
    {"std::pair< PlacementTreeEdge const *, size_t > ::genesis::placement::placement_count_max_edge (Sample const & smp)", "Get the number of placements on the edge with the most placements, and a pointer to this edge."},
    {"std::pair< PlacementTreeEdge const *, double > ::genesis::placement::placement_mass_max_edge (Sample const & smp)", "Get the summed mass of the placements on the heaviest edge, measured by their like_weight_ratio, and a pointer to this edge."},
    {"double ::genesis::placement::pquery_distance (const PqueryPlain & pqry_a, const PqueryPlain & pqry_b, const utils::Matrix< double > & node_distances, bool with_pendant_length=false)", "Calculates the normalized distance between two plain pqueries. It is mainly a helper method for distance calculations (e.g., pairwise distance, variance).\n\nFor each placement in the two pqueries, a distance is calculated, and their normalized sum is returned. Normalization is done using the mass of placements in both pqueries.The distance between two placements is calculated as the shortest path between them. This includes the their position on the branches, and - if specified - the pendant_length of both. There are three cases that might occur:\nBoth placements are on the same branch. In this case, their distance is caluclated as their difference in proximal_lengths (plus if specified the sum of their pendant_lengths).The path between the placements includes the root. The distance of a placement from its neighbouring nodes is usually given in form of the proximal_length, which is the distance of the placement to the node (at the end of its branch) that lies in direction of the root. Thus, there is an implicit notion of a root, that we need to consider. If the path between two placements contains the root, we can directly calculate their distance as the distance between the two promixal nodes plus proximal_lengths (and possibly pendant_lengths) of both placements. We call this the promixal-promixal case.The root is not part of the path between the placements. This case means that one of the two placements lies on the path between the other placement and the root  thus, the path between the placements does not contain the root. The distance between the placements cannot be calculated using the proximal_lengths directly, but we need to get the distal_length (away from the root) of the inner placement first. This is simply the difference between branch_length and proximal_length of that placement. Of course, this case comes in two flavours, because both placements can be the inner or outer one. They are called proximal-distal case and distal-proximal case, respectively.\nThe first case is easy to detect by comparing the edge nums. However, distinguishing between the latter two cases is expensive, as it involves finding the path to the root for both placements. To speed this up, we instead use a distance matrix that is calculated in the beginning of any algorithm using this method and contains the pairwise distances between all nodes of the tree. Using this, we do not need to find paths between placements, but simply go to the nodes at the end of the branches of the placements and do a lookup for those nodes.With this technique, we can calculate the distances between the placements for all three cases (promixal-promixal, proximal-distal and distal-proximal) cheaply. The wanted distance is then simply the minimum of those three distances. This is correct, because the two wrong cases will always produce an overestimation of the distance.This distance is normalized using the like_weight_ratio of both placements, before summing it up to calculate the total distance between the pqueries."},
    {"void ::genesis::placement::scale_all_branch_lengths (Sample & smp, double factor=1.0)", "Scale all branch lengths of the Tree and the position of the PqueryPlacements by a given factor.\n\nThis function calls tree::scale_all_branch_lengths() for scaling the tree, and also applies the same scaling to the PqueryPlacement::proximal_length."},
    {"void ::genesis::placement::sort_placements_by_weight (Pquery & pquery)", "Sort the PqueryPlacements of a Pquery by their like_weight_ratio, in descending order (most likely first)."},
    {"void ::genesis::placement::sort_placements_by_weight (Sample & smp)", "Sort the PqueryPlacements of all Pqueries  by their like_weight_ratio, in descending order (most likely first)."},
    {"double ::genesis::placement::total_multiplicity (Pquery const & pqry)", "Return the sum of all multiplicities  of the Pquery."},
    {"double ::genesis::placement::total_multiplicity (Sample const & sample)", "Return the sum of all multiplicities  of all the Pqueries of the Sample."},
    {"size_t ::genesis::placement::total_placement_count (Sample const & smp)", "Get the total number of PqueryPlacements in all Pqueries  of the given Sample."},
    {"double ::genesis::placement::total_placement_mass (Sample const & smp)", "Get the summed mass of all PqueryPlacements in all Pqueries  of the given Sample, where mass is measured by the like_weight_ratios of the PqueryPlacements.\n\nBe aware that this function only gives the pure sum of the like_weight_ratios. See total_placement_mass_with_multiplicities() for a version of this function, which also takes the multiplicities  of the Pqueries into account."},
    {"double ::genesis::placement::total_placement_mass_with_multiplicities (Sample const & smp)", "Get the mass of all PqueryPlacements of the Sample, using the multiplicities  as factors.\n\nThis function returns the summed mass of all PqueryPlacements in all Pqueries  of the given Sample, where mass is measured by like_weight_ratio, and the mass of each Pquery is multiplied by the sum of the multiplicities  of this Pquery.This method returns the same value as total_placement_mass() in case that the multiplicity is left at its default value of 1.0."},
    {"double ::genesis::placement::variance (const Sample & smp, bool with_pendant_length=false)", "Calculate the variance of the placements on a tree.\n\nThe variance is a measure of how far a set of items is spread out in its space (http://en.wikipedia.org/wiki/variance). In many cases, it can be measured using the mean of the items. However, when considering placements on a tree, this does not truly measure how far they are from each other. Thus, this algorithm applies a different method of calculating the variance in terms of squared deviations of all items from each other: $ Var(X) = \\frac{1}{n^2} \\sum_{i=1}^n \\sum_{j=1}^n \\frac{1}{2}(x_i - x_j)^2 $, where $ (x_i - x_j) $ denotes the distance between two placements.According to the formula above, each pair of placements is evaluated twice, and subsequently their distance need to be halfed when being added to the sum of distanaces. Instead of that, we calculate the distance for each pair only once, thus are able skip half the calculations, and of course skip the division by two.Furthermore, the normalizing factor $ \\frac{1}{n^2} $ of the variance usually contains the number of elements being processed. However, as the placements are weighted by their like_weight_ratio, we instead calculate n as the sum of the like_weight_ratio of all placements. In case that for each pquery the ratios of all its placements sum up to 1.0, this number will be equal to the number of pqueries (and thus be equal to the usual case of using the number of elements). However, as this is not required (placements with small ratio can be dropped, so that their sum per pquery is less than 1.0), we cannout simply use the count."},
    {"std::unordered_set< std::string > ::genesis::placement::all_pquery_names (Sample const & sample)", "Return a set of all unique PqueryNames of the Pqueries  of the given sample.\n\nIf a Pquery contains multiple names, all of them are added to the set."},
    {"std::pair< PlacementTreeEdge const *, double > ::genesis::placement::center_of_gravity (const Sample & smp, bool with_pendant_length=false)", "Calculate the Center of Gravity of the placements on a tree.\n\nThe center of gravity is the point on the tree where all masses of the placements on the one side of it times their distance from the point are equal to this sum on the other side of the point. In the following example, the hat ^ marks this point on a line with two placements: One has mass 1 and distance 3 from the central point, and one as mass 3 and distance 1, so that the product of their mass and distance to the point is the same:               3\n              |\n1             |\n|_____________|\n          ^\nIt is thus like calculating masses and torques on a lever in order to find their physical center of mass/gravity.This calculation is done for the whole tree, with the masses calculated from the like_weight_ratio and distances in terms of the branch_length of the edges and the proximal_length and (if specificed in the method parameter) the pendant_length of the placements."},
    {"double ::genesis::placement::center_of_gravity_distance (const Sample & smp_a, const Sample & smp_b, bool with_pendant_length=false)", "Calculate the distance between the two Centers of Gravity of two Samples.\n\nThe distance is measured in branch length units; for the Center of Gravity, see center_of_gravity()."},
    {"double ::genesis::placement::center_of_gravity_variance (const Sample & smp, bool with_pendant_length=false)", "Calcualte the variance of the PqueryPlacements of a Sample around its Center of Gravity.\n\nThe caluclation of the variance is as follows:$ Var(X) = E[ (x - \\mu)^2 ] = \\frac{\\sum (x - \\mu)^2 \\cdot \\omega} {\\sum \\omega} $, where the weights $ \\omega $ are the like_weight_ratios of the placements.See center_of_gravity() for more."},
    {"std::vector< int > ::genesis::placement::closest_leaf_depth_histogram (Sample const & smp)", "Return a histogram representing how many placements have which depth with respect to their closest leaf node.\n\nThe depth between two nodes on a tree is the number of edges between them. Thus, the depth of a placement (which sits on an edge of the tree) to a specific node is the number of edges between this node and the closer one of the two nodes at the end of the edge where the placement sits.The closest leaf to a placement is thus the leaf node which has the smallest depth to that placement. This function then returns a histogram of how many placements (values of the vector) are there that have a specific depth (indices of the vector) to their closest leaf.Example: A return vector of histogram[0] = 2334\nhistogram[1] = 349\nhistogram[2] = 65\nhistogram[3] = 17\nmeans that there are 2334 placements that sit on an edge which leads to a leaf node (thus, the depth of one of the nodes of the edge is 0). It has 349 placements that sit on an edge where one of its nodes has one neighbour that is a leaf; and so on.The vector is automatically resized to the needed number of elements."},
    {"std::vector< int > ::genesis::placement::closest_leaf_distance_histogram (Sample const & smp, const double min, const double max, const int bins=10)", "Returns a histogram counting the number of placements that have a certain distance to their closest leaf node, divided into equally large intervals between a min and a max distance.\n\nThe distance range between min and max is divided into bins many intervals of equal size. Then, the distance from each placement to its closest leaf node is calculated and the counter for this particular distance inverval in the histogram is incremented.The distance is measured along the branch_length values of the edges, taking the pendant_length and proximal_length of the placements into account. If the distances is outside of the interval [min,max], the counter of the first/last bin is incremented respectively.Example: double min      =  0.0;\ndouble max      = 20.0;\nint    bins     = 25;\ndouble bin_size = (max - min) / bins;\nstd::vector<int> hist = closest_leaf_distance_histogram (min, max, bins);\nfor (unsigned int bin = 0; bin < hist.size(); ++bin) {\n    LOG_INFO << \"Bin \" << bin << \" [\" << bin * bin_size << \"; \"\n             << (bin+1) * bin_size << \") has \" << hist[bin] << \" placements.\";\n}"},
    {"std::vector< int > ::genesis::placement::closest_leaf_distance_histogram_auto (Sample const & smp, double & min, double & max, const int bins=10)", "Returns the same type of histogram as closest_leaf_distance_histogram(), but automatically determines the needed boundaries.\n\nSee closest_leaf_distance_histogram() for general information about what this function does. The difference between both functions is that this one first procresses all distances from placements to their closest leaf nodes to find out what the shortest and longest are, then sets the boundaries of the histogram accordingly. The number of bins is then used to divide this range into intervals of equal size.The boundaries are returned by passing two doubles min and max to the function by reference. The value of max will actually contain the result of std::nextafter() called on the longest distance; this makes sure that the value itself will be placed in the interval.Example: double min, max;\nint    bins = 25;\nstd::vector<int> hist = closest_leaf_distance_histogram (min, max, bins);\ndouble bin_size = (max - min) / bins;\nLOG_INFO << \"Histogram boundaries: [\" << min << \",\" << max << \").\";\nfor (unsigned int bin = 0; bin < hist.size(); ++bin) {\n    LOG_INFO << \"Bin \" << bin << \" [\" << bin * bin_size << \"; \"\n             << (bin+1) * bin_size << \") has \" << hist[bin] << \" placements.\";\n}\nIt has a slightly higher time and memory consumption than the non-automatic version closest_leaf_distance_histogram(), as it needs to process the values twice in order to find their min and max."},
    {"void ::genesis::placement::collect_duplicate_pqueries (Sample & smp)", "Find all Pqueries  that share a common name and combine them into a single Pquery containing all their collective PqueryPlacements and PqueryNames.\n\nThe function collects all Pqueries that share at least one name. This is transitive, so that for example three Pqueries with two names each like (a,b) (b,c) (c,d) will be combined into one Pquery. Thus, the transitive closure of shared names is collected.All those Pqueries with shared names are combined by simply moving all their Placements and Names into one Pquery and deleting the others. This means that at least the shared names will be doubled after this function. Also, Placements on the same edge can occur. Thus, usually merge_duplicate_names() and merge_duplicate_placements() are called after this function. The function merge_duplicates() does exaclty this, for convenience."},
    {"void ::genesis::placement::copy_pqueries (Sample const & source, Sample & target)", "Copy all Pqueries  from the source Sample (left parameter) to the target Sample (right parameter).\n\nFor this method to succeed, the PlacementTrees of the Samples need to have the same topology, including identical edge_nums and node names. Otherwise, this function throws an std::runtime_error.The PlacementTree of the target Sample is not modified. If the average branch length tree is needed instead, see SampleSet::merge_all()."},
    {"double ::genesis::placement::earth_movers_distance (const Sample & lhs, const Sample & rhs, bool with_pendant_length=false)", "Calculate the earth mover's distance between two Samples.\n\nThis function interprets the like_weight_ratios of the PqueryPlacements as masses distributed along the branches of a tree. It then calculates the earth mover's distance between those masses for the distrubitons induced by the two given Samples.In order to do so, first, a tree with the average branch lengths of the two PlacementTrees is calculated. This is because of numerical issues that might yield different branch lengths. This necessiates that the trees have the same topology. If not, an std::runtime_error is thrown. The masses are then distributed on this tree, using the same relative position on their branches that they had in their original trees.The calculation furthermore takes the multiplicities of the Pqueries into account. That means, pqueries with higher (total) multiplicity have a higher influence on the calculated distance.As the two Samples might have a different total number of Pqueries, the masses of the Samples are first normalized to 1.0, using all the like_weight_ratios and multiplicities of the Pqueries. As a consequence, the resulting distance will not reflect the total number of Pqueries, but only their relative (normalized) distrubution on the tree.See earth_movers_distance( EmdTree const& )  for more information on the actual distance calculation."},
    {"utils::Matrix< double > ::genesis::placement::earth_movers_distance (SampleSet const & sample_set, bool with_pendant_length=false)", "Calculate the pairwise Earth Movers Distance for all Samples in a SampleSet.\n\nThe result is a pairwise distance Matrix using the indices of the Samples in the SampleSet. See the Sample version of this function for details on this distance measure."},
    {"double ::genesis::placement::edpl (Sample const & sample, Pquery const & pquery)", "Shortcut alias for expected_distance_between_placement_locations()."},
    {"std::vector< double > ::genesis::placement::edpl (Sample const & sample)", "Shortcut alias for expected_distance_between_placement_locations()."},
    {"void ::genesis::placement::epca_splitify_transform (utils::Matrix< double > & imbalance_matrix, double kappa=1.0)", "Perform a component-wise transformation of the imbalance matrix used for epca().\n\nAll entries of the Matrix are transformed inplace, using\\[ \\varphi_\\kappa(x) = \\mathrm{sgn}(x) \\cdot |x|^\\kappa \\]where the kappa ( $\\kappa$) parameter can be any non-negative number. This parameter scales between ignoring abundance information (kappa = 0), using it linearly (kappa = 1), and emphasizing it (kappa > 1).\n\nimbalance_matrix\n\n\nMatrix to transform inplace. \n\n\n\nkappa\n\n\nScaling value for abundance information. Has to be > 0."},
    {"double ::genesis::placement::expected_distance_between_placement_locations (Sample const & sample, Pquery const & pquery)", "Calculate the EDPL uncertainty values for a Pquery.\n\nSee http://matsen.github.io/pplacer/generated_rst/guppy_edpl.html for more information.This function expects a Pquery and the Sample it belongs to. This is necessary in order to get the Tree of the Sample and calculate distances between its Nodes."},
    {"std::vector< double > ::genesis::placement::expected_distance_between_placement_locations (Sample const & sample)", "Calculate the expected_distance_between_placement_locations() for all Pqueries in the Sample."},
    {"void ::genesis::placement::filter_min_accumulated_weight (Pquery & pquery, double threshold=0.99)", "Remove the PqueryPlacements with the lowest like_weight_ratio, while keeping the accumulated weight (sum of all remaining like_weight_ratios) above a given threshold.\n\nThis is a cleaning function to get rid of unlikely placement positions, withouth sacrificing too much detail of the overall distribution of weights. The EPA support a similar option, which only writes enough of the most likely placement positions to the output to fullfil a threshold."},
    {"void ::genesis::placement::filter_min_accumulated_weight (Sample & smp, double threshold=0.99)", "Remove the PqueryPlacements with the lowest like_weight_ratio, while keeping the accumulated weight (sum of all remaining like_weight_ratios) above a given threshold.\n\nThis function calls filter_min_accumulated_weight( Pquery& pquery, double threshold ) for all Pqueries of the Sample. See this version of the function for more information."},
    {"void ::genesis::placement::filter_min_weight_threshold (Pquery & pquery, double threshold=0.01)", "Remove all PqueryPlacements that have a like_weight_ratio below the given threshold."},
    {"void ::genesis::placement::filter_min_weight_threshold (Sample & smp, double threshold=0.01)", "Remove all PqueryPlacements that have a like_weight_ratio below the given threshold from all Pqueries  of the Sample."},
    {"void ::genesis::placement::filter_n_max_weight_placements (Pquery & pquery, size_t n=1)", "Remove all PqueryPlacements but the n most likely ones from the Pquery.\n\nPqueries can contain multiple placements on different branches. For example, the EPA algorithm of RAxML outputs up to the 7 most likely positions for placements to the output Jplace file by default. The property like_weight_ratio weights those placement positions so that the sum over all positions (all branches of the tree) per pquery is 1.0.This function removes all but the n most likely placements (the ones which have the highest like_weight_ratio) from the Pquery. The like_weight_ratio of the remaining placements is not changed."},
    {"void ::genesis::placement::filter_n_max_weight_placements (Sample & smp, size_t n=1)", "Remove all PqueryPlacements but the n most likely ones from all Pqueries  in the Sample.\n\nThis function calls filter_n_max_weight_placements( Pquery& pquery, size_t n ) for all Pqueries of the Sample. See this version of the function for more information."},
    {"void ::genesis::placement::filter_pqueries_differing_names (Sample & sample_1, Sample & sample_2)", "Remove all Pqueries from the two Samples that have a name in common.\n\nThis function builds the intersection of the set of names of both Samples and removes all those Pqueries that have a PqueryName with one of those names.This is not quite the same as building the symmetric difference and keeping those elements, and, although similar, it not the opposite of filter_pqueries_intersecting_names(), because Pqueries can have multiple names."},
    {"void ::genesis::placement::filter_pqueries_intersecting_names (Sample & sample_1, Sample & sample_2)", "Remove all Pqueries from the two Samples except the ones that have names in common.\n\nThis function builds the intersection of the set of names of both Samples and only keeps those Pqueries that have a PqueryName with one of those names."},
    {"void ::genesis::placement::filter_pqueries_keeping_names (Sample & smp, std::string const & regex)", "Remove all Pqueries which do not have at least one name that matches the given regex.\n\nIf the Pquery has a PqueryName whose PqueryName::name value matches the regex, the Pquery is kept. If none of its names matches, the Pquery is removed."},
    {"void ::genesis::placement::filter_pqueries_keeping_names (Sample & smp, std::unordered_set< std::string > keep_list)", "Remove all Pqueries which do not have at least one name that is in the given keep list.\n\nIf the Pquery has a PqueryName whose PqueryName::name value is in the keep_list, the Pquery is kept. If none of its names is in the keep_list, the Pquery is removed.This is similar to filter_pqueries_removing_names(), but not quite the opposite, as Pqueries can have multiple names."},
    {"void ::genesis::placement::filter_pqueries_removing_names (Sample & smp, std::string const & regex)", "Remove all Pqueries which have at least one name that matches the given regex.\n\nIf the Pquery has a PqueryName whose PqueryName::name value matches the reges, the Pquery is removed. If none of its names matches, the Pquery is kept."},
    {"void ::genesis::placement::filter_pqueries_removing_names (Sample & smp, std::unordered_set< std::string > remove_list)", "Remove all Pqueries which have at least one name that is in the given remove list.\n\nIf the Pquery has a PqueryName whose PqueryName::name value is in the remove_list, the Pquery is removed. If none of its names is in the remove_list, the Pquery is kept.This is similar to filter_pqueries_keeping_names(), but not quite the opposite, as Pqueries can have multiple names."},
    {"Pquery const * ::genesis::placement::find_pquery (Sample const & smp, std::string const & name)", "Return the first Pquery that has a particular name, or nullptr of none has."},
    {"Pquery * ::genesis::placement::find_pquery (Sample & smp, std::string const & name)", "Return the first Pquery that has a particular name, or nullptr of none has."},
    {"bool ::genesis::placement::has_name (Pquery const & pquery, std::string const & name)", "Return true iff the given Pquery contains a particular name."},
    {"bool ::genesis::placement::has_name (Sample const & smp, std::string const & name)", "Return true iff the given Sample contains a Pquery with a particular name, i.e., a PqueryName whose name member equals the given name."},
    {"void ::genesis::placement::merge_duplicate_names (Pquery & pquery)", "Merge all PqueryNames that have the same name property into one, while adding up their multiplicity."},
    {"void ::genesis::placement::merge_duplicate_names (Sample & smp)", "Call merge_duplicate_names() for each Pquery of the Sample."},
    {"void ::genesis::placement::merge_duplicate_placements (Pquery & pquery)", "Merge all PqueryPlacements of a Pquery that are on the same TreeEdge into one averaged PqueryPlacement.\n\nThe merging is done via averaging all values of the PqueryPlacement: likelihood, like_weight_ratio, proximal_length, pendant_length and parsimony."},
    {"void ::genesis::placement::merge_duplicate_placements (Sample & smp)", "Call merge_duplicate_placements( Pquery& ) for each Pquery of a Sample."},
    {"void ::genesis::placement::merge_duplicates (Sample & smp)", "Look for Pqueries  with the same name and merge them.\n\nThis function is a wrapper that simply calls three other functions on the provided Sample: * collect_duplicate_pqueries()\n* merge_duplicate_names()\n* merge_duplicate_placements()\nSee there for more information on what they do."},
    {"bool ::genesis::sequence::has_valid_label (Sequence const & seq)", "Check whether a Sequence has a valid label.\n\nThis might be important for printing the Sequence to a file that needs to be read by other applications. See is_valid_label() for details on what is considered a valid label. See sanitize_label() for a function that replaces all invalid characters of the label by underscores."},
    {"bool ::genesis::sequence::has_valid_labels (SequenceSet const & set)", "Check whether all Sequences in a SequenceSet have valid labels.\n\nThis might be important for printing the Sequences to a file that needs to be read by other applications. See is_valid_label() for details on what is considered a valid label. See sanitize_labels() for a function that replaces all invalid characters of the labels by underscores."},
    {"bool ::genesis::sequence::is_alignment (SequenceSet const & set)", "Return true iff all Sequences in the SequenceSet have the same length."},
    {"bool ::genesis::sequence::is_valid_label (std::string const & label)", "Check whether a given string is a valid label for a Sequence.\n\nWhile we can work with any form of label (as long as it is a string), most file formats and consequently most programs that read them restrict the set of valid characters for labels of sequences. We thus provide this function, which uses the most common interpretation of valid labels.A label is valid if its characters have a graphical representation (i.e., isgraph() is true) and if non of these characters occurs: :,();[]'\nThus, all whitespaces, control characters, and the listed special characters are invalid. See sanitize_label() for a function that replaces all invalid characters of the label by underscores."},
    {"std::unordered_set< std::string > ::genesis::sequence::labels (SequenceSet const & set)", "Return a set of all labels of the SequenceSet."},
    {"size_t ::genesis::sequence::longest_sequence_length (SequenceSet const & set)", "Return the length of the longest Sequence in the SequenceSet."},
    {"std::string ::genesis::sequence::nucleic_acid_ambiguities (char code)", "Return the possible ambiguous nucleic acid codes for a given code char.\n\nThe codes are resolved as follows: 'A' ==> \"A\"\n'C' ==> \"C\"\n'G' ==> \"G\"\n'T' ==> \"T\"\n'U' ==> \"T\"\n\n'W' ==> \"AT\"\n'S' ==> \"CG\"\n'M' ==> \"AC\"\n'K' ==> \"GT\"\n'R' ==> \"AG\"\n'Y' ==> \"CT\"\n\n'B' ==> \"CGT\"\n'D' ==> \"AGT\"\n'H' ==> \"ACT\"\n'V' ==> \"ACG\"\n\n'N' ==> \"ACGT\"\n'O' ==> \"-\"\n'X' ==> \"-\"\n'.' ==> \"-\"\n'-' ==> \"-\"\n'?' ==> \"-\"\nThe code char is treated case-insensitive. If the given code char is not valid, an std::out_of_range exception is thrown.See nucleic_acid_ambiguity_code() for a reverse version of this function. It is however not exactly the reverse, as some degenerated codes are mapped to the gap char. Thus, this function is not injective."},
    {"void ::genesis::sequence::merge_duplicate_sequences (SequenceSet & set, MergeDuplicateSequencesCountPolicy count_policy, std::string const & counter_prefix=\"_\")", "Merge all Sequences in a SequenceSet that have identical sites.\n\nThe merging is done by removing all but the first Sequence with identical sites. That means, the resulting \"representative\" of a set of merged Sequences has the label and metadata of the original Sequence that was first in the SequenceSet.Using the MergeDuplicateSequencesCountPolicy, it is possible to store the counts of the Sequences (i.e., how often they appeard before merging) within either the label or metadata of the Sequence, separated by counter_prefix. With the default settings, the count is appended to the label, separated by an underscore."},
    {"char ::genesis::sequence::nucleic_acid_ambiguity_code (std::string codes)", "Return the nucleic acid code that represents all given codes.\n\nThe codes are resolved as follows: \"A\"    ==> 'A'\n\"C\"    ==> 'C'\n\"G\"    ==> 'G'\n\"T\"    ==> 'T'\n\n\"AT\"   ==> 'W'\n\"CG\"   ==> 'S'\n\"AC\"   ==> 'M'\n\"GT\"   ==> 'K'\n\"AG\"   ==> 'R'\n\"CT\"   ==> 'Y'\n\n\"CGT\"  ==> 'B'\n\"AGT\"  ==> 'D'\n\"ACT\"  ==> 'H'\n\"ACG\"  ==> 'V'\n\n\"ACGT\" ==> 'N'\n\"-\"    ==> '-'\nThe given codes are treated case-insensitive and order-independent. For example, given \"tCgG\", the function still returns 'B'. However, if any of the given codes is not valid, an std::out_of_range exception is thrown.See nucleic_acid_ambiguities() for the reverse of this function."},
    {"std::string ::genesis::sequence::nucleic_acid_codes_all ()", "Return all valid nucleic acid codes. Those are \"ACGTUWSMKRYBDHVNOX.-?\"."},
    {"std::string ::genesis::sequence::nucleic_acid_codes_degenerated ()", "Return all degenerated nucleic acid codes. Those are \"WSMKRYBDHV\"."},
    {"std::string ::genesis::sequence::nucleic_acid_codes_undetermined ()", "Return all undetermined nucleic acid codes. Those are \"NOX.-?\"."},
    {"std::string ::genesis::sequence::nucleic_acid_codes_plain ()", "Return all plain nucleic acid codes. Those are \"ACGTU\"."},
    {"std::map< char, utils::Color > ::genesis::sequence::nucleic_acid_colors ()", "Return a map of Colors for each nucleic acid code.\n\nThis function gives a Color for each nucleic acid code."},
    {"std::map< char, std::string > ::genesis::sequence::nucleic_acid_text_colors ()", "Return a map of text colors for each nucleic acid code.\n\nThis function gives a color name usable for utils::Style for each nucleic acid code. The return value of this function can for example be used in sequence::print_color() function."},
    {"bool ::genesis::sequence::operator& (SiteEntropyOptions lhs, SiteEntropyOptions rhs)", "And-operator to check whether a SiteEntropyOptions is set.\n\nTypical usage: SiteEntropyOptions options;\nif( options & SiteEntropyOptions::kWeighted ) {\n    // Do stuff...\n}\nUse the or-operator in order to set and combine options."},
    {"SiteEntropyOptions ::genesis::sequence::operator| (SiteEntropyOptions lhs, SiteEntropyOptions rhs)", "Or-operator to combine two SiteEntropyOptionss.\n\nTypical usage: auto options = SiteEntropyOptions::kWeighted | SiteEntropyOptions::kNormalized;\nUse the and-operator in order to check whether an option is set."},
    {"SiteEntropyOptions & ::genesis::sequence::operator|= (SiteEntropyOptions & lhs, SiteEntropyOptions rhs)", "Or-assignment-operator to combine two SiteEntropyOptionss.\n\nTypical usage: SiteEntropyOptions options;\noptions |= SiteEntropyOptions::kWeighted;\nUse the and-operator in order to check whether an option is set."},
    {"void ::genesis::sequence::relabel_sha1 (Sequence & seq)", "Relabel the Sequence using the SHA1 hash digest of its sites."},
    {"void ::genesis::sequence::relabel_sha1 (SequenceSet & set)", "Relabel all Sequences in the SequenceSet using the SHA1 hash digest of the sites.\n\nIf there are duplicate Sequences, this function will lead to multiple Sequences with the same name, which might be an issue for downstream programs that expect unique labels. See has_unique_labels() to check this."},
    {"void ::genesis::sequence::remove_characters (Sequence & seq, std::string const & search)", "Remove all of the characters in search from the sites of the Sequence."},
    {"void ::genesis::sequence::remove_characters (SequenceSet & set, std::string const & search)", "Remove all of the characters in search from the sites of the Sequences in the SequenceSet."},
    {"void ::genesis::sequence::remove_gaps (Sequence & seq, std::string const & gap_chars)", "Remove all gap characters from the sites of the Sequence.\n\nThis function is an alias for remove_characters(), which by default uses the gap sites of nucleic_acid_codes_undetermined()."},
    {"void ::genesis::sequence::remove_gaps (SequenceSet & set, std::string const & gap_chars)", "Remove all gap characters from the sites of the Sequences in the SequenceSet.\n\nThis function is an alias for remove_characters(), which by default uses the gap sites of nucleic_acid_codes_undetermined()."},
    {"void ::genesis::sequence::remove_sites (SequenceSet & set, utils::Bitvector sites)", "Remove all sites from all Sequences in a SequenceSet where the given Bitvector is true, and keep all others.\n\nThe Bitvector and all Sequences need to have the same size, otherwise an expection is thrown. This check is done before any Sequence is changed. Thus, if the function throws for this reason, the Sequences are left unchanged.This function is for example useful in combination with gap_sites()."},
    {"void ::genesis::sequence::remove_sites (Sequence & seq, utils::Bitvector sites)", "Remove all sites from a Sequence where the given Bitvector is true, and keep all others.\n\nThe Bitvector needs to have the same size as the Sequence, otherwise an expection is thrown.This function is for example useful in combination with gap_sites()."},
    {"void ::genesis::sequence::replace_characters (Sequence & seq, std::string const & search, char replacement)", "Replace all occurences of the chars in search by the replace char, for all sites in the given Sequence.\n\nThe function is case sensitive. Thus, you need to use both cases for the search string if you are unsure. The replace char is always used as is, independent of the case of the matching search char."},
    {"void ::genesis::sequence::replace_t_with_u (SequenceSet & set)", "Replace all occurrences of T by U in the sites of all Sequences in the SequenceSet.\n\nThis is a small helper function for sequences with nucleic acid codes. It is case sensitive, that is, lower case t is replaced by lower case u, and upper case T by upper case U."},
    {"void ::genesis::sequence::replace_characters (SequenceSet & set, std::string const & search, char replacement)", "Replace all occurences of the chars in search by the replace char, for all sites in the Sequences in the given SequenceSet.\n\nThe function is case sensitive. Thus, you need to use both cases for the search string if you are unsure. The replace char is always used as is, independent of the case of the matching search char."},
    {"void ::genesis::sequence::replace_t_with_u (Sequence & seq)", "Replace all occurrences of T by U in the sites of the Sequence.\n\nThis is a small helper function for sequences with nucleic acid codes. It is case sensitive, that is, lower case t is replaced by lower case u, and upper case T by upper case U."},
    {"void ::genesis::sequence::replace_u_with_t (Sequence & seq)", "Replace all occurrences of U by T in the sites of the Sequence.\n\nThis is a small helper function for sequences with nucleic acid codes. It is case sensitive, that is, lower case u is replaced by lower case t, and upper case U by upper case T."},
    {"void ::genesis::sequence::replace_u_with_t (SequenceSet & set)", "Replace all occurrences of U by T in the sites of all Sequences in the SequenceSet.\n\nThis is a small helper function for sequences with nucleic acid codes. It is case sensitive, that is, lower case u is replaced by lower case t, and upper case U by upper case T."},
    {"std::string ::genesis::sequence::sanitize_label (std::string const & label)", "Sanitize a label by replacing all invalid characters with underscores.\n\nThis might be important for printing the Sequences to a file that needs to be read by other applications. See is_valid_label() for details on what is considered a valid label."},
    {"void ::genesis::sequence::sanitize_label (Sequence & seq)", "Sanitize a label by replacing all invalid characters with underscores.\n\nThis might be important for printing the Sequences to a file that needs to be read by other applications. See is_valid_label() for details on what is considered a valid label."},
    {"void ::genesis::sequence::sanitize_labels (SequenceSet & set)", "Sanitize the labels of all Sequences in the SequenceSet by replacing all invalid characters with underscores.\n\nThis might be important for printing the Sequences to a file that needs to be read by other applications. See is_valid_label() for details on what is considered a valid label."},
    {"std::map< char, size_t > ::genesis::sequence::site_histogram (Sequence const & seq)", "Get a histogram of the occurrences of particular sites, given a Sequence.\n\nThis gives the raw counts of how often each site (character) appears in the Sequence. See base_frequencies() for the relative version of this function."},
    {"double ::genesis::sequence::site_entropy (SequenceCounts const & counts, size_t site_index, SiteEntropyOptions options)", "Calculate the entropy at one site of a SequenceCounts object.\n\nThe entropy $ H $ (uncertainty) at site $ i $ (= site_idx) is calculated as $ H_{i}=-\\sum f_{{c,i}}\\times \\log _{2}f_{{c,i}} $, where $ f_{c,i} $ is the relative frequency of character $ c $ at site $ i $, summed over all characters in the SequenceCounts object.The function additionally takes optional flags to refine the calculation, see SiteEntropyOptions for their explanation."},
    {"std::map< char, size_t > ::genesis::sequence::site_histogram (SequenceSet const & set)", "Get a histogram of the occurrences of particular sites, given a SequenceSet.\n\nThis gives the raw counts of how often each site (character) appears in the whole set. See base_frequencies() for the relative version of this function."},
    {"double ::genesis::sequence::site_information (SequenceCounts const & counts, size_t site_index, bool use_small_sample_correction=false, SiteEntropyOptions options)", "Calculate the information content at one site of a SequenceCounts object.\n\nThe information content $ R $ at site $ i $ (= site_index) is calculated as $ R_{i} = \\log_{2}( s ) - (H_{i}+e_{n}) $.Here, $ s $ is the number of possible characters in the sequences (usually, 4 for nucleic acids and 20 for amino acids), which is taken from the characters() used in the SequenceCounts object. Furthermore, $ H_{i} $ is the site_entropy() at the given site.The optional term $ e_{n} $ is the small-sample correction, calculated as $ e_{n}={\\frac{1}{\\ln {2}}}\\times {\\frac{s-1}{2n}} $, with $ n $ being the number of sequences. It is only used if use_small_sample_correction is set to true (default is false).The function additionally takes optional flags to refine the site entropy calculation, see SiteEntropyOptions for their explanation."},
    {"std::string ::genesis::sequence::translate_amino_acid (char code)", "Get the name of a amino acid given its IUPAC code.\n\nThe codes are translated as follows: A Alanine\nB Aspartic acid or Asparagine\nC Cysteine\nD Aspartic acid\nE Glutamic acid\nF Phenylalanine\nG Glycine\nH Histidine\nI Isoleucine\nJ Leucine or Isoleucine\nK Lysine\nL Leucine\nM Methionine\nN Asparagine\nO Pyrrolysine\nP Proline\nQ Glutamine\nR Arginine\nS Serine\nT Threonine\nU Selenocysteine\nV Valine\nW Tryptophan\nY Tyrosine\nZ Glutamic acid or Glutamine\nX any\n* translation stop\n- gap\n? gap\nThe code char is treated case-insensitive. If the given code char is not valid, an std::out_of_range exception is thrown."},
    {"size_t ::genesis::sequence::total_length (SequenceSet const & set)", "Return the total length (sum) of all Sequences in the SequenceSet."},
    {"std::string ::genesis::sequence::translate_nucleic_acid (char code)", "Get the name of a nucleic acid given its IUPAC code.\n\nThe codes are translated as follows: A Adenine\nC Cytosine\nG Guanine\nT Thymine\nU Uracil\nW Weak\nS Strong\nM aMino\nK Keto\nR puRine\nY pYrimidine\nB not A\nD not C\nH not G\nV not T\nN any\nO omitted\nX masked\n. gap\n- gap\n? gap\nThe code char is treated case-insensitive. If the given code char is not valid, an std::out_of_range exception is thrown."},
    {"bool ::genesis::sequence::validate_chars (SequenceSet const & set, std::string const & chars)", "Returns true iff all Sequences only consist of the given chars.\n\nFor presettings of usable chars, see the functions nucleic_acid_codes_... and amino_acid_codes_.... For example, to check whether the sequences are nucleic acids, use nucleic_acid_codes_all(). The chars are treated case-insensitive.If chars contains invalid (non-standard ASCII) characters, an std::invalid_argument exception is thrown."},
    {"std::string ::genesis::genesis_header ()", "Return the header for genesis.\n\nThis is simply a text version of the logo, including the current version. It can for example be displayed at the start of a program to indicate that this program uses genesis."},
    {"std::string ::genesis::genesis_license ()", "Return the genesis license boilerplate information.\n\nThis function is useful for programs with terminal interaction. In such cases, you should include a command to show this license information."},
    {"std::string ::genesis::genesis_url ()", "Return the URL of the genesis home page."},
    {"std::string ::genesis::genesis_version ()", "Return the current genesis version.\n\nWe use semantic versioning 2.0.0 for genesis.Given a version number MAJOR.MINOR.PATCH, increment the:\nMAJOR version when you make incompatible API changes,MINOR version when you add functionality in a backwards-compatible manner, andPATCH version when you make backwards-compatible bug fixes.\nAdditional labels for pre-release and build metadata are available as extensions to the MAJOR.MINOR.PATCH format."},
    {"std::string ::genesis::sequence::amino_acid_codes_all ()", "Return all valid amino acid codes. Those are \"ACDEFGHIKLMNOPQRSTUVWYBJZX*-?\"."},
    {"std::string ::genesis::sequence::amino_acid_codes_plain ()", "Return all plain amino acid codes. Those are \"ACDEFGHIKLMNOPQRSTUVWY\"."},
    {"std::string ::genesis::sequence::amino_acid_codes_degenerated ()", "Return all degenerated amino acid codes. Those are \"BJZ\"."},
    {"std::string ::genesis::sequence::amino_acid_codes_undetermined ()", "Return all undetermined amino acid codes. Those are \"X*-?\"."},
    {"std::map< char, utils::Color > ::genesis::sequence::amino_acid_colors ()", "Return a map of Colors for each amino acid code.\n\nThis function gives a Color for each amino acid code."},
    {"std::map< char, std::string > ::genesis::sequence::amino_acid_text_colors ()", "Return a map of text colors for each amino acid code.\n\nThis function gives a color name usable for utils::Style for each amino acid code. The return value of this function can for example be used in sequence::print_color() function."},
    {"double ::genesis::sequence::averaged_entropy (SequenceCounts const & counts, bool only_determined_sites=false, SiteEntropyOptions per_site_options)", "Return the averaged sum of all site entropies.\n\nThis function sums up up the site_entropy() for all sites of the SequenceCount object and returns the average result per site.If only_determined_sites is false (default), the average is calculated using the total number of sites, that is, it simply calculates the average entropy per site.If only_determined_sites is true, the average is calculated using the number of determined sites only; that is, sites that only contain zeroes in all counts are skipped. Those sites do not contribute entropy anyway. Thus, it calcuates the average entropy per determiend site.The function additionally takes optional flags to refine the site entropy calculation, see SiteEntropyOptions for their explanation."},
    {"std::map< char, double > ::genesis::sequence::base_frequencies (SequenceSet const & set, std::string const & plain_chars)", "Get the base frequencies of the sites in a SequenceSet given the base chars.\n\nSee the Sequence implementation of this function for details."},
    {"std::map< char, double > ::genesis::sequence::base_frequencies (Sequence const & seq, std::string const & plain_chars)", "Get the base frequencies of the sites in a Sequence given the base chars.\n\nThis returns the relative proportions of the given plain_chars to each other. Typically, the given chars come from either nucleic_acid_codes_plain() or amino_acid_codes_plain(), depending on the dataset.It is necessary to select those chars on a per-dataset basis, as it is up to the user to define the meaning of those chars."},
    {"size_t ::genesis::sequence::count_chars (SequenceSet const & set, std::string const & chars)", "Count the number of occurrences of the given chars within the sites of the SequenceSet.\n\nThis function can be used to count e.g. gaps or ambiguous characters in sequences. For presettings of usable chars, see the functions nucleic_acid_codes_... and amino_acid_codes_.... The chars are treated case-insensitive.If chars contains invalid (non-standard ASCII) characters, an std::invalid_argument exception is thrown."},
    {"void ::genesis::sequence::filter_by_label_list (SequenceSet & set, std::unordered_set< std::string > const & labels, bool invert=false)", "Remove all those Sequences from a SequenceSet whose labels are in the given list.\n\nIf invert is set to true, it does the same inverted: it removes all Sequences except those whose label is in the list."},
    {"void ::genesis::sequence::filter_max_sequence_length (SequenceSet & set, size_t max_length)", "Remove all Sequences from the SequenceSet whose length is above the given max_length.\n\nSee also filter_min_sequence_length() and filter_min_max_sequence_length()."},
    {"void ::genesis::sequence::filter_min_max_sequence_length (SequenceSet & set, size_t min_length, size_t max_length)", "Remove all Sequences from the SequenceSet whose length is not inbetween the min_length and max_length.\n\nThis function has the same effect as calling filter_min_sequence_length() and filter_max_sequence_length(), but does it in one iteration over the SequenceSet."},
    {"void ::genesis::sequence::filter_min_sequence_length (SequenceSet & set, size_t min_length)", "Remove all Sequences from the SequenceSet whose length is below the given min_length.\n\nSee also filter_max_sequence_length() and filter_min_max_sequence_length()."},
    {"Sequence const * ::genesis::sequence::find_sequence (SequenceSet const & set, std::string const & label)", "Return a pointer to a Sequence with a specific label, or nullptr iff not found."},
    {"utils::Bitvector ::genesis::sequence::gap_sites (Sequence const & seq, std::string const & gap_chars)", "Return a Bitvector that is true where the Sequence has a gap and false where not.\n\nThe gap_chars are used case-insensitively to determine what is considerted to be a gap. By default, nucleic_acid_codes_undetermined() are used, but any other set of characters is allowed."},
    {"utils::Bitvector ::genesis::sequence::gap_sites (SequenceSet const & set, std::string const & gap_chars)", "Return a Bitvector that is true where all Sequences in the SequenceSet have a gap and false where not, that is, where at least on Sequence is not a gap.\n\nThe gap_chars are used case-insensitively to determine what is considerted to be a gap. By default, nucleic_acid_codes_undetermined() are used, but any other set of characters is allowed."},
    {"double ::genesis::sequence::gapyness (SequenceSet const & set, std::string const & gap_chars)", "Return the \"gapyness\" of the Sequences, i.e., the proportion of gap chars and other completely undetermined chars to the total length of all sequences.\n\nThis function returns a value in the interval 0.0 (no gaps and undetermined chars at all) and 1.0 (all chars are undetermined). See nucleic_acid_codes_undetermined() and amino_acid_codes_undetermined() for presettings of gap character that can be used here depending on the data set type. The chars are treated case-insensitive. In the special case that there are no sequences or sites, 0.0 is returned."},
    {"bool ::genesis::sequence::has_unique_labels (SequenceSet const & set, bool case_sensitive=true)", "Return true iff all labels of the Sequences in the SequenceSet are unique.\n\nThe optional parameter case_sensitive controls how labels are compared. Default is true, that is, Sequences are compared case-sensitively."},
    {"double ::genesis::sequence::absolute_entropy (SequenceCounts const & counts, SiteEntropyOptions per_site_options)", "Return the sum of all site entropies.\n\nThis function simply sums up up the site_entropy() for all sites of the SequenceCount object. The function additionally takes optional flags to refine the site entropy calculation, see SiteEntropyOptions for their explanation."},
    {"void ::genesis::taxonomy::levelorder_for_each (Taxonomy & tax, std::function< void(Taxon &)> fn, bool include_inner_taxa=true)", "Apply a function to all taxa of the Taxonomy, traversing it in levelorder.\n\nThe given Taxonomy is traversed in levelorder (i.e., breadth-first search). If include_inner_taxa is set to true (default), the provided functional is called for all Taxa . Otherwise, the functional is only called for the taxa of lowest rank, that is, for each Taxon that does not have sub-taxa.This is the non-const version of the function."},
    {"void ::genesis::taxonomy::levelorder_for_each (Taxonomy const & tax, std::function< void(Taxon const &)> fn, bool include_inner_taxa=true)", "Apply a function to all taxa of the Taxonomy, traversing it in levelorder.\n\nThe given Taxonomy is traversed in levelorder (i.e., breadth-first search). If include_inner_taxa is set to true (default), the provided functional is called for all Taxa . Otherwise, the functional is only called for the taxa of lowest rank, that is, for each Taxon that does not have sub-taxa.This is the const version of the function."},
    {"void ::genesis::taxonomy::postorder_for_each (Taxonomy & tax, std::function< void(Taxon &)> fn, bool include_inner_taxa=true)", "Apply a function to all taxa of the Taxonomy, traversing it in postorder.\n\nThe given Taxonomy is traversed in postorder (i.e., a variant of depth-first search). If include_inner_taxa is set to true (default), the provided functional is called for all Taxa . Otherwise, the functional is only called for the taxa of lowest rank, that is, for each Taxon that does not have sub-taxa.This is the non-const version of the function."},
    {"void ::genesis::taxonomy::postorder_for_each (Taxonomy const & tax, std::function< void(Taxon const &)> fn, bool include_inner_taxa=true)", "Apply a function to all taxa of the Taxonomy, traversing it in postorder.\n\nThe given Taxonomy is traversed in postorder (i.e., a variant of depth-first search). If include_inner_taxa is set to true (default), the provided functional is called for all Taxa . Otherwise, the functional is only called for the taxa of lowest rank, that is, for each Taxon that does not have sub-taxa.This is the const version of the function."},
    {"bool ::genesis::taxonomy::taxonomy_data_is (Taxonomy const & taxonomy)", "Check whether the data of a Taxonomy are exactly of the specified data type.\n\nThis function returns true iff all Taxa have data of the given type, using typeid() for this matching."},
    {"bool ::genesis::taxonomy::taxonomy_data_is_derived_from (Taxonomy const & taxonomy)", "Check whether the data of a Taxonomy are derived from the specified data type.\n\nThis function returns true iff all Taxa have data whose types are derived from the specified type. It uses dynamic_cast() for this."},
    {"bool ::genesis::taxonomy::validate_pruned_taxonomy (Taxonomy const & taxonomy)", "Validate that the pruning status of a Taxonomy is valid.\n\nThis function expects the Taxa of the Taxonomy to have data type EntropyTaxonData. It then checks whether the pruning states are all correctly set.That means:\nTaxa with status kInside can only have children of the same status or of kBorder.Taxa with status kBorder can only have children of status kOutside.Taxa with status kOutside can only have children of the same status.\nIf any of those conditions is not met, an information about the faulty Taxon is written to LOG_INFO, and the function returns false."},
    {"EmdTree ::genesis::tree::convert_default_tree_to_emd_tree (DefaultTree const & source)", "Helper function that takes a DefaultTree (or any Tree with Node and Edge data derived from it) and turns its data into an EmdTree, that is, a Tree with EmdNodeData and EmdEdgeData."},
    {"void ::genesis::taxonomy::preorder_for_each (Taxonomy & tax, std::function< void(Taxon &)> fn, bool include_inner_taxa=true)", "Apply a function to all taxa of the Taxonomy, traversing it in preorder.\n\nThe given Taxonomy is traversed in preorder (i.e., a variant of depth-first search). If include_inner_taxa is set to true (default), the provided functional is called for all Taxa . Otherwise, the functional is only called for the taxa of lowest rank, that is, for each Taxon that does not have sub-taxa.This is the non-const version of the function."},
    {"void ::genesis::taxonomy::preorder_for_each (Taxonomy const & tax, std::function< void(Taxon const &)> fn, bool include_inner_taxa=true)", "Apply a function to all taxa of the Taxonomy, traversing it in preorder.\n\nThe given Taxonomy is traversed in preorder (i.e., a variant of depth-first search). If include_inner_taxa is set to true (default), the provided functional is called for all Taxa . Otherwise, the functional is only called for the taxa of lowest rank, that is, for each Taxon that does not have sub-taxa.This is the const version of the function."},
    {"void ::genesis::taxonomy::prune_by_entropy (Taxonomy & taxonomy, size_t target_taxonomy_size, PruneByEntropySettings settings={})", "Prune a Taxonomy so that the result (approximately) contains a desired number of \"leaf\" Taxa, using the entropy of the Taxa as indicator where to prune.\n\nThe function takes a Taxonomy with data type EntropyTaxonData and a target size which indicates the desired number of \"leaf\" Taxa after pruning the Taxonomy. In the pruned Taxonomy, some Taxa are considered as belonging to the Taxonomy (have status EntropyTaxonData::PruneStatus::kInside or EntropyTaxonData::PruneStatus::kBorder), while others (deeper in the Taxonomy) are excluded (have status EntropyTaxonData::PruneStatus::kOutside). The number of border taxa (or \"leaves\") of the included Taxa then is aimed to be as close as possible to the target size.That means, this function sets the status of the Taxa, but does not remove any Taxa. All Taxa with status EntropyTaxonData::PruneStatus::kOutside are then considered to be pruned from the taxonomy.Example: The Taxonomy Tax_1\n    Tax_2\n        Tax_3\n        Tax_4\n    Tax_5\n        Tax_6\nTax_7\n    Tax_8\n    Tax_9\ncontains 5 \"leaf\" taxa, i.e., Tax_3, Tax_4, Tax_6, Tax_8 and Tax_9. If we want to prune it with a target size of 3, we might end up with either Tax_1\n    Tax_2\n    Tax_5\nTax_7\nor Tax_1\nTax_7\n    Tax_8\n    Tax_9\nas both contain 3 \"leaves\": Tax_2, Tax_5 and Tax_7 in the former case and Tax_1, Tax_8 and Tax_9 in the latter. Which of those two is used depends on the entropies of the Taxa.In the former case, Tax_1 is considered inside, Tax_2, Tax_5 and Tax_7 are border, and all other taxa are outside of the pruned Taxonomy. In the latter case, Tax_7 is inside, Tax_1, Tax_8 and Tax_9 are border, and again all others are outside.It is not always possible to prune a Taxonomy in a way the we exaclty hit the target size. The function then ends at a number of border Taxa that is closest (either below or above the target size).In order to decide which Taxa to set to inside (i.e., not include as leaves, but further resolve into their children), we use the entropies of the Taxa: We choose to split up at a current border Taxon with the highest entropy value, as long as this brings us closer to the target size.This means that the above case where we had two possible ways of splitting should be rare, as the entropies will rarely be identical with real world data sets. If this happens nonetheless, it is random which of the Taxa with equal entropy will be used.In order to control further settings, see PruneByEntropySettings."},
    {"std::string ::genesis::taxonomy::print_pruned_taxonomy (Taxonomy const & taxonomy)", "Print a Taxonomy, highlighting those Taxa that are the pruning border, i.e., where we cut off the sub-taxa, and print their entropies next to them."},
    {"void ::genesis::taxonomy::remove_pruned_taxonomy_children (Taxonomy & taxonomy)", "Remove the children of all Taxa that are pruned, i.e, that have prune status == kOutside.\n\nThe function does not validate the status before. Use validate_pruned_taxonomy() if you are unsure whether the status is correct for all Taxa."},
    {"void ::genesis::taxonomy::reset_taxonomy_data (Taxonomy & taxonomy, bool allow_overwrite=true)", "(Re-)set all Taxon data of a Taxonomy to a specified data type.\n\nThe data is created empty, using BaseTaxonData::create(). If the optional parameter allow_overwrite is set to false (instead of the default true), the function throws an exception if a Taxon already has data assigned to it."},
    {"void ::genesis::tree::reroot (Tree & tree, TreeNode & at_node)", "Reroot the Tree at the given TreeNode.\n\nSee this version of the function for details.The node needs to be part of the tree, otherwise an exception is thrown."},
    {"void ::genesis::tree::reroot (Tree & tree, TreeLink & at_link)", "Reroot the Tree at the given TreeLink.\n\nThe function sets the root of the tree to the node of the given link. This operation does not change the topology of the tree, but merely adjusts some internal properties. The main changes are that Tree::root_node() and Tree::root_link() will return the new root after calling this function, and that tree iterators will start traversing the tree from this new root by default.There are three internal changes made to the tree data structure:\nAll primary and secondary ends of the edges on the path between the new root and the old root are swapped. This is because the edges now need to point towards the new root.Similarly, all (primary) links of the nodes on that path are changed so that they point towards the new root.Also, the (internal) root_link_index is changed to the new root link. This is used for the functions Tree::root_node() and Tree::root_link().\nThe difference between this function and its node version is that when specifying a specific link, this link is used as the (primary) link of the new root node. This way, algorithms and iterators (e.g., IteratorLevelorder) will start traversing the tree in the direction of this link by default. When specifying a node for rerooting instead, the primary link of that node is used, so that iterators start traversing the tree in the direction of the old root instead. For most applications, this does not make a difference. However, there might be cases where the start directing makes a difference. Thus, we offer both versions of this function.The link needs to be part of the tree, otherwise an exception is thrown."},
    {"void ::genesis::tree::reroot_at_node (Tree & tree, size_t node_index)", "Reroot the Tree at the TreeNode with the given index.\n\nSee reroot(...) for details.The node index needs to be valid for the tree, otherwise an exception is thrown."},
    {"size_t ::genesis::taxonomy::count_taxa_with_prune_status (Taxonomy const & taxonomy, EntropyTaxonData::PruneStatus status)", "Return the number of Taxa that have a certain prune status."},
    {"void ::genesis::taxonomy::expand_small_subtaxonomies (Taxonomy & taxonomy, size_t min_subtaxonomy_size)", "Expand the leaves of a pruned Taxonomy if their sub-taxonomies are smaller than the given threshold.\n\nThis function takes a Taxonomy with EntropyTaxonData on its Taxa and looks for taxa with status kBorder which have fewer than the threshold many leaves. If so, this sub-taxonomy is expaneded. This is, it is turned into taxa with status kInside for inner taxa and kBorder for leaf taxa."},
    {"bool ::genesis::tree::tree_data_is (Tree const & tree)", "Check whether the data of the nodes and edges of the Tree are exactly of the specified data types.\n\nThis function returns true iff all data types of the nodes and edges match exaclty the specified types. It uses typeid() for this matching."},
    {"bool ::genesis::tree::tree_data_is_derived_from (Tree const & tree)", "Check whether the data of the nodes and edges of the Tree are derived from the specified data types.\n\nThis function returns true iff all data types of the nodes and edges are derived from the specified types. It uses dynamic_cast() for this."},
    {"bool ::genesis::tree::validate_topology (Tree const & tree)", "This function is defined in tree/functions/operators.hpp"},
};

const char* get_docstring (const std::string& signature)
{
    if (doc_strings_.count(signature) > 0) {
        return doc_strings_[signature].c_str();
    } else {
        return "";
    }
}

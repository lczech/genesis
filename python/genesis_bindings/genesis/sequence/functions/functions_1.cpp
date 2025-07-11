#include <array>
#include <genesis/sequence/functions/functions.hpp>
#include <genesis/sequence/functions/quality.hpp>
#include <genesis/sequence/sequence.hpp>
#include <genesis/sequence/sequence_set.hpp>
#include <genesis/utils/io/base_input_source.hpp>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include <functional>
#include <pybind11/pybind11.h>
#include <string>
#include <pybind11/functional.h>
#include <../python/custom_bindings/extensions/utils/matrix.hpp>
#include <genesis/utils/color/functions.hpp>
#include <../python/custom_bindings/extensions/utils/bitvector.hpp>
#include <../python/custom_bindings/extensions/utils/range.hpp>
#include <../python/custom_bindings/extensions/string_target.hpp>
#include <../python/custom_bindings/extensions/sequence/quality.hpp>
#include <../python/custom_bindings/extensions/sequence/sequence_set.hpp>
#include <../python/custom_bindings/extensions/sequence/sequence_dict.hpp>
#include <../python/custom_bindings/extensions/sequence/fasta_input_iterator.hpp>
#include <../python/custom_bindings/extensions/sequence/fastq_input_iterator.hpp>
#include <../python/custom_bindings/extensions/sequence/reference_genome.hpp>
#include <../python/custom_bindings/extensions/taxonomy/taxopath.hpp>
#include <../python/custom_bindings/extensions/taxonomy/iterator.hpp>
#include <../python/custom_bindings/extensions/tree/tree.hpp>
#include <../python/custom_bindings/extensions/tree/functions_tree.hpp>
#include <../python/custom_bindings/extensions/placement/helper.hpp>
#include <pybind11/stl.h>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>)
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*)
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>)
#endif

void bind_genesis_sequence_functions_functions_1(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::sequence::normalize_nucleic_acid_codes(class genesis::sequence::Sequence &, bool) file:genesis/sequence/functions/functions.hpp line:292
	M("genesis::sequence").def("normalize_nucleic_acid_codes", [](class genesis::sequence::Sequence & a0) -> void { return genesis::sequence::normalize_nucleic_acid_codes(a0); }, "", pybind11::arg("sequence"));
	M("genesis::sequence").def("normalize_nucleic_acid_codes", (void (*)(class genesis::sequence::Sequence &, bool)) &genesis::sequence::normalize_nucleic_acid_codes, "Call normalize_nucleic_acid_code() for each site of the Sequence.\n\n See there for details.\n\nC++: genesis::sequence::normalize_nucleic_acid_codes(class genesis::sequence::Sequence &, bool) --> void", pybind11::arg("sequence"), pybind11::arg("accept_degenerated"));

	// genesis::sequence::normalize_nucleic_acid_codes(class genesis::sequence::SequenceSet &, bool) file:genesis/sequence/functions/functions.hpp line:299
	M("genesis::sequence").def("normalize_nucleic_acid_codes", [](class genesis::sequence::SequenceSet & a0) -> void { return genesis::sequence::normalize_nucleic_acid_codes(a0); }, "", pybind11::arg("sequence_set"));
	M("genesis::sequence").def("normalize_nucleic_acid_codes", (void (*)(class genesis::sequence::SequenceSet &, bool)) &genesis::sequence::normalize_nucleic_acid_codes, "Call normalize_nucleic_acid_code() for each site of all Sequence%s in the SequenceSet.\n\n See there for details.\n\nC++: genesis::sequence::normalize_nucleic_acid_codes(class genesis::sequence::SequenceSet &, bool) --> void", pybind11::arg("sequence_set"), pybind11::arg("accept_degenerated"));

	// genesis::sequence::normalize_amino_acid_codes(class genesis::sequence::Sequence &, bool) file:genesis/sequence/functions/functions.hpp line:306
	M("genesis::sequence").def("normalize_amino_acid_codes", [](class genesis::sequence::Sequence & a0) -> void { return genesis::sequence::normalize_amino_acid_codes(a0); }, "", pybind11::arg("sequence"));
	M("genesis::sequence").def("normalize_amino_acid_codes", (void (*)(class genesis::sequence::Sequence &, bool)) &genesis::sequence::normalize_amino_acid_codes, "Call normalize_amino_acid_code() for each site of the Sequence.\n\n See there for details.\n\nC++: genesis::sequence::normalize_amino_acid_codes(class genesis::sequence::Sequence &, bool) --> void", pybind11::arg("sequence"), pybind11::arg("accept_degenerated"));

	// genesis::sequence::normalize_amino_acid_codes(class genesis::sequence::SequenceSet &, bool) file:genesis/sequence/functions/functions.hpp line:313
	M("genesis::sequence").def("normalize_amino_acid_codes", [](class genesis::sequence::SequenceSet & a0) -> void { return genesis::sequence::normalize_amino_acid_codes(a0); }, "", pybind11::arg("sequence_set"));
	M("genesis::sequence").def("normalize_amino_acid_codes", (void (*)(class genesis::sequence::SequenceSet &, bool)) &genesis::sequence::normalize_amino_acid_codes, "Call normalize_amino_acid_code() for each site of all Sequence%s in the SequenceSet.\n\n See there for details.\n\nC++: genesis::sequence::normalize_amino_acid_codes(class genesis::sequence::SequenceSet &, bool) --> void", pybind11::arg("sequence_set"), pybind11::arg("accept_degenerated"));

	// genesis::sequence::filter_min_sequence_length(class genesis::sequence::SequenceSet &, unsigned long) file:genesis/sequence/functions/functions.hpp line:325
	M("genesis::sequence").def("filter_min_sequence_length", (void (*)(class genesis::sequence::SequenceSet &, unsigned long)) &genesis::sequence::filter_min_sequence_length, "Remove all Sequence%s from the SequenceSet whose \n\n\n\n\n\nC++: genesis::sequence::filter_min_sequence_length(class genesis::sequence::SequenceSet &, unsigned long) --> void", pybind11::arg("set"), pybind11::arg("min_length"));

	// genesis::sequence::filter_max_sequence_length(class genesis::sequence::SequenceSet &, unsigned long) file:genesis/sequence/functions/functions.hpp line:333
	M("genesis::sequence").def("filter_max_sequence_length", (void (*)(class genesis::sequence::SequenceSet &, unsigned long)) &genesis::sequence::filter_max_sequence_length, "Remove all Sequence%s from the SequenceSet whose \n\n\n\n\n\nC++: genesis::sequence::filter_max_sequence_length(class genesis::sequence::SequenceSet &, unsigned long) --> void", pybind11::arg("set"), pybind11::arg("max_length"));

	// genesis::sequence::filter_min_max_sequence_length(class genesis::sequence::SequenceSet &, unsigned long, unsigned long) file:genesis/sequence/functions/functions.hpp line:342
	M("genesis::sequence").def("filter_min_max_sequence_length", (void (*)(class genesis::sequence::SequenceSet &, unsigned long, unsigned long)) &genesis::sequence::filter_min_max_sequence_length, "Remove all Sequence%s from the SequenceSet whose \n\n\n\n\n\n\nC++: genesis::sequence::filter_min_max_sequence_length(class genesis::sequence::SequenceSet &, unsigned long, unsigned long) --> void", pybind11::arg("set"), pybind11::arg("min_length"), pybind11::arg("max_length"));

	// genesis::sequence::QualityEncoding file:genesis/sequence/functions/quality.hpp line:72
	pybind11::enum_<genesis::sequence::QualityEncoding>(M("genesis::sequence"), "QualityEncoding", "List of quality encodings for which we support decoding.\n\n We offer the following quality score encodings:\n\n  *  Sanger, with offset 33.\n  *  Illumina 1.3+, with offset 64.\n  *  Illumina 1.5+, with offset 64.\n  *  Illumina 1.8+, with offset 33.\n  *  Solexa, with offset 64, and a special encoding equation.\n\n These are the types of encodings used in fastq files over the years.\n It seems that Sanger / Illumina 1.8+ is the most commonly used one today,\n so this is also what we use as a default.")
		.value("kSanger", genesis::sequence::QualityEncoding::kSanger)
		.value("kSolexa", genesis::sequence::QualityEncoding::kSolexa)
		.value("kIllumina13", genesis::sequence::QualityEncoding::kIllumina13)
		.value("kIllumina15", genesis::sequence::QualityEncoding::kIllumina15)
		.value("kIllumina18", genesis::sequence::QualityEncoding::kIllumina18);

;

	// genesis::sequence::quality_encoding_name(enum genesis::sequence::QualityEncoding, bool) file:genesis/sequence/functions/quality.hpp line:87
	M("genesis::sequence").def("quality_encoding_name", [](enum genesis::sequence::QualityEncoding const & a0) -> std::string { return genesis::sequence::quality_encoding_name(a0); }, "", pybind11::arg("encoding"));
	M("genesis::sequence").def("quality_encoding_name", (std::string (*)(enum genesis::sequence::QualityEncoding, bool)) &genesis::sequence::quality_encoding_name, "Return a readable name for each of the encoding types.\n\n See QualityEncoding for the names being used here. If  is set, the offset of the\n encoding is also reported in parenthesis (e.g., \"Illumina 1.8+ (ASCII offset 33)\").\n\nC++: genesis::sequence::quality_encoding_name(enum genesis::sequence::QualityEncoding, bool) --> std::string", pybind11::arg("encoding"), pybind11::arg("with_offset"));

	// genesis::sequence::guess_quality_encoding_from_name(const std::string &) file:genesis/sequence/functions/quality.hpp line:98
	M("genesis::sequence").def("guess_quality_encoding_from_name", (enum genesis::sequence::QualityEncoding (*)(const std::string &)) &genesis::sequence::guess_quality_encoding_from_name, "Guess the QualityEncoding type, given its description name.\n\n This is the reverse of quality_encoding_name(), but additionally allows the given  to be\n fuzzy. The  is stripped off all non-alphanumerical characters and made lower-case.\n The remainder (e.g., `illumina13`) then is matched to the enum QualityEncoding, with the\n additional option that just `illumina` (without any version number) is matched to\n QualityEncoding::kIllumina18.\n\nC++: genesis::sequence::guess_quality_encoding_from_name(const std::string &) --> enum genesis::sequence::QualityEncoding", pybind11::arg("name"));

	// genesis::sequence::quality_encode_from_phred_score(unsigned char, bool) file:genesis/sequence/functions/quality.hpp line:141
	M("genesis::sequence").def("quality_encode_from_phred_score", [](unsigned char const & a0) -> char { return genesis::sequence::quality_encode_from_phred_score(a0); }, "", pybind11::arg("phred_score"));
	M("genesis::sequence").def("quality_encode_from_phred_score", (char (*)(unsigned char, bool)) &genesis::sequence::quality_encode_from_phred_score, "Encode a phred score into a quality char, using the Sanger convention.\n\n This function takes a  in the range 0 to 93, and encodes it, for example for\n usage in a fastq file, by adding the ASCII offset 33 to it.\n\n While we can decode from numerous formats, see quality_decode_to_phred_score(), we only\n support encoding back to the Sanger format, because we want to minimize confusion and\n maximize compatability with other programs. Also, Sanger is used by the NCBI Short Read Archive\n and Illumina 1.8+, and hence the most common format as of today.\n\n If the flag  is set (default), values outside of the valid range 0 to 93 are clamped,\n that is, set to be inside the valid range. As the phred score is unsigned, this leads to values\n above 93 simply being encoded as if they were exactly 93. If  is set to false,\n an exception is thrown instead if a value above 93 is encountered.\n\nC++: genesis::sequence::quality_encode_from_phred_score(unsigned char, bool) --> char", pybind11::arg("phred_score"), pybind11::arg("clamp"));

	// genesis::sequence::quality_encode_from_phred_score(const class std::vector<unsigned char, class std::allocator<unsigned char> > &, bool) file:genesis/sequence/functions/quality.hpp line:162
	M("genesis::sequence").def("quality_encode_from_phred_score", [](const class std::vector<unsigned char, class std::allocator<unsigned char> > & a0) -> std::string { return genesis::sequence::quality_encode_from_phred_score(a0); }, "", pybind11::arg("phred_scores"));
	M("genesis::sequence").def("quality_encode_from_phred_score", (std::string (*)(const class std::vector<unsigned char, class std::allocator<unsigned char> > &, bool)) &genesis::sequence::quality_encode_from_phred_score, "Encode phred scores into quality chars, using the Sanger convention.\n\n  quality_encode_from_phred_score( unsigned char, bool )\n\nC++: genesis::sequence::quality_encode_from_phred_score(const class std::vector<unsigned char, class std::allocator<unsigned char> > &, bool) --> std::string", pybind11::arg("phred_scores"), pybind11::arg("clamp"));

	// genesis::sequence::compatible_quality_encodings(enum genesis::sequence::QualityEncoding, enum genesis::sequence::QualityEncoding) file:genesis/sequence/functions/quality.hpp line:187
	M("genesis::sequence").def("compatible_quality_encodings", (bool (*)(enum genesis::sequence::QualityEncoding, enum genesis::sequence::QualityEncoding)) &genesis::sequence::compatible_quality_encodings, "Return whether two quality encodings are compatible with each other.\n\n This is the case when both have the same ASCII offset, except for Solexa, which is not compatible\n with any of the others. This function is meant as a check when for example the user specified\n one encoding, but functions such as guess_quality_encoding() and guess_fastq_quality_encoding()\n return a different one - the two encodings might be indistinguishable for that function, because\n the have the same offset. That also means that one can be used to decode the other - they are\n compatible.\n\nC++: genesis::sequence::compatible_quality_encodings(enum genesis::sequence::QualityEncoding, enum genesis::sequence::QualityEncoding) --> bool", pybind11::arg("lhs"), pybind11::arg("rhs"));

	// genesis::sequence::guess_quality_encoding(const struct std::array<unsigned long, 128> &) file:genesis/sequence/functions/quality.hpp line:201
	M("genesis::sequence").def("guess_quality_encoding", (enum genesis::sequence::QualityEncoding (*)(const struct std::array<unsigned long, 128> &)) &genesis::sequence::guess_quality_encoding, "Guess the quality score encoding, based on counts of how often each char appeared\n in the quality string (of a fastq file for example).\n\n The  needs to be filled with counts of how often each quality code char appeared\n in the (fastq) quality strings. If any values outside of the printable character range (ASCII 33\n to 127) are non-zero in the  the function throws, as these are invaliv qualiy\n encodings. Otherwise, it guesses which QualityEncoding was used for the data, based on\n which chars appear in it.\n\n \n guess_fastq_quality_encoding()\n\nC++: genesis::sequence::guess_quality_encoding(const struct std::array<unsigned long, 128> &) --> enum genesis::sequence::QualityEncoding", pybind11::arg("char_counts"));

	// genesis::sequence::guess_fastq_quality_encoding(class std::shared_ptr<class genesis::utils::BaseInputSource>, unsigned long, unsigned long) file:genesis/sequence/functions/quality.hpp line:214
	M("genesis::sequence").def("guess_fastq_quality_encoding", [](class std::shared_ptr<class genesis::utils::BaseInputSource> const & a0) -> genesis::sequence::QualityEncoding { return genesis::sequence::guess_fastq_quality_encoding(a0); }, "", pybind11::arg("source"));
	M("genesis::sequence").def("guess_fastq_quality_encoding", [](class std::shared_ptr<class genesis::utils::BaseInputSource> const & a0, unsigned long const & a1) -> genesis::sequence::QualityEncoding { return genesis::sequence::guess_fastq_quality_encoding(a0, a1); }, "", pybind11::arg("source"), pybind11::arg("max_lines"));
	M("genesis::sequence").def("guess_fastq_quality_encoding", (enum genesis::sequence::QualityEncoding (*)(class std::shared_ptr<class genesis::utils::BaseInputSource>, unsigned long, unsigned long)) &genesis::sequence::guess_fastq_quality_encoding, "Guess the quality score encoding for a fastq input, based on counts of how often each\n char appeared in the quality string (of the input fastq file for example).\n\n The function reads and parses the input source as a fastq file, counts all quality score chars\n as they appear in there, and then guesses the encoding that was used.\n If  is set to a value greater than 0, only that many lines are read.\n If  is set to a value greater than 0, only that many quality score charaters are read.\n\n \n guess_quality_encoding()\n\nC++: genesis::sequence::guess_fastq_quality_encoding(class std::shared_ptr<class genesis::utils::BaseInputSource>, unsigned long, unsigned long) --> enum genesis::sequence::QualityEncoding", pybind11::arg("source"), pybind11::arg("max_lines"), pybind11::arg("max_chars"));

	// genesis::sequence::error_probability_to_phred_score(double) file:genesis/sequence/functions/quality.hpp line:224
	M("genesis::sequence").def("error_probability_to_phred_score", (unsigned char (*)(double)) &genesis::sequence::error_probability_to_phred_score, "C++: genesis::sequence::error_probability_to_phred_score(double) --> unsigned char", pybind11::arg("error_probability"));

	// genesis::sequence::error_probability_to_phred_score(class std::vector<double, class std::allocator<double> >) file:genesis/sequence/functions/quality.hpp line:225
	M("genesis::sequence").def("error_probability_to_phred_score", (class std::vector<unsigned char, class std::allocator<unsigned char> > (*)(class std::vector<double, class std::allocator<double> >)) &genesis::sequence::error_probability_to_phred_score, "C++: genesis::sequence::error_probability_to_phred_score(class std::vector<double, class std::allocator<double> >) --> class std::vector<unsigned char, class std::allocator<unsigned char> >", pybind11::arg("error_probability"));

	// genesis::sequence::phred_score_to_error_probability(unsigned char) file:genesis/sequence/functions/quality.hpp line:227
	M("genesis::sequence").def("phred_score_to_error_probability", (double (*)(unsigned char)) &genesis::sequence::phred_score_to_error_probability, "C++: genesis::sequence::phred_score_to_error_probability(unsigned char) --> double", pybind11::arg("phred_score"));

	// genesis::sequence::phred_score_to_error_probability(class std::vector<unsigned char, class std::allocator<unsigned char> >) file:genesis/sequence/functions/quality.hpp line:228
	M("genesis::sequence").def("phred_score_to_error_probability", (class std::vector<double, class std::allocator<double> > (*)(class std::vector<unsigned char, class std::allocator<unsigned char> >)) &genesis::sequence::phred_score_to_error_probability, "C++: genesis::sequence::phred_score_to_error_probability(class std::vector<unsigned char, class std::allocator<unsigned char> >) --> class std::vector<double, class std::allocator<double> >", pybind11::arg("phred_score"));

}

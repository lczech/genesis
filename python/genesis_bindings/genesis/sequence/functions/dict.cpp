#include <genesis/sequence/formats/fasta_input_iterator.hpp>
#include <genesis/sequence/formats/fasta_output_iterator.hpp>
#include <genesis/sequence/formats/fasta_reader.hpp>
#include <genesis/sequence/formats/fasta_writer.hpp>
#include <genesis/sequence/functions/dict.hpp>
#include <genesis/sequence/reference_genome.hpp>
#include <genesis/sequence/sequence.hpp>
#include <genesis/sequence/sequence_dict.hpp>
#include <genesis/sequence/sequence_set.hpp>
#include <genesis/utils/io/base_input_source.hpp>
#include <genesis/utils/io/base_output_target.hpp>
#include <genesis/utils/io/input_stream.hpp>
#include <genesis/utils/tools/char_lookup.hpp>
#include <ios>
#include <iterator>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
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

void bind_genesis_sequence_functions_dict(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::sequence::read_sequence_dict(class std::shared_ptr<class genesis::utils::BaseInputSource>) file:genesis/sequence/functions/dict.hpp line:61
	M("genesis::sequence").def("read_sequence_dict", (class genesis::sequence::SequenceDict (*)(class std::shared_ptr<class genesis::utils::BaseInputSource>)) &genesis::sequence::read_sequence_dict, "Read a `.dict` sequence dictionary file, describing, e.g., reference genome sequence\n properties.\n\n The file format is for example produced by `GATK/Picard CreateSequenceDictionary`\n or by `samtools dict`. More information on the format can be found at:\n\n    * http://www.htslib.org/doc/samtools-dict.html\n    * https://gatk.broadinstitute.org/hc/en-us/articles/360036729911-CreateSequenceDictionary-Picard-\n\n See there for details.\n\nC++: genesis::sequence::read_sequence_dict(class std::shared_ptr<class genesis::utils::BaseInputSource>) --> class genesis::sequence::SequenceDict", pybind11::arg("source"));

	// genesis::sequence::read_sequence_fai(class std::shared_ptr<class genesis::utils::BaseInputSource>) file:genesis/sequence/functions/dict.hpp line:73
	M("genesis::sequence").def("read_sequence_fai", (class genesis::sequence::SequenceDict (*)(class std::shared_ptr<class genesis::utils::BaseInputSource>)) &genesis::sequence::read_sequence_fai, "Read a `.fai` sequence index file, describing, e.g., reference genome sequence properties.\n\n The file format is for example produced by `samtools faidx`.\n More information on the format can be found at:\n\n    * http://www.htslib.org/doc/samtools-faidx.html\n\n See there for details.\n\nC++: genesis::sequence::read_sequence_fai(class std::shared_ptr<class genesis::utils::BaseInputSource>) --> class genesis::sequence::SequenceDict", pybind11::arg("source"));

	// genesis::sequence::sequence_set_to_dict(const class genesis::sequence::SequenceSet &) file:genesis/sequence/functions/dict.hpp line:83
	M("genesis::sequence").def("sequence_set_to_dict", (class genesis::sequence::SequenceDict (*)(const class genesis::sequence::SequenceSet &)) &genesis::sequence::sequence_set_to_dict, "Get the sequence dict/index information of a given set of Sequence%s.\n\n This is a helper function that can be used to obtain an overview of the names and lengths\n of a set of sequences, for some downstream processing, or as information about a reference genome.\n\n \n reference_genome_to_dict()\n\nC++: genesis::sequence::sequence_set_to_dict(const class genesis::sequence::SequenceSet &) --> class genesis::sequence::SequenceDict", pybind11::arg("set"));

	// genesis::sequence::reference_genome_to_dict(const class genesis::sequence::ReferenceGenome &) file:genesis/sequence/functions/dict.hpp line:94
	M("genesis::sequence").def("reference_genome_to_dict", (class genesis::sequence::SequenceDict (*)(const class genesis::sequence::ReferenceGenome &)) &genesis::sequence::reference_genome_to_dict, "Get the sequence dict/index information of a given set of Sequence%s that are stored\n in a ReferenceGenome.\n\n This is a helper function that can be used to obtain an overview of the names and lengths\n of a set of sequences, for some downstream processing, or as information about a reference genome.\n\n \n sequence_set_to_dict()\n\nC++: genesis::sequence::reference_genome_to_dict(const class genesis::sequence::ReferenceGenome &) --> class genesis::sequence::SequenceDict", pybind11::arg("rg"));

	// genesis::sequence::verify(const class genesis::sequence::SequenceDict &, const class genesis::sequence::SequenceSet &, bool) file:genesis/sequence/functions/dict.hpp line:106
	M("genesis::sequence").def("verify", [](const class genesis::sequence::SequenceDict & a0, const class genesis::sequence::SequenceSet & a1) -> bool { return genesis::sequence::verify(a0, a1); }, "", pybind11::arg("dict"), pybind11::arg("set"));
	M("genesis::sequence").def("verify", (bool (*)(const class genesis::sequence::SequenceDict &, const class genesis::sequence::SequenceSet &, bool)) &genesis::sequence::verify, "Verify that a SequenceDict fits a SequenceSet.\n\n The function return `true` iff the  has sequences of the same name and length,\n in the same order, as the  and `false` otherwise.\n\n If  is set, we cut off the sequence names after the first tab or space\n character, in case there are any, as this is what typical fasta indexing tools also seem to do.\n In that case, anything after the first tab or space is not used for the comparison.\n\nC++: genesis::sequence::verify(const class genesis::sequence::SequenceDict &, const class genesis::sequence::SequenceSet &, bool) --> bool", pybind11::arg("dict"), pybind11::arg("set"), pybind11::arg("match_first_word"));

	{ // genesis::sequence::FastaWriter file:genesis/sequence/formats/fasta_writer.hpp line:67
		pybind11::class_<genesis::sequence::FastaWriter, std::shared_ptr<genesis::sequence::FastaWriter>> cl(M("genesis::sequence"), "FastaWriter", "Write Fasta data.\n\n This class provides simple facilities for writing Fasta data.\n\n Exemplary usage:\n\n     std::string outfile = \"path/to/file.fasta\";\n     SequenceSet sequence_set;\n\n     FastaWriter().line_length( 100 ).write( sequence_set, utils::to_file( outfile ));\n\n See FastaReader for a description of the Fasta format.");
		cl.def( pybind11::init( [](){ return new genesis::sequence::FastaWriter(); } ) );
		cl.def( pybind11::init( [](genesis::sequence::FastaWriter const &o){ return new genesis::sequence::FastaWriter(o); } ) );

		pybind11::enum_<genesis::sequence::FastaWriter::AbundanceNotation>(cl, "AbundanceNotation", "Enumeration of types for how to write Sequence abundances.")
			.value("kNone", genesis::sequence::FastaWriter::AbundanceNotation::kNone)
			.value("kUnderscore", genesis::sequence::FastaWriter::AbundanceNotation::kUnderscore)
			.value("kSize", genesis::sequence::FastaWriter::AbundanceNotation::kSize);

		cl.def("assign", (class genesis::sequence::FastaWriter & (genesis::sequence::FastaWriter::*)(const class genesis::sequence::FastaWriter &)) &genesis::sequence::FastaWriter::operator=, "C++: genesis::sequence::FastaWriter::operator=(const class genesis::sequence::FastaWriter &) --> class genesis::sequence::FastaWriter &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("write", (void (genesis::sequence::FastaWriter::*)(const class genesis::sequence::Sequence &, class std::shared_ptr<class genesis::utils::BaseOutputTarget>) const) &genesis::sequence::FastaWriter::write, "Write a single Sequence to an output target, using the Fasta format.\n\n See the output target convenience functions utils::to_file(), utils::to_stream(), and\n utils::to_string() for examples of how to obtain a suitable output target.\n\nC++: genesis::sequence::FastaWriter::write(const class genesis::sequence::Sequence &, class std::shared_ptr<class genesis::utils::BaseOutputTarget>) const --> void", pybind11::arg("sequence"), pybind11::arg("target"));
		cl.def("write", (void (genesis::sequence::FastaWriter::*)(const class genesis::sequence::SequenceSet &, class std::shared_ptr<class genesis::utils::BaseOutputTarget>) const) &genesis::sequence::FastaWriter::write, "Write a SequenceSet to an output target, using the Fasta format.\n\n See the output target convenience functions utils::to_file(), utils::to_stream(), and\n utils::to_string() for examples of how to obtain a suitable output target.\n\nC++: genesis::sequence::FastaWriter::write(const class genesis::sequence::SequenceSet &, class std::shared_ptr<class genesis::utils::BaseOutputTarget>) const --> void", pybind11::arg("sequence_set"), pybind11::arg("target"));
		cl.def("write_sequence", (void (genesis::sequence::FastaWriter::*)(const class genesis::sequence::Sequence &, std::ostream &) const) &genesis::sequence::FastaWriter::write_sequence, "Write a single Sequence to an output stream in Fasta format.\n\nC++: genesis::sequence::FastaWriter::write_sequence(const class genesis::sequence::Sequence &, std::ostream &) const --> void", pybind11::arg("sequence"), pybind11::arg("os"));
		cl.def("line_length", (class genesis::sequence::FastaWriter & (genesis::sequence::FastaWriter::*)(unsigned long)) &genesis::sequence::FastaWriter::line_length, "Set the line length, which determines after how many chars (Sequence sites) lines breaks\n are inserted when writing the Fasta file.\n\n Default is `80`. If set to `0`, no breaks are inserted.\n The functions returns the FastaWriter object to allow fluent interfaces.\n\nC++: genesis::sequence::FastaWriter::line_length(unsigned long) --> class genesis::sequence::FastaWriter &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("line_length", (unsigned long (genesis::sequence::FastaWriter::*)() const) &genesis::sequence::FastaWriter::line_length, "Get the current line length.\n\n See the setter line_length( size_t ) for details.\n\nC++: genesis::sequence::FastaWriter::line_length() const --> unsigned long");
		cl.def("abundance_notation", (class genesis::sequence::FastaWriter & (genesis::sequence::FastaWriter::*)(enum genesis::sequence::FastaWriter::AbundanceNotation)) &genesis::sequence::FastaWriter::abundance_notation, "Set the notation for how to write Sequence abundances.\n\n Default is to not write abundances. Use this setting to change that.\n\nC++: genesis::sequence::FastaWriter::abundance_notation(enum genesis::sequence::FastaWriter::AbundanceNotation) --> class genesis::sequence::FastaWriter &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("abundance_notation", (enum genesis::sequence::FastaWriter::AbundanceNotation (genesis::sequence::FastaWriter::*)() const) &genesis::sequence::FastaWriter::abundance_notation, "Get the current setting for how to write abundances.\n\nC++: genesis::sequence::FastaWriter::abundance_notation() const --> enum genesis::sequence::FastaWriter::AbundanceNotation");
	}
	{ // genesis::sequence::FastaInputIterator file:genesis/sequence/formats/fasta_input_iterator.hpp line:87
		pybind11::class_<genesis::sequence::FastaInputIterator, std::shared_ptr<genesis::sequence::FastaInputIterator>> cl(M("genesis::sequence"), "FastaInputIterator", "Iterate an input source and parse it as Fasta sequences.\n\n This class allows to iterate over an input source, interpreting it as Fasta sequences, and\n yielding one such sequence per iteration step. This is useful for processing large files without\n having to keep them fully in memory.\n\n Example:\n\n     for( auto const& s : FastaInputIterator( from_file( \"/path/to/large_file.fasta\" ))) {\n         std::cout << s.length() << \"\";\n     }\n\n Alternatively, the following also works:\n\n     auto it = FastaInputIterator( from_file( \"/path/to/large_file.fasta\" ) );\n     while( it ) {\n         std::cout << it->length() << \"\";\n         ++it;\n     }\n\n Use functions such as utils::from_file() and utils::from_string() to conveniently\n get an input source that can be used here.\n\n See FastaReader for a description of the expected format. In order to change the reading\n behaviour, a FastaReader object can be handed over from which the settings are copied.\n\n The copy constructur copies a pointer to the underlying source.\n Thus, when advancing the copy to the next Sequence, the original \"skips\" this Sequence,\n as the source then is at the next one.\n\n Thread safety: No thread safety. The common use case for this iterator is to loop over a file.\n Thus, guarding induces unnecessary overhead. If multiple threads read from this iterator, both\n dereferencing and incrementing need to be guarded.");
		cl.def( pybind11::init( [](){ return new genesis::sequence::FastaInputIterator(); } ) );
		cl.def( pybind11::init<class std::shared_ptr<class genesis::utils::BaseInputSource>>(), pybind11::arg("source") );

		cl.def( pybind11::init<class std::shared_ptr<class genesis::utils::BaseInputSource>, const class genesis::sequence::FastaReader &>(), pybind11::arg("source"), pybind11::arg("settings") );

		cl.def( pybind11::init( [](genesis::sequence::FastaInputIterator const &o){ return new genesis::sequence::FastaInputIterator(o); } ) );
		cl.def("assign", (class genesis::sequence::FastaInputIterator & (genesis::sequence::FastaInputIterator::*)(const class genesis::sequence::FastaInputIterator &)) &genesis::sequence::FastaInputIterator::operator=, "C++: genesis::sequence::FastaInputIterator::operator=(const class genesis::sequence::FastaInputIterator &) --> class genesis::sequence::FastaInputIterator &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("__eq__", (bool (genesis::sequence::FastaInputIterator::*)(const class genesis::sequence::FastaInputIterator &) const) &genesis::sequence::FastaInputIterator::operator==, "C++: genesis::sequence::FastaInputIterator::operator==(const class genesis::sequence::FastaInputIterator &) const --> bool", pybind11::arg("other"));
		cl.def("__ne__", (bool (genesis::sequence::FastaInputIterator::*)(const class genesis::sequence::FastaInputIterator &) const) &genesis::sequence::FastaInputIterator::operator!=, "C++: genesis::sequence::FastaInputIterator::operator!=(const class genesis::sequence::FastaInputIterator &) const --> bool", pybind11::arg("other"));
		cl.def("dereference", (const class genesis::sequence::Sequence & (genesis::sequence::FastaInputIterator::*)() const) &genesis::sequence::FastaInputIterator::operator*, "C++: genesis::sequence::FastaInputIterator::operator*() const --> const class genesis::sequence::Sequence &", pybind11::return_value_policy::reference_internal);
		cl.def("arrow", (const class genesis::sequence::Sequence * (genesis::sequence::FastaInputIterator::*)() const) &genesis::sequence::FastaInputIterator::operator->, "C++: genesis::sequence::FastaInputIterator::operator->() const --> const class genesis::sequence::Sequence *", pybind11::return_value_policy::reference_internal);
		cl.def("dereference", (const class genesis::sequence::Sequence & (genesis::sequence::FastaInputIterator::*)() const) &genesis::sequence::FastaInputIterator::dereference, "C++: genesis::sequence::FastaInputIterator::dereference() const --> const class genesis::sequence::Sequence &", pybind11::return_value_policy::reference_internal);
		cl.def("begin", (class genesis::sequence::FastaInputIterator & (genesis::sequence::FastaInputIterator::*)()) &genesis::sequence::FastaInputIterator::begin, "Beginning of the iterator.\n\n This is a bit uncommon, as the iterator provides its own begin() and end() functions.\n We do this to allow the easy use case that is explained above.\n\nC++: genesis::sequence::FastaInputIterator::begin() --> class genesis::sequence::FastaInputIterator &", pybind11::return_value_policy::reference_internal);
		cl.def("end", (class genesis::sequence::FastaInputIterator (genesis::sequence::FastaInputIterator::*)()) &genesis::sequence::FastaInputIterator::end, "End of the iterator.\n\n  begin()\n\nC++: genesis::sequence::FastaInputIterator::end() --> class genesis::sequence::FastaInputIterator");
		cl.def("pre_increment", (class genesis::sequence::FastaInputIterator & (genesis::sequence::FastaInputIterator::*)()) &genesis::sequence::FastaInputIterator::operator++, "C++: genesis::sequence::FastaInputIterator::operator++() --> class genesis::sequence::FastaInputIterator &", pybind11::return_value_policy::reference_internal);
		cl.def("post_increment", (class genesis::sequence::FastaInputIterator (genesis::sequence::FastaInputIterator::*)(int)) &genesis::sequence::FastaInputIterator::operator++, "C++: genesis::sequence::FastaInputIterator::operator++(int) --> class genesis::sequence::FastaInputIterator", pybind11::arg(""));
		cl.def("increment", (void (genesis::sequence::FastaInputIterator::*)()) &genesis::sequence::FastaInputIterator::increment, "C++: genesis::sequence::FastaInputIterator::increment() --> void");

		extend_fasta_input_iterator(cl);
	}
	{ // genesis::sequence::FastaOutputIterator file:genesis/sequence/formats/fasta_output_iterator.hpp line:66
		pybind11::class_<genesis::sequence::FastaOutputIterator, std::shared_ptr<genesis::sequence::FastaOutputIterator>> cl(M("genesis::sequence"), "FastaOutputIterator", "Write Fasta data, sequentially.\n\n This class allows to write Sequence data to an output target, using Fasta format, without\n the need to have a full SequenceSet containing all Sequence%s in memory.\n\n Exemplary usage:\n\n     auto out_it = FastaOutputIterator( utils::to_file( \"path/to/out.fasta\" ));\n     while( ... ) {\n         Sequence seq = ...\n         out_it << seq;\n     }\n\n See the output target convenience functions utils::to_file(), utils::to_stream(), and\n utils::to_string() for examples of how to obtain a suitable output target.");
		cl.def( pybind11::init<class std::shared_ptr<class genesis::utils::BaseOutputTarget>>(), pybind11::arg("target") );

		cl.def( pybind11::init<class std::shared_ptr<class genesis::utils::BaseOutputTarget>, const class genesis::sequence::FastaWriter &>(), pybind11::arg("target"), pybind11::arg("writer") );

		cl.def( pybind11::init( [](genesis::sequence::FastaOutputIterator const &o){ return new genesis::sequence::FastaOutputIterator(o); } ) );
		cl.def("assign", (class genesis::sequence::FastaOutputIterator & (genesis::sequence::FastaOutputIterator::*)(const class genesis::sequence::FastaOutputIterator &)) &genesis::sequence::FastaOutputIterator::operator=, "C++: genesis::sequence::FastaOutputIterator::operator=(const class genesis::sequence::FastaOutputIterator &) --> class genesis::sequence::FastaOutputIterator &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("__lshift__", (class genesis::sequence::FastaOutputIterator & (genesis::sequence::FastaOutputIterator::*)(const class genesis::sequence::Sequence &)) &genesis::sequence::FastaOutputIterator::operator<<, "C++: genesis::sequence::FastaOutputIterator::operator<<(const class genesis::sequence::Sequence &) --> class genesis::sequence::FastaOutputIterator &", pybind11::return_value_policy::reference_internal, pybind11::arg("seq"));
		cl.def("writer", (class genesis::sequence::FastaWriter & (genesis::sequence::FastaOutputIterator::*)()) &genesis::sequence::FastaOutputIterator::writer, "Return the FastaWriter used for this iterator.\n\n Use this to change the settings and writing behaviour of the iterator.\n See FastaWriter for details.\n\nC++: genesis::sequence::FastaOutputIterator::writer() --> class genesis::sequence::FastaWriter &", pybind11::return_value_policy::reference_internal);

		extend_fasta_output_iterator(cl);
	}
}

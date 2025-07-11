#include <functional>
#include <genesis/sequence/formats/fastq_input_iterator.hpp>
#include <genesis/sequence/formats/fastq_output_iterator.hpp>
#include <genesis/sequence/formats/fastq_reader.hpp>
#include <genesis/sequence/formats/fastq_writer.hpp>
#include <genesis/sequence/functions/quality.hpp>
#include <genesis/sequence/functions/stats.hpp>
#include <genesis/sequence/sequence.hpp>
#include <genesis/sequence/sequence_set.hpp>
#include <genesis/utils/io/base_input_source.hpp>
#include <genesis/utils/io/base_output_target.hpp>
#include <genesis/utils/io/input_stream.hpp>
#include <genesis/utils/tools/char_lookup.hpp>
#include <ios>
#include <iterator>
#include <map>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
#include <string>
#include <utility>
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

void bind_genesis_sequence_formats_fastq_writer(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::sequence::FastqWriter file:genesis/sequence/formats/fastq_writer.hpp line:73
		pybind11::class_<genesis::sequence::FastqWriter, std::shared_ptr<genesis::sequence::FastqWriter>> cl(M("genesis::sequence"), "FastqWriter", "Write Fastq data.\n\n This class provides simple facilities for writing Fastq data.\n\n Exemplary usage:\n\n     std::string outfile = \"path/to/file.fastq\";\n     SequenceSet sequence_set;\n\n     FastqWriter().write( sequence_set, utils::to_file( outfile ));\n\n See FastqReader for a description of the Fastq format.\n Sequences are written to Fastq format, using the Sanger encoding (phred score plus offset 33),\n see also quality_encode_from_phred_score().\n\n We expect Sequence%s to have a phred quality score by default, and throw an exception otherwise.\n To change this behaviour, and instead fill Sequence%s without quality scores with a constant\n quality score value, use fill_missing_quality( unsigned char ).");
		cl.def( pybind11::init( [](){ return new genesis::sequence::FastqWriter(); } ) );
		cl.def( pybind11::init( [](genesis::sequence::FastqWriter const &o){ return new genesis::sequence::FastqWriter(o); } ) );
		cl.def("assign", (class genesis::sequence::FastqWriter & (genesis::sequence::FastqWriter::*)(const class genesis::sequence::FastqWriter &)) &genesis::sequence::FastqWriter::operator=, "C++: genesis::sequence::FastqWriter::operator=(const class genesis::sequence::FastqWriter &) --> class genesis::sequence::FastqWriter &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("write", (void (genesis::sequence::FastqWriter::*)(const class genesis::sequence::Sequence &, class std::shared_ptr<class genesis::utils::BaseOutputTarget>) const) &genesis::sequence::FastqWriter::write, "Write a single Sequence to an output target, using the Fastq format.\n\n See the output target convenience functions utils::to_file(), utils::to_stream(), and\n utils::to_string() for examples of how to obtain a suitable output target.\n\nC++: genesis::sequence::FastqWriter::write(const class genesis::sequence::Sequence &, class std::shared_ptr<class genesis::utils::BaseOutputTarget>) const --> void", pybind11::arg("sequence"), pybind11::arg("target"));
		cl.def("write", (void (genesis::sequence::FastqWriter::*)(const class genesis::sequence::SequenceSet &, class std::shared_ptr<class genesis::utils::BaseOutputTarget>) const) &genesis::sequence::FastqWriter::write, "Write a SequenceSet to an output target, using the Fastq format.\n\n See the output target convenience functions utils::to_file(), utils::to_stream(), and\n utils::to_string() for examples of how to obtain a suitable output target.\n\nC++: genesis::sequence::FastqWriter::write(const class genesis::sequence::SequenceSet &, class std::shared_ptr<class genesis::utils::BaseOutputTarget>) const --> void", pybind11::arg("sequence_set"), pybind11::arg("target"));
		cl.def("write_sequence", (void (genesis::sequence::FastqWriter::*)(const class genesis::sequence::Sequence &, std::ostream &) const) &genesis::sequence::FastqWriter::write_sequence, "Write a single Sequence to an output stream in Fastq format.\n\nC++: genesis::sequence::FastqWriter::write_sequence(const class genesis::sequence::Sequence &, std::ostream &) const --> void", pybind11::arg("sequence"), pybind11::arg("os"));
		cl.def("line_length", (class genesis::sequence::FastqWriter & (genesis::sequence::FastqWriter::*)(unsigned long)) &genesis::sequence::FastqWriter::line_length, "Set the line length, which determines after how many chars (Sequence sites) lines breaks\n are inserted when writing the Fastq file.\n\n Default is `0`, which means, no line breaks are inserted. This is because many other parsing\n tools are implemented lazily and expect sequences in Fastq format to be on one line,\n so we use this by default. If set to any value greater than zero, line breaks are inserted\n into both the sequence and the quality score lines so that the do not exceed the specified\n length.\n\n The functions returns the FastqWriter object to allow fluent interfaces.\n\nC++: genesis::sequence::FastqWriter::line_length(unsigned long) --> class genesis::sequence::FastqWriter &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("line_length", (unsigned long (genesis::sequence::FastqWriter::*)() const) &genesis::sequence::FastqWriter::line_length, "Get the current line length.\n\n See the setter line_length( size_t ) for details.\n\nC++: genesis::sequence::FastqWriter::line_length() const --> unsigned long");
		cl.def("fill_missing_quality", (class genesis::sequence::FastqWriter & (genesis::sequence::FastqWriter::*)(unsigned char)) &genesis::sequence::FastqWriter::fill_missing_quality, "Set the value to fill the quality scores of sequences that do not have a phread\n score assigned to them.\n\n By default, we do not accept sequences without phred scores assigned to them. If such a\n sequence occurs while writing, an exception is thrown. To change that behaviour, set the\n value to fill all sites of sequences that are missing their phread scores with this function.\n All values in the range `[0, 254]` are valid, but the encoding used in Fastq only ranges\n from `0` to `93` - all values above that are clamped to be `93`.\n See quality_encode_from_phred_score() for details.\n\n Lastly, in order to restore the original behaviour (that is, throw an exception if a sequence\n is missing phred scores), set this function to the magic value `255`.\n\nC++: genesis::sequence::FastqWriter::fill_missing_quality(unsigned char) --> class genesis::sequence::FastqWriter &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("fill_missing_quality", (unsigned char (genesis::sequence::FastqWriter::*)() const) &genesis::sequence::FastqWriter::fill_missing_quality, "Get the current value to fill missing phred quality scores.\n\nC++: genesis::sequence::FastqWriter::fill_missing_quality() const --> unsigned char");
		cl.def("repeat_label", (class genesis::sequence::FastqWriter & (genesis::sequence::FastqWriter::*)(bool)) &genesis::sequence::FastqWriter::repeat_label, "Set whether to repeat the sequence identifier (label) on the third line of each\n sequence.\n\n Fastq allows the third line (starting with a `+` char) to either only contain that char,\n or to repeat the label of the first line (for whatever reason...). By default, we do not\n write the label again, to save a bit of storage space. Use this function to change that\n behaviour.\n\nC++: genesis::sequence::FastqWriter::repeat_label(bool) --> class genesis::sequence::FastqWriter &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("repeat_label", (bool (genesis::sequence::FastqWriter::*)() const) &genesis::sequence::FastqWriter::repeat_label, "Get whether the setting to repeat the sequence identifier (label) on the third line\n is set.\n\nC++: genesis::sequence::FastqWriter::repeat_label() const --> bool");
	}
	{ // genesis::sequence::FastqInputIterator file:genesis/sequence/formats/fastq_input_iterator.hpp line:87
		pybind11::class_<genesis::sequence::FastqInputIterator, std::shared_ptr<genesis::sequence::FastqInputIterator>> cl(M("genesis::sequence"), "FastqInputIterator", "Iterate an input source and parse it as Fastq sequences.\n\n This class allows to iterate over an input source, interpreting it as Fastq sequences, and\n yielding one such sequence per iteration step. This is useful for processing large files without\n having to keep them fully in memory.\n\n Example:\n\n     for( auto const& s : FastqInputIterator( from_file( \"/path/to/large_file.fastq\" ))) {\n         std::cout << s.length() << \"\";\n     }\n\n Alternatively, the following also works:\n\n     auto it = FastqInputIterator( from_file( \"/path/to/large_file.fastq\" ));\n     while( it ) {\n         std::cout << it->length() << \"\";\n         ++it;\n     }\n\n Use functions such as utils::from_file() and utils::from_string() to conveniently\n get an input source that can be used here.\n\n See FastqReader for a description of the expected format. In order to change the reading\n behaviour, a FastqReader object can be handed over from which the settings are copied.\n\n The copy constructur copies a pointer to the underlying source.\n Thus, when advancing the copy to the next Sequence, the original \"skips\" this Sequence,\n as the source then is at the next one.\n\n Thread safety: No thread safety. The common use case for this iterator is to loop over a file.\n Thus, guarding induces unnecessary overhead. If multiple threads read from this iterator, both\n dereferencing and incrementing need to be guarded.");
		cl.def( pybind11::init( [](){ return new genesis::sequence::FastqInputIterator(); } ) );
		cl.def( pybind11::init<class std::shared_ptr<class genesis::utils::BaseInputSource>>(), pybind11::arg("source") );

		cl.def( pybind11::init<class std::shared_ptr<class genesis::utils::BaseInputSource>, const class genesis::sequence::FastqReader &>(), pybind11::arg("source"), pybind11::arg("settings") );

		cl.def( pybind11::init( [](genesis::sequence::FastqInputIterator const &o){ return new genesis::sequence::FastqInputIterator(o); } ) );
		cl.def("assign", (class genesis::sequence::FastqInputIterator & (genesis::sequence::FastqInputIterator::*)(const class genesis::sequence::FastqInputIterator &)) &genesis::sequence::FastqInputIterator::operator=, "C++: genesis::sequence::FastqInputIterator::operator=(const class genesis::sequence::FastqInputIterator &) --> class genesis::sequence::FastqInputIterator &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("__eq__", (bool (genesis::sequence::FastqInputIterator::*)(const class genesis::sequence::FastqInputIterator &) const) &genesis::sequence::FastqInputIterator::operator==, "C++: genesis::sequence::FastqInputIterator::operator==(const class genesis::sequence::FastqInputIterator &) const --> bool", pybind11::arg("other"));
		cl.def("__ne__", (bool (genesis::sequence::FastqInputIterator::*)(const class genesis::sequence::FastqInputIterator &) const) &genesis::sequence::FastqInputIterator::operator!=, "C++: genesis::sequence::FastqInputIterator::operator!=(const class genesis::sequence::FastqInputIterator &) const --> bool", pybind11::arg("other"));
		cl.def("dereference", (const class genesis::sequence::Sequence & (genesis::sequence::FastqInputIterator::*)() const) &genesis::sequence::FastqInputIterator::operator*, "C++: genesis::sequence::FastqInputIterator::operator*() const --> const class genesis::sequence::Sequence &", pybind11::return_value_policy::reference_internal);
		cl.def("arrow", (const class genesis::sequence::Sequence * (genesis::sequence::FastqInputIterator::*)() const) &genesis::sequence::FastqInputIterator::operator->, "C++: genesis::sequence::FastqInputIterator::operator->() const --> const class genesis::sequence::Sequence *", pybind11::return_value_policy::reference_internal);
		cl.def("dereference", (const class genesis::sequence::Sequence & (genesis::sequence::FastqInputIterator::*)() const) &genesis::sequence::FastqInputIterator::dereference, "C++: genesis::sequence::FastqInputIterator::dereference() const --> const class genesis::sequence::Sequence &", pybind11::return_value_policy::reference_internal);
		cl.def("begin", (class genesis::sequence::FastqInputIterator & (genesis::sequence::FastqInputIterator::*)()) &genesis::sequence::FastqInputIterator::begin, "Beginning of the iterator.\n\n This is a bit uncommon, as the iterator provides its own begin() and end() functions.\n We do this to allow the easy use case that is explained above.\n\nC++: genesis::sequence::FastqInputIterator::begin() --> class genesis::sequence::FastqInputIterator &", pybind11::return_value_policy::reference_internal);
		cl.def("end", (class genesis::sequence::FastqInputIterator (genesis::sequence::FastqInputIterator::*)()) &genesis::sequence::FastqInputIterator::end, "End of the iterator.\n\n  begin()\n\nC++: genesis::sequence::FastqInputIterator::end() --> class genesis::sequence::FastqInputIterator");
		cl.def("pre_increment", (class genesis::sequence::FastqInputIterator & (genesis::sequence::FastqInputIterator::*)()) &genesis::sequence::FastqInputIterator::operator++, "C++: genesis::sequence::FastqInputIterator::operator++() --> class genesis::sequence::FastqInputIterator &", pybind11::return_value_policy::reference_internal);
		cl.def("post_increment", (class genesis::sequence::FastqInputIterator (genesis::sequence::FastqInputIterator::*)(int)) &genesis::sequence::FastqInputIterator::operator++, "C++: genesis::sequence::FastqInputIterator::operator++(int) --> class genesis::sequence::FastqInputIterator", pybind11::arg(""));
		cl.def("increment", (void (genesis::sequence::FastqInputIterator::*)()) &genesis::sequence::FastqInputIterator::increment, "C++: genesis::sequence::FastqInputIterator::increment() --> void");

		extend_fastq_input_iterator(cl);
	}
	{ // genesis::sequence::FastqOutputIterator file:genesis/sequence/formats/fastq_output_iterator.hpp line:66
		pybind11::class_<genesis::sequence::FastqOutputIterator, std::shared_ptr<genesis::sequence::FastqOutputIterator>> cl(M("genesis::sequence"), "FastqOutputIterator", "Write Fastq data, sequentially.\n\n This class allows to write Sequence data to an output target, using Fastq format, without\n the need to have a full SequenceSet containing all Sequence%s in memory.\n\n Exemplary usage:\n\n     auto out_it = FastqOutputIterator( utils::to_file( \"path/to/out.fastq\" ));\n     while( ... ) {\n         Sequence seq = ...\n         out_it << seq;\n     }\n\n See the output target convenience functions utils::to_file(), utils::to_stream(), and\n utils::to_string() for examples of how to obtain a suitable output target.");
		cl.def( pybind11::init<class std::shared_ptr<class genesis::utils::BaseOutputTarget>>(), pybind11::arg("target") );

		cl.def( pybind11::init<class std::shared_ptr<class genesis::utils::BaseOutputTarget>, const class genesis::sequence::FastqWriter &>(), pybind11::arg("target"), pybind11::arg("writer") );

		cl.def( pybind11::init( [](genesis::sequence::FastqOutputIterator const &o){ return new genesis::sequence::FastqOutputIterator(o); } ) );
		cl.def("assign", (class genesis::sequence::FastqOutputIterator & (genesis::sequence::FastqOutputIterator::*)(const class genesis::sequence::FastqOutputIterator &)) &genesis::sequence::FastqOutputIterator::operator=, "C++: genesis::sequence::FastqOutputIterator::operator=(const class genesis::sequence::FastqOutputIterator &) --> class genesis::sequence::FastqOutputIterator &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("__lshift__", (class genesis::sequence::FastqOutputIterator & (genesis::sequence::FastqOutputIterator::*)(const class genesis::sequence::Sequence &)) &genesis::sequence::FastqOutputIterator::operator<<, "C++: genesis::sequence::FastqOutputIterator::operator<<(const class genesis::sequence::Sequence &) --> class genesis::sequence::FastqOutputIterator &", pybind11::return_value_policy::reference_internal, pybind11::arg("seq"));
		cl.def("writer", (class genesis::sequence::FastqWriter & (genesis::sequence::FastqOutputIterator::*)()) &genesis::sequence::FastqOutputIterator::writer, "Return the FastqWriter used for this iterator.\n\n Use this to change the settings and writing behaviour of the iterator.\n See FastqWriter for details.\n\nC++: genesis::sequence::FastqOutputIterator::writer() --> class genesis::sequence::FastqWriter &", pybind11::return_value_policy::reference_internal);
	}
	// genesis::sequence::site_histogram(const class genesis::sequence::Sequence &) file:genesis/sequence/functions/stats.hpp line:59
	M("genesis::sequence").def("site_histogram", (class std::map<char, unsigned long, struct std::less<char>, class std::allocator<struct std::pair<const char, unsigned long> > > (*)(const class genesis::sequence::Sequence &)) &genesis::sequence::site_histogram, "Get a histogram of the occurrences of particular sites, given a Sequence.\n\n This gives the raw counts of how often each site (character) appears in the Sequence.\n See base_frequencies() for the relative version of this function.\n\nC++: genesis::sequence::site_histogram(const class genesis::sequence::Sequence &) --> class std::map<char, unsigned long, struct std::less<char>, class std::allocator<struct std::pair<const char, unsigned long> > >", pybind11::arg("seq"));

}

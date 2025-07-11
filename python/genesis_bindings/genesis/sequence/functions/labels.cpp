#include <functional>
#include <genesis/sequence/formats/phylip_writer.hpp>
#include <genesis/sequence/functions/labels.hpp>
#include <genesis/sequence/printers/bitmap.hpp>
#include <genesis/sequence/printers/simple.hpp>
#include <genesis/sequence/sequence.hpp>
#include <genesis/sequence/sequence_set.hpp>
#include <genesis/utils/color/color.hpp>
#include <genesis/utils/io/base_output_target.hpp>
#include <ios>
#include <iterator>
#include <map>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
#include <string>
#include <unordered_set>
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

void bind_genesis_sequence_functions_labels(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::sequence::sanitize_label(class genesis::sequence::Sequence &) file:genesis/sequence/functions/labels.hpp line:218
	M("genesis::sequence").def("sanitize_label", (void (*)(class genesis::sequence::Sequence &)) &genesis::sequence::sanitize_label, "Sanitize a label by replacing all invalid characters with underscores.\n\n This might be important for printing the Sequences to a file that needs to be read by other\n applications. See is_valid_label() for details on what is considered a valid label.\n\nC++: genesis::sequence::sanitize_label(class genesis::sequence::Sequence &) --> void", pybind11::arg("seq"));

	// genesis::sequence::sanitize_labels(class genesis::sequence::SequenceSet &) file:genesis/sequence/functions/labels.hpp line:227
	M("genesis::sequence").def("sanitize_labels", (void (*)(class genesis::sequence::SequenceSet &)) &genesis::sequence::sanitize_labels, "Sanitize the labels of all Sequence%s in the SequenceSet by replacing all invalid\n characters with underscores.\n\n This might be important for printing the Sequences to a file that needs to be read by other\n applications. See is_valid_label() for details on what is considered a valid label.\n\nC++: genesis::sequence::sanitize_labels(class genesis::sequence::SequenceSet &) --> void", pybind11::arg("set"));

	// genesis::sequence::filter_by_label_list(class genesis::sequence::SequenceSet &, const class std::unordered_set<std::string, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<std::string > > &, bool) file:genesis/sequence/functions/labels.hpp line:239
	M("genesis::sequence").def("filter_by_label_list", [](class genesis::sequence::SequenceSet & a0, const class std::unordered_set<std::string, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<std::string > > & a1) -> void { return genesis::sequence::filter_by_label_list(a0, a1); }, "", pybind11::arg("set"), pybind11::arg("labels"));
	M("genesis::sequence").def("filter_by_label_list", (void (*)(class genesis::sequence::SequenceSet &, const class std::unordered_set<std::string, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<std::string > > &, bool)) &genesis::sequence::filter_by_label_list, "Remove all those Sequence%s from a SequenceSet whose labels are in the given list.\n\n If `invert` is set to true, it does the same inverted: it removes all Sequence%s except those\n whose label is in the list.\n\nC++: genesis::sequence::filter_by_label_list(class genesis::sequence::SequenceSet &, const class std::unordered_set<std::string, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<std::string > > &, bool) --> void", pybind11::arg("set"), pybind11::arg("labels"), pybind11::arg("invert"));

	{ // genesis::sequence::PhylipWriter file:genesis/sequence/formats/phylip_writer.hpp line:52
		pybind11::class_<genesis::sequence::PhylipWriter, std::shared_ptr<genesis::sequence::PhylipWriter>> cl(M("genesis::sequence"), "PhylipWriter", "");
		cl.def( pybind11::init( [](){ return new genesis::sequence::PhylipWriter(); } ) );
		cl.def( pybind11::init( [](genesis::sequence::PhylipWriter const &o){ return new genesis::sequence::PhylipWriter(o); } ) );
		cl.def("assign", (class genesis::sequence::PhylipWriter & (genesis::sequence::PhylipWriter::*)(const class genesis::sequence::PhylipWriter &)) &genesis::sequence::PhylipWriter::operator=, "C++: genesis::sequence::PhylipWriter::operator=(const class genesis::sequence::PhylipWriter &) --> class genesis::sequence::PhylipWriter &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("write", (void (genesis::sequence::PhylipWriter::*)(const class genesis::sequence::SequenceSet &, class std::shared_ptr<class genesis::utils::BaseOutputTarget>) const) &genesis::sequence::PhylipWriter::write, "Write a SequenceSet to an output target, using the Phylip format.\n\n See the output target convenience functions utils::to_file(), utils::to_stream(), and\n utils::to_string() for examples of how to obtain a suitable output target.\n\nC++: genesis::sequence::PhylipWriter::write(const class genesis::sequence::SequenceSet &, class std::shared_ptr<class genesis::utils::BaseOutputTarget>) const --> void", pybind11::arg("sequence_set"), pybind11::arg("target"));
		cl.def("label_length", (class genesis::sequence::PhylipWriter & (genesis::sequence::PhylipWriter::*)(unsigned long)) &genesis::sequence::PhylipWriter::label_length, "Set the length of the label in front of the sequences.\n\n Phylip has the weird property that labels are written in front of sequences and do not need\n to have a delimiter, but instead are simply the first `n` characters of the string. This value\n determines after how many chars the label ends and the actual sequence begins.\n\n If set to 0 (default), a relaxed version of Phylip is used, where the sequence begin is\n automatically detected. Labels can then be of arbitrary lengths, as long as they do not contain\n white spaces. After the label, a space is appended.\n\n If set to a value greater than 0, the label will be cut off after that many chars. For shorter\n labels, the remaining number is filled with spaces.\n The functions returns the PhylipWriter object to allow fluent interfaces.\n\nC++: genesis::sequence::PhylipWriter::label_length(unsigned long) --> class genesis::sequence::PhylipWriter &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("label_length", (unsigned long (genesis::sequence::PhylipWriter::*)() const) &genesis::sequence::PhylipWriter::label_length, "Return the currently set label length.\n\n See the setter label_length( size_t ) for details.\n\nC++: genesis::sequence::PhylipWriter::label_length() const --> unsigned long");
		cl.def("line_length", (class genesis::sequence::PhylipWriter & (genesis::sequence::PhylipWriter::*)(unsigned long)) &genesis::sequence::PhylipWriter::line_length, "Set the line length, which determines after how many chars (Sequence sites) lines breaks\n are inserted when writing the Phylip file.\n\n Default is `80`. If set to `0`, no breaks are inserted.\n The functions returns the PhylipWriter object to allow fluent interfaces.\n\nC++: genesis::sequence::PhylipWriter::line_length(unsigned long) --> class genesis::sequence::PhylipWriter &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("line_length", (unsigned long (genesis::sequence::PhylipWriter::*)() const) &genesis::sequence::PhylipWriter::line_length, "Get the current line length.\n\n See the setter line_length( size_t ) for details.\n\nC++: genesis::sequence::PhylipWriter::line_length() const --> unsigned long");
	}
	{ // genesis::sequence::PrinterBitmap file:genesis/sequence/printers/bitmap.hpp line:75
		pybind11::class_<genesis::sequence::PrinterBitmap, std::shared_ptr<genesis::sequence::PrinterBitmap>> cl(M("genesis::sequence"), "PrinterBitmap", "Print the sites of a SequenceSet as pixels in a bitmap.\n\n By default, no colors are set, thus, the image will be completely black. You have to set the\n \n\n\n");
		cl.def( pybind11::init( [](){ return new genesis::sequence::PrinterBitmap(); } ) );
		cl.def( pybind11::init( [](genesis::sequence::PrinterBitmap const &o){ return new genesis::sequence::PrinterBitmap(o); } ) );
		cl.def("assign", (class genesis::sequence::PrinterBitmap & (genesis::sequence::PrinterBitmap::*)(const class genesis::sequence::PrinterBitmap &)) &genesis::sequence::PrinterBitmap::operator=, "C++: genesis::sequence::PrinterBitmap::operator=(const class genesis::sequence::PrinterBitmap &) --> class genesis::sequence::PrinterBitmap &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("write", (void (genesis::sequence::PrinterBitmap::*)(const class genesis::sequence::SequenceSet &, class std::shared_ptr<class genesis::utils::BaseOutputTarget>) const) &genesis::sequence::PrinterBitmap::write, "C++: genesis::sequence::PrinterBitmap::write(const class genesis::sequence::SequenceSet &, class std::shared_ptr<class genesis::utils::BaseOutputTarget>) const --> void", pybind11::arg("set"), pybind11::arg("target"));
		cl.def("color_map", (class genesis::sequence::PrinterBitmap & (genesis::sequence::PrinterBitmap::*)(const class std::map<char, class genesis::utils::Color, struct std::less<char>, class std::allocator<struct std::pair<const char, class genesis::utils::Color> > > &)) &genesis::sequence::PrinterBitmap::color_map, "Set the list of Color%s to use for the Sequence charaters.\n\n The provided map sets the colors for each Sequence character.\n The presettings `nucleic_acid_colors()` and `amino_acid_colors()` for default sequence\n types can be used as input for this parameter.\n If the `colors` map does not contain a key for one of the chars in the sequence, the char is\n printed in black.\n\nC++: genesis::sequence::PrinterBitmap::color_map(const class std::map<char, class genesis::utils::Color, struct std::less<char>, class std::allocator<struct std::pair<const char, class genesis::utils::Color> > > &) --> class genesis::sequence::PrinterBitmap &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("color_map", (const class std::map<char, class genesis::utils::Color, struct std::less<char>, class std::allocator<struct std::pair<const char, class genesis::utils::Color> > > & (genesis::sequence::PrinterBitmap::*)() const) &genesis::sequence::PrinterBitmap::color_map, "Get the currently set list of Color%s for each Sequence character.\n\n See \n\n\nC++: genesis::sequence::PrinterBitmap::color_map() const --> const class std::map<char, class genesis::utils::Color, struct std::less<char>, class std::allocator<struct std::pair<const char, class genesis::utils::Color> > > &", pybind11::return_value_policy::reference_internal);
		cl.def("pixel_height_per_char", (class genesis::sequence::PrinterBitmap & (genesis::sequence::PrinterBitmap::*)(unsigned long)) &genesis::sequence::PrinterBitmap::pixel_height_per_char, "C++: genesis::sequence::PrinterBitmap::pixel_height_per_char(unsigned long) --> class genesis::sequence::PrinterBitmap &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("pixel_height_per_char", (unsigned long (genesis::sequence::PrinterBitmap::*)() const) &genesis::sequence::PrinterBitmap::pixel_height_per_char, "C++: genesis::sequence::PrinterBitmap::pixel_height_per_char() const --> unsigned long");
		cl.def("pixel_width_per_char", (class genesis::sequence::PrinterBitmap & (genesis::sequence::PrinterBitmap::*)(unsigned long)) &genesis::sequence::PrinterBitmap::pixel_width_per_char, "C++: genesis::sequence::PrinterBitmap::pixel_width_per_char(unsigned long) --> class genesis::sequence::PrinterBitmap &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("pixel_width_per_char", (unsigned long (genesis::sequence::PrinterBitmap::*)() const) &genesis::sequence::PrinterBitmap::pixel_width_per_char, "C++: genesis::sequence::PrinterBitmap::pixel_width_per_char() const --> unsigned long");
	}
	{ // genesis::sequence::PrinterSimple file:genesis/sequence/printers/simple.hpp line:61
		pybind11::class_<genesis::sequence::PrinterSimple, std::shared_ptr<genesis::sequence::PrinterSimple>> cl(M("genesis::sequence"), "PrinterSimple", "Simple printer class for Sequence%s and SequenceSet%s.\n\n It provides to print() Sequence%s to strings and ostreams.\n\n By default, no colors are set, thus, Sequence%s are printed without color. You have to set the\n \n\n\n");
		cl.def( pybind11::init( [](){ return new genesis::sequence::PrinterSimple(); } ) );
		cl.def( pybind11::init( [](genesis::sequence::PrinterSimple const &o){ return new genesis::sequence::PrinterSimple(o); } ) );

		pybind11::enum_<genesis::sequence::PrinterSimple::ColorMode>(cl, "ColorMode", "Modes for how the Sequence sites are colored.\n\n In order for the coloring to work, a map from site characters to the name of the color\n needs to be provided via color_map(). See there for details.\n\n The coloring is applied using escape sequences. This works on most modern terminals.\n See utils::Style for details and for a list of the available color names.")
			.value("kNone", genesis::sequence::PrinterSimple::ColorMode::kNone)
			.value("kForeground", genesis::sequence::PrinterSimple::ColorMode::kForeground)
			.value("kBackground", genesis::sequence::PrinterSimple::ColorMode::kBackground);


		pybind11::enum_<genesis::sequence::PrinterSimple::LabelMode>(cl, "LabelMode", "Modes for how to print Sequence labels.")
			.value("kNone", genesis::sequence::PrinterSimple::LabelMode::kNone)
			.value("kSameLine", genesis::sequence::PrinterSimple::LabelMode::kSameLine)
			.value("kSeparateLine", genesis::sequence::PrinterSimple::LabelMode::kSeparateLine);

		cl.def("assign", (class genesis::sequence::PrinterSimple & (genesis::sequence::PrinterSimple::*)(const class genesis::sequence::PrinterSimple &)) &genesis::sequence::PrinterSimple::operator=, "C++: genesis::sequence::PrinterSimple::operator=(const class genesis::sequence::PrinterSimple &) --> class genesis::sequence::PrinterSimple &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("print", (void (genesis::sequence::PrinterSimple::*)(std::ostream &, const class genesis::sequence::Sequence &) const) &genesis::sequence::PrinterSimple::print, "Print a single Sequence to a stream.\n\nC++: genesis::sequence::PrinterSimple::print(std::ostream &, const class genesis::sequence::Sequence &) const --> void", pybind11::arg("out"), pybind11::arg("seq"));
		cl.def("print", (void (genesis::sequence::PrinterSimple::*)(std::ostream &, const class genesis::sequence::SequenceSet &) const) &genesis::sequence::PrinterSimple::print, "Print a SequenceSet to a stream.\n\nC++: genesis::sequence::PrinterSimple::print(std::ostream &, const class genesis::sequence::SequenceSet &) const --> void", pybind11::arg("out"), pybind11::arg("set"));
		cl.def("print", (std::string (genesis::sequence::PrinterSimple::*)(const class genesis::sequence::Sequence &) const) &genesis::sequence::PrinterSimple::print, "Return a string representing the print of a single Sequence.\n\nC++: genesis::sequence::PrinterSimple::print(const class genesis::sequence::Sequence &) const --> std::string", pybind11::arg("seq"));
		cl.def("print", (std::string (genesis::sequence::PrinterSimple::*)(const class genesis::sequence::SequenceSet &) const) &genesis::sequence::PrinterSimple::print, "Return a string representing the print of a SequenceSet.\n\nC++: genesis::sequence::PrinterSimple::print(const class genesis::sequence::SequenceSet &) const --> std::string", pybind11::arg("set"));
		cl.def("__call__", (std::string (genesis::sequence::PrinterSimple::*)(const class genesis::sequence::Sequence &) const) &genesis::sequence::PrinterSimple::operator(), "Return a string representing the print of a single Sequence.\n\n This is simply an alias for print();\n\nC++: genesis::sequence::PrinterSimple::operator()(const class genesis::sequence::Sequence &) const --> std::string", pybind11::arg("seq"));
		cl.def("__call__", (std::string (genesis::sequence::PrinterSimple::*)(const class genesis::sequence::SequenceSet &) const) &genesis::sequence::PrinterSimple::operator(), "Return a string representing the print of a SequenceSet.\n\n This is simply an alias for print();\n\nC++: genesis::sequence::PrinterSimple::operator()(const class genesis::sequence::SequenceSet &) const --> std::string", pybind11::arg("set"));
		cl.def("sequence_limit", (class genesis::sequence::PrinterSimple & (genesis::sequence::PrinterSimple::*)(unsigned long)) &genesis::sequence::PrinterSimple::sequence_limit, "Set the limit for how many Sequence%s to print.\n\n If this parameter is set to a value other than 0, only this number of Sequence%s are printed.\n Default is 0, thus, all Sequences are printed. If the given limit is lower than the acutal number\n of Sequences in the SequenceSet, ellipsis \" ...\" are appended.\n\nC++: genesis::sequence::PrinterSimple::sequence_limit(unsigned long) --> class genesis::sequence::PrinterSimple &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("sequence_limit", (unsigned long (genesis::sequence::PrinterSimple::*)() const) &genesis::sequence::PrinterSimple::sequence_limit, "Get the currently set limit for how many Sequence%s to print.\n\n See \n\n\nC++: genesis::sequence::PrinterSimple::sequence_limit() const --> unsigned long");
		cl.def("line_length", (class genesis::sequence::PrinterSimple & (genesis::sequence::PrinterSimple::*)(unsigned long)) &genesis::sequence::PrinterSimple::line_length, "Set the length of each line, i.e., when to wrap.\n\n If this parameter is set to a value other than 0, the Sequence is wrapped at this line length.\n This also works in combination with length_limit().\n\nC++: genesis::sequence::PrinterSimple::line_length(unsigned long) --> class genesis::sequence::PrinterSimple &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("line_length", (unsigned long (genesis::sequence::PrinterSimple::*)() const) &genesis::sequence::PrinterSimple::line_length, "Get the currently set line length, i.e., when to wrap.\n\n See \n\n\nC++: genesis::sequence::PrinterSimple::line_length() const --> unsigned long");
		cl.def("length_limit", (class genesis::sequence::PrinterSimple & (genesis::sequence::PrinterSimple::*)(unsigned long)) &genesis::sequence::PrinterSimple::length_limit, "Set the length limit for printing Sequence%s.\n\n This parameter limits the output length to the give number of chars. If set to 0, the whole\n Sequence is printed (default). If the limit is lower than the acutal number of sites in the\n Sequence, ellipsis \" ...\" are appended.\n\nC++: genesis::sequence::PrinterSimple::length_limit(unsigned long) --> class genesis::sequence::PrinterSimple &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("length_limit", (unsigned long (genesis::sequence::PrinterSimple::*)() const) &genesis::sequence::PrinterSimple::length_limit, "Get the currently set length limit.\n\n See \n\n\nC++: genesis::sequence::PrinterSimple::length_limit() const --> unsigned long");
		cl.def("color_map", (class genesis::sequence::PrinterSimple & (genesis::sequence::PrinterSimple::*)(const class std::map<char, std::string, struct std::less<char>, class std::allocator<struct std::pair<const char, std::string > > > &)) &genesis::sequence::PrinterSimple::color_map, "Set the list of colors to use for the Sequence charaters.\n\n The provided map sets the color name for each Sequence character.\n The presettings `nucleic_acid_text_colors()` and `amino_acid_text_colors()` for default sequence\n types can be used as input for this parameter.\n If the `colors` map does not contain a key for one of the chars in the sequence, the char is\n printed without color.\n\n See \n\n\n\nC++: genesis::sequence::PrinterSimple::color_map(const class std::map<char, std::string, struct std::less<char>, class std::allocator<struct std::pair<const char, std::string > > > &) --> class genesis::sequence::PrinterSimple &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("color_map", (const class std::map<char, std::string, struct std::less<char>, class std::allocator<struct std::pair<const char, std::string > > > & (genesis::sequence::PrinterSimple::*)() const) &genesis::sequence::PrinterSimple::color_map, "Get the currently set list of colors for each Sequence character.\n\n See \n\n\nC++: genesis::sequence::PrinterSimple::color_map() const --> const class std::map<char, std::string, struct std::less<char>, class std::allocator<struct std::pair<const char, std::string > > > &", pybind11::return_value_policy::reference_internal);
		cl.def("color_mode", (class genesis::sequence::PrinterSimple & (genesis::sequence::PrinterSimple::*)(enum genesis::sequence::PrinterSimple::ColorMode)) &genesis::sequence::PrinterSimple::color_mode, "Set whether to use color in the background, foreground, or not at all.\n\n See \n\n\nC++: genesis::sequence::PrinterSimple::color_mode(enum genesis::sequence::PrinterSimple::ColorMode) --> class genesis::sequence::PrinterSimple &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("color_mode", (enum genesis::sequence::PrinterSimple::ColorMode (genesis::sequence::PrinterSimple::*)() const) &genesis::sequence::PrinterSimple::color_mode, "Get the currently set color mode.\n\n See \n\n\nC++: genesis::sequence::PrinterSimple::color_mode() const --> enum genesis::sequence::PrinterSimple::ColorMode");
		cl.def("label_mode", (class genesis::sequence::PrinterSimple & (genesis::sequence::PrinterSimple::*)(enum genesis::sequence::PrinterSimple::LabelMode)) &genesis::sequence::PrinterSimple::label_mode, "Set the currently set \n\n\n\n\nC++: genesis::sequence::PrinterSimple::label_mode(enum genesis::sequence::PrinterSimple::LabelMode) --> class genesis::sequence::PrinterSimple &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("label_mode", (enum genesis::sequence::PrinterSimple::LabelMode (genesis::sequence::PrinterSimple::*)() const) &genesis::sequence::PrinterSimple::label_mode, "Get the currently set \n\n\n\n\n\nC++: genesis::sequence::PrinterSimple::label_mode() const --> enum genesis::sequence::PrinterSimple::LabelMode");
	}
}

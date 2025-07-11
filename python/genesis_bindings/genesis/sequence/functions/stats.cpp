#include <functional>
#include <genesis/sequence/counts.hpp>
#include <genesis/sequence/functions/labels.hpp>
#include <genesis/sequence/functions/stats.hpp>
#include <genesis/sequence/sequence.hpp>
#include <genesis/sequence/sequence_set.hpp>
#include <genesis/utils/tools/hash/functions.hpp>
#include <iterator>
#include <map>
#include <memory>
#include <sstream> // __str__
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

void bind_genesis_sequence_functions_stats(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::sequence::site_histogram(const class genesis::sequence::SequenceSet &) file:genesis/sequence/functions/stats.hpp line:67
	M("genesis::sequence").def("site_histogram", (class std::map<char, unsigned long, struct std::less<char>, class std::allocator<struct std::pair<const char, unsigned long> > > (*)(const class genesis::sequence::SequenceSet &)) &genesis::sequence::site_histogram, "Get a histogram of the occurrences of particular sites, given a SequenceSet.\n\n This gives the raw counts of how often each site (character) appears in the whole set.\n See base_frequencies() for the relative version of this function.\n\nC++: genesis::sequence::site_histogram(const class genesis::sequence::SequenceSet &) --> class std::map<char, unsigned long, struct std::less<char>, class std::allocator<struct std::pair<const char, unsigned long> > >", pybind11::arg("set"));

	// genesis::sequence::base_frequencies(const class genesis::sequence::Sequence &, const std::string &) file:genesis/sequence/functions/stats.hpp line:79
	M("genesis::sequence").def("base_frequencies", (class std::map<char, double, struct std::less<char>, class std::allocator<struct std::pair<const char, double> > > (*)(const class genesis::sequence::Sequence &, const std::string &)) &genesis::sequence::base_frequencies, "Get the base frequencies of the sites in a Sequence given the base chars.\n\n This returns the relative proportions of the given `plain_chars` to each other. Typically,\n the given chars come from either nucleic_acid_codes_plain() or amino_acid_codes_plain(),\n depending on the dataset.\n\n It is necessary to select those chars on a per-dataset basis, as it is up to the user to define\n the meaning of those chars.\n\nC++: genesis::sequence::base_frequencies(const class genesis::sequence::Sequence &, const std::string &) --> class std::map<char, double, struct std::less<char>, class std::allocator<struct std::pair<const char, double> > >", pybind11::arg("seq"), pybind11::arg("plain_chars"));

	// genesis::sequence::base_frequencies(const class genesis::sequence::SequenceSet &, const std::string &) file:genesis/sequence/functions/stats.hpp line:86
	M("genesis::sequence").def("base_frequencies", (class std::map<char, double, struct std::less<char>, class std::allocator<struct std::pair<const char, double> > > (*)(const class genesis::sequence::SequenceSet &, const std::string &)) &genesis::sequence::base_frequencies, "Get the base frequencies of the sites in a SequenceSet given the base chars.\n\n See the Sequence implementation of this function for details.\n\nC++: genesis::sequence::base_frequencies(const class genesis::sequence::SequenceSet &, const std::string &) --> class std::map<char, double, struct std::less<char>, class std::allocator<struct std::pair<const char, double> > >", pybind11::arg("set"), pybind11::arg("plain_chars"));

	// genesis::sequence::count_chars(const class genesis::sequence::SequenceSet &, const std::string &) file:genesis/sequence/functions/stats.hpp line:98
	M("genesis::sequence").def("count_chars", (unsigned long (*)(const class genesis::sequence::SequenceSet &, const std::string &)) &genesis::sequence::count_chars, "Count the number of occurrences of the given `chars` within the sites of the SequenceSet.\n\n This function can be used to count e.g. gaps or ambiguous characters in sequences.\n For presettings of usable chars, see the functions `nucleic_acid_codes_...` and\n `amino_acid_codes_...`. The chars are treated case-insensitive.\n\n If `chars` contains invalid (non-standard ASCII) characters, an `std::invalid_argument`\n exception is thrown.\n\nC++: genesis::sequence::count_chars(const class genesis::sequence::SequenceSet &, const std::string &) --> unsigned long", pybind11::arg("set"), pybind11::arg("chars"));

	// genesis::sequence::gapyness(const class genesis::sequence::SequenceSet &, const std::string &) file:genesis/sequence/functions/stats.hpp line:111
	M("genesis::sequence").def("gapyness", (double (*)(const class genesis::sequence::SequenceSet &, const std::string &)) &genesis::sequence::gapyness, "Return the \"gapyness\" of the Sequence%s, i.e., the proportion of gap chars\n and other completely undetermined chars to the total length of all sequences.\n\n This function returns a value in the interval 0.0 (no gaps and undetermined chars at all)\n and 1.0 (all chars are undetermined).\n See `nucleic_acid_codes_undetermined()` and `amino_acid_codes_undetermined()` for presettings\n of gap character that can be used here depending on the data set type.\n The chars are treated case-insensitive.\n In the special case that there are no sequences or sites, 0.0 is returned.\n\nC++: genesis::sequence::gapyness(const class genesis::sequence::SequenceSet &, const std::string &) --> double", pybind11::arg("set"), pybind11::arg("gap_chars"));

	// genesis::sequence::gap_site_count(const class genesis::sequence::SiteCounts &) file:genesis/sequence/functions/stats.hpp line:113
	M("genesis::sequence").def("gap_site_count", (unsigned long (*)(const class genesis::sequence::SiteCounts &)) &genesis::sequence::gap_site_count, "C++: genesis::sequence::gap_site_count(const class genesis::sequence::SiteCounts &) --> unsigned long", pybind11::arg("counts"));

	{ // genesis::sequence::LabelAttributes file:genesis/sequence/functions/labels.hpp line:55
		pybind11::class_<genesis::sequence::LabelAttributes, std::shared_ptr<genesis::sequence::LabelAttributes>> cl(M("genesis::sequence"), "LabelAttributes", "");
		cl.def( pybind11::init( [](){ return new genesis::sequence::LabelAttributes(); } ) );
		cl.def( pybind11::init( [](genesis::sequence::LabelAttributes const &o){ return new genesis::sequence::LabelAttributes(o); } ) );
		cl.def_readwrite("label", &genesis::sequence::LabelAttributes::label);
		cl.def_readwrite("attributes", &genesis::sequence::LabelAttributes::attributes);
		cl.def("assign", (struct genesis::sequence::LabelAttributes & (genesis::sequence::LabelAttributes::*)(const struct genesis::sequence::LabelAttributes &)) &genesis::sequence::LabelAttributes::operator=, "C++: genesis::sequence::LabelAttributes::operator=(const struct genesis::sequence::LabelAttributes &) --> struct genesis::sequence::LabelAttributes &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
	// genesis::sequence::find_sequence(const class genesis::sequence::SequenceSet &, const std::string &) file:genesis/sequence/functions/labels.hpp line:68
	M("genesis::sequence").def("find_sequence", (const class genesis::sequence::Sequence * (*)(const class genesis::sequence::SequenceSet &, const std::string &)) &genesis::sequence::find_sequence, "Return a pointer to a Sequence with a specific label, or `nullptr` iff not found.\n\nC++: genesis::sequence::find_sequence(const class genesis::sequence::SequenceSet &, const std::string &) --> const class genesis::sequence::Sequence *", pybind11::return_value_policy::reference_internal, pybind11::arg("set"), pybind11::arg("label"));

	// genesis::sequence::labels(const class genesis::sequence::SequenceSet &) file:genesis/sequence/functions/labels.hpp line:73
	M("genesis::sequence").def("labels", (class std::unordered_set<std::string, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<std::string > > (*)(const class genesis::sequence::SequenceSet &)) &genesis::sequence::labels, "Return a set of all labels of the SequenceSet.\n\nC++: genesis::sequence::labels(const class genesis::sequence::SequenceSet &) --> class std::unordered_set<std::string, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<std::string > >", pybind11::arg("set"));

	// genesis::sequence::guess_sequence_abundance(const class genesis::sequence::Sequence &) file:genesis/sequence/functions/labels.hpp line:92
	M("genesis::sequence").def("guess_sequence_abundance", (struct std::pair<std::string, unsigned long> (*)(const class genesis::sequence::Sequence &)) &genesis::sequence::guess_sequence_abundance, "Guess the abundance of a Sequence, using it's label.\n\n The function splits the label of a Sequence into two parts: the descriptive name of the\n sequence, and an abundance value (weight or multiplicity of the sequence),\n which are returned as a `std::pair`.\n\n The function accepts two patterns of reporting abundances via the\n \n\n\n\n\n\n\n\n\n\n\nC++: genesis::sequence::guess_sequence_abundance(const class genesis::sequence::Sequence &) --> struct std::pair<std::string, unsigned long>", pybind11::arg("sequence"));

	// genesis::sequence::guess_sequence_abundance(const std::string &) file:genesis/sequence/functions/labels.hpp line:100
	M("genesis::sequence").def("guess_sequence_abundance", (struct std::pair<std::string, unsigned long> (*)(const std::string &)) &genesis::sequence::guess_sequence_abundance, "Guess the abundance of a Sequence, given it's label.\n\n This is the same as guess_sequence_abundance( Sequence const& ), but takes the label\n as a string, instead of the Sequence object. See there for details.\n\nC++: genesis::sequence::guess_sequence_abundance(const std::string &) --> struct std::pair<std::string, unsigned long>", pybind11::arg("label"));

	// genesis::sequence::label_attributes(const class genesis::sequence::Sequence &) file:genesis/sequence/functions/labels.hpp line:121
	M("genesis::sequence").def("label_attributes", (struct genesis::sequence::LabelAttributes (*)(const class genesis::sequence::Sequence &)) &genesis::sequence::label_attributes, "Get the attributes list (semicolons-separated) from a Sequence.\n\n It is common to store additional information in sequence headers, e.g., in the `fasta` format,\n using a semicolon-separated list of attributes like this:\n\n     >some_name;size=123;thing=foo;\n\n This function disects this kind of information and returns it.\n The returned struct contains the label (the part before the first semicolon),\n as well as a map for the attributes. As this is not a multimap, later attributes with the same\n key overwrite earlier ones.\n\n If the sequence label does not contain any information that is separated via a semicolon,\n the attributes list is returned empty. However, if semicola are found in the label,\n the correct format is expected (with the syntax `;key=value;`) for each attribute.\n Otherwise, an exception is thrown. The last semicolon is optional; that is, the label\n can simply end after the last value.\n\nC++: genesis::sequence::label_attributes(const class genesis::sequence::Sequence &) --> struct genesis::sequence::LabelAttributes", pybind11::arg("sequence"));

	// genesis::sequence::label_attributes(const std::string &) file:genesis/sequence/functions/labels.hpp line:129
	M("genesis::sequence").def("label_attributes", (struct genesis::sequence::LabelAttributes (*)(const std::string &)) &genesis::sequence::label_attributes, "Get the attributes list (semicolons-separated) from a Sequence, given it's label.\n\n This is the same as label_attributes( Sequence const& ), but takes the label\n as a string, instead of the Sequence object. See there for details.\n\nC++: genesis::sequence::label_attributes(const std::string &) --> struct genesis::sequence::LabelAttributes", pybind11::arg("label"));

	// genesis::sequence::has_unique_labels(const class genesis::sequence::SequenceSet &, bool) file:genesis/sequence/functions/labels.hpp line:141
	M("genesis::sequence").def("has_unique_labels", [](const class genesis::sequence::SequenceSet & a0) -> bool { return genesis::sequence::has_unique_labels(a0); }, "", pybind11::arg("set"));
	M("genesis::sequence").def("has_unique_labels", (bool (*)(const class genesis::sequence::SequenceSet &, bool)) &genesis::sequence::has_unique_labels, "Return true iff all labels of the Sequence%s in the SequenceSet are unique.\n\n The optional parameter `case_sensitive` controls how labels are compared. Default is `true`,\n that is, Sequence%s are compared case-sensitively.\n\nC++: genesis::sequence::has_unique_labels(const class genesis::sequence::SequenceSet &, bool) --> bool", pybind11::arg("set"), pybind11::arg("case_sensitive"));

	// genesis::sequence::relabel_with_hash(class genesis::sequence::Sequence &, enum genesis::utils::HashingFunctions) file:genesis/sequence/functions/labels.hpp line:148
	M("genesis::sequence").def("relabel_with_hash", (void (*)(class genesis::sequence::Sequence &, enum genesis::utils::HashingFunctions)) &genesis::sequence::relabel_with_hash, "Relabel the Sequence using the hash digest of its sites.\n\n See ::utils::HashingFunctions for the available hashing functions.\n\nC++: genesis::sequence::relabel_with_hash(class genesis::sequence::Sequence &, enum genesis::utils::HashingFunctions) --> void", pybind11::arg("seq"), pybind11::arg("hash_function"));

	// genesis::sequence::relabel_with_hash(class genesis::sequence::SequenceSet &, enum genesis::utils::HashingFunctions) file:genesis/sequence/functions/labels.hpp line:159
	M("genesis::sequence").def("relabel_with_hash", (void (*)(class genesis::sequence::SequenceSet &, enum genesis::utils::HashingFunctions)) &genesis::sequence::relabel_with_hash, "Relabel all Sequence%s in the SequenceSet using the hash digest of the sites.\n\n See ::utils::HashingFunctions for the available hashing functions.\n\n If there are duplicate Sequence%s, this function will lead to multiple Sequence%s with the same\n name, which might be an issue for downstream programs that expect unique labels.\n See has_unique_labels() to check this.\n\nC++: genesis::sequence::relabel_with_hash(class genesis::sequence::SequenceSet &, enum genesis::utils::HashingFunctions) --> void", pybind11::arg("set"), pybind11::arg("hash_function"));

	// genesis::sequence::is_valid_label(const std::string &) file:genesis/sequence/functions/labels.hpp line:182
	M("genesis::sequence").def("is_valid_label", (bool (*)(const std::string &)) &genesis::sequence::is_valid_label, "Check whether a given string is a valid label for a Sequence.\n\n While we can work with any form of label (as long as it is a string), most file formats and\n consequently most programs that read them restrict the set of valid characters for labels of\n sequences. We thus provide this function, which uses the most common interpretation of valid\n labels.\n\n A label is valid if its characters have a graphical representation (i.e., isgraph() is true) and\n if non of these characters occurs:\n\n     :,();[]'\n\n Thus, all whitespaces, control characters, and the listed special characters are invalid.\n See sanitize_label() for a function that replaces all invalid characters of the label by\n underscores.\n\nC++: genesis::sequence::is_valid_label(const std::string &) --> bool", pybind11::arg("label"));

	// genesis::sequence::has_valid_label(const class genesis::sequence::Sequence &) file:genesis/sequence/functions/labels.hpp line:192
	M("genesis::sequence").def("has_valid_label", (bool (*)(const class genesis::sequence::Sequence &)) &genesis::sequence::has_valid_label, "Check whether a Sequence has a valid label.\n\n This might be important for printing the Sequence to a file that needs to be read by other\n applications. See is_valid_label() for details on what is considered a valid label.\n See sanitize_label() for a function that replaces all invalid characters of the label by\n underscores.\n\nC++: genesis::sequence::has_valid_label(const class genesis::sequence::Sequence &) --> bool", pybind11::arg("seq"));

	// genesis::sequence::has_valid_labels(const class genesis::sequence::SequenceSet &) file:genesis/sequence/functions/labels.hpp line:202
	M("genesis::sequence").def("has_valid_labels", (bool (*)(const class genesis::sequence::SequenceSet &)) &genesis::sequence::has_valid_labels, "Check whether all Sequence%s in a SequenceSet have valid labels.\n\n This might be important for printing the Sequences to a file that needs to be read by other\n applications. See is_valid_label() for details on what is considered a valid label.\n See sanitize_labels() for a function that replaces all invalid characters of the labels by\n underscores.\n\nC++: genesis::sequence::has_valid_labels(const class genesis::sequence::SequenceSet &) --> bool", pybind11::arg("set"));

	// genesis::sequence::sanitize_label(const std::string &) file:genesis/sequence/functions/labels.hpp line:210
	M("genesis::sequence").def("sanitize_label", (std::string (*)(const std::string &)) &genesis::sequence::sanitize_label, "Sanitize a label by replacing all invalid characters with underscores.\n\n This might be important for printing the Sequences to a file that needs to be read by other\n applications. See is_valid_label() for details on what is considered a valid label.\n\nC++: genesis::sequence::sanitize_label(const std::string &) --> std::string", pybind11::arg("label"));

}

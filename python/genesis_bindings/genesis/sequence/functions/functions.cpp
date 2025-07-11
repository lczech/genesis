#include <functional>
#include <genesis/sequence/functions/functions.hpp>
#include <genesis/sequence/sequence.hpp>
#include <genesis/sequence/sequence_set.hpp>
#include <genesis/utils/math/bitvector.hpp>
#include <genesis/utils/tools/char_lookup.hpp>
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

void bind_genesis_sequence_functions_functions(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::sequence::find_sites(const class genesis::sequence::Sequence &, const std::string &) file:genesis/sequence/functions/functions.hpp line:63
	M("genesis::sequence").def("find_sites", (class genesis::utils::Bitvector (*)(const class genesis::sequence::Sequence &, const std::string &)) &genesis::sequence::find_sites, "Find sites by character and mark them in a \n\n\n\n\n\n\nC++: genesis::sequence::find_sites(const class genesis::sequence::Sequence &, const std::string &) --> class genesis::utils::Bitvector", pybind11::arg("seq"), pybind11::arg("chars"));

	// genesis::sequence::find_sites(const class genesis::sequence::Sequence &, const class genesis::utils::CharLookup<bool> &) file:genesis/sequence/functions/functions.hpp line:75
	M("genesis::sequence").def("find_sites", (class genesis::utils::Bitvector (*)(const class genesis::sequence::Sequence &, const class genesis::utils::CharLookup<bool> &)) &genesis::sequence::find_sites, "Find sites by character and mark them in a \n\n\n\n\n\n\nC++: genesis::sequence::find_sites(const class genesis::sequence::Sequence &, const class genesis::utils::CharLookup<bool> &) --> class genesis::utils::Bitvector", pybind11::arg("seq"), pybind11::arg("chars"));

	// genesis::sequence::gap_sites(const class genesis::sequence::Sequence &, const std::string &) file:genesis/sequence/functions/functions.hpp line:88
	M("genesis::sequence").def("gap_sites", [](const class genesis::sequence::Sequence & a0) -> genesis::utils::Bitvector { return genesis::sequence::gap_sites(a0); }, "", pybind11::arg("seq"));
	M("genesis::sequence").def("gap_sites", (class genesis::utils::Bitvector (*)(const class genesis::sequence::Sequence &, const std::string &)) &genesis::sequence::gap_sites, "Return a \n\n\n\n\n\n\n\nC++: genesis::sequence::gap_sites(const class genesis::sequence::Sequence &, const std::string &) --> class genesis::utils::Bitvector", pybind11::arg("seq"), pybind11::arg("gap_chars"));

	// genesis::sequence::gap_sites(const class genesis::sequence::SequenceSet &, const std::string &) file:genesis/sequence/functions/functions.hpp line:102
	M("genesis::sequence").def("gap_sites", [](const class genesis::sequence::SequenceSet & a0) -> genesis::utils::Bitvector { return genesis::sequence::gap_sites(a0); }, "", pybind11::arg("set"));
	M("genesis::sequence").def("gap_sites", (class genesis::utils::Bitvector (*)(const class genesis::sequence::SequenceSet &, const std::string &)) &genesis::sequence::gap_sites, "Return a \n\n\n\n\n\n\n\n\nC++: genesis::sequence::gap_sites(const class genesis::sequence::SequenceSet &, const std::string &) --> class genesis::utils::Bitvector", pybind11::arg("set"), pybind11::arg("gap_chars"));

	// genesis::sequence::validate_chars(const class genesis::sequence::SequenceSet &, const std::string &) file:genesis/sequence/functions/functions.hpp line:117
	M("genesis::sequence").def("validate_chars", (bool (*)(const class genesis::sequence::SequenceSet &, const std::string &)) &genesis::sequence::validate_chars, "Returns true iff all Sequence%s only consist of the given `chars`.\n\n For presettings of usable chars, see the functions `nucleic_acid_codes_...` and\n `amino_acid_codes_...`. For example, to check whether the sequences are nucleic acids,\n use `nucleic_acid_codes_all()`. The chars are treated case-insensitive.\n\n If `chars` contains invalid (non-standard ASCII) characters, an `std::invalid_argument`\n exception is thrown.\n\nC++: genesis::sequence::validate_chars(const class genesis::sequence::SequenceSet &, const std::string &) --> bool", pybind11::arg("set"), pybind11::arg("chars"));

	// genesis::sequence::longest_sequence_length(const class genesis::sequence::SequenceSet &) file:genesis/sequence/functions/functions.hpp line:122
	M("genesis::sequence").def("longest_sequence_length", (unsigned long (*)(const class genesis::sequence::SequenceSet &)) &genesis::sequence::longest_sequence_length, "Return the length of the longest Sequence in the SequenceSet.\n\nC++: genesis::sequence::longest_sequence_length(const class genesis::sequence::SequenceSet &) --> unsigned long", pybind11::arg("set"));

	// genesis::sequence::total_length(const class genesis::sequence::SequenceSet &) file:genesis/sequence/functions/functions.hpp line:127
	M("genesis::sequence").def("total_length", (unsigned long (*)(const class genesis::sequence::SequenceSet &)) &genesis::sequence::total_length, "Return the total length (sum) of all Sequence%s in the SequenceSet.\n\nC++: genesis::sequence::total_length(const class genesis::sequence::SequenceSet &) --> unsigned long", pybind11::arg("set"));

	// genesis::sequence::is_alignment(const class genesis::sequence::SequenceSet &) file:genesis/sequence/functions/functions.hpp line:132
	M("genesis::sequence").def("is_alignment", (bool (*)(const class genesis::sequence::SequenceSet &)) &genesis::sequence::is_alignment, "Return true iff all Sequence%s in the SequenceSet have the same length.\n\nC++: genesis::sequence::is_alignment(const class genesis::sequence::SequenceSet &) --> bool", pybind11::arg("set"));

	// genesis::sequence::remove_sites(class genesis::sequence::Sequence &, class genesis::utils::Bitvector) file:genesis/sequence/functions/functions.hpp line:146
	M("genesis::sequence").def("remove_sites", (void (*)(class genesis::sequence::Sequence &, class genesis::utils::Bitvector)) &genesis::sequence::remove_sites, "Remove all sites from a Sequence where the given \n\n\n\n\n\n\n\nC++: genesis::sequence::remove_sites(class genesis::sequence::Sequence &, class genesis::utils::Bitvector) --> void", pybind11::arg("seq"), pybind11::arg("sites"));

	// genesis::sequence::remove_sites(class genesis::sequence::SequenceSet &, class genesis::utils::Bitvector) file:genesis/sequence/functions/functions.hpp line:158
	M("genesis::sequence").def("remove_sites", (void (*)(class genesis::sequence::SequenceSet &, class genesis::utils::Bitvector)) &genesis::sequence::remove_sites, "Remove all sites from all Sequence%s in a SequenceSet where the given\n \n\n\n\n\n\n\n\n\n\nC++: genesis::sequence::remove_sites(class genesis::sequence::SequenceSet &, class genesis::utils::Bitvector) --> void", pybind11::arg("set"), pybind11::arg("sites"));

	// genesis::sequence::remove_gap_sites(class genesis::sequence::SequenceSet &, const std::string &) file:genesis/sequence/functions/functions.hpp line:163
	M("genesis::sequence").def("remove_gap_sites", [](class genesis::sequence::SequenceSet & a0) -> void { return genesis::sequence::remove_gap_sites(a0); }, "", pybind11::arg("set"));
	M("genesis::sequence").def("remove_gap_sites", (void (*)(class genesis::sequence::SequenceSet &, const std::string &)) &genesis::sequence::remove_gap_sites, "Remove all sites that only contain gap characters from the SequenceSet.\n\nC++: genesis::sequence::remove_gap_sites(class genesis::sequence::SequenceSet &, const std::string &) --> void", pybind11::arg("set"), pybind11::arg("gap_chars"));

	// genesis::sequence::remove_characters(class genesis::sequence::Sequence &, const std::string &) file:genesis/sequence/functions/functions.hpp line:168
	M("genesis::sequence").def("remove_characters", (void (*)(class genesis::sequence::Sequence &, const std::string &)) &genesis::sequence::remove_characters, "Remove all of the characters in `search` from the sites of the Sequence.\n\nC++: genesis::sequence::remove_characters(class genesis::sequence::Sequence &, const std::string &) --> void", pybind11::arg("seq"), pybind11::arg("search"));

	// genesis::sequence::remove_characters(class genesis::sequence::SequenceSet &, const std::string &) file:genesis/sequence/functions/functions.hpp line:174
	M("genesis::sequence").def("remove_characters", (void (*)(class genesis::sequence::SequenceSet &, const std::string &)) &genesis::sequence::remove_characters, "Remove all of the characters in `search` from the sites of the Sequence%s in the\n SequenceSet.\n\nC++: genesis::sequence::remove_characters(class genesis::sequence::SequenceSet &, const std::string &) --> void", pybind11::arg("set"), pybind11::arg("search"));

	// genesis::sequence::remove_all_gaps(class genesis::sequence::Sequence &, const std::string &) file:genesis/sequence/functions/functions.hpp line:182
	M("genesis::sequence").def("remove_all_gaps", [](class genesis::sequence::Sequence & a0) -> void { return genesis::sequence::remove_all_gaps(a0); }, "", pybind11::arg("seq"));
	M("genesis::sequence").def("remove_all_gaps", (void (*)(class genesis::sequence::Sequence &, const std::string &)) &genesis::sequence::remove_all_gaps, "Remove all gap characters from the sites of the Sequence.\n\n This function is an alias for remove_characters(), which by default uses the gap sites of\n nucleic_acid_codes_undetermined().\n\nC++: genesis::sequence::remove_all_gaps(class genesis::sequence::Sequence &, const std::string &) --> void", pybind11::arg("seq"), pybind11::arg("gap_chars"));

	// genesis::sequence::remove_all_gaps(class genesis::sequence::SequenceSet &, const std::string &) file:genesis/sequence/functions/functions.hpp line:190
	M("genesis::sequence").def("remove_all_gaps", [](class genesis::sequence::SequenceSet & a0) -> void { return genesis::sequence::remove_all_gaps(a0); }, "", pybind11::arg("set"));
	M("genesis::sequence").def("remove_all_gaps", (void (*)(class genesis::sequence::SequenceSet &, const std::string &)) &genesis::sequence::remove_all_gaps, "Remove all gap characters from the sites of the Sequence%s in the SequenceSet.\n\n This function is an alias for remove_characters(), which by default uses the gap sites of\n nucleic_acid_codes_undetermined().\n\nC++: genesis::sequence::remove_all_gaps(class genesis::sequence::SequenceSet &, const std::string &) --> void", pybind11::arg("set"), pybind11::arg("gap_chars"));

	// genesis::sequence::replace_characters(class genesis::sequence::Sequence &, const std::string &, char) file:genesis/sequence/functions/functions.hpp line:200
	M("genesis::sequence").def("replace_characters", (void (*)(class genesis::sequence::Sequence &, const std::string &, char)) &genesis::sequence::replace_characters, "Replace all occurences of the chars in `search` by the `replace` char, for all sites in\n the given Sequence.\n\n The function is case sensitive. Thus, you need to use both cases for the search string if you\n are unsure. The replace char is always used as is, independent of the case of the matching\n search char.\n\nC++: genesis::sequence::replace_characters(class genesis::sequence::Sequence &, const std::string &, char) --> void", pybind11::arg("seq"), pybind11::arg("search"), pybind11::arg("replacement"));

	// genesis::sequence::replace_characters(class genesis::sequence::SequenceSet &, const std::string &, char) file:genesis/sequence/functions/functions.hpp line:210
	M("genesis::sequence").def("replace_characters", (void (*)(class genesis::sequence::SequenceSet &, const std::string &, char)) &genesis::sequence::replace_characters, "Replace all occurences of the chars in `search` by the `replace` char, for all sites in\n the Sequence%s in the given SequenceSet.\n\n The function is case sensitive. Thus, you need to use both cases for the search string if you\n are unsure. The replace char is always used as is, independent of the case of the matching\n search char.\n\nC++: genesis::sequence::replace_characters(class genesis::sequence::SequenceSet &, const std::string &, char) --> void", pybind11::arg("set"), pybind11::arg("search"), pybind11::arg("replacement"));

	// genesis::sequence::replace_u_with_t(class genesis::sequence::Sequence &) file:genesis/sequence/functions/functions.hpp line:218
	M("genesis::sequence").def("replace_u_with_t", (void (*)(class genesis::sequence::Sequence &)) &genesis::sequence::replace_u_with_t, "Replace all occurrences of `U` by `T` in the sites of the Sequence.\n\n This is a small helper function for sequences with nucleic acid codes. It is case sensitive,\n that is, lower case `u` is replaced by lower case `t`, and upper case `U` by upper case `T`.\n\nC++: genesis::sequence::replace_u_with_t(class genesis::sequence::Sequence &) --> void", pybind11::arg("seq"));

	// genesis::sequence::replace_u_with_t(class genesis::sequence::SequenceSet &) file:genesis/sequence/functions/functions.hpp line:226
	M("genesis::sequence").def("replace_u_with_t", (void (*)(class genesis::sequence::SequenceSet &)) &genesis::sequence::replace_u_with_t, "Replace all occurrences of `U` by `T` in the sites of all Sequence%s in the SequenceSet.\n\n This is a small helper function for sequences with nucleic acid codes. It is case sensitive,\n that is, lower case `u` is replaced by lower case `t`, and upper case `U` by upper case `T`.\n\nC++: genesis::sequence::replace_u_with_t(class genesis::sequence::SequenceSet &) --> void", pybind11::arg("set"));

	// genesis::sequence::replace_t_with_u(class genesis::sequence::Sequence &) file:genesis/sequence/functions/functions.hpp line:234
	M("genesis::sequence").def("replace_t_with_u", (void (*)(class genesis::sequence::Sequence &)) &genesis::sequence::replace_t_with_u, "Replace all occurrences of `T` by `U` in the sites of the Sequence.\n\n This is a small helper function for sequences with nucleic acid codes. It is case sensitive,\n that is, lower case `t` is replaced by lower case `u`, and upper case `T` by upper case `U`.\n\nC++: genesis::sequence::replace_t_with_u(class genesis::sequence::Sequence &) --> void", pybind11::arg("seq"));

	// genesis::sequence::replace_t_with_u(class genesis::sequence::SequenceSet &) file:genesis/sequence/functions/functions.hpp line:242
	M("genesis::sequence").def("replace_t_with_u", (void (*)(class genesis::sequence::SequenceSet &)) &genesis::sequence::replace_t_with_u, "Replace all occurrences of `T` by `U` in the sites of all Sequence%s in the SequenceSet.\n\n This is a small helper function for sequences with nucleic acid codes. It is case sensitive,\n that is, lower case `t` is replaced by lower case `u`, and upper case `T` by upper case `U`.\n\nC++: genesis::sequence::replace_t_with_u(class genesis::sequence::SequenceSet &) --> void", pybind11::arg("set"));

	// genesis::sequence::MergeDuplicateSequencesCountPolicy file:genesis/sequence/functions/functions.hpp line:251
	pybind11::enum_<genesis::sequence::MergeDuplicateSequencesCountPolicy>(M("genesis::sequence"), "MergeDuplicateSequencesCountPolicy", "Provide options for changing how merge_duplicate_sequences() handles the counts\n of merged Sequence%s.\n\n This allows to remove duplicate sequences while still keeping track of how many there were\n before merging them.")
		.value("kDiscard", genesis::sequence::MergeDuplicateSequencesCountPolicy::kDiscard)
		.value("kAppendToLabel", genesis::sequence::MergeDuplicateSequencesCountPolicy::kAppendToLabel);

;

	// genesis::sequence::merge_duplicate_sequences(class genesis::sequence::SequenceSet &, enum genesis::sequence::MergeDuplicateSequencesCountPolicy, const std::string &) file:genesis/sequence/functions/functions.hpp line:277
	M("genesis::sequence").def("merge_duplicate_sequences", [](class genesis::sequence::SequenceSet & a0) -> void { return genesis::sequence::merge_duplicate_sequences(a0); }, "", pybind11::arg("set"));
	M("genesis::sequence").def("merge_duplicate_sequences", [](class genesis::sequence::SequenceSet & a0, enum genesis::sequence::MergeDuplicateSequencesCountPolicy const & a1) -> void { return genesis::sequence::merge_duplicate_sequences(a0, a1); }, "", pybind11::arg("set"), pybind11::arg("count_policy"));
	M("genesis::sequence").def("merge_duplicate_sequences", (void (*)(class genesis::sequence::SequenceSet &, enum genesis::sequence::MergeDuplicateSequencesCountPolicy, const std::string &)) &genesis::sequence::merge_duplicate_sequences, "Merge all Sequence%s in a SequenceSet that have identical sites.\n\n The merging is done by removing all but the first Sequence with identical sites. That means,\n the resulting \"representative\" of a set of merged Sequences has the label of the\n original Sequence that was first in the SequenceSet.\n\n Using the MergeDuplicateSequencesCountPolicy, it is possible to store the counts of the\n Sequences (i.e., how often they appeard before merging) within the label of the Sequence,\n separated by `counter_prefix`. With the default settings, the count is\n appended to the label, separated by an underscore.\n\nC++: genesis::sequence::merge_duplicate_sequences(class genesis::sequence::SequenceSet &, enum genesis::sequence::MergeDuplicateSequencesCountPolicy, const std::string &) --> void", pybind11::arg("set"), pybind11::arg("count_policy"), pybind11::arg("counter_prefix"));

}

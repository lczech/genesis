#include <genesis/sequence/functions/signature_specifications.hpp>
#include <genesis/sequence/functions/signatures.hpp>
#include <genesis/sequence/sequence.hpp>
#include <ios>
#include <iterator>
#include <locale>
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

void bind_genesis_sequence_functions_signatures_1(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::sequence::kmer_strings_non_overlapping(const class genesis::sequence::Sequence &, const class genesis::sequence::SignatureSpecifications &, std::ostream &) file:genesis/sequence/functions/signatures.hpp line:316
	M("genesis::sequence").def("kmer_strings_non_overlapping", (void (*)(const class genesis::sequence::Sequence &, const class genesis::sequence::SignatureSpecifications &, std::ostream &)) &genesis::sequence::kmer_strings_non_overlapping, "Print the  spitted into non-overlapping k-mers.\n\n This is identical to\n kmer_strings_non_overlapping( Sequence const&, SignatureSpecifications const& ),\n but prints directly to a stream, which is better for processing large files.\n After each k-mer sequence, a new line character is printed.\n\nC++: genesis::sequence::kmer_strings_non_overlapping(const class genesis::sequence::Sequence &, const class genesis::sequence::SignatureSpecifications &, std::ostream &) --> void", pybind11::arg("sequence"), pybind11::arg("settings"), pybind11::arg("out"));

	{ // genesis::sequence::SignatureSpecifications file:genesis/sequence/functions/signature_specifications.hpp line:64
		pybind11::class_<genesis::sequence::SignatureSpecifications, std::shared_ptr<genesis::sequence::SignatureSpecifications>> cl(M("genesis::sequence"), "SignatureSpecifications", "Specifications for calculating signatures (like k-mer counts) from Sequence%s.\n\n This class stores settings needed for signature functions like signature_counts(),\n signature_frequencies(), signature_symmetrized_frequencies() etc.\n It mainly stores the alphabet() and k() to use for these calculations.\n\n It also serves as storage and lookup for index tables that are needed by those functions.\n Thus, the indices are only created once per instance of this class, that is, once per alphabet\n and k. This saves costs when calculating signatures for many Sequence%s.");
		cl.def( pybind11::init( [](){ return new genesis::sequence::SignatureSpecifications(); } ) );
		cl.def( pybind11::init<const std::string &, unsigned long>(), pybind11::arg("alphabet"), pybind11::arg("k") );

		cl.def( pybind11::init( [](genesis::sequence::SignatureSpecifications const &o){ return new genesis::sequence::SignatureSpecifications(o); } ) );

		pybind11::enum_<genesis::sequence::SignatureSpecifications::UnknownCharBehavior>(cl, "UnknownCharBehavior", "List of policies to decide what to do when a char that is not part of the alphabet\n occurs while counting kmers.")
			.value("kSkip", genesis::sequence::SignatureSpecifications::UnknownCharBehavior::kSkip)
			.value("kThrow", genesis::sequence::SignatureSpecifications::UnknownCharBehavior::kThrow);

		cl.def("assign", (class genesis::sequence::SignatureSpecifications & (genesis::sequence::SignatureSpecifications::*)(const class genesis::sequence::SignatureSpecifications &)) &genesis::sequence::SignatureSpecifications::operator=, "Default copy assignment.\n\nC++: genesis::sequence::SignatureSpecifications::operator=(const class genesis::sequence::SignatureSpecifications &) --> class genesis::sequence::SignatureSpecifications &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("alphabet", (const std::string & (genesis::sequence::SignatureSpecifications::*)() const) &genesis::sequence::SignatureSpecifications::alphabet, "C++: genesis::sequence::SignatureSpecifications::alphabet() const --> const std::string &", pybind11::return_value_policy::reference_internal);
		cl.def("k", (unsigned long (genesis::sequence::SignatureSpecifications::*)() const) &genesis::sequence::SignatureSpecifications::k, "C++: genesis::sequence::SignatureSpecifications::k() const --> unsigned long");
		cl.def("unknown_char_behavior", (enum genesis::sequence::SignatureSpecifications::UnknownCharBehavior (genesis::sequence::SignatureSpecifications::*)() const) &genesis::sequence::SignatureSpecifications::unknown_char_behavior, "C++: genesis::sequence::SignatureSpecifications::unknown_char_behavior() const --> enum genesis::sequence::SignatureSpecifications::UnknownCharBehavior");
		cl.def("is_nucleic_acids", (bool (genesis::sequence::SignatureSpecifications::*)() const) &genesis::sequence::SignatureSpecifications::is_nucleic_acids, "Speedup and shortcut to test whether the alphabet() is `\"ACGT\"`.\n\nC++: genesis::sequence::SignatureSpecifications::is_nucleic_acids() const --> bool");
		cl.def("char_index", (unsigned long (genesis::sequence::SignatureSpecifications::*)(char) const) &genesis::sequence::SignatureSpecifications::char_index, "Return the index of a char within the alphabet().\n\n For chars that are not in the alphabet, InvalidCharIndex is returned as\n an indicator value.\n\nC++: genesis::sequence::SignatureSpecifications::char_index(char) const --> unsigned long", pybind11::arg("c"));
		cl.def("kmer_list", (const class std::vector<std::string, class std::allocator<std::string > > & (genesis::sequence::SignatureSpecifications::*)() const) &genesis::sequence::SignatureSpecifications::kmer_list, "Return the list of all possible k-mers for the given `k` and `alphabet`.\n\nC++: genesis::sequence::SignatureSpecifications::kmer_list() const --> const class std::vector<std::string, class std::allocator<std::string > > &", pybind11::return_value_policy::reference_internal);
		cl.def("kmer_list_size", (unsigned long (genesis::sequence::SignatureSpecifications::*)() const) &genesis::sequence::SignatureSpecifications::kmer_list_size, "C++: genesis::sequence::SignatureSpecifications::kmer_list_size() const --> unsigned long");
		cl.def("kmer_combined_reverse_complement_map", (const class std::vector<unsigned long, class std::allocator<unsigned long> > & (genesis::sequence::SignatureSpecifications::*)() const) &genesis::sequence::SignatureSpecifications::kmer_combined_reverse_complement_map, "Get a map from indices of kmer_list() and signature_counts() vectors to a smaller list\n which combines reverse complementary kmers for nucleic acid sequences.\n\nC++: genesis::sequence::SignatureSpecifications::kmer_combined_reverse_complement_map() const --> const class std::vector<unsigned long, class std::allocator<unsigned long> > &", pybind11::return_value_policy::reference_internal);
		cl.def("kmer_reverse_complement_indices", (const class std::vector<unsigned long, class std::allocator<unsigned long> > & (genesis::sequence::SignatureSpecifications::*)() const) &genesis::sequence::SignatureSpecifications::kmer_reverse_complement_indices, "Get the indices for each kmer in kmer_list() to its reverse complement in the list.\n\nC++: genesis::sequence::SignatureSpecifications::kmer_reverse_complement_indices() const --> const class std::vector<unsigned long, class std::allocator<unsigned long> > &", pybind11::return_value_policy::reference_internal);
		cl.def("kmer_reverse_complement_list", (const class std::vector<std::string, class std::allocator<std::string > > & (genesis::sequence::SignatureSpecifications::*)() const) &genesis::sequence::SignatureSpecifications::kmer_reverse_complement_list, "C++: genesis::sequence::SignatureSpecifications::kmer_reverse_complement_list() const --> const class std::vector<std::string, class std::allocator<std::string > > &", pybind11::return_value_policy::reference_internal);
		cl.def("kmer_reverse_complement_list_size", [](genesis::sequence::SignatureSpecifications const &o) -> unsigned long { return o.kmer_reverse_complement_list_size(); }, "");
		cl.def("kmer_reverse_complement_list_size", (unsigned long (genesis::sequence::SignatureSpecifications::*)(bool) const) &genesis::sequence::SignatureSpecifications::kmer_reverse_complement_list_size, "C++: genesis::sequence::SignatureSpecifications::kmer_reverse_complement_list_size(bool) const --> unsigned long", pybind11::arg("with_palindromes"));
		cl.def("unknown_char_behavior", (class genesis::sequence::SignatureSpecifications & (genesis::sequence::SignatureSpecifications::*)(enum genesis::sequence::SignatureSpecifications::UnknownCharBehavior)) &genesis::sequence::SignatureSpecifications::unknown_char_behavior, "C++: genesis::sequence::SignatureSpecifications::unknown_char_behavior(enum genesis::sequence::SignatureSpecifications::UnknownCharBehavior) --> class genesis::sequence::SignatureSpecifications &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
	}
}

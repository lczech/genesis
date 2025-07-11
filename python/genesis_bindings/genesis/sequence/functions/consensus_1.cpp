#include <genesis/sequence/counts.hpp>
#include <genesis/sequence/functions/consensus.hpp>
#include <genesis/sequence/functions/entropy.hpp>
#include <genesis/sequence/sequence.hpp>
#include <genesis/sequence/sequence_dict.hpp>
#include <genesis/sequence/sequence_set.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
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

void bind_genesis_sequence_functions_consensus_1(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::sequence::consensus_sequence_cavener(const class genesis::sequence::SequenceSet &, bool) file:genesis/sequence/functions/consensus.hpp line:319
	M("genesis::sequence").def("consensus_sequence_cavener", [](const class genesis::sequence::SequenceSet & a0) -> std::string { return genesis::sequence::consensus_sequence_cavener(a0); }, "", pybind11::arg("sequences"));
	M("genesis::sequence").def("consensus_sequence_cavener", (std::string (*)(const class genesis::sequence::SequenceSet &, bool)) &genesis::sequence::consensus_sequence_cavener, "Calculate a consensus sequence using the method by Cavener\n for nucleic acid codes `ACGT` and their ambiguities.\n\n See \n\n\n\n\nC++: genesis::sequence::consensus_sequence_cavener(const class genesis::sequence::SequenceSet &, bool) --> std::string", pybind11::arg("sequences"), pybind11::arg("allow_gaps"));

	// genesis::sequence::SiteEntropyOptions file:genesis/sequence/functions/entropy.hpp line:70
	pybind11::enum_<genesis::sequence::SiteEntropyOptions>(M("genesis::sequence"), "SiteEntropyOptions", "Option flags to refine the calculation of site_entropy().\n\n The flags can be combined via the binary or operator `|`:\n\n     auto flags = SiteEntropyOptions::kIncludeGaps | SiteEntropyOptions::kNormalized;\n\n For checking whether a partcular option is set, use the binary and operator `&`:\n\n     if( flags & SiteEntropyOptions::kIncludeGaps ) {\n         // ...\n     }\n\n The option flags can be used with all functions that calculate the entropy. They are applied\n on a per-site basis; i.e., they are used for calculating the site_entropy(), which is the\n basis for higher level entropy functions like absolute_entropy() and averaged_entropy().")
		.value("kDefault", genesis::sequence::SiteEntropyOptions::kDefault)
		.value("kIncludeGaps", genesis::sequence::SiteEntropyOptions::kIncludeGaps)
		.value("kWeighted", genesis::sequence::SiteEntropyOptions::kWeighted)
		.value("kNormalized", genesis::sequence::SiteEntropyOptions::kNormalized);

;

	// genesis::sequence::site_entropy(const class genesis::sequence::SiteCounts &, unsigned long, enum genesis::sequence::SiteEntropyOptions) file:genesis/sequence/functions/entropy.hpp line:211
	M("genesis::sequence").def("site_entropy", [](const class genesis::sequence::SiteCounts & a0, unsigned long const & a1) -> double { return genesis::sequence::site_entropy(a0, a1); }, "", pybind11::arg("counts"), pybind11::arg("site_index"));
	M("genesis::sequence").def("site_entropy", (double (*)(const class genesis::sequence::SiteCounts &, unsigned long, enum genesis::sequence::SiteEntropyOptions)) &genesis::sequence::site_entropy, "Calculate the entropy at one site of a SiteCounts object.\n\n The entropy \n (uncertainty) at site \n (= `site_idx`) is calculated as\n \n\n, where\n \n\n is the relative frequency of character \n at site \n, summed\n over all characters in the SiteCounts object.\n\n The function additionally takes optional flags to refine the calculation, see\n ::SiteEntropyOptions for their explanation.\n\nC++: genesis::sequence::site_entropy(const class genesis::sequence::SiteCounts &, unsigned long, enum genesis::sequence::SiteEntropyOptions) --> double", pybind11::arg("counts"), pybind11::arg("site_index"), pybind11::arg("options"));

	// genesis::sequence::site_information(const class genesis::sequence::SiteCounts &, unsigned long, bool, enum genesis::sequence::SiteEntropyOptions) file:genesis/sequence/functions/entropy.hpp line:236
	M("genesis::sequence").def("site_information", [](const class genesis::sequence::SiteCounts & a0, unsigned long const & a1) -> double { return genesis::sequence::site_information(a0, a1); }, "", pybind11::arg("counts"), pybind11::arg("site_index"));
	M("genesis::sequence").def("site_information", [](const class genesis::sequence::SiteCounts & a0, unsigned long const & a1, bool const & a2) -> double { return genesis::sequence::site_information(a0, a1, a2); }, "", pybind11::arg("counts"), pybind11::arg("site_index"), pybind11::arg("use_small_sample_correction"));
	M("genesis::sequence").def("site_information", (double (*)(const class genesis::sequence::SiteCounts &, unsigned long, bool, enum genesis::sequence::SiteEntropyOptions)) &genesis::sequence::site_information, "Calculate the information content at one site of a SiteCounts object.\n\n The information content \n at site \n (=  is calculated as\n \n\n.\n\n Here, \n is the number of possible characters in the sequences\n (usually, 4 for nucleic acids and 20 for amino acids), which is taken from the\n \n\n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::sequence::site_information(const class genesis::sequence::SiteCounts &, unsigned long, bool, enum genesis::sequence::SiteEntropyOptions) --> double", pybind11::arg("counts"), pybind11::arg("site_index"), pybind11::arg("use_small_sample_correction"), pybind11::arg("options"));

	// genesis::sequence::absolute_entropy(const class genesis::sequence::SiteCounts &, enum genesis::sequence::SiteEntropyOptions) file:genesis/sequence/functions/entropy.hpp line:254
	M("genesis::sequence").def("absolute_entropy", [](const class genesis::sequence::SiteCounts & a0) -> double { return genesis::sequence::absolute_entropy(a0); }, "", pybind11::arg("counts"));
	M("genesis::sequence").def("absolute_entropy", (double (*)(const class genesis::sequence::SiteCounts &, enum genesis::sequence::SiteEntropyOptions)) &genesis::sequence::absolute_entropy, "Return the sum of all site entropies.\n\n This function simply sums up up the site_entropy() for all sites of the SiteCounts object.\n The function additionally takes optional flags to refine the site entropy calculation,\n see ::SiteEntropyOptions for their explanation.\n\nC++: genesis::sequence::absolute_entropy(const class genesis::sequence::SiteCounts &, enum genesis::sequence::SiteEntropyOptions) --> double", pybind11::arg("counts"), pybind11::arg("per_site_options"));

	// genesis::sequence::average_entropy(const class genesis::sequence::SiteCounts &, bool, enum genesis::sequence::SiteEntropyOptions) file:genesis/sequence/functions/entropy.hpp line:276
	M("genesis::sequence").def("average_entropy", [](const class genesis::sequence::SiteCounts & a0) -> double { return genesis::sequence::average_entropy(a0); }, "", pybind11::arg("counts"));
	M("genesis::sequence").def("average_entropy", [](const class genesis::sequence::SiteCounts & a0, bool const & a1) -> double { return genesis::sequence::average_entropy(a0, a1); }, "", pybind11::arg("counts"), pybind11::arg("only_determined_sites"));
	M("genesis::sequence").def("average_entropy", (double (*)(const class genesis::sequence::SiteCounts &, bool, enum genesis::sequence::SiteEntropyOptions)) &genesis::sequence::average_entropy, "Return the average sum of all site entropies.\n\n This function sums up up the site_entropy() for all sites of the SiteCounts object and\n returns the average result per site.\n\n If `only_determined_sites` is `false` (default), the average is calculated using the total\n number of sites, that is, it simply calculates the average entropy per site.\n\n If `only_determined_sites` is `true`, the average is calculated using the number of determined\n sites only; that is, sites that only contain zeroes in all counts are skipped.\n Those sites do not contribute entropy anyway. Thus, it calcuates the average entropy per\n determiend site.\n\n The function additionally takes optional flags to refine the site entropy calculation,\n see ::SiteEntropyOptions for their explanation.\n\nC++: genesis::sequence::average_entropy(const class genesis::sequence::SiteCounts &, bool, enum genesis::sequence::SiteEntropyOptions) --> double", pybind11::arg("counts"), pybind11::arg("only_determined_sites"), pybind11::arg("per_site_options"));

	// genesis::sequence::absolute_information(const class genesis::sequence::SiteCounts &, bool, enum genesis::sequence::SiteEntropyOptions) file:genesis/sequence/functions/entropy.hpp line:291
	M("genesis::sequence").def("absolute_information", [](const class genesis::sequence::SiteCounts & a0) -> double { return genesis::sequence::absolute_information(a0); }, "", pybind11::arg("counts"));
	M("genesis::sequence").def("absolute_information", [](const class genesis::sequence::SiteCounts & a0, bool const & a1) -> double { return genesis::sequence::absolute_information(a0, a1); }, "", pybind11::arg("counts"), pybind11::arg("use_small_sample_correction"));
	M("genesis::sequence").def("absolute_information", (double (*)(const class genesis::sequence::SiteCounts &, bool, enum genesis::sequence::SiteEntropyOptions)) &genesis::sequence::absolute_information, "Calculate the information content across all sites of a SiteCounts object.\n\n See site_information() for details. This function builds the sum of all per-site information\n content values.\n\n The function additionally takes optional flags to refine the site entropy calculation,\n see ::SiteEntropyOptions for their explanation.\n\nC++: genesis::sequence::absolute_information(const class genesis::sequence::SiteCounts &, bool, enum genesis::sequence::SiteEntropyOptions) --> double", pybind11::arg("counts"), pybind11::arg("use_small_sample_correction"), pybind11::arg("per_site_options"));

	// genesis::sequence::average_information(const class genesis::sequence::SiteCounts &, bool, bool, enum genesis::sequence::SiteEntropyOptions) file:genesis/sequence/functions/entropy.hpp line:314
	M("genesis::sequence").def("average_information", [](const class genesis::sequence::SiteCounts & a0) -> double { return genesis::sequence::average_information(a0); }, "", pybind11::arg("counts"));
	M("genesis::sequence").def("average_information", [](const class genesis::sequence::SiteCounts & a0, bool const & a1) -> double { return genesis::sequence::average_information(a0, a1); }, "", pybind11::arg("counts"), pybind11::arg("only_determined_sites"));
	M("genesis::sequence").def("average_information", [](const class genesis::sequence::SiteCounts & a0, bool const & a1, bool const & a2) -> double { return genesis::sequence::average_information(a0, a1, a2); }, "", pybind11::arg("counts"), pybind11::arg("only_determined_sites"), pybind11::arg("use_small_sample_correction"));
	M("genesis::sequence").def("average_information", (double (*)(const class genesis::sequence::SiteCounts &, bool, bool, enum genesis::sequence::SiteEntropyOptions)) &genesis::sequence::average_information, "Calculate the information content across all sites of a SiteCounts object.\n\n See site_information() for details. This function builds the sum of all per-site information\n content values.\n\n If `only_determined_sites` is `false` (default), the average is calculated using the total\n number of sites, that is, it simply calculates the average entropy per site.\n\n If `only_determined_sites` is `true`, the average is calculated using the number of determined\n sites only; that is, sites that only contain zeroes in all counts are skipped.\n Those sites do not contribute entropy anyway. Thus, it calcuates the average entropy per\n determiend site.\n\n The function additionally takes optional flags to refine the site entropy calculation,\n see ::SiteEntropyOptions for their explanation.\n\nC++: genesis::sequence::average_information(const class genesis::sequence::SiteCounts &, bool, bool, enum genesis::sequence::SiteEntropyOptions) --> double", pybind11::arg("counts"), pybind11::arg("only_determined_sites"), pybind11::arg("use_small_sample_correction"), pybind11::arg("per_site_options"));

	{ // genesis::sequence::SequenceDict file:genesis/sequence/sequence_dict.hpp line:63
		pybind11::class_<genesis::sequence::SequenceDict, std::shared_ptr<genesis::sequence::SequenceDict>> cl(M("genesis::sequence"), "SequenceDict", "Store dictionary/index data on sequence files, such as coming from `.fai` or `.dict` files.\n\n See DictReader and FaiReader for the input format readers. Furthermore, FastaReader also offers\n a mode to read a `fasta` file, and just retaining the dict/index information.\n Lastly, see sequence_set_to_dict() for a function to get this information from a given\n SequenceSet.\n\n Currently, we only store the sequence names and their lengths, in the order as provided in the\n input file. We might add further information such as offset in the fasta file in the future,\n once we offer to read with jumps in fasta files.\n\n \n ReferenceGenome");
		cl.def( pybind11::init( [](){ return new genesis::sequence::SequenceDict(); } ) );
		cl.def( pybind11::init( [](genesis::sequence::SequenceDict const &o){ return new genesis::sequence::SequenceDict(o); } ) );
		cl.def("assign", (class genesis::sequence::SequenceDict & (genesis::sequence::SequenceDict::*)(const class genesis::sequence::SequenceDict &)) &genesis::sequence::SequenceDict::operator=, "C++: genesis::sequence::SequenceDict::operator=(const class genesis::sequence::SequenceDict &) --> class genesis::sequence::SequenceDict &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("add", [](genesis::sequence::SequenceDict &o, const class genesis::sequence::Sequence & a0) -> void { return o.add(a0); }, "", pybind11::arg("sequence"));
		cl.def("add", (void (genesis::sequence::SequenceDict::*)(const class genesis::sequence::Sequence &, bool)) &genesis::sequence::SequenceDict::add, "Add a Sequence to the dictionary.\n\n  add( Entry&&, bool )\n\nC++: genesis::sequence::SequenceDict::add(const class genesis::sequence::Sequence &, bool) --> void", pybind11::arg("sequence"), pybind11::arg("also_look_up_first_word"));
		cl.def("add", [](genesis::sequence::SequenceDict &o, const std::string & a0, unsigned long const & a1) -> void { return o.add(a0, a1); }, "", pybind11::arg("name"), pybind11::arg("length"));
		cl.def("add", (void (genesis::sequence::SequenceDict::*)(const std::string &, unsigned long, bool)) &genesis::sequence::SequenceDict::add, "Add an entry to the dictionary, given its name and length.\n\n  add( Entry&&, bool )\n\nC++: genesis::sequence::SequenceDict::add(const std::string &, unsigned long, bool) --> void", pybind11::arg("name"), pybind11::arg("length"), pybind11::arg("also_look_up_first_word"));
		cl.def("add", [](genesis::sequence::SequenceDict &o, const struct genesis::sequence::SequenceDict::Entry & a0) -> void { return o.add(a0); }, "", pybind11::arg("entry"));
		cl.def("add", (void (genesis::sequence::SequenceDict::*)(const struct genesis::sequence::SequenceDict::Entry &, bool)) &genesis::sequence::SequenceDict::add, "Add an entry to the dictionary.\n\n  add( Entry&&, bool )\n\nC++: genesis::sequence::SequenceDict::add(const struct genesis::sequence::SequenceDict::Entry &, bool) --> void", pybind11::arg("entry"), pybind11::arg("also_look_up_first_word"));
		cl.def("clear", (void (genesis::sequence::SequenceDict::*)()) &genesis::sequence::SequenceDict::clear, "C++: genesis::sequence::SequenceDict::clear() --> void");
		cl.def("size", (unsigned long (genesis::sequence::SequenceDict::*)() const) &genesis::sequence::SequenceDict::size, "C++: genesis::sequence::SequenceDict::size() const --> unsigned long");
		cl.def("__getitem__", (const struct genesis::sequence::SequenceDict::Entry & (genesis::sequence::SequenceDict::*)(unsigned long) const) &genesis::sequence::SequenceDict::operator[], "C++: genesis::sequence::SequenceDict::operator[](unsigned long) const --> const struct genesis::sequence::SequenceDict::Entry &", pybind11::return_value_policy::reference_internal, pybind11::arg("index"));
		cl.def("at", (const struct genesis::sequence::SequenceDict::Entry & (genesis::sequence::SequenceDict::*)(unsigned long) const) &genesis::sequence::SequenceDict::at, "C++: genesis::sequence::SequenceDict::at(unsigned long) const --> const struct genesis::sequence::SequenceDict::Entry &", pybind11::return_value_policy::reference_internal, pybind11::arg("index"));
		cl.def("get", (const struct genesis::sequence::SequenceDict::Entry & (genesis::sequence::SequenceDict::*)(const std::string &) const) &genesis::sequence::SequenceDict::get, "C++: genesis::sequence::SequenceDict::get(const std::string &) const --> const struct genesis::sequence::SequenceDict::Entry &", pybind11::return_value_policy::reference_internal, pybind11::arg("name"));
		cl.def("index_of", (unsigned long (genesis::sequence::SequenceDict::*)(const std::string &) const) &genesis::sequence::SequenceDict::index_of, "C++: genesis::sequence::SequenceDict::index_of(const std::string &) const --> unsigned long", pybind11::arg("name"));
		cl.def("contains", (bool (genesis::sequence::SequenceDict::*)(const std::string &) const) &genesis::sequence::SequenceDict::contains, "C++: genesis::sequence::SequenceDict::contains(const std::string &) const --> bool", pybind11::arg("name"));

		extend_sequence_dict(cl);

		{ // genesis::sequence::SequenceDict::Entry file:genesis/sequence/sequence_dict.hpp line:71
			auto & enclosing_class = cl;
			pybind11::class_<genesis::sequence::SequenceDict::Entry, std::shared_ptr<genesis::sequence::SequenceDict::Entry>> cl(enclosing_class, "Entry", "");
			cl.def( pybind11::init( [](genesis::sequence::SequenceDict::Entry const &o){ return new genesis::sequence::SequenceDict::Entry(o); } ) );
			cl.def( pybind11::init( [](){ return new genesis::sequence::SequenceDict::Entry(); } ) );
			cl.def_readwrite("name", &genesis::sequence::SequenceDict::Entry::name);
			cl.def_readwrite("length", &genesis::sequence::SequenceDict::Entry::length);
			cl.def("assign", (struct genesis::sequence::SequenceDict::Entry & (genesis::sequence::SequenceDict::Entry::*)(const struct genesis::sequence::SequenceDict::Entry &)) &genesis::sequence::SequenceDict::Entry::operator=, "C++: genesis::sequence::SequenceDict::Entry::operator=(const struct genesis::sequence::SequenceDict::Entry &) --> struct genesis::sequence::SequenceDict::Entry &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		}

	}
}

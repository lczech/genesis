#include <genesis/sequence/counts.hpp>
#include <genesis/sequence/functions/functions.hpp>
#include <genesis/sequence/sequence.hpp>
#include <genesis/sequence/sequence_set.hpp>
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

void bind_genesis_sequence_counts(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::sequence::SiteCounts file:genesis/sequence/counts.hpp line:85
		pybind11::class_<genesis::sequence::SiteCounts, std::shared_ptr<genesis::sequence::SiteCounts>> cl(M("genesis::sequence"), "SiteCounts", "Store counts of the occurence for certain characters at the sites of Sequence%s.\n\n This class is a helper for statistical analyses of Sequences, and for calculating consensus\n sequences and the like. It stores a \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
		cl.def( pybind11::init( [](){ return new genesis::sequence::SiteCounts(); } ) );
		cl.def( pybind11::init<const std::string &, unsigned long>(), pybind11::arg("characters"), pybind11::arg("length") );

		cl.def( pybind11::init( [](genesis::sequence::SiteCounts const &o){ return new genesis::sequence::SiteCounts(o); } ) );
		cl.def("assign", (class genesis::sequence::SiteCounts & (genesis::sequence::SiteCounts::*)(const class genesis::sequence::SiteCounts &)) &genesis::sequence::SiteCounts::operator=, "Default copy assignment.\n\nC++: genesis::sequence::SiteCounts::operator=(const class genesis::sequence::SiteCounts &) --> class genesis::sequence::SiteCounts &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("length", (unsigned long (genesis::sequence::SiteCounts::*)() const) &genesis::sequence::SiteCounts::length, "Return the number of sites used for counting.\n\n This has to match the Sequence::length() property of the Sequence%s to be added for counting.\n\nC++: genesis::sequence::SiteCounts::length() const --> unsigned long");
		cl.def("characters", (std::string (genesis::sequence::SiteCounts::*)() const) &genesis::sequence::SiteCounts::characters, "Return the character set that is used for counting.\n\n This function returns the upper case letters of the internal list of characters that is used\n for counting, in the order that is also used by the count_at() function.\n\nC++: genesis::sequence::SiteCounts::characters() const --> std::string");
		cl.def("added_sequences_count", (unsigned int (genesis::sequence::SiteCounts::*)() const) &genesis::sequence::SiteCounts::added_sequences_count, "Return the number of processed Sequence%s, i.e., how many Sequences were added in\n total.\n\nC++: genesis::sequence::SiteCounts::added_sequences_count() const --> unsigned int");
		cl.def("count_of", (unsigned int (genesis::sequence::SiteCounts::*)(char, unsigned long) const) &genesis::sequence::SiteCounts::count_of, "Return the count of a specific character at a given site.\n\n If the charater is not part of the set of used characters, the function throws an exception.\n This function is case-independent. See characters() to retrieve the set of characters.\n\nC++: genesis::sequence::SiteCounts::count_of(char, unsigned long) const --> unsigned int", pybind11::arg("character"), pybind11::arg("site_index"));
		cl.def("count_at", (unsigned int (genesis::sequence::SiteCounts::*)(unsigned long, unsigned long) const) &genesis::sequence::SiteCounts::count_at, "Return the count for a character and a site, given their indices.\n\n The characters are indexed in the order given by characters(). This function is thus\n mainly for speedup reasons when iterating the whole \n\n\n\nC++: genesis::sequence::SiteCounts::count_at(unsigned long, unsigned long) const --> unsigned int", pybind11::arg("character_index"), pybind11::arg("site_index"));
		cl.def("add_sequence", [](genesis::sequence::SiteCounts &o, const class genesis::sequence::Sequence & a0) -> void { return o.add_sequence(a0); }, "", pybind11::arg("sequence"));
		cl.def("add_sequence", (void (genesis::sequence::SiteCounts::*)(const class genesis::sequence::Sequence &, bool)) &genesis::sequence::SiteCounts::add_sequence, "Process a single Sequence and add its counts to the existing ones.\n\n If  is `true` (default), the abundance of the Sequence is used as weight\n for the counting. Otherwise, a weight of `1` is used.\n\nC++: genesis::sequence::SiteCounts::add_sequence(const class genesis::sequence::Sequence &, bool) --> void", pybind11::arg("sequence"), pybind11::arg("use_abundance"));
		cl.def("add_sequence", [](genesis::sequence::SiteCounts &o, const std::string & a0) -> void { return o.add_sequence(a0); }, "", pybind11::arg("sites"));
		cl.def("add_sequence", (void (genesis::sequence::SiteCounts::*)(const std::string &, unsigned int)) &genesis::sequence::SiteCounts::add_sequence, "Process a single sequence in string form and add its counts to the existing ones.\n\n Using  the sequence can be weighted. That is, the count is increased by the weight.\n\nC++: genesis::sequence::SiteCounts::add_sequence(const std::string &, unsigned int) --> void", pybind11::arg("sites"), pybind11::arg("weight"));
		cl.def("add_sequences", [](genesis::sequence::SiteCounts &o, const class genesis::sequence::SequenceSet & a0) -> void { return o.add_sequences(a0); }, "", pybind11::arg("sequences"));
		cl.def("add_sequences", (void (genesis::sequence::SiteCounts::*)(const class genesis::sequence::SequenceSet &, bool)) &genesis::sequence::SiteCounts::add_sequences, "Process a SequenceSet and add its counts to the existing ones for all contained\n Sequence%s.\n\n If  is `true` (default), the abundances of the Sequences are used as weights\n for the counting. Otherwise, a weight of `1` is used.\n\nC++: genesis::sequence::SiteCounts::add_sequences(const class genesis::sequence::SequenceSet &, bool) --> void", pybind11::arg("sequences"), pybind11::arg("use_abundances"));
		cl.def("clear", (void (genesis::sequence::SiteCounts::*)()) &genesis::sequence::SiteCounts::clear, "Clear the object, that is, delete everything.\n\n This function sets the object status to the same that the\n \n\n\n\n\n\n\n\nC++: genesis::sequence::SiteCounts::clear() --> void");
		cl.def("clear_counts", (void (genesis::sequence::SiteCounts::*)()) &genesis::sequence::SiteCounts::clear_counts, "Reset all counts to 0.\n\n This clears the counts so that the object is as if newly created, while keeping the\n counted characters and length of the count matrix. It also clears the count for\n added_sequences_count().\n\nC++: genesis::sequence::SiteCounts::clear_counts() --> void");
	}
	{ // genesis::sequence::Sequence file:genesis/sequence/sequence.hpp line:40
		pybind11::class_<genesis::sequence::Sequence, std::shared_ptr<genesis::sequence::Sequence>> cl(M("genesis::sequence"), "Sequence", "");
		cl.def( pybind11::init( [](){ return new genesis::sequence::Sequence(); } ) );
		cl.def( pybind11::init( [](const std::string & a0, const std::string & a1){ return new genesis::sequence::Sequence(a0, a1); } ), "doc" , pybind11::arg("label"), pybind11::arg("sites"));
		cl.def( pybind11::init<const std::string &, const std::string &, unsigned long>(), pybind11::arg("label"), pybind11::arg("sites"), pybind11::arg("abundance") );

		cl.def( pybind11::init( [](genesis::sequence::Sequence const &o){ return new genesis::sequence::Sequence(o); } ) );
		cl.def("assign", (class genesis::sequence::Sequence & (genesis::sequence::Sequence::*)(const class genesis::sequence::Sequence &)) &genesis::sequence::Sequence::operator=, "C++: genesis::sequence::Sequence::operator=(const class genesis::sequence::Sequence &) --> class genesis::sequence::Sequence &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("swap", (void (genesis::sequence::Sequence::*)(class genesis::sequence::Sequence &)) &genesis::sequence::Sequence::swap, "C++: genesis::sequence::Sequence::swap(class genesis::sequence::Sequence &) --> void", pybind11::arg("other"));
		cl.def("clear", (void (genesis::sequence::Sequence::*)()) &genesis::sequence::Sequence::clear, "C++: genesis::sequence::Sequence::clear() --> void");
		cl.def("label", (std::string & (genesis::sequence::Sequence::*)()) &genesis::sequence::Sequence::label, "C++: genesis::sequence::Sequence::label() --> std::string &", pybind11::return_value_policy::reference_internal);
		cl.def("label", (void (genesis::sequence::Sequence::*)(const std::string &)) &genesis::sequence::Sequence::label, "C++: genesis::sequence::Sequence::label(const std::string &) --> void", pybind11::arg("value"));
		cl.def("sites", (std::string & (genesis::sequence::Sequence::*)()) &genesis::sequence::Sequence::sites, "C++: genesis::sequence::Sequence::sites() --> std::string &", pybind11::return_value_policy::reference_internal);
		cl.def("sites", (void (genesis::sequence::Sequence::*)(const std::string &)) &genesis::sequence::Sequence::sites, "C++: genesis::sequence::Sequence::sites(const std::string &) --> void", pybind11::arg("value"));
		cl.def("phred_scores", (class std::vector<unsigned char, class std::allocator<unsigned char> > & (genesis::sequence::Sequence::*)()) &genesis::sequence::Sequence::phred_scores, "C++: genesis::sequence::Sequence::phred_scores() --> class std::vector<unsigned char, class std::allocator<unsigned char> > &", pybind11::return_value_policy::reference_internal);
		cl.def("phred_scores", (void (genesis::sequence::Sequence::*)(const class std::vector<unsigned char, class std::allocator<unsigned char> > &)) &genesis::sequence::Sequence::phred_scores, "C++: genesis::sequence::Sequence::phred_scores(const class std::vector<unsigned char, class std::allocator<unsigned char> > &) --> void", pybind11::arg("value"));
		cl.def("abundance", (unsigned long (genesis::sequence::Sequence::*)() const) &genesis::sequence::Sequence::abundance, "C++: genesis::sequence::Sequence::abundance() const --> unsigned long");
		cl.def("abundance", (void (genesis::sequence::Sequence::*)(unsigned long)) &genesis::sequence::Sequence::abundance, "C++: genesis::sequence::Sequence::abundance(unsigned long) --> void", pybind11::arg("value"));
		cl.def("length", (unsigned long (genesis::sequence::Sequence::*)() const) &genesis::sequence::Sequence::length, "Return the length (number of sites) of this sequence.\n\nC++: genesis::sequence::Sequence::length() const --> unsigned long");
		cl.def("size", (unsigned long (genesis::sequence::Sequence::*)() const) &genesis::sequence::Sequence::size, "Alias for length().\n\nC++: genesis::sequence::Sequence::size() const --> unsigned long");
		cl.def("site_at", (char & (genesis::sequence::Sequence::*)(unsigned long)) &genesis::sequence::Sequence::site_at, "C++: genesis::sequence::Sequence::site_at(unsigned long) --> char &", pybind11::return_value_policy::reference_internal, pybind11::arg("index"));
		cl.def("__getitem__", (char & (genesis::sequence::Sequence::*)(unsigned long)) &genesis::sequence::Sequence::operator[], "C++: genesis::sequence::Sequence::operator[](unsigned long) --> char &", pybind11::return_value_policy::reference_internal, pybind11::arg("index"));
		cl.def("begin", (class __gnu_cxx::__normal_iterator<char *, std::string > (genesis::sequence::Sequence::*)()) &genesis::sequence::Sequence::begin, "C++: genesis::sequence::Sequence::begin() --> class __gnu_cxx::__normal_iterator<char *, std::string >");
		cl.def("end", (class __gnu_cxx::__normal_iterator<char *, std::string > (genesis::sequence::Sequence::*)()) &genesis::sequence::Sequence::end, "C++: genesis::sequence::Sequence::end() --> class __gnu_cxx::__normal_iterator<char *, std::string >");
		cl.def("cbegin", (class __gnu_cxx::__normal_iterator<const char *, std::string > (genesis::sequence::Sequence::*)() const) &genesis::sequence::Sequence::cbegin, "C++: genesis::sequence::Sequence::cbegin() const --> class __gnu_cxx::__normal_iterator<const char *, std::string >");
		cl.def("cend", (class __gnu_cxx::__normal_iterator<const char *, std::string > (genesis::sequence::Sequence::*)() const) &genesis::sequence::Sequence::cend, "C++: genesis::sequence::Sequence::cend() const --> class __gnu_cxx::__normal_iterator<const char *, std::string >");

		cl.def("__str__", [](genesis::sequence::Sequence const &o) -> std::string { std::ostringstream s; genesis::sequence::operator<<(s, o); return s.str(); } );
	}
	{ // genesis::sequence::SequenceSet file:genesis/sequence/sequence_set.hpp line:53
		pybind11::class_<genesis::sequence::SequenceSet, std::shared_ptr<genesis::sequence::SequenceSet>> cl(M("genesis::sequence"), "SequenceSet", "Store a set of Sequence%s.\n\n A SequenceSet is a simple container that allows to add(), remove() and iterate over Sequence%s.\n Sequences are kept in the order in which they were added and can be accessed via an index, see\n at().");
		cl.def( pybind11::init( [](){ return new genesis::sequence::SequenceSet(); } ) );
		cl.def( pybind11::init( [](genesis::sequence::SequenceSet const &o){ return new genesis::sequence::SequenceSet(o); } ) );
		cl.def("assign", (class genesis::sequence::SequenceSet & (genesis::sequence::SequenceSet::*)(const class genesis::sequence::SequenceSet &)) &genesis::sequence::SequenceSet::operator=, "C++: genesis::sequence::SequenceSet::operator=(const class genesis::sequence::SequenceSet &) --> class genesis::sequence::SequenceSet &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("size", (unsigned long (genesis::sequence::SequenceSet::*)() const) &genesis::sequence::SequenceSet::size, "Return the number of Sequence%s in the SequenceSet.\n\nC++: genesis::sequence::SequenceSet::size() const --> unsigned long");
		cl.def("empty", (bool (genesis::sequence::SequenceSet::*)() const) &genesis::sequence::SequenceSet::empty, "Return whether the SequenceSet is empty, i.e. whether its size() is 0.\n\nC++: genesis::sequence::SequenceSet::empty() const --> bool");
		cl.def("at", (class genesis::sequence::Sequence & (genesis::sequence::SequenceSet::*)(unsigned long)) &genesis::sequence::SequenceSet::at, "C++: genesis::sequence::SequenceSet::at(unsigned long) --> class genesis::sequence::Sequence &", pybind11::return_value_policy::reference_internal, pybind11::arg("index"));
		cl.def("__getitem__", (class genesis::sequence::Sequence & (genesis::sequence::SequenceSet::*)(unsigned long)) &genesis::sequence::SequenceSet::operator[], "C++: genesis::sequence::SequenceSet::operator[](unsigned long) --> class genesis::sequence::Sequence &", pybind11::return_value_policy::reference_internal, pybind11::arg("index"));
		cl.def("add", (class genesis::sequence::Sequence & (genesis::sequence::SequenceSet::*)(const class genesis::sequence::Sequence &)) &genesis::sequence::SequenceSet::add, "Add a Sequence to the SequenceSet by copying it, and return a reference to it.\n\nC++: genesis::sequence::SequenceSet::add(const class genesis::sequence::Sequence &) --> class genesis::sequence::Sequence &", pybind11::return_value_policy::reference_internal, pybind11::arg("s"));
		cl.def("remove", (void (genesis::sequence::SequenceSet::*)(unsigned long)) &genesis::sequence::SequenceSet::remove, "Remove the Sequence at a given `index` from the SequenceSet.\n\nC++: genesis::sequence::SequenceSet::remove(unsigned long) --> void", pybind11::arg("index"));
		cl.def("remove", (void (genesis::sequence::SequenceSet::*)(unsigned long, unsigned long)) &genesis::sequence::SequenceSet::remove, "Remove the Sequence%s between the `first_index` (inclusive) and the `last_index`\n (exclusive) from the SequenceSet.\n\nC++: genesis::sequence::SequenceSet::remove(unsigned long, unsigned long) --> void", pybind11::arg("first_index"), pybind11::arg("last_index"));
		cl.def("remove", (void (genesis::sequence::SequenceSet::*)(class __gnu_cxx::__normal_iterator<class genesis::sequence::Sequence *, class std::vector<class genesis::sequence::Sequence, class std::allocator<class genesis::sequence::Sequence> > >)) &genesis::sequence::SequenceSet::remove, "Remove the Sequence at a given iterator `position` from the SequenceSet.\n\nC++: genesis::sequence::SequenceSet::remove(class __gnu_cxx::__normal_iterator<class genesis::sequence::Sequence *, class std::vector<class genesis::sequence::Sequence, class std::allocator<class genesis::sequence::Sequence> > >) --> void", pybind11::arg("position"));
		cl.def("remove", (void (genesis::sequence::SequenceSet::*)(class __gnu_cxx::__normal_iterator<class genesis::sequence::Sequence *, class std::vector<class genesis::sequence::Sequence, class std::allocator<class genesis::sequence::Sequence> > >, class __gnu_cxx::__normal_iterator<class genesis::sequence::Sequence *, class std::vector<class genesis::sequence::Sequence, class std::allocator<class genesis::sequence::Sequence> > >)) &genesis::sequence::SequenceSet::remove, "Remove the Sequence%s between the `first` (inclusive) and the `last`\n (exclusive) iterator position from the SequenceSet.\n\nC++: genesis::sequence::SequenceSet::remove(class __gnu_cxx::__normal_iterator<class genesis::sequence::Sequence *, class std::vector<class genesis::sequence::Sequence, class std::allocator<class genesis::sequence::Sequence> > >, class __gnu_cxx::__normal_iterator<class genesis::sequence::Sequence *, class std::vector<class genesis::sequence::Sequence, class std::allocator<class genesis::sequence::Sequence> > >) --> void", pybind11::arg("first"), pybind11::arg("last"));
		cl.def("clear", (void (genesis::sequence::SequenceSet::*)()) &genesis::sequence::SequenceSet::clear, "Remove all Sequence%s from the SequenceSet, leaving it with a size() of 0.\n\nC++: genesis::sequence::SequenceSet::clear() --> void");
		cl.def("begin", (class __gnu_cxx::__normal_iterator<class genesis::sequence::Sequence *, class std::vector<class genesis::sequence::Sequence, class std::allocator<class genesis::sequence::Sequence> > > (genesis::sequence::SequenceSet::*)()) &genesis::sequence::SequenceSet::begin, "C++: genesis::sequence::SequenceSet::begin() --> class __gnu_cxx::__normal_iterator<class genesis::sequence::Sequence *, class std::vector<class genesis::sequence::Sequence, class std::allocator<class genesis::sequence::Sequence> > >");
		cl.def("end", (class __gnu_cxx::__normal_iterator<class genesis::sequence::Sequence *, class std::vector<class genesis::sequence::Sequence, class std::allocator<class genesis::sequence::Sequence> > > (genesis::sequence::SequenceSet::*)()) &genesis::sequence::SequenceSet::end, "C++: genesis::sequence::SequenceSet::end() --> class __gnu_cxx::__normal_iterator<class genesis::sequence::Sequence *, class std::vector<class genesis::sequence::Sequence, class std::allocator<class genesis::sequence::Sequence> > >");
		cl.def("cbegin", (class __gnu_cxx::__normal_iterator<const class genesis::sequence::Sequence *, class std::vector<class genesis::sequence::Sequence, class std::allocator<class genesis::sequence::Sequence> > > (genesis::sequence::SequenceSet::*)() const) &genesis::sequence::SequenceSet::cbegin, "C++: genesis::sequence::SequenceSet::cbegin() const --> class __gnu_cxx::__normal_iterator<const class genesis::sequence::Sequence *, class std::vector<class genesis::sequence::Sequence, class std::allocator<class genesis::sequence::Sequence> > >");
		cl.def("cend", (class __gnu_cxx::__normal_iterator<const class genesis::sequence::Sequence *, class std::vector<class genesis::sequence::Sequence, class std::allocator<class genesis::sequence::Sequence> > > (genesis::sequence::SequenceSet::*)() const) &genesis::sequence::SequenceSet::cend, "C++: genesis::sequence::SequenceSet::cend() const --> class __gnu_cxx::__normal_iterator<const class genesis::sequence::Sequence *, class std::vector<class genesis::sequence::Sequence, class std::allocator<class genesis::sequence::Sequence> > >");

		cl.def("__str__", [](genesis::sequence::SequenceSet const &o) -> std::string { std::ostringstream s; genesis::sequence::operator<<(s, o); return s.str(); } );

		extend_sequence_set(cl);
	}
}

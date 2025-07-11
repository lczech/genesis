#include <functional>
#include <genesis/placement/function/functions.hpp>
#include <genesis/placement/function/operators.hpp>
#include <genesis/placement/placement_tree.hpp>
#include <genesis/placement/pquery.hpp>
#include <genesis/placement/pquery/name.hpp>
#include <genesis/placement/pquery/placement.hpp>
#include <genesis/placement/sample.hpp>
#include <genesis/placement/sample_set.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
#include <genesis/tree/tree.hpp>
#include <genesis/tree/tree/edge.hpp>
#include <genesis/tree/tree/edge_data.hpp>
#include <genesis/tree/tree/link.hpp>
#include <genesis/tree/tree/node.hpp>
#include <genesis/utils/containers/deref_iterator.hpp>
#include <genesis/utils/containers/range.hpp>
#include <ios>
#include <iterator>
#include <locale>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
#include <string>
#include <unordered_set>
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

void bind_genesis_placement_sample(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::placement::Sample file:genesis/placement/sample.hpp line:68
		pybind11::class_<genesis::placement::Sample, std::shared_ptr<genesis::placement::Sample>> cl(M("genesis::placement"), "Sample", "Manage a set of \n\n\n\n\n\n\n\n\n\n\n\n\n\n");
		cl.def( pybind11::init( [](){ return new genesis::placement::Sample(); } ) );
		cl.def( pybind11::init<const class genesis::tree::Tree &>(), pybind11::arg("tree") );

		cl.def( pybind11::init( [](genesis::placement::Sample const &o){ return new genesis::placement::Sample(o); } ) );
		cl.def_readwrite("metadata", &genesis::placement::Sample::metadata);
		cl.def("assign", (class genesis::placement::Sample & (genesis::placement::Sample::*)(const class genesis::placement::Sample &)) &genesis::placement::Sample::operator=, "Copy assignment.\n\nC++: genesis::placement::Sample::operator=(const class genesis::placement::Sample &) --> class genesis::placement::Sample &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("swap", (void (genesis::placement::Sample::*)(class genesis::placement::Sample &)) &genesis::placement::Sample::swap, "Swap the contents of this Sample with the contents of another Sample.\n\nC++: genesis::placement::Sample::swap(class genesis::placement::Sample &) --> void", pybind11::arg("other"));
		cl.def("clear", (void (genesis::placement::Sample::*)()) &genesis::placement::Sample::clear, "Clears all data of this object.\n\n The \n\n\nC++: genesis::placement::Sample::clear() --> void");
		cl.def("tree", (class genesis::tree::Tree & (genesis::placement::Sample::*)()) &genesis::placement::Sample::tree, "Get the PlacementTree of this Sample.\n\nC++: genesis::placement::Sample::tree() --> class genesis::tree::Tree &", pybind11::return_value_policy::reference_internal);
		cl.def("size", (unsigned long (genesis::placement::Sample::*)() const) &genesis::placement::Sample::size, "Return the number of \n\n\nC++: genesis::placement::Sample::size() const --> unsigned long");
		cl.def("empty", (bool (genesis::placement::Sample::*)() const) &genesis::placement::Sample::empty, "Return whether the tree or the set of \n\n\nC++: genesis::placement::Sample::empty() const --> bool");
		cl.def("add", (class genesis::placement::Pquery & (genesis::placement::Sample::*)()) &genesis::placement::Sample::add, "Create an empty Pquery, add it to the Sample and return it.\n\n The returned reference can then be used to add PqueryPlacement%s and PqueryName%s to the Pquery.\n\n As this function might reallocate the memory where Pqueries are stored, all iterators\n and pointers to the Pqueries of this Sample are invalidated.\n\nC++: genesis::placement::Sample::add() --> class genesis::placement::Pquery &", pybind11::return_value_policy::reference_internal);
		cl.def("add", (class genesis::placement::Pquery & (genesis::placement::Sample::*)(const class genesis::placement::Pquery &)) &genesis::placement::Sample::add, "Create a Pquery as a copy of the provided one, add it to the sample and return it.\n\n It is possible to provide a Pquery from a different Sample, as long as the tree topology of both\n Sample%s is identical, including identical \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::placement::Sample::add(const class genesis::placement::Pquery &) --> class genesis::placement::Pquery &", pybind11::return_value_policy::reference_internal, pybind11::arg("other"));
		cl.def("at", (class genesis::placement::Pquery & (genesis::placement::Sample::*)(unsigned long)) &genesis::placement::Sample::at, "Return the Pquery at a certain index.\n\nC++: genesis::placement::Sample::at(unsigned long) --> class genesis::placement::Pquery &", pybind11::return_value_policy::reference_internal, pybind11::arg("index"));
		cl.def("remove", (void (genesis::placement::Sample::*)(unsigned long)) &genesis::placement::Sample::remove, "Remove the Pquery at a given `index` from the Sample.\n\n As this function might reallocate the memory where Pqueries are stored, all iterators\n and pointer to the Pqueries of this Sample are invalidated.\n\nC++: genesis::placement::Sample::remove(unsigned long) --> void", pybind11::arg("index"));
		cl.def("remove", (void (genesis::placement::Sample::*)(unsigned long, unsigned long)) &genesis::placement::Sample::remove, "Remove the \n\n\n\nC++: genesis::placement::Sample::remove(unsigned long, unsigned long) --> void", pybind11::arg("first_index"), pybind11::arg("last_index"));
		cl.def("remove", (void (genesis::placement::Sample::*)(class __gnu_cxx::__normal_iterator<class genesis::placement::Pquery *, class std::vector<class genesis::placement::Pquery, class std::allocator<class genesis::placement::Pquery> > >)) &genesis::placement::Sample::remove, "Remove the Pquery at a given iterator `position` from the Sample.\n\nC++: genesis::placement::Sample::remove(class __gnu_cxx::__normal_iterator<class genesis::placement::Pquery *, class std::vector<class genesis::placement::Pquery, class std::allocator<class genesis::placement::Pquery> > >) --> void", pybind11::arg("position"));
		cl.def("remove", (void (genesis::placement::Sample::*)(class __gnu_cxx::__normal_iterator<class genesis::placement::Pquery *, class std::vector<class genesis::placement::Pquery, class std::allocator<class genesis::placement::Pquery> > >, class __gnu_cxx::__normal_iterator<class genesis::placement::Pquery *, class std::vector<class genesis::placement::Pquery, class std::allocator<class genesis::placement::Pquery> > >)) &genesis::placement::Sample::remove, "Remove the \n\n\n\nC++: genesis::placement::Sample::remove(class __gnu_cxx::__normal_iterator<class genesis::placement::Pquery *, class std::vector<class genesis::placement::Pquery, class std::allocator<class genesis::placement::Pquery> > >, class __gnu_cxx::__normal_iterator<class genesis::placement::Pquery *, class std::vector<class genesis::placement::Pquery, class std::allocator<class genesis::placement::Pquery> > >) --> void", pybind11::arg("first"), pybind11::arg("last"));
		cl.def("clear_pqueries", (void (genesis::placement::Sample::*)()) &genesis::placement::Sample::clear_pqueries, "Clear all \n\n\n\n\n\n\nC++: genesis::placement::Sample::clear_pqueries() --> void");
		cl.def("begin", (class __gnu_cxx::__normal_iterator<class genesis::placement::Pquery *, class std::vector<class genesis::placement::Pquery, class std::allocator<class genesis::placement::Pquery> > > (genesis::placement::Sample::*)()) &genesis::placement::Sample::begin, "Return an iterator to the beginning of the \n\n\nC++: genesis::placement::Sample::begin() --> class __gnu_cxx::__normal_iterator<class genesis::placement::Pquery *, class std::vector<class genesis::placement::Pquery, class std::allocator<class genesis::placement::Pquery> > >");
		cl.def("end", (class __gnu_cxx::__normal_iterator<class genesis::placement::Pquery *, class std::vector<class genesis::placement::Pquery, class std::allocator<class genesis::placement::Pquery> > > (genesis::placement::Sample::*)()) &genesis::placement::Sample::end, "Return an iterator to the end of the \n\n\nC++: genesis::placement::Sample::end() --> class __gnu_cxx::__normal_iterator<class genesis::placement::Pquery *, class std::vector<class genesis::placement::Pquery, class std::allocator<class genesis::placement::Pquery> > >");
		cl.def("pqueries", (class genesis::utils::Range<class __gnu_cxx::__normal_iterator<class genesis::placement::Pquery *, class std::vector<class genesis::placement::Pquery, class std::allocator<class genesis::placement::Pquery> > > > (genesis::placement::Sample::*)()) &genesis::placement::Sample::pqueries, "Return a Range iterator to the \n\n\n\n\nC++: genesis::placement::Sample::pqueries() --> class genesis::utils::Range<class __gnu_cxx::__normal_iterator<class genesis::placement::Pquery *, class std::vector<class genesis::placement::Pquery, class std::allocator<class genesis::placement::Pquery> > > >");

		cl.def("__str__", [](genesis::placement::Sample const &o) -> std::string { std::ostringstream s; genesis::placement::operator<<(s, o); return s.str(); } );
	}
	{ // genesis::placement::SampleSet file:genesis/placement/sample_set.hpp line:54
		pybind11::class_<genesis::placement::SampleSet, std::shared_ptr<genesis::placement::SampleSet>> cl(M("genesis::placement"), "SampleSet", "Store a set of Sample%s with associated names.\n\n The elements in this set are stored in the sequence in which\n they are added to the set, and can be accessed via an index.");
		cl.def( pybind11::init( [](){ return new genesis::placement::SampleSet(); } ) );
		cl.def( pybind11::init( [](genesis::placement::SampleSet const &o){ return new genesis::placement::SampleSet(o); } ) );
		cl.def("assign", (class genesis::placement::SampleSet & (genesis::placement::SampleSet::*)(const class genesis::placement::SampleSet &)) &genesis::placement::SampleSet::operator=, "C++: genesis::placement::SampleSet::operator=(const class genesis::placement::SampleSet &) --> class genesis::placement::SampleSet &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("swap", (void (genesis::placement::SampleSet::*)(class genesis::placement::SampleSet &)) &genesis::placement::SampleSet::swap, "C++: genesis::placement::SampleSet::swap(class genesis::placement::SampleSet &) --> void", pybind11::arg("other"));
		cl.def("add", [](genesis::placement::SampleSet &o, const class genesis::placement::Sample & a0) -> void { return o.add(a0); }, "", pybind11::arg("smp"));
		cl.def("add", (void (genesis::placement::SampleSet::*)(const class genesis::placement::Sample &, const std::string &)) &genesis::placement::SampleSet::add, "Add a Sample with a name to the SampleSet.\n\n The Sample is copied.\n\nC++: genesis::placement::SampleSet::add(const class genesis::placement::Sample &, const std::string &) --> void", pybind11::arg("smp"), pybind11::arg("name"));
		cl.def("remove_at", (void (genesis::placement::SampleSet::*)(unsigned long)) &genesis::placement::SampleSet::remove_at, "Remove the Sample at a certain index position.\n\n As this function moves Sample%s in the container around, all iterators and pointers to\n the elements of this SampleSet are considered to be invalidated.\n\nC++: genesis::placement::SampleSet::remove_at(unsigned long) --> void", pybind11::arg("index"));
		cl.def("clear", (void (genesis::placement::SampleSet::*)()) &genesis::placement::SampleSet::clear, "Delete all Sample%s in this SampleSet.\n\nC++: genesis::placement::SampleSet::clear() --> void");
		cl.def("name_at", (const std::string & (genesis::placement::SampleSet::*)(unsigned long) const) &genesis::placement::SampleSet::name_at, "C++: genesis::placement::SampleSet::name_at(unsigned long) const --> const std::string &", pybind11::return_value_policy::reference_internal, pybind11::arg("index"));
		cl.def("names", (const class std::vector<std::string, class std::allocator<std::string > > & (genesis::placement::SampleSet::*)() const) &genesis::placement::SampleSet::names, "C++: genesis::placement::SampleSet::names() const --> const class std::vector<std::string, class std::allocator<std::string > > &", pybind11::return_value_policy::reference_internal);
		cl.def("begin", (class __gnu_cxx::__normal_iterator<class genesis::placement::Sample *, class std::vector<class genesis::placement::Sample, class std::allocator<class genesis::placement::Sample> > > (genesis::placement::SampleSet::*)()) &genesis::placement::SampleSet::begin, "C++: genesis::placement::SampleSet::begin() --> class __gnu_cxx::__normal_iterator<class genesis::placement::Sample *, class std::vector<class genesis::placement::Sample, class std::allocator<class genesis::placement::Sample> > >");
		cl.def("end", (class __gnu_cxx::__normal_iterator<class genesis::placement::Sample *, class std::vector<class genesis::placement::Sample, class std::allocator<class genesis::placement::Sample> > > (genesis::placement::SampleSet::*)()) &genesis::placement::SampleSet::end, "C++: genesis::placement::SampleSet::end() --> class __gnu_cxx::__normal_iterator<class genesis::placement::Sample *, class std::vector<class genesis::placement::Sample, class std::allocator<class genesis::placement::Sample> > >");
		cl.def("at", (class genesis::placement::Sample & (genesis::placement::SampleSet::*)(unsigned long)) &genesis::placement::SampleSet::at, "C++: genesis::placement::SampleSet::at(unsigned long) --> class genesis::placement::Sample &", pybind11::return_value_policy::reference_internal, pybind11::arg("index"));
		cl.def("__getitem__", (class genesis::placement::Sample & (genesis::placement::SampleSet::*)(unsigned long)) &genesis::placement::SampleSet::operator[], "C++: genesis::placement::SampleSet::operator[](unsigned long) --> class genesis::placement::Sample &", pybind11::return_value_policy::reference_internal, pybind11::arg("index"));
		cl.def("samples", (const class std::vector<class genesis::placement::Sample, class std::allocator<class genesis::placement::Sample> > & (genesis::placement::SampleSet::*)() const) &genesis::placement::SampleSet::samples, "C++: genesis::placement::SampleSet::samples() const --> const class std::vector<class genesis::placement::Sample, class std::allocator<class genesis::placement::Sample> > &", pybind11::return_value_policy::reference_internal);
		cl.def("empty", (bool (genesis::placement::SampleSet::*)() const) &genesis::placement::SampleSet::empty, "Return whether the SampleSet is empty.\n\nC++: genesis::placement::SampleSet::empty() const --> bool");
		cl.def("size", (unsigned long (genesis::placement::SampleSet::*)() const) &genesis::placement::SampleSet::size, "Return the size of the SampleSet, i.e., the number of Sample%s.\n\nC++: genesis::placement::SampleSet::size() const --> unsigned long");
	}
	// genesis::placement::has_name(const class genesis::placement::Pquery &, const std::string &) file:genesis/placement/function/functions.hpp line:54
	M("genesis::placement").def("has_name", (bool (*)(const class genesis::placement::Pquery &, const std::string &)) &genesis::placement::has_name, "Return true iff the given Pquery contains a particular name.\n\nC++: genesis::placement::has_name(const class genesis::placement::Pquery &, const std::string &) --> bool", pybind11::arg("pquery"), pybind11::arg("name"));

	// genesis::placement::has_name(const class genesis::placement::Sample &, const std::string &) file:genesis/placement/function/functions.hpp line:60
	M("genesis::placement").def("has_name", (bool (*)(const class genesis::placement::Sample &, const std::string &)) &genesis::placement::has_name, "Return true iff the given Sample contains a Pquery with a particular name, i.e.,\n a PqueryName whose name member equals the given name.\n\nC++: genesis::placement::has_name(const class genesis::placement::Sample &, const std::string &) --> bool", pybind11::arg("smp"), pybind11::arg("name"));

	// genesis::placement::find_pquery(const class genesis::placement::Sample &, const std::string &) file:genesis/placement/function/functions.hpp line:65
	M("genesis::placement").def("find_pquery", (const class genesis::placement::Pquery * (*)(const class genesis::placement::Sample &, const std::string &)) &genesis::placement::find_pquery, "Return the first Pquery that has a particular name, or nullptr of none has.\n\nC++: genesis::placement::find_pquery(const class genesis::placement::Sample &, const std::string &) --> const class genesis::placement::Pquery *", pybind11::return_value_policy::reference_internal, pybind11::arg("smp"), pybind11::arg("name"));

	// genesis::placement::find_pquery(class genesis::placement::Sample &, const std::string &) file:genesis/placement/function/functions.hpp line:70
	M("genesis::placement").def("find_pquery", (class genesis::placement::Pquery * (*)(class genesis::placement::Sample &, const std::string &)) &genesis::placement::find_pquery, "Return the first Pquery that has a particular name, or nullptr of none has.\n\nC++: genesis::placement::find_pquery(class genesis::placement::Sample &, const std::string &) --> class genesis::placement::Pquery *", pybind11::return_value_policy::reference_internal, pybind11::arg("smp"), pybind11::arg("name"));

	// genesis::placement::all_pquery_names(const class genesis::placement::Sample &) file:genesis/placement/function/functions.hpp line:78
	M("genesis::placement").def("all_pquery_names", (class std::unordered_set<std::string, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<std::string > > (*)(const class genesis::placement::Sample &)) &genesis::placement::all_pquery_names, "Return a set of all unique PqueryName%s of the \n\n\n\n\n\nC++: genesis::placement::all_pquery_names(const class genesis::placement::Sample &) --> class std::unordered_set<std::string, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<std::string > >", pybind11::arg("sample"));

}

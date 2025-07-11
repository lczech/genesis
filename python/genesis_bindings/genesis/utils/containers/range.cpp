#include <genesis/placement/placement_tree.hpp>
#include <genesis/placement/pquery.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
#include <genesis/tree/iterator/path.hpp>
#include <genesis/tree/iterator/path_set.hpp>
#include <genesis/tree/tree/edge.hpp>
#include <genesis/tree/tree/link.hpp>
#include <genesis/tree/tree/node.hpp>
#include <genesis/utils/containers/matrix.hpp>
#include <genesis/utils/containers/matrix/operators.hpp>
#include <genesis/utils/containers/range.hpp>
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

void bind_genesis_utils_containers_range(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::utils::Range file:genesis/utils/containers/range.hpp line:46
		pybind11::class_<genesis::utils::Range<genesis::tree::IteratorPath<false>>, std::shared_ptr<genesis::utils::Range<genesis::tree::IteratorPath<false>>>> cl(M("genesis::utils"), "Range_genesis_tree_IteratorPath_false_t", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::Range<genesis::tree::IteratorPath<false>>(); } ) );
		cl.def( pybind11::init<class genesis::tree::IteratorPath<false>, class genesis::tree::IteratorPath<false>>(), pybind11::arg("begin"), pybind11::arg("end") );

		cl.def( pybind11::init( [](genesis::utils::Range<genesis::tree::IteratorPath<false>> const &o){ return new genesis::utils::Range<genesis::tree::IteratorPath<false>>(o); } ) );
		cl.def("assign", (class genesis::utils::Range<class genesis::tree::IteratorPath<false> > & (genesis::utils::Range<genesis::tree::IteratorPath<false>>::*)(const class genesis::utils::Range<class genesis::tree::IteratorPath<false> > &)) &genesis::utils::Range<genesis::tree::IteratorPath<false>>::operator=, "C++: genesis::utils::Range<genesis::tree::IteratorPath<false>>::operator=(const class genesis::utils::Range<class genesis::tree::IteratorPath<false> > &) --> class genesis::utils::Range<class genesis::tree::IteratorPath<false> > &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("begin", (class genesis::tree::IteratorPath<false> (genesis::utils::Range<genesis::tree::IteratorPath<false>>::*)()) &genesis::utils::Range<genesis::tree::IteratorPath<false>>::begin, "C++: genesis::utils::Range<genesis::tree::IteratorPath<false>>::begin() --> class genesis::tree::IteratorPath<false>");
		cl.def("end", (class genesis::tree::IteratorPath<false> (genesis::utils::Range<genesis::tree::IteratorPath<false>>::*)()) &genesis::utils::Range<genesis::tree::IteratorPath<false>>::end, "C++: genesis::utils::Range<genesis::tree::IteratorPath<false>>::end() --> class genesis::tree::IteratorPath<false>");

		extend_range(cl);
	}
	{ // genesis::utils::Range file:genesis/utils/containers/range.hpp line:46
		pybind11::class_<genesis::utils::Range<genesis::tree::IteratorPathSet<true>>, std::shared_ptr<genesis::utils::Range<genesis::tree::IteratorPathSet<true>>>> cl(M("genesis::utils"), "Range_genesis_tree_IteratorPathSet_true_t", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::Range<genesis::tree::IteratorPathSet<true>>(); } ) );
		cl.def( pybind11::init<class genesis::tree::IteratorPathSet<true>, class genesis::tree::IteratorPathSet<true>>(), pybind11::arg("begin"), pybind11::arg("end") );

		cl.def( pybind11::init( [](genesis::utils::Range<genesis::tree::IteratorPathSet<true>> const &o){ return new genesis::utils::Range<genesis::tree::IteratorPathSet<true>>(o); } ) );
		cl.def("begin", (class genesis::tree::IteratorPathSet<true> (genesis::utils::Range<genesis::tree::IteratorPathSet<true>>::*)()) &genesis::utils::Range<genesis::tree::IteratorPathSet<true>>::begin, "C++: genesis::utils::Range<genesis::tree::IteratorPathSet<true>>::begin() --> class genesis::tree::IteratorPathSet<true>");
		cl.def("end", (class genesis::tree::IteratorPathSet<true> (genesis::utils::Range<genesis::tree::IteratorPathSet<true>>::*)()) &genesis::utils::Range<genesis::tree::IteratorPathSet<true>>::end, "C++: genesis::utils::Range<genesis::tree::IteratorPathSet<true>>::end() --> class genesis::tree::IteratorPathSet<true>");

		extend_range(cl);
	}
	{ // genesis::utils::Range file:genesis/utils/containers/range.hpp line:46
		pybind11::class_<genesis::utils::Range<genesis::tree::IteratorPathSet<false>>, std::shared_ptr<genesis::utils::Range<genesis::tree::IteratorPathSet<false>>>> cl(M("genesis::utils"), "Range_genesis_tree_IteratorPathSet_false_t", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::Range<genesis::tree::IteratorPathSet<false>>(); } ) );
		cl.def( pybind11::init<class genesis::tree::IteratorPathSet<false>, class genesis::tree::IteratorPathSet<false>>(), pybind11::arg("begin"), pybind11::arg("end") );

		cl.def( pybind11::init( [](genesis::utils::Range<genesis::tree::IteratorPathSet<false>> const &o){ return new genesis::utils::Range<genesis::tree::IteratorPathSet<false>>(o); } ) );
		cl.def("begin", (class genesis::tree::IteratorPathSet<false> (genesis::utils::Range<genesis::tree::IteratorPathSet<false>>::*)()) &genesis::utils::Range<genesis::tree::IteratorPathSet<false>>::begin, "C++: genesis::utils::Range<genesis::tree::IteratorPathSet<false>>::begin() --> class genesis::tree::IteratorPathSet<false>");
		cl.def("end", (class genesis::tree::IteratorPathSet<false> (genesis::utils::Range<genesis::tree::IteratorPathSet<false>>::*)()) &genesis::utils::Range<genesis::tree::IteratorPathSet<false>>::end, "C++: genesis::utils::Range<genesis::tree::IteratorPathSet<false>>::end() --> class genesis::tree::IteratorPathSet<false>");

		extend_range(cl);
	}
	{ // genesis::utils::Range file:genesis/utils/containers/range.hpp line:46
		pybind11::class_<genesis::utils::Range<__gnu_cxx::__normal_iterator<genesis::placement::Pquery *, std::vector<genesis::placement::Pquery, std::allocator<genesis::placement::Pquery> > >>, std::shared_ptr<genesis::utils::Range<__gnu_cxx::__normal_iterator<genesis::placement::Pquery *, std::vector<genesis::placement::Pquery, std::allocator<genesis::placement::Pquery> > >>>> cl(M("genesis::utils"), "Range___gnu_cxx___normal_iterator_genesis_placement_Pquery__star__std_vector_genesis_placement_Pquery_std_allocator_genesis_placement_Pquery_t", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::Range<__gnu_cxx::__normal_iterator<genesis::placement::Pquery *, std::vector<genesis::placement::Pquery, std::allocator<genesis::placement::Pquery> > >>(); } ) );
		cl.def( pybind11::init<class __gnu_cxx::__normal_iterator<class genesis::placement::Pquery *, class std::vector<class genesis::placement::Pquery, class std::allocator<class genesis::placement::Pquery> > >, class __gnu_cxx::__normal_iterator<class genesis::placement::Pquery *, class std::vector<class genesis::placement::Pquery, class std::allocator<class genesis::placement::Pquery> > >>(), pybind11::arg("begin"), pybind11::arg("end") );

		cl.def( pybind11::init( [](genesis::utils::Range<__gnu_cxx::__normal_iterator<genesis::placement::Pquery *, std::vector<genesis::placement::Pquery, std::allocator<genesis::placement::Pquery> > >> const &o){ return new genesis::utils::Range<__gnu_cxx::__normal_iterator<genesis::placement::Pquery *, std::vector<genesis::placement::Pquery, std::allocator<genesis::placement::Pquery> > >>(o); } ) );
		cl.def("assign", (class genesis::utils::Range<class __gnu_cxx::__normal_iterator<class genesis::placement::Pquery *, class std::vector<class genesis::placement::Pquery, class std::allocator<class genesis::placement::Pquery> > > > & (genesis::utils::Range<__gnu_cxx::__normal_iterator<genesis::placement::Pquery *, std::vector<genesis::placement::Pquery, std::allocator<genesis::placement::Pquery> > >>::*)(const class genesis::utils::Range<class __gnu_cxx::__normal_iterator<class genesis::placement::Pquery *, class std::vector<class genesis::placement::Pquery, class std::allocator<class genesis::placement::Pquery> > > > &)) &genesis::utils::Range<__gnu_cxx::__normal_iterator<genesis::placement::Pquery *, std::vector<genesis::placement::Pquery, std::allocator<genesis::placement::Pquery>>>>::operator=, "C++: genesis::utils::Range<__gnu_cxx::__normal_iterator<genesis::placement::Pquery *, std::vector<genesis::placement::Pquery, std::allocator<genesis::placement::Pquery>>>>::operator=(const class genesis::utils::Range<class __gnu_cxx::__normal_iterator<class genesis::placement::Pquery *, class std::vector<class genesis::placement::Pquery, class std::allocator<class genesis::placement::Pquery> > > > &) --> class genesis::utils::Range<class __gnu_cxx::__normal_iterator<class genesis::placement::Pquery *, class std::vector<class genesis::placement::Pquery, class std::allocator<class genesis::placement::Pquery> > > > &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("begin", (class __gnu_cxx::__normal_iterator<class genesis::placement::Pquery *, class std::vector<class genesis::placement::Pquery, class std::allocator<class genesis::placement::Pquery> > > (genesis::utils::Range<__gnu_cxx::__normal_iterator<genesis::placement::Pquery *, std::vector<genesis::placement::Pquery, std::allocator<genesis::placement::Pquery> > >>::*)()) &genesis::utils::Range<__gnu_cxx::__normal_iterator<genesis::placement::Pquery *, std::vector<genesis::placement::Pquery, std::allocator<genesis::placement::Pquery>>>>::begin, "C++: genesis::utils::Range<__gnu_cxx::__normal_iterator<genesis::placement::Pquery *, std::vector<genesis::placement::Pquery, std::allocator<genesis::placement::Pquery>>>>::begin() --> class __gnu_cxx::__normal_iterator<class genesis::placement::Pquery *, class std::vector<class genesis::placement::Pquery, class std::allocator<class genesis::placement::Pquery> > >");
		cl.def("end", (class __gnu_cxx::__normal_iterator<class genesis::placement::Pquery *, class std::vector<class genesis::placement::Pquery, class std::allocator<class genesis::placement::Pquery> > > (genesis::utils::Range<__gnu_cxx::__normal_iterator<genesis::placement::Pquery *, std::vector<genesis::placement::Pquery, std::allocator<genesis::placement::Pquery> > >>::*)()) &genesis::utils::Range<__gnu_cxx::__normal_iterator<genesis::placement::Pquery *, std::vector<genesis::placement::Pquery, std::allocator<genesis::placement::Pquery>>>>::end, "C++: genesis::utils::Range<__gnu_cxx::__normal_iterator<genesis::placement::Pquery *, std::vector<genesis::placement::Pquery, std::allocator<genesis::placement::Pquery>>>>::end() --> class __gnu_cxx::__normal_iterator<class genesis::placement::Pquery *, class std::vector<class genesis::placement::Pquery, class std::allocator<class genesis::placement::Pquery> > >");

		extend_range(cl);
	}
	{ // genesis::utils::Matrix file:genesis/utils/containers/matrix.hpp line:60
		pybind11::class_<genesis::utils::Matrix<signed char>, std::shared_ptr<genesis::utils::Matrix<signed char>>> cl(M("genesis::utils"), "Matrix_signed_char_t", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::Matrix<signed char>(); } ) );
		cl.def( pybind11::init<unsigned long, unsigned long>(), pybind11::arg("rows"), pybind11::arg("cols") );

		cl.def( pybind11::init<unsigned long, unsigned long, signed char>(), pybind11::arg("rows"), pybind11::arg("cols"), pybind11::arg("init") );

		cl.def( pybind11::init<unsigned long, unsigned long, const class std::vector<signed char, class std::allocator<signed char> > &>(), pybind11::arg("rows"), pybind11::arg("cols"), pybind11::arg("data") );

		cl.def( pybind11::init( [](genesis::utils::Matrix<signed char> const &o){ return new genesis::utils::Matrix<signed char>(o); } ) );
		cl.def("assign", (class genesis::utils::Matrix<signed char> & (genesis::utils::Matrix<signed char>::*)(const class genesis::utils::Matrix<signed char> &)) &genesis::utils::Matrix<signed char>::operator=, "C++: genesis::utils::Matrix<signed char>::operator=(const class genesis::utils::Matrix<signed char> &) --> class genesis::utils::Matrix<signed char> &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("swap", (void (genesis::utils::Matrix<signed char>::*)(class genesis::utils::Matrix<signed char> &)) &genesis::utils::Matrix<signed char>::swap, "C++: genesis::utils::Matrix<signed char>::swap(class genesis::utils::Matrix<signed char> &) --> void", pybind11::arg("other"));
		cl.def("rows", (unsigned long (genesis::utils::Matrix<signed char>::*)() const) &genesis::utils::Matrix<signed char>::rows, "C++: genesis::utils::Matrix<signed char>::rows() const --> unsigned long");
		cl.def("cols", (unsigned long (genesis::utils::Matrix<signed char>::*)() const) &genesis::utils::Matrix<signed char>::cols, "C++: genesis::utils::Matrix<signed char>::cols() const --> unsigned long");
		cl.def("size", (unsigned long (genesis::utils::Matrix<signed char>::*)() const) &genesis::utils::Matrix<signed char>::size, "C++: genesis::utils::Matrix<signed char>::size() const --> unsigned long");
		cl.def("empty", (bool (genesis::utils::Matrix<signed char>::*)() const) &genesis::utils::Matrix<signed char>::empty, "C++: genesis::utils::Matrix<signed char>::empty() const --> bool");
		cl.def("data", (const class std::vector<signed char, class std::allocator<signed char> > & (genesis::utils::Matrix<signed char>::*)() const) &genesis::utils::Matrix<signed char>::data, "C++: genesis::utils::Matrix<signed char>::data() const --> const class std::vector<signed char, class std::allocator<signed char> > &", pybind11::return_value_policy::reference_internal);
		cl.def("at", (signed char & (genesis::utils::Matrix<signed char>::*)(const unsigned long, const unsigned long)) &genesis::utils::Matrix<signed char>::at, "C++: genesis::utils::Matrix<signed char>::at(const unsigned long, const unsigned long) --> signed char &", pybind11::return_value_policy::reference_internal, pybind11::arg("row"), pybind11::arg("col"));
		cl.def("__call__", (signed char & (genesis::utils::Matrix<signed char>::*)(const unsigned long, const unsigned long)) &genesis::utils::Matrix<signed char>::operator(), "C++: genesis::utils::Matrix<signed char>::operator()(const unsigned long, const unsigned long) --> signed char &", pybind11::return_value_policy::reference_internal, pybind11::arg("row"), pybind11::arg("col"));
		cl.def("begin", (class __gnu_cxx::__normal_iterator<signed char *, class std::vector<signed char, class std::allocator<signed char> > > (genesis::utils::Matrix<signed char>::*)()) &genesis::utils::Matrix<signed char>::begin, "C++: genesis::utils::Matrix<signed char>::begin() --> class __gnu_cxx::__normal_iterator<signed char *, class std::vector<signed char, class std::allocator<signed char> > >");
		cl.def("end", (class __gnu_cxx::__normal_iterator<signed char *, class std::vector<signed char, class std::allocator<signed char> > > (genesis::utils::Matrix<signed char>::*)()) &genesis::utils::Matrix<signed char>::end, "C++: genesis::utils::Matrix<signed char>::end() --> class __gnu_cxx::__normal_iterator<signed char *, class std::vector<signed char, class std::allocator<signed char> > >");
		cl.def("cbegin", (class __gnu_cxx::__normal_iterator<const signed char *, class std::vector<signed char, class std::allocator<signed char> > > (genesis::utils::Matrix<signed char>::*)() const) &genesis::utils::Matrix<signed char>::cbegin, "C++: genesis::utils::Matrix<signed char>::cbegin() const --> class __gnu_cxx::__normal_iterator<const signed char *, class std::vector<signed char, class std::allocator<signed char> > >");
		cl.def("cend", (class __gnu_cxx::__normal_iterator<const signed char *, class std::vector<signed char, class std::allocator<signed char> > > (genesis::utils::Matrix<signed char>::*)() const) &genesis::utils::Matrix<signed char>::cend, "C++: genesis::utils::Matrix<signed char>::cend() const --> class __gnu_cxx::__normal_iterator<const signed char *, class std::vector<signed char, class std::allocator<signed char> > >");
		cl.def("__eq__", (bool (genesis::utils::Matrix<signed char>::*)(const class genesis::utils::Matrix<signed char> &) const) &genesis::utils::Matrix<signed char>::operator==, "C++: genesis::utils::Matrix<signed char>::operator==(const class genesis::utils::Matrix<signed char> &) const --> bool", pybind11::arg("other"));
		cl.def("__ne__", (bool (genesis::utils::Matrix<signed char>::*)(const class genesis::utils::Matrix<signed char> &) const) &genesis::utils::Matrix<signed char>::operator!=, "C++: genesis::utils::Matrix<signed char>::operator!=(const class genesis::utils::Matrix<signed char> &) const --> bool", pybind11::arg("other"));

		cl.def("__str__", [](genesis::utils::Matrix<signed char> const &o) -> std::string { std::ostringstream s; genesis::utils::operator<<(s, o); return s.str(); } );

		extend_matrix(cl);
	}
	// genesis::utils::transpose_inplace(class genesis::utils::Matrix<signed char> &) file:genesis/utils/containers/matrix.hpp line:170
	M("genesis::utils").def("transpose_inplace", (void (*)(class genesis::utils::Matrix<signed char> &)) &genesis::utils::transpose_inplace<signed char>, "C++: genesis::utils::transpose_inplace(class genesis::utils::Matrix<signed char> &) --> void", pybind11::arg(""));

}

#include <genesis/utils/containers/matrix.hpp>
#include <genesis/utils/containers/matrix/operators.hpp>
#include <genesis/utils/containers/matrix/row.hpp>
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

void bind_genesis_utils_containers_matrix_1(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::utils::transpose_inplace(class genesis::utils::Matrix<unsigned int> &) file:genesis/utils/containers/matrix.hpp line:170
	M("genesis::utils").def("transpose_inplace", (void (*)(class genesis::utils::Matrix<unsigned int> &)) &genesis::utils::transpose_inplace<unsigned int>, "C++: genesis::utils::transpose_inplace(class genesis::utils::Matrix<unsigned int> &) --> void", pybind11::arg(""));

	{ // genesis::utils::Matrix file:genesis/utils/containers/matrix.hpp line:60
		pybind11::class_<genesis::utils::Matrix<int>, std::shared_ptr<genesis::utils::Matrix<int>>> cl(M("genesis::utils"), "Matrix_int_t", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::Matrix<int>(); } ) );
		cl.def( pybind11::init<unsigned long, unsigned long>(), pybind11::arg("rows"), pybind11::arg("cols") );

		cl.def( pybind11::init<unsigned long, unsigned long, int>(), pybind11::arg("rows"), pybind11::arg("cols"), pybind11::arg("init") );

		cl.def( pybind11::init<unsigned long, unsigned long, const class std::vector<int, class std::allocator<int> > &>(), pybind11::arg("rows"), pybind11::arg("cols"), pybind11::arg("data") );

		cl.def( pybind11::init( [](genesis::utils::Matrix<int> const &o){ return new genesis::utils::Matrix<int>(o); } ) );
		cl.def("assign", (class genesis::utils::Matrix<int> & (genesis::utils::Matrix<int>::*)(const class genesis::utils::Matrix<int> &)) &genesis::utils::Matrix<int>::operator=, "C++: genesis::utils::Matrix<int>::operator=(const class genesis::utils::Matrix<int> &) --> class genesis::utils::Matrix<int> &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("swap", (void (genesis::utils::Matrix<int>::*)(class genesis::utils::Matrix<int> &)) &genesis::utils::Matrix<int>::swap, "C++: genesis::utils::Matrix<int>::swap(class genesis::utils::Matrix<int> &) --> void", pybind11::arg("other"));
		cl.def("rows", (unsigned long (genesis::utils::Matrix<int>::*)() const) &genesis::utils::Matrix<int>::rows, "C++: genesis::utils::Matrix<int>::rows() const --> unsigned long");
		cl.def("cols", (unsigned long (genesis::utils::Matrix<int>::*)() const) &genesis::utils::Matrix<int>::cols, "C++: genesis::utils::Matrix<int>::cols() const --> unsigned long");
		cl.def("size", (unsigned long (genesis::utils::Matrix<int>::*)() const) &genesis::utils::Matrix<int>::size, "C++: genesis::utils::Matrix<int>::size() const --> unsigned long");
		cl.def("empty", (bool (genesis::utils::Matrix<int>::*)() const) &genesis::utils::Matrix<int>::empty, "C++: genesis::utils::Matrix<int>::empty() const --> bool");
		cl.def("data", (const class std::vector<int, class std::allocator<int> > & (genesis::utils::Matrix<int>::*)() const) &genesis::utils::Matrix<int>::data, "C++: genesis::utils::Matrix<int>::data() const --> const class std::vector<int, class std::allocator<int> > &", pybind11::return_value_policy::reference_internal);
		cl.def("at", (int & (genesis::utils::Matrix<int>::*)(const unsigned long, const unsigned long)) &genesis::utils::Matrix<int>::at, "C++: genesis::utils::Matrix<int>::at(const unsigned long, const unsigned long) --> int &", pybind11::return_value_policy::reference_internal, pybind11::arg("row"), pybind11::arg("col"));
		cl.def("__call__", (int & (genesis::utils::Matrix<int>::*)(const unsigned long, const unsigned long)) &genesis::utils::Matrix<int>::operator(), "C++: genesis::utils::Matrix<int>::operator()(const unsigned long, const unsigned long) --> int &", pybind11::return_value_policy::reference_internal, pybind11::arg("row"), pybind11::arg("col"));
		cl.def("begin", (class __gnu_cxx::__normal_iterator<int *, class std::vector<int, class std::allocator<int> > > (genesis::utils::Matrix<int>::*)()) &genesis::utils::Matrix<int>::begin, "C++: genesis::utils::Matrix<int>::begin() --> class __gnu_cxx::__normal_iterator<int *, class std::vector<int, class std::allocator<int> > >");
		cl.def("end", (class __gnu_cxx::__normal_iterator<int *, class std::vector<int, class std::allocator<int> > > (genesis::utils::Matrix<int>::*)()) &genesis::utils::Matrix<int>::end, "C++: genesis::utils::Matrix<int>::end() --> class __gnu_cxx::__normal_iterator<int *, class std::vector<int, class std::allocator<int> > >");
		cl.def("cbegin", (class __gnu_cxx::__normal_iterator<const int *, class std::vector<int, class std::allocator<int> > > (genesis::utils::Matrix<int>::*)() const) &genesis::utils::Matrix<int>::cbegin, "C++: genesis::utils::Matrix<int>::cbegin() const --> class __gnu_cxx::__normal_iterator<const int *, class std::vector<int, class std::allocator<int> > >");
		cl.def("cend", (class __gnu_cxx::__normal_iterator<const int *, class std::vector<int, class std::allocator<int> > > (genesis::utils::Matrix<int>::*)() const) &genesis::utils::Matrix<int>::cend, "C++: genesis::utils::Matrix<int>::cend() const --> class __gnu_cxx::__normal_iterator<const int *, class std::vector<int, class std::allocator<int> > >");
		cl.def("__eq__", (bool (genesis::utils::Matrix<int>::*)(const class genesis::utils::Matrix<int> &) const) &genesis::utils::Matrix<int>::operator==, "C++: genesis::utils::Matrix<int>::operator==(const class genesis::utils::Matrix<int> &) const --> bool", pybind11::arg("other"));
		cl.def("__ne__", (bool (genesis::utils::Matrix<int>::*)(const class genesis::utils::Matrix<int> &) const) &genesis::utils::Matrix<int>::operator!=, "C++: genesis::utils::Matrix<int>::operator!=(const class genesis::utils::Matrix<int> &) const --> bool", pybind11::arg("other"));

		cl.def("__str__", [](genesis::utils::Matrix<int> const &o) -> std::string { std::ostringstream s; genesis::utils::operator<<(s, o); return s.str(); } );

		extend_matrix(cl);
	}
	// genesis::utils::transpose_inplace(class genesis::utils::Matrix<int> &) file:genesis/utils/containers/matrix.hpp line:170
	M("genesis::utils").def("transpose_inplace", (void (*)(class genesis::utils::Matrix<int> &)) &genesis::utils::transpose_inplace<int>, "C++: genesis::utils::transpose_inplace(class genesis::utils::Matrix<int> &) --> void", pybind11::arg(""));

	{ // genesis::utils::Matrix file:genesis/utils/containers/matrix.hpp line:60
		pybind11::class_<genesis::utils::Matrix<unsigned long>, std::shared_ptr<genesis::utils::Matrix<unsigned long>>> cl(M("genesis::utils"), "Matrix_unsigned_long_t", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::Matrix<unsigned long>(); } ) );
		cl.def( pybind11::init<unsigned long, unsigned long>(), pybind11::arg("rows"), pybind11::arg("cols") );

		cl.def( pybind11::init<unsigned long, unsigned long, unsigned long>(), pybind11::arg("rows"), pybind11::arg("cols"), pybind11::arg("init") );

		cl.def( pybind11::init<unsigned long, unsigned long, const class std::vector<unsigned long, class std::allocator<unsigned long> > &>(), pybind11::arg("rows"), pybind11::arg("cols"), pybind11::arg("data") );

		cl.def( pybind11::init( [](genesis::utils::Matrix<unsigned long> const &o){ return new genesis::utils::Matrix<unsigned long>(o); } ) );
		cl.def("assign", (class genesis::utils::Matrix<unsigned long> & (genesis::utils::Matrix<unsigned long>::*)(const class genesis::utils::Matrix<unsigned long> &)) &genesis::utils::Matrix<unsigned long>::operator=, "C++: genesis::utils::Matrix<unsigned long>::operator=(const class genesis::utils::Matrix<unsigned long> &) --> class genesis::utils::Matrix<unsigned long> &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("swap", (void (genesis::utils::Matrix<unsigned long>::*)(class genesis::utils::Matrix<unsigned long> &)) &genesis::utils::Matrix<unsigned long>::swap, "C++: genesis::utils::Matrix<unsigned long>::swap(class genesis::utils::Matrix<unsigned long> &) --> void", pybind11::arg("other"));
		cl.def("rows", (unsigned long (genesis::utils::Matrix<unsigned long>::*)() const) &genesis::utils::Matrix<unsigned long>::rows, "C++: genesis::utils::Matrix<unsigned long>::rows() const --> unsigned long");
		cl.def("cols", (unsigned long (genesis::utils::Matrix<unsigned long>::*)() const) &genesis::utils::Matrix<unsigned long>::cols, "C++: genesis::utils::Matrix<unsigned long>::cols() const --> unsigned long");
		cl.def("size", (unsigned long (genesis::utils::Matrix<unsigned long>::*)() const) &genesis::utils::Matrix<unsigned long>::size, "C++: genesis::utils::Matrix<unsigned long>::size() const --> unsigned long");
		cl.def("empty", (bool (genesis::utils::Matrix<unsigned long>::*)() const) &genesis::utils::Matrix<unsigned long>::empty, "C++: genesis::utils::Matrix<unsigned long>::empty() const --> bool");
		cl.def("data", (const class std::vector<unsigned long, class std::allocator<unsigned long> > & (genesis::utils::Matrix<unsigned long>::*)() const) &genesis::utils::Matrix<unsigned long>::data, "C++: genesis::utils::Matrix<unsigned long>::data() const --> const class std::vector<unsigned long, class std::allocator<unsigned long> > &", pybind11::return_value_policy::reference_internal);
		cl.def("at", (unsigned long & (genesis::utils::Matrix<unsigned long>::*)(const unsigned long, const unsigned long)) &genesis::utils::Matrix<unsigned long>::at, "C++: genesis::utils::Matrix<unsigned long>::at(const unsigned long, const unsigned long) --> unsigned long &", pybind11::return_value_policy::reference_internal, pybind11::arg("row"), pybind11::arg("col"));
		cl.def("__call__", (unsigned long & (genesis::utils::Matrix<unsigned long>::*)(const unsigned long, const unsigned long)) &genesis::utils::Matrix<unsigned long>::operator(), "C++: genesis::utils::Matrix<unsigned long>::operator()(const unsigned long, const unsigned long) --> unsigned long &", pybind11::return_value_policy::reference_internal, pybind11::arg("row"), pybind11::arg("col"));
		cl.def("begin", (class __gnu_cxx::__normal_iterator<unsigned long *, class std::vector<unsigned long, class std::allocator<unsigned long> > > (genesis::utils::Matrix<unsigned long>::*)()) &genesis::utils::Matrix<unsigned long>::begin, "C++: genesis::utils::Matrix<unsigned long>::begin() --> class __gnu_cxx::__normal_iterator<unsigned long *, class std::vector<unsigned long, class std::allocator<unsigned long> > >");
		cl.def("end", (class __gnu_cxx::__normal_iterator<unsigned long *, class std::vector<unsigned long, class std::allocator<unsigned long> > > (genesis::utils::Matrix<unsigned long>::*)()) &genesis::utils::Matrix<unsigned long>::end, "C++: genesis::utils::Matrix<unsigned long>::end() --> class __gnu_cxx::__normal_iterator<unsigned long *, class std::vector<unsigned long, class std::allocator<unsigned long> > >");
		cl.def("cbegin", (class __gnu_cxx::__normal_iterator<const unsigned long *, class std::vector<unsigned long, class std::allocator<unsigned long> > > (genesis::utils::Matrix<unsigned long>::*)() const) &genesis::utils::Matrix<unsigned long>::cbegin, "C++: genesis::utils::Matrix<unsigned long>::cbegin() const --> class __gnu_cxx::__normal_iterator<const unsigned long *, class std::vector<unsigned long, class std::allocator<unsigned long> > >");
		cl.def("cend", (class __gnu_cxx::__normal_iterator<const unsigned long *, class std::vector<unsigned long, class std::allocator<unsigned long> > > (genesis::utils::Matrix<unsigned long>::*)() const) &genesis::utils::Matrix<unsigned long>::cend, "C++: genesis::utils::Matrix<unsigned long>::cend() const --> class __gnu_cxx::__normal_iterator<const unsigned long *, class std::vector<unsigned long, class std::allocator<unsigned long> > >");
		cl.def("__eq__", (bool (genesis::utils::Matrix<unsigned long>::*)(const class genesis::utils::Matrix<unsigned long> &) const) &genesis::utils::Matrix<unsigned long>::operator==, "C++: genesis::utils::Matrix<unsigned long>::operator==(const class genesis::utils::Matrix<unsigned long> &) const --> bool", pybind11::arg("other"));
		cl.def("__ne__", (bool (genesis::utils::Matrix<unsigned long>::*)(const class genesis::utils::Matrix<unsigned long> &) const) &genesis::utils::Matrix<unsigned long>::operator!=, "C++: genesis::utils::Matrix<unsigned long>::operator!=(const class genesis::utils::Matrix<unsigned long> &) const --> bool", pybind11::arg("other"));

		cl.def("__str__", [](genesis::utils::Matrix<unsigned long> const &o) -> std::string { std::ostringstream s; genesis::utils::operator<<(s, o); return s.str(); } );

		extend_matrix(cl);
	}
	// genesis::utils::transpose_inplace(class genesis::utils::Matrix<unsigned long> &) file:genesis/utils/containers/matrix.hpp line:170
	M("genesis::utils").def("transpose_inplace", (void (*)(class genesis::utils::Matrix<unsigned long> &)) &genesis::utils::transpose_inplace<unsigned long>, "C++: genesis::utils::transpose_inplace(class genesis::utils::Matrix<unsigned long> &) --> void", pybind11::arg(""));

	{ // genesis::utils::Matrix file:genesis/utils/containers/matrix.hpp line:60
		pybind11::class_<genesis::utils::Matrix<pybind11::object>, std::shared_ptr<genesis::utils::Matrix<pybind11::object>>> cl(M("genesis::utils"), "Matrix_pybind11_object_t", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::Matrix<pybind11::object>(); } ) );
		cl.def( pybind11::init<unsigned long, unsigned long>(), pybind11::arg("rows"), pybind11::arg("cols") );

		cl.def( pybind11::init<unsigned long, unsigned long, class pybind11::object>(), pybind11::arg("rows"), pybind11::arg("cols"), pybind11::arg("init") );

		cl.def( pybind11::init<unsigned long, unsigned long, const class std::vector<class pybind11::object, class std::allocator<class pybind11::object> > &>(), pybind11::arg("rows"), pybind11::arg("cols"), pybind11::arg("data") );

		cl.def( pybind11::init( [](genesis::utils::Matrix<pybind11::object> const &o){ return new genesis::utils::Matrix<pybind11::object>(o); } ) );
		cl.def("assign", (class genesis::utils::Matrix<class pybind11::object> & (genesis::utils::Matrix<pybind11::object>::*)(const class genesis::utils::Matrix<class pybind11::object> &)) &genesis::utils::Matrix<pybind11::object>::operator=, "C++: genesis::utils::Matrix<pybind11::object>::operator=(const class genesis::utils::Matrix<class pybind11::object> &) --> class genesis::utils::Matrix<class pybind11::object> &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("swap", (void (genesis::utils::Matrix<pybind11::object>::*)(class genesis::utils::Matrix<class pybind11::object> &)) &genesis::utils::Matrix<pybind11::object>::swap, "C++: genesis::utils::Matrix<pybind11::object>::swap(class genesis::utils::Matrix<class pybind11::object> &) --> void", pybind11::arg("other"));
		cl.def("rows", (unsigned long (genesis::utils::Matrix<pybind11::object>::*)() const) &genesis::utils::Matrix<pybind11::object>::rows, "C++: genesis::utils::Matrix<pybind11::object>::rows() const --> unsigned long");
		cl.def("cols", (unsigned long (genesis::utils::Matrix<pybind11::object>::*)() const) &genesis::utils::Matrix<pybind11::object>::cols, "C++: genesis::utils::Matrix<pybind11::object>::cols() const --> unsigned long");
		cl.def("size", (unsigned long (genesis::utils::Matrix<pybind11::object>::*)() const) &genesis::utils::Matrix<pybind11::object>::size, "C++: genesis::utils::Matrix<pybind11::object>::size() const --> unsigned long");
		cl.def("empty", (bool (genesis::utils::Matrix<pybind11::object>::*)() const) &genesis::utils::Matrix<pybind11::object>::empty, "C++: genesis::utils::Matrix<pybind11::object>::empty() const --> bool");
		cl.def("data", (const class std::vector<class pybind11::object, class std::allocator<class pybind11::object> > & (genesis::utils::Matrix<pybind11::object>::*)() const) &genesis::utils::Matrix<pybind11::object>::data, "C++: genesis::utils::Matrix<pybind11::object>::data() const --> const class std::vector<class pybind11::object, class std::allocator<class pybind11::object> > &", pybind11::return_value_policy::reference_internal);
		cl.def("at", (class pybind11::object & (genesis::utils::Matrix<pybind11::object>::*)(const unsigned long, const unsigned long)) &genesis::utils::Matrix<pybind11::object>::at, "C++: genesis::utils::Matrix<pybind11::object>::at(const unsigned long, const unsigned long) --> class pybind11::object &", pybind11::return_value_policy::reference_internal, pybind11::arg("row"), pybind11::arg("col"));
		cl.def("__call__", (class pybind11::object & (genesis::utils::Matrix<pybind11::object>::*)(const unsigned long, const unsigned long)) &genesis::utils::Matrix<pybind11::object>::operator(), "C++: genesis::utils::Matrix<pybind11::object>::operator()(const unsigned long, const unsigned long) --> class pybind11::object &", pybind11::return_value_policy::reference_internal, pybind11::arg("row"), pybind11::arg("col"));
		cl.def("begin", (class __gnu_cxx::__normal_iterator<class pybind11::object *, class std::vector<class pybind11::object, class std::allocator<class pybind11::object> > > (genesis::utils::Matrix<pybind11::object>::*)()) &genesis::utils::Matrix<pybind11::object>::begin, "C++: genesis::utils::Matrix<pybind11::object>::begin() --> class __gnu_cxx::__normal_iterator<class pybind11::object *, class std::vector<class pybind11::object, class std::allocator<class pybind11::object> > >");
		cl.def("end", (class __gnu_cxx::__normal_iterator<class pybind11::object *, class std::vector<class pybind11::object, class std::allocator<class pybind11::object> > > (genesis::utils::Matrix<pybind11::object>::*)()) &genesis::utils::Matrix<pybind11::object>::end, "C++: genesis::utils::Matrix<pybind11::object>::end() --> class __gnu_cxx::__normal_iterator<class pybind11::object *, class std::vector<class pybind11::object, class std::allocator<class pybind11::object> > >");
		cl.def("cbegin", (class __gnu_cxx::__normal_iterator<const class pybind11::object *, class std::vector<class pybind11::object, class std::allocator<class pybind11::object> > > (genesis::utils::Matrix<pybind11::object>::*)() const) &genesis::utils::Matrix<pybind11::object>::cbegin, "C++: genesis::utils::Matrix<pybind11::object>::cbegin() const --> class __gnu_cxx::__normal_iterator<const class pybind11::object *, class std::vector<class pybind11::object, class std::allocator<class pybind11::object> > >");
		cl.def("cend", (class __gnu_cxx::__normal_iterator<const class pybind11::object *, class std::vector<class pybind11::object, class std::allocator<class pybind11::object> > > (genesis::utils::Matrix<pybind11::object>::*)() const) &genesis::utils::Matrix<pybind11::object>::cend, "C++: genesis::utils::Matrix<pybind11::object>::cend() const --> class __gnu_cxx::__normal_iterator<const class pybind11::object *, class std::vector<class pybind11::object, class std::allocator<class pybind11::object> > >");
		cl.def("__eq__", (bool (genesis::utils::Matrix<pybind11::object>::*)(const class genesis::utils::Matrix<class pybind11::object> &) const) &genesis::utils::Matrix<pybind11::object>::operator==, "C++: genesis::utils::Matrix<pybind11::object>::operator==(const class genesis::utils::Matrix<class pybind11::object> &) const --> bool", pybind11::arg("other"));
		cl.def("__ne__", (bool (genesis::utils::Matrix<pybind11::object>::*)(const class genesis::utils::Matrix<class pybind11::object> &) const) &genesis::utils::Matrix<pybind11::object>::operator!=, "C++: genesis::utils::Matrix<pybind11::object>::operator!=(const class genesis::utils::Matrix<class pybind11::object> &) const --> bool", pybind11::arg("other"));

		cl.def("__str__", [](genesis::utils::Matrix<pybind11::object> const &o) -> std::string { std::ostringstream s; genesis::utils::operator<<(s, o); return s.str(); } );

		extend_matrix(cl);
	}
}

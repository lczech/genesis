#include <functional>
#include <genesis/utils/containers/matrix.hpp>
#include <genesis/utils/containers/matrix/operators.hpp>
#include <genesis/utils/containers/matrix/row.hpp>
#include <genesis/utils/tools/char_lookup.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
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

void bind_genesis_utils_containers_matrix_2(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::utils::transpose_inplace(class genesis::utils::Matrix<class pybind11::object> &) file:genesis/utils/containers/matrix.hpp line:170
	M("genesis::utils").def("transpose_inplace", (void (*)(class genesis::utils::Matrix<class pybind11::object> &)) &genesis::utils::transpose_inplace<pybind11::object>, "C++: genesis::utils::transpose_inplace(class genesis::utils::Matrix<class pybind11::object> &) --> void", pybind11::arg(""));

	{ // genesis::utils::MatrixRow file:genesis/utils/containers/matrix/row.hpp line:60
		pybind11::class_<genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>,unsigned int>, std::shared_ptr<genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>,unsigned int>>> cl(M("genesis::utils"), "MatrixRow_genesis_utils_Matrix_unsigned_int_unsigned_int_t", "");
		cl.def( pybind11::init<class genesis::utils::Matrix<unsigned int> &, unsigned long>(), pybind11::arg("mat"), pybind11::arg("row") );

		cl.def( pybind11::init( [](genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>,unsigned int> const &o){ return new genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>,unsigned int>(o); } ) );
		cl.def("assign", (class genesis::utils::MatrixRow<class genesis::utils::Matrix<unsigned int>, unsigned int> & (genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>,unsigned int>::*)(const class genesis::utils::MatrixRow<class genesis::utils::Matrix<unsigned int>, unsigned int> &)) &genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>, unsigned int>::operator=, "C++: genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>, unsigned int>::operator=(const class genesis::utils::MatrixRow<class genesis::utils::Matrix<unsigned int>, unsigned int> &) --> class genesis::utils::MatrixRow<class genesis::utils::Matrix<unsigned int>, unsigned int> &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("at", (unsigned int & (genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>,unsigned int>::*)(unsigned long) const) &genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>, unsigned int>::at, "C++: genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>, unsigned int>::at(unsigned long) const --> unsigned int &", pybind11::return_value_policy::reference_internal, pybind11::arg("column"));
		cl.def("__getitem__", (unsigned int & (genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>,unsigned int>::*)(unsigned long) const) &genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>, unsigned int>::operator[], "C++: genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>, unsigned int>::operator[](unsigned long) const --> unsigned int &", pybind11::return_value_policy::reference_internal, pybind11::arg("column"));
		cl.def("matrix", (class genesis::utils::Matrix<unsigned int> & (genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>,unsigned int>::*)() const) &genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>, unsigned int>::matrix, "C++: genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>, unsigned int>::matrix() const --> class genesis::utils::Matrix<unsigned int> &", pybind11::return_value_policy::reference_internal);
		cl.def("row", (unsigned long (genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>,unsigned int>::*)() const) &genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>, unsigned int>::row, "C++: genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>, unsigned int>::row() const --> unsigned long");
		cl.def("size", (unsigned long (genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>,unsigned int>::*)() const) &genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>, unsigned int>::size, "C++: genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>, unsigned int>::size() const --> unsigned long");
		cl.def("to_vector", (class std::vector<unsigned int, class std::allocator<unsigned int> > (genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>,unsigned int>::*)() const) &genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>, unsigned int>::to_vector, "C++: genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>, unsigned int>::to_vector() const --> class std::vector<unsigned int, class std::allocator<unsigned int> >");
		cl.def("assign", (class genesis::utils::MatrixRow<class genesis::utils::Matrix<unsigned int>, unsigned int> & (genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>,unsigned int>::*)(const class std::vector<unsigned int, class std::allocator<unsigned int> > &)) &genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>, unsigned int>::operator=, "C++: genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>, unsigned int>::operator=(const class std::vector<unsigned int, class std::allocator<unsigned int> > &) --> class genesis::utils::MatrixRow<class genesis::utils::Matrix<unsigned int>, unsigned int> &", pybind11::return_value_policy::reference_internal, pybind11::arg("vec"));
		cl.def("assign", (class genesis::utils::MatrixRow<class genesis::utils::Matrix<unsigned int>, unsigned int> & (genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>,unsigned int>::*)(const class genesis::utils::MatrixRow<class genesis::utils::Matrix<unsigned int>, unsigned int> &)) &genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>, unsigned int>::assign, "C++: genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>, unsigned int>::assign(const class genesis::utils::MatrixRow<class genesis::utils::Matrix<unsigned int>, unsigned int> &) --> class genesis::utils::MatrixRow<class genesis::utils::Matrix<unsigned int>, unsigned int> &", pybind11::return_value_policy::reference_internal, pybind11::arg("other"));
		cl.def("__eq__", (bool (genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>,unsigned int>::*)(const class genesis::utils::MatrixRow<class genesis::utils::Matrix<unsigned int>, unsigned int> &) const) &genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>, unsigned int>::operator==, "C++: genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>, unsigned int>::operator==(const class genesis::utils::MatrixRow<class genesis::utils::Matrix<unsigned int>, unsigned int> &) const --> bool", pybind11::arg("other"));
		cl.def("__ne__", (bool (genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>,unsigned int>::*)(const class genesis::utils::MatrixRow<class genesis::utils::Matrix<unsigned int>, unsigned int> &) const) &genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>, unsigned int>::operator!=, "C++: genesis::utils::MatrixRow<genesis::utils::Matrix<unsigned int>, unsigned int>::operator!=(const class genesis::utils::MatrixRow<class genesis::utils::Matrix<unsigned int>, unsigned int> &) const --> bool", pybind11::arg("other"));
	}
	// genesis::utils::triangular_indices(unsigned long, unsigned long) file:genesis/utils/containers/matrix/operators.hpp line:83
	M("genesis::utils").def("triangular_indices", (struct std::pair<unsigned long, unsigned long> (*)(unsigned long, unsigned long)) &genesis::utils::triangular_indices, "Given a linear index in a upper triangular Matrix, find the corresponding Matrix indices.\n\n Given an upper triangle Matrix of size `n == 5`\n\n     0  k0  k1  k2  k3\n     0   0  k4  k5  k6\n     0   0   0  k7  k8\n     0   0   0   0  k9\n     0   0   0   0   0\n\n and given a  (like above), find the corresponding indices `i` and `j` for this \n e.g.,\n\n     k == 0 --> i == 0, j == 1\n     k == 5 --> i == 1, j == 3\n     k == 9 --> i == 3, j == 4\n     ...\n\n Because the calculation involves solving\n [triangular numbers](https://en.wikipedia.org/wiki/Triangular_number), the function internally\n operates on `double` values. This is a bit smelly, but seems to work (tested with `n == 100,000`).\n\n See also triangular_index() for the opposite function, and triangular_size() to calculate the\n maximal  that will occur in a trian Matrix of a given size \n\n \n Linear index in the upper triangle of a quadratic Matrix.\n \n\n Size of the quadratic Matrix, i.e., the row/column length.\n \n\n  Pair `( i, j )` of the indices for the given `k`.\n\nC++: genesis::utils::triangular_indices(unsigned long, unsigned long) --> struct std::pair<unsigned long, unsigned long>", pybind11::arg("k"), pybind11::arg("n"));

	// genesis::utils::triangular_index(unsigned long, unsigned long, unsigned long) file:genesis/utils/containers/matrix/operators.hpp line:96
	M("genesis::utils").def("triangular_index", (unsigned long (*)(unsigned long, unsigned long, unsigned long)) &genesis::utils::triangular_index, "Given indices  and  in a quadratic Matrix, find the corresponding linear index.\n\n See triangular_indices() for the opposite function, which also explains the details, and\n triangular_size() to calculate the\n maximal  that will occur in a trian Matrix of a given size \n\n \n j Indices of the Matrix for which to calculate the linear index `k`.\n \n\n    Size of the quadratic Matrix, i.e., the row/column length.\n \n\n     Linear index `k` for the given  and \n \n\nC++: genesis::utils::triangular_index(unsigned long, unsigned long, unsigned long) --> unsigned long", pybind11::arg("i"), pybind11::arg("j"), pybind11::arg("n"));

	// genesis::utils::triangular_size(unsigned long) file:genesis/utils/containers/matrix/operators.hpp line:117
	M("genesis::utils").def("triangular_size", (unsigned long (*)(unsigned long)) &genesis::utils::triangular_size, "Calculate the number of linear indices needed for a triangular Matrix of size \n\n Given an upper triangle Matrix of size `n == 5`\n\n     0  k0  k1  k2  k3\n     0   0  k4  k5  k6\n     0   0   0  k7  k8\n     0   0   0   0  k9\n     0   0   0   0   0\n\n we need `10` indices `k == 0..9` to linearly describe the positions in the triangle.\n This function returns this number of indices for a given \n\n See also triangular_indices() for calculating the Matrix indices `i` and `j` for a given `k`.\n\n \n Size of the quadratic Matrix, i.e., the row/column length.\n \n\n  Number of needed linear indices `k` to describe a triangle of the Matrix.\n\nC++: genesis::utils::triangular_size(unsigned long) --> unsigned long", pybind11::arg("n"));

	{ // genesis::utils::CharLookup file:genesis/utils/tools/char_lookup.hpp line:54
		pybind11::class_<genesis::utils::CharLookup<unsigned char>, std::shared_ptr<genesis::utils::CharLookup<unsigned char>>> cl(M("genesis::utils"), "CharLookup_unsigned_char_t", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::CharLookup<unsigned char>(); } ) );
		cl.def( pybind11::init<const unsigned char &>(), pybind11::arg("init_all") );

		cl.def( pybind11::init( [](genesis::utils::CharLookup<unsigned char> const &o){ return new genesis::utils::CharLookup<unsigned char>(o); } ) );
		cl.def("assign", (class genesis::utils::CharLookup<unsigned char> & (genesis::utils::CharLookup<unsigned char>::*)(const class genesis::utils::CharLookup<unsigned char> &)) &genesis::utils::CharLookup<unsigned char>::operator=, "C++: genesis::utils::CharLookup<unsigned char>::operator=(const class genesis::utils::CharLookup<unsigned char> &) --> class genesis::utils::CharLookup<unsigned char> &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("set_char", (void (genesis::utils::CharLookup<unsigned char>::*)(char, unsigned char)) &genesis::utils::CharLookup<unsigned char>::set_char, "C++: genesis::utils::CharLookup<unsigned char>::set_char(char, unsigned char) --> void", pybind11::arg("c"), pybind11::arg("value"));
		cl.def("set_char_upper_lower", (void (genesis::utils::CharLookup<unsigned char>::*)(char, unsigned char)) &genesis::utils::CharLookup<unsigned char>::set_char_upper_lower, "C++: genesis::utils::CharLookup<unsigned char>::set_char_upper_lower(char, unsigned char) --> void", pybind11::arg("c"), pybind11::arg("value"));
		cl.def("set_if", (void (genesis::utils::CharLookup<unsigned char>::*)(class std::function<bool (char)>, unsigned char)) &genesis::utils::CharLookup<unsigned char>::set_if, "C++: genesis::utils::CharLookup<unsigned char>::set_if(class std::function<bool (char)>, unsigned char) --> void", pybind11::arg("predicate"), pybind11::arg("value"));
		cl.def("set_selection", (void (genesis::utils::CharLookup<unsigned char>::*)(const std::string &, unsigned char)) &genesis::utils::CharLookup<unsigned char>::set_selection, "C++: genesis::utils::CharLookup<unsigned char>::set_selection(const std::string &, unsigned char) --> void", pybind11::arg("chars"), pybind11::arg("value"));
		cl.def("set_selection_upper_lower", (void (genesis::utils::CharLookup<unsigned char>::*)(const std::string &, unsigned char)) &genesis::utils::CharLookup<unsigned char>::set_selection_upper_lower, "C++: genesis::utils::CharLookup<unsigned char>::set_selection_upper_lower(const std::string &, unsigned char) --> void", pybind11::arg("chars"), pybind11::arg("value"));
		cl.def("set_range", (void (genesis::utils::CharLookup<unsigned char>::*)(char, char, unsigned char)) &genesis::utils::CharLookup<unsigned char>::set_range, "C++: genesis::utils::CharLookup<unsigned char>::set_range(char, char, unsigned char) --> void", pybind11::arg("first"), pybind11::arg("last"), pybind11::arg("value"));
		cl.def("set_all", (void (genesis::utils::CharLookup<unsigned char>::*)(unsigned char)) &genesis::utils::CharLookup<unsigned char>::set_all, "C++: genesis::utils::CharLookup<unsigned char>::set_all(unsigned char) --> void", pybind11::arg("value"));
		cl.def("__getitem__", (unsigned char (genesis::utils::CharLookup<unsigned char>::*)(char) const) &genesis::utils::CharLookup<unsigned char>::operator[], "C++: genesis::utils::CharLookup<unsigned char>::operator[](char) const --> unsigned char", pybind11::arg("c"));
		cl.def("get", (unsigned char (genesis::utils::CharLookup<unsigned char>::*)(char) const) &genesis::utils::CharLookup<unsigned char>::get, "C++: genesis::utils::CharLookup<unsigned char>::get(char) const --> unsigned char", pybind11::arg("c"));
		cl.def("get_chars_equal_to", (std::string (genesis::utils::CharLookup<unsigned char>::*)(unsigned char) const) &genesis::utils::CharLookup<unsigned char>::get_chars_equal_to, "C++: genesis::utils::CharLookup<unsigned char>::get_chars_equal_to(unsigned char) const --> std::string", pybind11::arg("comp_value"));
		cl.def("all_equal_to", (bool (genesis::utils::CharLookup<unsigned char>::*)(unsigned char) const) &genesis::utils::CharLookup<unsigned char>::all_equal_to, "C++: genesis::utils::CharLookup<unsigned char>::all_equal_to(unsigned char) const --> bool", pybind11::arg("comp_value"));
	}
	{ // genesis::utils::CharLookup file:genesis/utils/tools/char_lookup.hpp line:54
		pybind11::class_<genesis::utils::CharLookup<bool>, std::shared_ptr<genesis::utils::CharLookup<bool>>> cl(M("genesis::utils"), "CharLookup_bool_t", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::CharLookup<bool>(); } ) );
		cl.def( pybind11::init<const bool &>(), pybind11::arg("init_all") );

		cl.def( pybind11::init( [](genesis::utils::CharLookup<bool> const &o){ return new genesis::utils::CharLookup<bool>(o); } ) );
		cl.def("assign", (class genesis::utils::CharLookup<bool> & (genesis::utils::CharLookup<bool>::*)(const class genesis::utils::CharLookup<bool> &)) &genesis::utils::CharLookup<bool>::operator=, "C++: genesis::utils::CharLookup<bool>::operator=(const class genesis::utils::CharLookup<bool> &) --> class genesis::utils::CharLookup<bool> &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("set_char", (void (genesis::utils::CharLookup<bool>::*)(char, bool)) &genesis::utils::CharLookup<bool>::set_char, "C++: genesis::utils::CharLookup<bool>::set_char(char, bool) --> void", pybind11::arg("c"), pybind11::arg("value"));
		cl.def("set_char_upper_lower", (void (genesis::utils::CharLookup<bool>::*)(char, bool)) &genesis::utils::CharLookup<bool>::set_char_upper_lower, "C++: genesis::utils::CharLookup<bool>::set_char_upper_lower(char, bool) --> void", pybind11::arg("c"), pybind11::arg("value"));
		cl.def("set_if", (void (genesis::utils::CharLookup<bool>::*)(class std::function<bool (char)>, bool)) &genesis::utils::CharLookup<bool>::set_if, "C++: genesis::utils::CharLookup<bool>::set_if(class std::function<bool (char)>, bool) --> void", pybind11::arg("predicate"), pybind11::arg("value"));
		cl.def("set_selection", (void (genesis::utils::CharLookup<bool>::*)(const std::string &, bool)) &genesis::utils::CharLookup<bool>::set_selection, "C++: genesis::utils::CharLookup<bool>::set_selection(const std::string &, bool) --> void", pybind11::arg("chars"), pybind11::arg("value"));
		cl.def("set_selection_upper_lower", (void (genesis::utils::CharLookup<bool>::*)(const std::string &, bool)) &genesis::utils::CharLookup<bool>::set_selection_upper_lower, "C++: genesis::utils::CharLookup<bool>::set_selection_upper_lower(const std::string &, bool) --> void", pybind11::arg("chars"), pybind11::arg("value"));
		cl.def("set_range", (void (genesis::utils::CharLookup<bool>::*)(char, char, bool)) &genesis::utils::CharLookup<bool>::set_range, "C++: genesis::utils::CharLookup<bool>::set_range(char, char, bool) --> void", pybind11::arg("first"), pybind11::arg("last"), pybind11::arg("value"));
		cl.def("set_all", (void (genesis::utils::CharLookup<bool>::*)(bool)) &genesis::utils::CharLookup<bool>::set_all, "C++: genesis::utils::CharLookup<bool>::set_all(bool) --> void", pybind11::arg("value"));
		cl.def("__getitem__", (bool (genesis::utils::CharLookup<bool>::*)(char) const) &genesis::utils::CharLookup<bool>::operator[], "C++: genesis::utils::CharLookup<bool>::operator[](char) const --> bool", pybind11::arg("c"));
		cl.def("get", (bool (genesis::utils::CharLookup<bool>::*)(char) const) &genesis::utils::CharLookup<bool>::get, "C++: genesis::utils::CharLookup<bool>::get(char) const --> bool", pybind11::arg("c"));
		cl.def("get_chars_equal_to", (std::string (genesis::utils::CharLookup<bool>::*)(bool) const) &genesis::utils::CharLookup<bool>::get_chars_equal_to, "C++: genesis::utils::CharLookup<bool>::get_chars_equal_to(bool) const --> std::string", pybind11::arg("comp_value"));
		cl.def("all_equal_to", (bool (genesis::utils::CharLookup<bool>::*)(bool) const) &genesis::utils::CharLookup<bool>::all_equal_to, "C++: genesis::utils::CharLookup<bool>::all_equal_to(bool) const --> bool", pybind11::arg("comp_value"));
	}
}

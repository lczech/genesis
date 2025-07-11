#include <../python/custom_bindings/template_instances/char_lookup.hpp>
#include <../python/custom_bindings/template_instances/math_matrix.hpp>
#include <../python/custom_bindings/template_instances/matrix.hpp>
#include <functional>
#include <genesis/utils/color/color.hpp>
#include <genesis/utils/containers/matrix.hpp>
#include <genesis/utils/containers/matrix/row.hpp>
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

void bind____python_custom_bindings_template_instances_matrix(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// template_matrix_uint32_t(class genesis::utils::Matrix<unsigned int>) file:../python/custom_bindings/template_instances/matrix.hpp line:9
	M("").def("template_matrix_uint32_t", (void (*)(class genesis::utils::Matrix<unsigned int>)) &template_matrix_uint32_t, "C++: template_matrix_uint32_t(class genesis::utils::Matrix<unsigned int>) --> void", pybind11::arg(""));

	// template_matrix_int(class genesis::utils::Matrix<int>) file:../python/custom_bindings/template_instances/matrix.hpp line:10
	M("").def("template_matrix_int", (void (*)(class genesis::utils::Matrix<int>)) &template_matrix_int, "C++: template_matrix_int(class genesis::utils::Matrix<int>) --> void", pybind11::arg(""));

	// template_matrix_row_uint32_t(class genesis::utils::MatrixRow<class genesis::utils::Matrix<unsigned int>, unsigned int>) file:../python/custom_bindings/template_instances/matrix.hpp line:11
	M("").def("template_matrix_row_uint32_t", (void (*)(class genesis::utils::MatrixRow<class genesis::utils::Matrix<unsigned int>, unsigned int>)) &template_matrix_row_uint32_t, "C++: template_matrix_row_uint32_t(class genesis::utils::MatrixRow<class genesis::utils::Matrix<unsigned int>, unsigned int>) --> void", pybind11::arg(""));

	// template_matrix_size_t_t(class genesis::utils::Matrix<unsigned long>) file:../python/custom_bindings/template_instances/matrix.hpp line:12
	M("").def("template_matrix_size_t_t", (void (*)(class genesis::utils::Matrix<unsigned long>)) &template_matrix_size_t_t, "C++: template_matrix_size_t_t(class genesis::utils::Matrix<unsigned long>) --> void", pybind11::arg(""));

	// template_matrix_double_t(class genesis::utils::Matrix<double>) file:../python/custom_bindings/template_instances/matrix.hpp line:13
	M("").def("template_matrix_double_t", (void (*)(class genesis::utils::Matrix<double>)) &template_matrix_double_t, "C++: template_matrix_double_t(class genesis::utils::Matrix<double>) --> void", pybind11::arg(""));

	// template_matrix_signed_char_t(class genesis::utils::Matrix<signed char>) file:../python/custom_bindings/template_instances/matrix.hpp line:14
	M("").def("template_matrix_signed_char_t", (void (*)(class genesis::utils::Matrix<signed char>)) &template_matrix_signed_char_t, "C++: template_matrix_signed_char_t(class genesis::utils::Matrix<signed char>) --> void", pybind11::arg(""));

	// template_matrix_color_t(class genesis::utils::Matrix<class genesis::utils::Color>) file:../python/custom_bindings/template_instances/matrix.hpp line:15
	M("").def("template_matrix_color_t", (void (*)(class genesis::utils::Matrix<class genesis::utils::Color>)) &template_matrix_color_t, "C++: template_matrix_color_t(class genesis::utils::Matrix<class genesis::utils::Color>) --> void", pybind11::arg(""));

	// template_matrix_pybind11_object_t(class genesis::utils::Matrix<class pybind11::object>) file:../python/custom_bindings/template_instances/matrix.hpp line:17
	M("").def("template_matrix_pybind11_object_t", (void (*)(class genesis::utils::Matrix<class pybind11::object>)) &template_matrix_pybind11_object_t, "C++: template_matrix_pybind11_object_t(class genesis::utils::Matrix<class pybind11::object>) --> void", pybind11::arg(""));

	// template_char_lookup_unsigned_char(class genesis::utils::CharLookup<unsigned char>) file:../python/custom_bindings/template_instances/char_lookup.hpp line:3
	M("").def("template_char_lookup_unsigned_char", (void (*)(class genesis::utils::CharLookup<unsigned char>)) &template_char_lookup_unsigned_char, "C++: template_char_lookup_unsigned_char(class genesis::utils::CharLookup<unsigned char>) --> void", pybind11::arg(""));

	// template_char_lookup_bool(class genesis::utils::CharLookup<bool>) file:../python/custom_bindings/template_instances/char_lookup.hpp line:4
	M("").def("template_char_lookup_bool", (void (*)(class genesis::utils::CharLookup<bool>)) &template_char_lookup_bool, "C++: template_char_lookup_bool(class genesis::utils::CharLookup<bool>) --> void", pybind11::arg(""));

	// template_matrix_multiplication_t() file:../python/custom_bindings/template_instances/math_matrix.hpp line:7
	M("").def("template_matrix_multiplication_t", (void (*)()) &template_matrix_multiplication_t, "C++: template_matrix_multiplication_t() --> void");

}

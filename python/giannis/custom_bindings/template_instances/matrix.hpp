#ifndef TEMPLATE_INSTANCE_MATRIX_INCLUDED_
#define TEMPLATE_INSTANCE_MATRIX_INCLUDED_

#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/containers/matrix/row.hpp"
#include "genesis/utils/color/color.hpp"
#include <pybind11/pybind11.h>

void template_matrix_uint32_t( ::genesis::utils::Matrix< uint32_t > ) {}
void template_matrix_int( ::genesis::utils::Matrix< int > ) {}
void template_matrix_row_uint32_t( ::genesis::utils::MatrixRow< ::genesis::utils::Matrix<uint32_t>, uint32_t> ) {}
void template_matrix_size_t_t( ::genesis::utils::Matrix< size_t > ) {}
void template_matrix_double_t( ::genesis::utils::Matrix< double >) {}
void template_matrix_signed_char_t( ::genesis::utils::Matrix< signed char > ) {}
void template_matrix_color_t( ::genesis::utils::Matrix< ::genesis::utils::Color > ) {}

void template_matrix_pybind11_object_t( ::genesis::utils::Matrix< pybind11::object > ) {}

#endif

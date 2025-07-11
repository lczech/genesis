#ifndef TEMPLATE_INSTANCE_MATH_MATRIX_INCLUDED_
#define TEMPLATE_INSTANCE_MATH_MATRIX_INCLUDED_

#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/math/matrix.hpp"

void template_matrix_multiplication_t()
{
	::genesis::utils::Matrix<signed char> a;
	std::vector<double> b;
	::genesis::utils::matrix_multiplication< double, signed char, double >( a, b);
}

#endif

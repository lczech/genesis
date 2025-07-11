#ifndef CUSTOM_BINDING_MATRIX_INCLUDED_
#define CUSTOM_BINDING_MATRIX_INCLUDED_

#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/containers/matrix/operators.hpp"
#include "genesis/utils/color/color.hpp"

template<class T>
inline void extend_matrix( pybind11::class_<T, std::shared_ptr<T> >& cl )
{
	cl.def("__iter__", [](::genesis::utils::Matrix<pybind11::object>& mi) { return pybind11::make_iterator(mi.begin(), mi.end()); }, pybind11::keep_alive<0, 1>());

	cl.def("__getitem__", [](::genesis::utils::Matrix< ::genesis::utils::Color >& self, std::pair<int,int> p) { return self( p.first, p.second); }, pybind11::return_value_policy::reference );
	cl.def("__setitem__", [](::genesis::utils::Matrix< ::genesis::utils::Color >& self, std::pair<int,int> p, ::genesis::utils::Color& c)
	{
		self( p.first, p.second) = c;
	});
}

inline void extend_matrix_operations(pybind11::module &handle)
{
	handle.def("swap_rows_Matrix_int_t", &::genesis::utils::swap_rows<int>);
	handle.def("swap_cols_Matrix_int_t", &::genesis::utils::swap_cols<int>);
}

#endif

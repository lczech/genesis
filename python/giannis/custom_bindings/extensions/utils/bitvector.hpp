#ifndef CUSTOM_BINDING_BITVECTOR_INCLUDED_
#define CUSTOM_BINDING_BITVECTOR_INCLUDED_

#include <vector>
#include "genesis/utils/math/bitvector.hpp"

template<class C>
inline void extend_bitvector( C& cl )
{
	cl.def( pybind11::init( [](std::vector<bool> const& input) { return ::genesis::utils::Bitvector(input.begin(), input.end()); }), pybind11::arg("values"));

	cl.def( "__and__", [](::genesis::utils::Bitvector &self, ::genesis::utils::Bitvector const& rhs) { return self & rhs; }, pybind11::arg("rhs") );
	cl.def( "__or__", [](::genesis::utils::Bitvector &self, ::genesis::utils::Bitvector const& rhs) { return self | rhs; }, pybind11::arg("rhs") );
	cl.def( "__xor__", [](::genesis::utils::Bitvector &self, ::genesis::utils::Bitvector const& rhs) { return self ^ rhs; }, pybind11::arg("rhs") );
}

#endif

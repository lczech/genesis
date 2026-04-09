#ifndef CUSTOM_BINDING_FUNCTIONS_TREE_INCLUDED_
#define CUSTOM_BINDING_FUNCTIONS_TREE_INCLUDED_

#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/tree.hpp"

inline void extend_tree_functions(pybind11::module &handle)
{
	handle.def("lowest_common_ancestors", [](::genesis::tree::Tree const& arg) -> ::genesis::utils::Matrix<size_t>
	{
		return ::genesis::tree::lowest_common_ancestors(arg);
	});
}

#endif

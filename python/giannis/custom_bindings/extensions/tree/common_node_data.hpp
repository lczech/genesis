#ifndef CUSTOM_BINDING_COMMON_NODE_DATA_INCLUDED_
#define CUSTOM_BINDING_COMMON_NODE_DATA_INCLUDED_

#include <pybind11/pybind11.h>
#include "genesis/tree/common_tree/tree.hpp"

template<class T>
inline void extend_common_node_data( T& cl )
{
	cl.def("clone", [](::genesis::tree::CommonNodeData& self)
	{
		::genesis::tree::CommonNodeData *toReturn = (::genesis::tree::CommonNodeData*) self.clone().release();
		return *toReturn;
	}, pybind11::return_value_policy::move);
}

#endif

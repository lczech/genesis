#ifndef CUSTOM_BINDING_TREE_INCLUDED_
#define CUSTOM_BINDING_TREE_INCLUDED_

#include "genesis/utils/containers/range.hpp"
#include "genesis/tree/tree.hpp"

template<class T>
inline void extend_tree( pybind11::class_<T, std::shared_ptr<T> >& cl )
{
	cl.def("nodes", [](::genesis::tree::Tree& self)
	{
		return pybind11::make_iterator( self.nodes().begin(), self.nodes().end());
	}, pybind11::keep_alive<0,1>());

	cl.def("edges", [](::genesis::tree::Tree& self)
        {
                return pybind11::make_iterator( self.edges().begin(), self.edges().end());
        }, pybind11::keep_alive<0,1>());
}

#endif

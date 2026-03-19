#ifndef CUSTOM_BINDING_RANGE_INCLUDED_
#define CUSTOM_BINDING_RANGE_INCLUDED_

#include "genesis/utils/containers/range.hpp"

#include "genesis/taxonomy/iterator/preorder.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/taxonomy/taxon.hpp"

#include "genesis/tree/iterator/path.hpp"
#include "genesis/tree/iterator/path_set.hpp"

#include "genesis/placement/pquery.hpp"

template<class T>
inline void extend_range( T& cl )
{
	cl.def("__iter__", []( ::genesis::utils::Range< std::vector< ::genesis::placement::Pquery >::iterator >& r)
	{
		return pybind11::make_iterator(r.begin(), r.end());
	}, pybind11::keep_alive<0, 1>());

	cl.def("__iter__", []( ::genesis::utils::Range< ::genesis::taxonomy::IteratorPreorder< ::genesis::taxonomy::Taxonomy const, ::genesis::taxonomy::Taxon const >>& r)
	{
		return pybind11::make_iterator(r.begin(), r.end());
	}, pybind11::keep_alive<0, 1>());

	cl.def("__iter__", []( ::genesis::utils::Range< ::genesis::taxonomy::IteratorPreorder< ::genesis::taxonomy::Taxonomy, ::genesis::taxonomy::Taxon >>& r)
	{
		return pybind11::make_iterator(r.begin(), r.end());
	}, pybind11::keep_alive<0, 1>());

	cl.def("__iter__", [](::genesis::utils::Range< ::genesis::tree::IteratorPath<false> >& r) { return pybind11::make_iterator(r.begin(), r.end()); }, pybind11::keep_alive<0, 1>());
	cl.def("__iter__", [](::genesis::utils::Range< ::genesis::tree::IteratorPath<true> >& r) { return pybind11::make_iterator(r.begin(), r.end()); }, pybind11::keep_alive<0, 1>());

	cl.def("__iter__", [](::genesis::utils::Range< ::genesis::tree::IteratorPathSet<false> >& r) { return pybind11::make_iterator(r.begin(), r.end()); }, pybind11::keep_alive<0, 1>());
	cl.def("__iter__", [](::genesis::utils::Range< ::genesis::tree::IteratorPathSet<true> >& r) { return pybind11::make_iterator(r.begin(), r.end()); }, pybind11::keep_alive<0, 1>());

}

#endif

#ifndef CUSTOM_BINDING_FUNCTIONS_TAXONOMY_INCLUDED_
#define CUSTOM_BINDING_FUNCTIONS_TAXONOMY_INCLUDED_

#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/taxonomy/functions/taxonomy.hpp"

inline void extend_functions_taxonomy( pybind11::module &handle)
{
	handle.def(
		"find_taxon_by_name",
		[](::genesis::taxonomy::Taxonomy& tax, std::string& name) -> pybind11::object
		{
			auto t = ::genesis::taxonomy::find_taxon_by_name(tax,name);
			if (t != NULL) {
				return pybind11::cast(*t);
			} else {
				return pybind11::none();
			}
		}
	);
}

#endif

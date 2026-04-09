#ifndef CUSTOM_BINDING_TAXONOMY_ITERATOR_INCLUDED_
#define CUSTOM_BINDING_TAXONOMY_ITERATOR_INCLUDED_

#include "genesis/taxonomy/iterator/preorder.hpp"
#include "genesis/taxonomy/iterator/levelorder.hpp"
#include "genesis/taxonomy/iterator/postorder.hpp"

#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/taxonomy/taxon.hpp"

inline void preorder_for_each2( ::genesis::taxonomy::Taxonomy& tax, std::function< void( ::genesis::taxonomy::Taxon* )> fn, bool include_inner_taxa)
{
	for( auto& t : tax )
	{
		if( include_inner_taxa || t.size() == 0 )
		{
			fn( &t );
		}
		preorder_for_each2( t, fn, include_inner_taxa );
	}
}

inline void extend_iterator( pybind11::module &handle )
{
	handle.def(
		"preorder_for_each",
		[] ( ::genesis::taxonomy::Taxonomy& tax, std::function< void( ::genesis::taxonomy::Taxon* )> fn, bool include_inner_taxa = true)
		{
			preorder_for_each2( tax, fn, include_inner_taxa);
		},
		pybind11::arg("tax"),
		pybind11::arg("fn"),
		pybind11::arg("include_inner_taxa") = true);
}

#endif

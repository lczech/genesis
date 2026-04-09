#ifndef TEMPLATE_BINDING_RANGE_INCLUDED_
#define TEMPLATE_BINDING_RANGE_INCLUDED_

#include "genesis/utils/containers/range.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/iterator/preorder.hpp"
#include "genesis/placement/pquery.hpp"

#include <vector>

void template_range_iteratorpreorder_taxonomy_taxon_t()
{
	::genesis::utils::Range< std::vector< ::genesis::placement::Pquery >::iterator > c;
	::genesis::utils::Range< ::genesis::taxonomy::IteratorPreorder< ::genesis::taxonomy::Taxonomy, ::genesis::taxonomy::Taxon > > a;
	::genesis::utils::Range< ::genesis::taxonomy::IteratorPreorder< ::genesis::taxonomy::Taxonomy const, ::genesis::taxonomy::Taxon const >> b;
}

#endif

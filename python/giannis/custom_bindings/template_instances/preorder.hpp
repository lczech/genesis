#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/taxonomy/iterator/preorder.hpp"

void template_preorder_taxonomy_t()
{
	::genesis::taxonomy::Taxonomy tax;
	::genesis::taxonomy::preorder< ::genesis::taxonomy::Taxonomy>(tax);
}

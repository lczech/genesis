#ifndef TEMPLATE_BINDING_CHROMOSOME_ITERATOR_INCLUDED_
#define TEMPLATE_BINDING_CHROMOSOME_ITERATOR_INCLUDED_

#include "genesis/utils/containers/lambda_iterator.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/population/formats/variant_input_iterator.hpp"

#include "genesis/population/window/chromosome_iterator.hpp"

void template_chromosome_iterator_lambda_iterator_variant_variant_input_data()
{
	::genesis::utils::LambdaIterator< ::genesis::population::Variant, ::genesis::population::VariantInputIteratorData > a;
	::genesis::population::ChromosomeIterator<
		::genesis::utils::LambdaIterator< genesis::population::Variant, ::genesis::population::VariantInputIteratorData >::Iterator,
		::genesis::population::Variant
	> b = ::genesis::population::make_chromosome_iterator( a.begin(), a.end() );
//	::genesis::population::make_default_chromosome_iterator( a.begin() , a.end() );
}

#endif

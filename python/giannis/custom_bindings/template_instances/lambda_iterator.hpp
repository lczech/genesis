#ifndef TEMPLATE_BINDING_LAMBDA_ITERATOR_INCLUDED_
#define TEMPLATE_BINDING_LAMBDA_ITERATOR_INCLUDED_

#include "genesis/utils/containers/lambda_iterator.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/population/formats/variant_input_iterator.hpp"

void template_lambda_iterator_variant_variant_input_data()
{
	::genesis::utils::LambdaIterator< ::genesis::population::Variant, ::genesis::population::VariantInputIteratorData> a;
}

#endif

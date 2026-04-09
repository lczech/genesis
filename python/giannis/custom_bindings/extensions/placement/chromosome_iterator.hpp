#ifndef CUSTOM_BINDING_CHROMOSOME_ITERATOR_INCLUDED_
#define CUSTOM_BINDING_CHROMOSOME_ITERATOR_INCLUDED_

#include "genesis/population/window/chromosome_iterator.hpp"
#include "genesis/utils/containers/lambda_iterator.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/population/window/variant_window_iterator.hpp"

using genesis::population::ChromosomeIterator;

template<class T>
inline void extend_chromosome_iterator( T& cl )
{
	cl.def(
		"__iter__",

		[](ChromosomeIterator< genesis::utils::LambdaIterator<genesis::population::Variant, genesis::population::VariantInputIteratorData>::Iterator>& self)
		{
			return pybind11::make_iterator(self.begin(), self.end());
		},
		pybind11::keep_alive<0, 1>()
        );
}

#endif

#ifndef CUSTOM_BINDING_FASTA_INPUT_ITERATOR_INCLUDED_
#define CUSTOM_BINDING_FASTA_INPUT_ITERATOR_INCLUDED_

#include "genesis/sequence/formats/fasta_input_iterator.hpp"

template<class T>
inline void extend_fasta_input_iterator( pybind11::class_<T, std::shared_ptr<T> >& cl )
{
	cl.def(
		"__iter__",
		[](::genesis::sequence::FastaInputIterator& self)
		{
			return pybind11::make_iterator(self.begin(), self.end());
		},
		pybind11::keep_alive<0, 1>()
	);
}

#endif

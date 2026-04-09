#ifndef CUSTOM_BINDING_FASTA_OUTPUT_ITERATOR_INCLUDED_
#define CUSTOM_BINDING_FASTA_OUTPUT_ITERATOR_INCLUDED_

#include "genesis/sequence/formats/fasta_output_iterator.hpp"

template<class T>
inline void extend_fasta_output_iterator( pybind11::class_<T, std::shared_ptr<T> >& cl )
{
	cl.def(
		pybind11::init<class std::shared_ptr<class genesis::utils::BaseOutputTarget>>(),
		pybind11::arg("target"),
		pybind11::keep_alive<1,2>()
	);

	cl.def(
		pybind11::init<class std::shared_ptr<class genesis::utils::BaseOutputTarget>,
		const class genesis::sequence::FastaWriter &>(),
		pybind11::arg("target"),
		pybind11::arg("writer"),
		pybind11::keep_alive<1,2>()
	);

	cl.def(
		pybind11::init( [](genesis::sequence::FastaOutputIterator const &o)
		{
			return new genesis::sequence::FastaOutputIterator(o);
		})
	);
}

#endif

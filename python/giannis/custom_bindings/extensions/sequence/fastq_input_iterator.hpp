#ifndef CUSTOM_BINDING_FASTQ_INPUT_ITERATOR_INCLUDED_
#define CUSTOM_BINDING_FASTQ_INPUT_ITERATOR_INCLUDED_

#include "genesis/sequence/formats/fastq_input_iterator.hpp"

template<class T>
inline void extend_fastq_input_iterator( pybind11::class_<T, std::shared_ptr<T> >& cl )
{
        cl.def(
                "__iter__",
                [](::genesis::sequence::FastqInputIterator& self)
                {
                        return pybind11::make_iterator(self.begin(), self.end());
                },
                pybind11::keep_alive<0, 1>()
        );
}

#endif

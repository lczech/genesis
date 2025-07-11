#ifndef CUSTOM_BINDING_SEQUENCE_SET_INCLUDED_
#define CUSTOM_BINDING_SEQUENCE_SET_INCLUDED_

#include "genesis/sequence/sequence_set.hpp"

template<class T>
inline void extend_sequence_set( pybind11::class_<T, std::shared_ptr<T> >& cl )
{
        cl.def(
                "__iter__",
                [](::genesis::sequence::SequenceSet& self)
                {
                        return pybind11::make_iterator(self.begin(), self.end());
                },
                pybind11::keep_alive<0, 1>()
        );
}

#endif

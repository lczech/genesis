#ifndef CUSTOM_BINDING_SEQUENCE_DICT_INCLUDED_
#define CUSTOM_BINDING_SEQUENCE_DICT_INCLUDED_

#include <string>
#include "genesis/sequence/sequence_dict.hpp"

template<class T>
inline void extend_sequence_dict( pybind11::class_<T, std::shared_ptr<T> >& cl )
{
	cl.def(
		"__iter__",
		[](::genesis::sequence::SequenceDict& self)
		{
			return pybind11::make_iterator(self.begin(), self.end());
		},
		pybind11::keep_alive<0, 1>()
	);

	cl.def(
		"begin",
		[](::genesis::sequence::SequenceDict& self)
		{
			return *(self.begin());
		}
	);

	cl.def(
		"end",
		[](::genesis::sequence::SequenceDict& self)
		{
			return new pybind11::none;

		}
	);

	cl.def(
		"find",
		[](::genesis::sequence::SequenceDict& self, std::string name) -> pybind11::object
		{
			if (self.find(name) != self.end()) {
				return pybind11::cast(*(self.find(name)));
			} else {
				return pybind11::none();
			}
		},
		pybind11::arg("name")
	);

	cl.def(
		"__contains__",
		[](::genesis::sequence::SequenceDict& self, std::string name)
		{
			if (self.find(name) != self.end()) {
				return true;
			} else {
				return false;
			}
		},
		pybind11::arg("name")
	);
}

#endif

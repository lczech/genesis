#ifndef CUSTOM_BINDING_REFERENCE_GENOME_INCLUDED_
#define CUSTOM_BINDING_REFERENCE_GENOME_INCLUDED_

#include <string>
#include "genesis/sequence/reference_genome.hpp"

template<class T>
inline void extend_reference_genome( pybind11::class_<T, std::shared_ptr<T> >& cl )
{
	cl.def(
		"begin",
		[](::genesis::sequence::ReferenceGenome& self)
		{
			return *(self.begin());
		}
	);

	cl.def(
		"end",
		[](::genesis::sequence::ReferenceGenome& self)
		{
			return new pybind11::none;
		}
	);

	cl.def(
		"find",
		[](::genesis::sequence::ReferenceGenome& self, std::string name) -> pybind11::object
		{
			if (self.find(name) != self.end()) {
				return pybind11::cast(*(self.find(name)));
			} else {
				return pybind11::none();
			}
		},
		pybind11::arg("label")
	);

	cl.def(
		"__contains__",
		[](::genesis::sequence::ReferenceGenome& self, std::string name)
		{
			if (self.find(name) != self.end()) {
				return true;
			} else {
				return false;
			}
		},
		pybind11::arg("label")
	);
}

#endif

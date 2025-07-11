#ifndef CUSTOM_BINDING_TAXOPATH_INCLUDED_
#define CUSTOM_BINDING_TAXOPATH_INCLUDED_

#include "genesis/taxonomy/taxopath.hpp"

template<class T>
inline void extend_taxopath( pybind11::class_<T, std::shared_ptr<T> >& cl )
{
   cl.def(
      "__eq__",
      [](::genesis::taxonomy::Taxopath const & self, ::genesis::taxonomy::Taxopath const & rhs)
      {
         return self == rhs;
      },
      pybind11::arg("rhs")
   );

	cl.def(
		"__ne__",
		[](::genesis::taxonomy::Taxopath const & self, ::genesis::taxonomy::Taxopath const & rhs)
		{
			return self != rhs;
		},
		pybind11::arg("rhs")
	);

	cl.def(
		"__lt__",
		[](::genesis::taxonomy::Taxopath const & self, ::genesis::taxonomy::Taxopath const & rhs)
		{
			return self <  rhs;
		},
		pybind11::arg("rhs")
	);
	
	cl.def(
		"__gt__",
		[](::genesis::taxonomy::Taxopath const & self, ::genesis::taxonomy::Taxopath const & rhs)
		{
			return self >  rhs;
		},
		pybind11::arg("rhs")
	);
	
	cl.def(
		"__ge__",
		[](::genesis::taxonomy::Taxopath const & self, ::genesis::taxonomy::Taxopath const & rhs)
		{
			return self >= rhs;
		},
		pybind11::arg("rhs")
	);
	
	cl.def(
		"__le__",
		[](::genesis::taxonomy::Taxopath const & self, ::genesis::taxonomy::Taxopath const & rhs)
		{
			return self <= rhs;
		},
		pybind11::arg("rhs")
	);
}

#endif

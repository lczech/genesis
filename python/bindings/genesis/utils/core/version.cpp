#include <genesis/utils/core/version.hpp>
#include <iterator>
#include <memory>
#include <string>

#include <functional>
#include <pybind11/pybind11.h>
#include <string>
#include <pybind11/stl.h>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>, false)
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*, false)
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>)
#endif

void bind_genesis_utils_core_version(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::genesis_version() file:genesis/utils/core/version.hpp line:63
	M("genesis").def("genesis_version", (std::string (*)()) &genesis::genesis_version, "Return the current genesis version.\n\n We use [semantic versioning](http://semver.org/) 2.0.0 for genesis.\n\n Given a version number MAJOR.MINOR.PATCH, increment the:\n\n  *  MAJOR version when you make incompatible API changes,\n  *  MINOR version when you add functionality in a backwards-compatible manner, and\n  *  PATCH version when you make backwards-compatible bug fixes.\n\n Additional labels for pre-release and build metadata are available as extensions to the\n MAJOR.MINOR.PATCH format.\n\nC++: genesis::genesis_version() --> std::string");

	// genesis::genesis_version_name() file:genesis/utils/core/version.hpp line:74
	M("genesis").def("genesis_version_name", (std::string (*)()) &genesis::genesis_version_name, "Return the current genenis version name.\n\n Just for fun.\n\nC++: genesis::genesis_version_name() --> std::string");

	// genesis::genesis_url() file:genesis/utils/core/version.hpp line:83
	M("genesis").def("genesis_url", (std::string (*)()) &genesis::genesis_url, "Return the URL of the genesis home page.\n\nC++: genesis::genesis_url() --> std::string");

	// genesis::genesis_header() file:genesis/utils/core/version.hpp line:94
	M("genesis").def("genesis_header", (std::string (*)()) &genesis::genesis_header, "Return the header for genesis.\n\n This is simply a text version of the logo, including the current version. It can for example be\n displayed at the start of a program to indicate that this program uses genesis.\n\nC++: genesis::genesis_header() --> std::string");

	// genesis::genesis_license() file:genesis/utils/core/version.hpp line:114
	M("genesis").def("genesis_license", (std::string (*)()) &genesis::genesis_license, "Return the genesis license boilerplate information.\n\n This function is useful for programs with terminal interaction. In such cases, you should include\n a command to show this license information.\n\nC++: genesis::genesis_license() --> std::string");

	// genesis::ee(int) file:genesis/utils/core/version.hpp line:139
	M("genesis").def("ee", (std::string (*)(int)) &genesis::ee, "C++: genesis::ee(int) --> std::string", pybind11::arg("r"));

}

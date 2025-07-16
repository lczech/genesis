#include <custom/utils/extra_bindings.hpp>
#include <genesis/utils/io/base_input_source.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
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

void bind_genesis_utils_io_base_input_source(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::utils::BaseInputSource file:genesis/utils/io/base_input_source.hpp line:50
		pybind11::class_<genesis::utils::BaseInputSource, std::shared_ptr<genesis::utils::BaseInputSource>> cl(M("genesis::utils"), "BaseInputSource", "Abstract base class for reading byte data from input sources.\n\n It offers to read() a certain amount of bytes into a char buffer.");
		// function-signature: class genesis::utils::BaseInputSource & genesis::utils::BaseInputSource::operator=(const class genesis::utils::BaseInputSource &)(const class genesis::utils::BaseInputSource &) file:genesis/utils/io/base_input_source.hpp line:63
		cl.def("assign", (class genesis::utils::BaseInputSource & (genesis::utils::BaseInputSource::*)(const class genesis::utils::BaseInputSource &)) &genesis::utils::BaseInputSource::operator=, "C++: genesis::utils::BaseInputSource::operator=(const class genesis::utils::BaseInputSource &) --> class genesis::utils::BaseInputSource &", pybind11::return_value_policy::automatic, pybind11::arg(""));
		// function-signature: std::string genesis::utils::BaseInputSource::source_name() const() file:genesis/utils/io/base_input_source.hpp line:88
		cl.def("source_name", (std::string (genesis::utils::BaseInputSource::*)() const) &genesis::utils::BaseInputSource::source_name, "Get a name of the input source. This is intended for user output.\n\nC++: genesis::utils::BaseInputSource::source_name() const --> std::string");
		// function-signature: std::string genesis::utils::BaseInputSource::source_string() const() file:genesis/utils/io/base_input_source.hpp line:98
		cl.def("source_string", (std::string (genesis::utils::BaseInputSource::*)() const) &genesis::utils::BaseInputSource::source_string, "Get a string representing the input source. This is intended for the reader classes,\n which for example might want to examine the input file name.\n\nC++: genesis::utils::BaseInputSource::source_string() const --> std::string");
		// function-signature: bool genesis::utils::BaseInputSource::is_trivial() const() file:genesis/utils/io/base_input_source.hpp line:116
		cl.def("is_trivial", (bool (genesis::utils::BaseInputSource::*)() const) &genesis::utils::BaseInputSource::is_trivial, "Return whether reading from this source is trivial, such as reading from a file,\n or involves more complex operations, such as reading from a compressed input that needs\n to be decompressed first.\n\n This helps downstream processes to decide if reading can be done in a cheap async thread,\n or needs to use the global thread pool in order to not oversubscribe the system when many\n such inputs (e.g., many gzipped files) are read in parallel. A thread that is just reading\n a block of data from a file is not using computation, and will spend its time in I/O wait,\n so it's okay to have many of those. However, compressed files need computation, and are\n hence not trivial.\n\nC++: genesis::utils::BaseInputSource::is_trivial() const --> bool");
	}
}

#include <iterator>
#include <memory>
#include <sstream> // __str__
#include <stdexcept>
#include <string>
#include <system_error>

#include <functional>
#include <pybind11/pybind11.h>
#include <string>
#include <pybind11/functional.h>
#include <../python/custom_bindings/extensions/utils/matrix.hpp>
#include <genesis/utils/color/functions.hpp>
#include <../python/custom_bindings/extensions/utils/bitvector.hpp>
#include <../python/custom_bindings/extensions/utils/range.hpp>
#include <../python/custom_bindings/extensions/string_target.hpp>
#include <../python/custom_bindings/extensions/sequence/quality.hpp>
#include <../python/custom_bindings/extensions/sequence/sequence_set.hpp>
#include <../python/custom_bindings/extensions/sequence/sequence_dict.hpp>
#include <../python/custom_bindings/extensions/sequence/fasta_input_iterator.hpp>
#include <../python/custom_bindings/extensions/sequence/fastq_input_iterator.hpp>
#include <../python/custom_bindings/extensions/sequence/reference_genome.hpp>
#include <../python/custom_bindings/extensions/taxonomy/taxopath.hpp>
#include <../python/custom_bindings/extensions/taxonomy/iterator.hpp>
#include <../python/custom_bindings/extensions/tree/tree.hpp>
#include <../python/custom_bindings/extensions/tree/functions_tree.hpp>
#include <../python/custom_bindings/extensions/placement/helper.hpp>
#include <pybind11/stl.h>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>)
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*)
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>)
#endif

// std::runtime_error file:stdexcept line:219
struct PyCallBack_std_runtime_error : public std::runtime_error {
	using std::runtime_error::runtime_error;

	const char * what() const throw() override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const std::runtime_error *>(this), "what");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<const char *>::value) {
				static pybind11::detail::override_caster_t<const char *> caster;
				return pybind11::detail::cast_ref<const char *>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<const char *>(std::move(o));
		}
		return runtime_error::what();
	}
};

// std::system_error file:system_error line:446
struct PyCallBack_std_system_error : public std::system_error {
	using std::system_error::system_error;

	const char * what() const throw() override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const std::system_error *>(this), "what");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<const char *>::value) {
				static pybind11::detail::override_caster_t<const char *> caster;
				return pybind11::detail::cast_ref<const char *>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<const char *>(std::move(o));
		}
		return runtime_error::what();
	}
};

void bind_std_stdexcept(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // std::runtime_error file:stdexcept line:219
		pybind11::class_<std::runtime_error, std::shared_ptr<std::runtime_error>, PyCallBack_std_runtime_error, std::exception> cl(M("std"), "runtime_error", "");
		cl.def( pybind11::init<const std::string &>(), pybind11::arg("__arg") );

		cl.def( pybind11::init<const char *>(), pybind11::arg("") );

		cl.def( pybind11::init( [](PyCallBack_std_runtime_error const &o){ return new PyCallBack_std_runtime_error(o); } ) );
		cl.def( pybind11::init( [](std::runtime_error const &o){ return new std::runtime_error(o); } ) );
		cl.def("assign", (class std::runtime_error & (std::runtime_error::*)(const class std::runtime_error &)) &std::runtime_error::operator=, "C++: std::runtime_error::operator=(const class std::runtime_error &) --> class std::runtime_error &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("what", (const char * (std::runtime_error::*)() const) &std::runtime_error::what, "C++: std::runtime_error::what() const --> const char *", pybind11::return_value_policy::reference_internal);
	}
	{ // std::error_code file:system_error line:196
		pybind11::class_<std::error_code, std::shared_ptr<std::error_code>> cl(M("std"), "error_code", "");
		cl.def( pybind11::init( [](){ return new std::error_code(); } ) );
		cl.def( pybind11::init( [](std::error_code const &o){ return new std::error_code(o); } ) );
		cl.def("clear", (void (std::error_code::*)()) &std::error_code::clear, "C++: std::error_code::clear() --> void");
		cl.def("value", (int (std::error_code::*)() const) &std::error_code::value, "C++: std::error_code::value() const --> int");
		cl.def("default_error_condition", (class std::error_condition (std::error_code::*)() const) &std::error_code::default_error_condition, "C++: std::error_code::default_error_condition() const --> class std::error_condition");
		cl.def("message", (std::string (std::error_code::*)() const) &std::error_code::message, "C++: std::error_code::message() const --> std::string");
		cl.def("assign", (class std::error_code & (std::error_code::*)(const class std::error_code &)) &std::error_code::operator=, "C++: std::error_code::operator=(const class std::error_code &) --> class std::error_code &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
	{ // std::error_condition file:system_error line:297
		pybind11::class_<std::error_condition, std::shared_ptr<std::error_condition>> cl(M("std"), "error_condition", "");
		cl.def( pybind11::init( [](){ return new std::error_condition(); } ) );
		cl.def( pybind11::init( [](std::error_condition const &o){ return new std::error_condition(o); } ) );
		cl.def("clear", (void (std::error_condition::*)()) &std::error_condition::clear, "C++: std::error_condition::clear() --> void");
		cl.def("value", (int (std::error_condition::*)() const) &std::error_condition::value, "C++: std::error_condition::value() const --> int");
		cl.def("message", (std::string (std::error_condition::*)() const) &std::error_condition::message, "C++: std::error_condition::message() const --> std::string");
	}
	{ // std::system_error file:system_error line:446
		pybind11::class_<std::system_error, std::shared_ptr<std::system_error>, PyCallBack_std_system_error, std::runtime_error> cl(M("std"), "system_error", "");
		cl.def( pybind11::init( [](){ return new std::system_error(); }, [](){ return new PyCallBack_std_system_error(); } ), "doc");
		cl.def( pybind11::init<class std::error_code>(), pybind11::arg("__ec") );

		cl.def( pybind11::init<class std::error_code, const std::string &>(), pybind11::arg("__ec"), pybind11::arg("__what") );

		cl.def( pybind11::init<class std::error_code, const char *>(), pybind11::arg("__ec"), pybind11::arg("__what") );

		cl.def( pybind11::init( [](PyCallBack_std_system_error const &o){ return new PyCallBack_std_system_error(o); } ) );
		cl.def( pybind11::init( [](std::system_error const &o){ return new std::system_error(o); } ) );
		cl.def("assign", (class std::system_error & (std::system_error::*)(const class std::system_error &)) &std::system_error::operator=, "C++: std::system_error::operator=(const class std::system_error &) --> class std::system_error &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("code", (const class std::error_code & (std::system_error::*)() const) &std::system_error::code, "C++: std::system_error::code() const --> const class std::error_code &", pybind11::return_value_policy::reference_internal);
	}
}

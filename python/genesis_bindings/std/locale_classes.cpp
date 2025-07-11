#include <ios>
#include <iterator>
#include <locale>
#include <memory>
#include <sstream> // __str__
#include <streambuf>
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

// std::ios_base::failure file:bits/ios_base.h line:255
struct PyCallBack_std_ios_base_failure : public std::ios_base::failure {
	using std::ios_base::failure::failure;

	const char * what() const throw() override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const std::ios_base::failure *>(this), "what");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<const char *>::value) {
				static pybind11::detail::override_caster_t<const char *> caster;
				return pybind11::detail::cast_ref<const char *>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<const char *>(std::move(o));
		}
		return failure::what();
	}
};

// std::basic_streambuf file:bits/streambuf.tcc line:149
struct PyCallBack_std_streambuf : public std::streambuf {
	using std::streambuf::basic_streambuf;

	void imbue(const class std::locale & a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const std::streambuf *>(this), "imbue");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<void>::value) {
				static pybind11::detail::override_caster_t<void> caster;
				return pybind11::detail::cast_ref<void>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<void>(std::move(o));
		}
		return basic_streambuf::imbue(a0);
	}
	class std::basic_streambuf<char> * setbuf(char * a0, long a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const std::streambuf *>(this), "setbuf");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<class std::basic_streambuf<char> *>::value) {
				static pybind11::detail::override_caster_t<class std::basic_streambuf<char> *> caster;
				return pybind11::detail::cast_ref<class std::basic_streambuf<char> *>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<class std::basic_streambuf<char> *>(std::move(o));
		}
		return basic_streambuf::setbuf(a0, a1);
	}
	int sync() override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const std::streambuf *>(this), "sync");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<int>::value) {
				static pybind11::detail::override_caster_t<int> caster;
				return pybind11::detail::cast_ref<int>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<int>(std::move(o));
		}
		return basic_streambuf::sync();
	}
	long showmanyc() override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const std::streambuf *>(this), "showmanyc");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<long>::value) {
				static pybind11::detail::override_caster_t<long> caster;
				return pybind11::detail::cast_ref<long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<long>(std::move(o));
		}
		return basic_streambuf::showmanyc();
	}
	long xsgetn(char * a0, long a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const std::streambuf *>(this), "xsgetn");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<long>::value) {
				static pybind11::detail::override_caster_t<long> caster;
				return pybind11::detail::cast_ref<long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<long>(std::move(o));
		}
		return basic_streambuf::xsgetn(a0, a1);
	}
	int underflow() override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const std::streambuf *>(this), "underflow");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<int>::value) {
				static pybind11::detail::override_caster_t<int> caster;
				return pybind11::detail::cast_ref<int>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<int>(std::move(o));
		}
		return basic_streambuf::underflow();
	}
	int uflow() override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const std::streambuf *>(this), "uflow");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<int>::value) {
				static pybind11::detail::override_caster_t<int> caster;
				return pybind11::detail::cast_ref<int>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<int>(std::move(o));
		}
		return basic_streambuf::uflow();
	}
	int pbackfail(int a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const std::streambuf *>(this), "pbackfail");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<int>::value) {
				static pybind11::detail::override_caster_t<int> caster;
				return pybind11::detail::cast_ref<int>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<int>(std::move(o));
		}
		return basic_streambuf::pbackfail(a0);
	}
	long xsputn(const char * a0, long a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const std::streambuf *>(this), "xsputn");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<long>::value) {
				static pybind11::detail::override_caster_t<long> caster;
				return pybind11::detail::cast_ref<long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<long>(std::move(o));
		}
		return basic_streambuf::xsputn(a0, a1);
	}
	int overflow(int a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const std::streambuf *>(this), "overflow");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<int>::value) {
				static pybind11::detail::override_caster_t<int> caster;
				return pybind11::detail::cast_ref<int>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<int>(std::move(o));
		}
		return basic_streambuf::overflow(a0);
	}
};

void bind_std_locale_classes(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // std::locale file:bits/locale_classes.h line:62
		pybind11::class_<std::locale, std::shared_ptr<std::locale>> cl(M("std"), "locale", "");
		cl.def( pybind11::init( [](){ return new std::locale(); } ) );
		cl.def( pybind11::init( [](std::locale const &o){ return new std::locale(o); } ) );
		cl.def( pybind11::init<const char *>(), pybind11::arg("__s") );

		cl.def( pybind11::init<const class std::locale &, const char *, int>(), pybind11::arg("__base"), pybind11::arg("__s"), pybind11::arg("__cat") );

		cl.def( pybind11::init<const std::string &>(), pybind11::arg("__s") );

		cl.def( pybind11::init<const class std::locale &, const std::string &, int>(), pybind11::arg("__base"), pybind11::arg("__s"), pybind11::arg("__cat") );

		cl.def( pybind11::init<const class std::locale &, const class std::locale &, int>(), pybind11::arg("__base"), pybind11::arg("__add"), pybind11::arg("__cat") );

		cl.def("assign", (const class std::locale & (std::locale::*)(const class std::locale &)) &std::locale::operator=, "C++: std::locale::operator=(const class std::locale &) --> const class std::locale &", pybind11::return_value_policy::reference_internal, pybind11::arg("__other"));
		cl.def("name", (std::string (std::locale::*)() const) &std::locale::name, "C++: std::locale::name() const --> std::string");
		cl.def("__eq__", (bool (std::locale::*)(const class std::locale &) const) &std::locale::operator==, "C++: std::locale::operator==(const class std::locale &) const --> bool", pybind11::arg("__other"));
		cl.def("__ne__", (bool (std::locale::*)(const class std::locale &) const) &std::locale::operator!=, "C++: std::locale::operator!=(const class std::locale &) const --> bool", pybind11::arg("__other"));
		cl.def_static("global", (class std::locale (*)(const class std::locale &)) &std::locale::global, "C++: std::locale::global(const class std::locale &) --> class std::locale", pybind11::arg("__loc"));
		cl.def_static("classic", (const class std::locale & (*)()) &std::locale::classic, "C++: std::locale::classic() --> const class std::locale &", pybind11::return_value_policy::reference_internal);

		{ // std::locale::id file:bits/locale_classes.h line:485
			auto & enclosing_class = cl;
			pybind11::class_<std::locale::id, std::shared_ptr<std::locale::id>> cl(enclosing_class, "id", "");
			cl.def( pybind11::init( [](){ return new std::locale::id(); } ) );
			cl.def("_M_id", (unsigned long (std::locale::id::*)() const) &std::locale::id::_M_id, "C++: std::locale::id::_M_id() const --> unsigned long");
		}

		{ // std::locale::_Impl file:bits/locale_classes.h line:524
			auto & enclosing_class = cl;
			pybind11::class_<std::locale::_Impl, std::locale::_Impl*> cl(enclosing_class, "_Impl", "");
		}

	}
	// std::_Ios_Fmtflags file:bits/ios_base.h line:57
	pybind11::enum_<std::_Ios_Fmtflags>(M("std"), "_Ios_Fmtflags", pybind11::arithmetic(), "")
		.value("_S_boolalpha", std::_S_boolalpha)
		.value("_S_dec", std::_S_dec)
		.value("_S_fixed", std::_S_fixed)
		.value("_S_hex", std::_S_hex)
		.value("_S_internal", std::_S_internal)
		.value("_S_left", std::_S_left)
		.value("_S_oct", std::_S_oct)
		.value("_S_right", std::_S_right)
		.value("_S_scientific", std::_S_scientific)
		.value("_S_showbase", std::_S_showbase)
		.value("_S_showpoint", std::_S_showpoint)
		.value("_S_showpos", std::_S_showpos)
		.value("_S_skipws", std::_S_skipws)
		.value("_S_unitbuf", std::_S_unitbuf)
		.value("_S_uppercase", std::_S_uppercase)
		.value("_S_adjustfield", std::_S_adjustfield)
		.value("_S_basefield", std::_S_basefield)
		.value("_S_floatfield", std::_S_floatfield)
		.value("_S_ios_fmtflags_end", std::_S_ios_fmtflags_end)
		.value("_S_ios_fmtflags_max", std::_S_ios_fmtflags_max)
		.value("_S_ios_fmtflags_min", std::_S_ios_fmtflags_min)
		.export_values();

;

	// std::_Ios_Openmode file:bits/ios_base.h line:111
	pybind11::enum_<std::_Ios_Openmode>(M("std"), "_Ios_Openmode", pybind11::arithmetic(), "")
		.value("_S_app", std::_S_app)
		.value("_S_ate", std::_S_ate)
		.value("_S_bin", std::_S_bin)
		.value("_S_in", std::_S_in)
		.value("_S_out", std::_S_out)
		.value("_S_trunc", std::_S_trunc)
		.value("_S_ios_openmode_end", std::_S_ios_openmode_end)
		.value("_S_ios_openmode_max", std::_S_ios_openmode_max)
		.value("_S_ios_openmode_min", std::_S_ios_openmode_min)
		.export_values();

;

	// std::_Ios_Iostate file:bits/ios_base.h line:153
	pybind11::enum_<std::_Ios_Iostate>(M("std"), "_Ios_Iostate", pybind11::arithmetic(), "")
		.value("_S_goodbit", std::_S_goodbit)
		.value("_S_badbit", std::_S_badbit)
		.value("_S_eofbit", std::_S_eofbit)
		.value("_S_failbit", std::_S_failbit)
		.value("_S_ios_iostate_end", std::_S_ios_iostate_end)
		.value("_S_ios_iostate_max", std::_S_ios_iostate_max)
		.value("_S_ios_iostate_min", std::_S_ios_iostate_min)
		.export_values();

;

	// std::_Ios_Seekdir file:bits/ios_base.h line:193
	pybind11::enum_<std::_Ios_Seekdir>(M("std"), "_Ios_Seekdir", pybind11::arithmetic(), "")
		.value("_S_beg", std::_S_beg)
		.value("_S_cur", std::_S_cur)
		.value("_S_end", std::_S_end)
		.value("_S_ios_seekdir_end", std::_S_ios_seekdir_end)
		.export_values();

;

	{ // std::ios_base file:bits/ios_base.h line:228
		pybind11::class_<std::ios_base, std::shared_ptr<std::ios_base>> cl(M("std"), "ios_base", "");

		pybind11::enum_<std::ios_base::event>(cl, "event", pybind11::arithmetic(), "")
			.value("erase_event", std::ios_base::erase_event)
			.value("imbue_event", std::ios_base::imbue_event)
			.value("copyfmt_event", std::ios_base::copyfmt_event)
			.export_values();

		cl.def("flags", (enum std::_Ios_Fmtflags (std::ios_base::*)() const) &std::ios_base::flags, "C++: std::ios_base::flags() const --> enum std::_Ios_Fmtflags");
		cl.def("flags", (enum std::_Ios_Fmtflags (std::ios_base::*)(enum std::_Ios_Fmtflags)) &std::ios_base::flags, "C++: std::ios_base::flags(enum std::_Ios_Fmtflags) --> enum std::_Ios_Fmtflags", pybind11::arg("__fmtfl"));
		cl.def("setf", (enum std::_Ios_Fmtflags (std::ios_base::*)(enum std::_Ios_Fmtflags)) &std::ios_base::setf, "C++: std::ios_base::setf(enum std::_Ios_Fmtflags) --> enum std::_Ios_Fmtflags", pybind11::arg("__fmtfl"));
		cl.def("setf", (enum std::_Ios_Fmtflags (std::ios_base::*)(enum std::_Ios_Fmtflags, enum std::_Ios_Fmtflags)) &std::ios_base::setf, "C++: std::ios_base::setf(enum std::_Ios_Fmtflags, enum std::_Ios_Fmtflags) --> enum std::_Ios_Fmtflags", pybind11::arg("__fmtfl"), pybind11::arg("__mask"));
		cl.def("unsetf", (void (std::ios_base::*)(enum std::_Ios_Fmtflags)) &std::ios_base::unsetf, "C++: std::ios_base::unsetf(enum std::_Ios_Fmtflags) --> void", pybind11::arg("__mask"));
		cl.def("precision", (long (std::ios_base::*)() const) &std::ios_base::precision, "C++: std::ios_base::precision() const --> long");
		cl.def("precision", (long (std::ios_base::*)(long)) &std::ios_base::precision, "C++: std::ios_base::precision(long) --> long", pybind11::arg("__prec"));
		cl.def("width", (long (std::ios_base::*)() const) &std::ios_base::width, "C++: std::ios_base::width() const --> long");
		cl.def("width", (long (std::ios_base::*)(long)) &std::ios_base::width, "C++: std::ios_base::width(long) --> long", pybind11::arg("__wide"));
		cl.def_static("sync_with_stdio", []() -> bool { return std::ios_base::sync_with_stdio(); }, "");
		cl.def_static("sync_with_stdio", (bool (*)(bool)) &std::ios_base::sync_with_stdio, "C++: std::ios_base::sync_with_stdio(bool) --> bool", pybind11::arg("__sync"));
		cl.def("imbue", (class std::locale (std::ios_base::*)(const class std::locale &)) &std::ios_base::imbue, "C++: std::ios_base::imbue(const class std::locale &) --> class std::locale", pybind11::arg("__loc"));
		cl.def("getloc", (class std::locale (std::ios_base::*)() const) &std::ios_base::getloc, "C++: std::ios_base::getloc() const --> class std::locale");
		cl.def("_M_getloc", (const class std::locale & (std::ios_base::*)() const) &std::ios_base::_M_getloc, "C++: std::ios_base::_M_getloc() const --> const class std::locale &", pybind11::return_value_policy::reference_internal);
		cl.def_static("xalloc", (int (*)()) &std::ios_base::xalloc, "C++: std::ios_base::xalloc() --> int");
		cl.def("iword", (long & (std::ios_base::*)(int)) &std::ios_base::iword, "C++: std::ios_base::iword(int) --> long &", pybind11::return_value_policy::reference_internal, pybind11::arg("__ix"));
		cl.def("pword", (void *& (std::ios_base::*)(int)) &std::ios_base::pword, "C++: std::ios_base::pword(int) --> void *&", pybind11::return_value_policy::reference_internal, pybind11::arg("__ix"));

		{ // std::ios_base::failure file:bits/ios_base.h line:255
			auto & enclosing_class = cl;
			pybind11::class_<std::ios_base::failure, std::shared_ptr<std::ios_base::failure>, PyCallBack_std_ios_base_failure, std::system_error> cl(enclosing_class, "failure", "");
			cl.def( pybind11::init<const std::string &>(), pybind11::arg("__str") );

			cl.def( pybind11::init<const std::string &, const class std::error_code &>(), pybind11::arg(""), pybind11::arg("") );

			cl.def( pybind11::init( [](const char * a0){ return new std::ios_base::failure(a0); }, [](const char * a0){ return new PyCallBack_std_ios_base_failure(a0); } ), "doc");
			cl.def( pybind11::init<const char *, const class std::error_code &>(), pybind11::arg(""), pybind11::arg("") );

			cl.def( pybind11::init( [](PyCallBack_std_ios_base_failure const &o){ return new PyCallBack_std_ios_base_failure(o); } ) );
			cl.def( pybind11::init( [](std::ios_base::failure const &o){ return new std::ios_base::failure(o); } ) );
			cl.def("what", (const char * (std::ios_base::failure::*)() const) &std::ios_base::failure::what, "C++: std::ios_base::failure::what() const --> const char *", pybind11::return_value_policy::reference_internal);
			cl.def("assign", (class std::ios_base::failure & (std::ios_base::failure::*)(const class std::ios_base::failure &)) &std::ios_base::failure::operator=, "C++: std::ios_base::failure::operator=(const class std::ios_base::failure &) --> class std::ios_base::failure &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		}

		{ // std::ios_base::Init file:bits/ios_base.h line:626
			auto & enclosing_class = cl;
			pybind11::class_<std::ios_base::Init, std::shared_ptr<std::ios_base::Init>> cl(enclosing_class, "Init", "");
			cl.def( pybind11::init( [](){ return new std::ios_base::Init(); } ) );
			cl.def( pybind11::init( [](std::ios_base::Init const &o){ return new std::ios_base::Init(o); } ) );
			cl.def("assign", (class std::ios_base::Init & (std::ios_base::Init::*)(const class std::ios_base::Init &)) &std::ios_base::Init::operator=, "C++: std::ios_base::Init::operator=(const class std::ios_base::Init &) --> class std::ios_base::Init &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		}

	}
	{ // std::basic_streambuf file:bits/streambuf.tcc line:149
		pybind11::class_<std::streambuf, std::shared_ptr<std::streambuf>, PyCallBack_std_streambuf> cl(M("std"), "streambuf", "");
		cl.def("pubimbue", (class std::locale (std::streambuf::*)(const class std::locale &)) &std::basic_streambuf<char, std::char_traits<char>>::pubimbue, "C++: std::basic_streambuf<char, std::char_traits<char>>::pubimbue(const class std::locale &) --> class std::locale", pybind11::arg("__loc"));
		cl.def("getloc", (class std::locale (std::streambuf::*)() const) &std::basic_streambuf<char, std::char_traits<char>>::getloc, "C++: std::basic_streambuf<char, std::char_traits<char>>::getloc() const --> class std::locale");
		cl.def("pubsetbuf", (class std::basic_streambuf<char> * (std::streambuf::*)(char *, long)) &std::basic_streambuf<char, std::char_traits<char>>::pubsetbuf, "C++: std::basic_streambuf<char, std::char_traits<char>>::pubsetbuf(char *, long) --> class std::basic_streambuf<char> *", pybind11::return_value_policy::reference_internal, pybind11::arg("__s"), pybind11::arg("__n"));
		cl.def("pubsync", (int (std::streambuf::*)()) &std::basic_streambuf<char, std::char_traits<char>>::pubsync, "C++: std::basic_streambuf<char, std::char_traits<char>>::pubsync() --> int");
		cl.def("in_avail", (long (std::streambuf::*)()) &std::basic_streambuf<char, std::char_traits<char>>::in_avail, "C++: std::basic_streambuf<char, std::char_traits<char>>::in_avail() --> long");
		cl.def("snextc", (int (std::streambuf::*)()) &std::basic_streambuf<char, std::char_traits<char>>::snextc, "C++: std::basic_streambuf<char, std::char_traits<char>>::snextc() --> int");
		cl.def("sbumpc", (int (std::streambuf::*)()) &std::basic_streambuf<char, std::char_traits<char>>::sbumpc, "C++: std::basic_streambuf<char, std::char_traits<char>>::sbumpc() --> int");
		cl.def("sgetc", (int (std::streambuf::*)()) &std::basic_streambuf<char, std::char_traits<char>>::sgetc, "C++: std::basic_streambuf<char, std::char_traits<char>>::sgetc() --> int");
		cl.def("sgetn", (long (std::streambuf::*)(char *, long)) &std::basic_streambuf<char, std::char_traits<char>>::sgetn, "C++: std::basic_streambuf<char, std::char_traits<char>>::sgetn(char *, long) --> long", pybind11::arg("__s"), pybind11::arg("__n"));
		cl.def("sputbackc", (int (std::streambuf::*)(char)) &std::basic_streambuf<char, std::char_traits<char>>::sputbackc, "C++: std::basic_streambuf<char, std::char_traits<char>>::sputbackc(char) --> int", pybind11::arg("__c"));
		cl.def("sungetc", (int (std::streambuf::*)()) &std::basic_streambuf<char, std::char_traits<char>>::sungetc, "C++: std::basic_streambuf<char, std::char_traits<char>>::sungetc() --> int");
		cl.def("sputc", (int (std::streambuf::*)(char)) &std::basic_streambuf<char, std::char_traits<char>>::sputc, "C++: std::basic_streambuf<char, std::char_traits<char>>::sputc(char) --> int", pybind11::arg("__c"));
		cl.def("sputn", (long (std::streambuf::*)(const char *, long)) &std::basic_streambuf<char, std::char_traits<char>>::sputn, "C++: std::basic_streambuf<char, std::char_traits<char>>::sputn(const char *, long) --> long", pybind11::arg("__s"), pybind11::arg("__n"));
		cl.def("stossc", (void (std::streambuf::*)()) &std::basic_streambuf<char, std::char_traits<char>>::stossc, "C++: std::basic_streambuf<char, std::char_traits<char>>::stossc() --> void");
		cl.def("__safe_gbump", (void (std::streambuf::*)(long)) &std::basic_streambuf<char, std::char_traits<char>>::__safe_gbump, "C++: std::basic_streambuf<char, std::char_traits<char>>::__safe_gbump(long) --> void", pybind11::arg("__n"));
		cl.def("__safe_pbump", (void (std::streambuf::*)(long)) &std::basic_streambuf<char, std::char_traits<char>>::__safe_pbump, "C++: std::basic_streambuf<char, std::char_traits<char>>::__safe_pbump(long) --> void", pybind11::arg("__n"));
	}
}

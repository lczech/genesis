#include <ios>
#include <iterator>
#include <locale>
#include <memory>
#include <random>
#include <sstream>
#include <sstream> // __str__
#include <streambuf>
#include <string>

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

// std::basic_stringbuf file:bits/sstream.tcc line:291
struct PyCallBack_std_stringbuf : public std::stringbuf {
	using std::stringbuf::basic_stringbuf;

	long showmanyc() override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const std::stringbuf *>(this), "showmanyc");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<long>::value) {
				static pybind11::detail::override_caster_t<long> caster;
				return pybind11::detail::cast_ref<long>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<long>(std::move(o));
		}
		return basic_stringbuf::showmanyc();
	}
	int underflow() override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const std::stringbuf *>(this), "underflow");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<int>::value) {
				static pybind11::detail::override_caster_t<int> caster;
				return pybind11::detail::cast_ref<int>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<int>(std::move(o));
		}
		return basic_stringbuf::underflow();
	}
	int pbackfail(int a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const std::stringbuf *>(this), "pbackfail");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<int>::value) {
				static pybind11::detail::override_caster_t<int> caster;
				return pybind11::detail::cast_ref<int>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<int>(std::move(o));
		}
		return basic_stringbuf::pbackfail(a0);
	}
	int overflow(int a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const std::stringbuf *>(this), "overflow");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<int>::value) {
				static pybind11::detail::override_caster_t<int> caster;
				return pybind11::detail::cast_ref<int>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<int>(std::move(o));
		}
		return basic_stringbuf::overflow(a0);
	}
	class std::basic_streambuf<char> * setbuf(char * a0, long a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const std::stringbuf *>(this), "setbuf");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0, a1);
			if (pybind11::detail::cast_is_temporary_value_reference<class std::basic_streambuf<char> *>::value) {
				static pybind11::detail::override_caster_t<class std::basic_streambuf<char> *> caster;
				return pybind11::detail::cast_ref<class std::basic_streambuf<char> *>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<class std::basic_streambuf<char> *>(std::move(o));
		}
		return basic_stringbuf::setbuf(a0, a1);
	}
	void imbue(const class std::locale & a0) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const std::stringbuf *>(this), "imbue");
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
	int sync() override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const std::stringbuf *>(this), "sync");
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
	long xsgetn(char * a0, long a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const std::stringbuf *>(this), "xsgetn");
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
	int uflow() override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const std::stringbuf *>(this), "uflow");
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
	long xsputn(const char * a0, long a1) override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const std::stringbuf *>(this), "xsputn");
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
};

void bind_std_random(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // std::linear_congruential_engine file:bits/random.h line:255
		pybind11::class_<std::linear_congruential_engine<unsigned long,16807,0,2147483647>, std::shared_ptr<std::linear_congruential_engine<unsigned long,16807,0,2147483647>>> cl(M("std"), "linear_congruential_engine_unsigned_long_16807_0_2147483647_t", "");
		cl.def( pybind11::init( [](){ return new std::linear_congruential_engine<unsigned long,16807,0,2147483647>(); } ) );
		cl.def( pybind11::init<unsigned long>(), pybind11::arg("__s") );

		cl.def( pybind11::init( [](std::linear_congruential_engine<unsigned long,16807,0,2147483647> const &o){ return new std::linear_congruential_engine<unsigned long,16807,0,2147483647>(o); } ) );
		cl.def("seed", [](std::linear_congruential_engine<unsigned long,16807,0,2147483647> &o) -> void { return o.seed(); }, "");
		cl.def("seed", (void (std::linear_congruential_engine<unsigned long,16807,0,2147483647>::*)(unsigned long)) &std::linear_congruential_engine<unsigned long, 16807, 0, 2147483647>::seed, "C++: std::linear_congruential_engine<unsigned long, 16807, 0, 2147483647>::seed(unsigned long) --> void", pybind11::arg("__s"));
		cl.def_static("min", (unsigned long (*)()) &std::linear_congruential_engine<unsigned long, 16807, 0, 2147483647>::min, "C++: std::linear_congruential_engine<unsigned long, 16807, 0, 2147483647>::min() --> unsigned long");
		cl.def_static("max", (unsigned long (*)()) &std::linear_congruential_engine<unsigned long, 16807, 0, 2147483647>::max, "C++: std::linear_congruential_engine<unsigned long, 16807, 0, 2147483647>::max() --> unsigned long");
		cl.def("discard", (void (std::linear_congruential_engine<unsigned long,16807,0,2147483647>::*)(unsigned long long)) &std::linear_congruential_engine<unsigned long, 16807, 0, 2147483647>::discard, "C++: std::linear_congruential_engine<unsigned long, 16807, 0, 2147483647>::discard(unsigned long long) --> void", pybind11::arg("__z"));
		cl.def("__call__", (unsigned long (std::linear_congruential_engine<unsigned long,16807,0,2147483647>::*)()) &std::linear_congruential_engine<unsigned long, 16807, 0, 2147483647>::operator(), "C++: std::linear_congruential_engine<unsigned long, 16807, 0, 2147483647>::operator()() --> unsigned long");
	}
	{ // std::basic_stringbuf file:bits/sstream.tcc line:291
		pybind11::class_<std::stringbuf, std::shared_ptr<std::stringbuf>, PyCallBack_std_stringbuf, std::streambuf> cl(M("std"), "stringbuf", "");
		cl.def( pybind11::init( [](){ return new std::stringbuf(); }, [](){ return new PyCallBack_std_stringbuf(); } ) );
		cl.def( pybind11::init<enum std::_Ios_Openmode>(), pybind11::arg("__mode") );

		cl.def( pybind11::init( [](const std::string & a0){ return new std::stringbuf(a0); }, [](const std::string & a0){ return new PyCallBack_std_stringbuf(a0); } ), "doc");
		cl.def( pybind11::init<const std::string &, enum std::_Ios_Openmode>(), pybind11::arg("__str"), pybind11::arg("__mode") );

		cl.def("swap", (void (std::stringbuf::*)(class std::basic_stringbuf<char> &)) &std::basic_stringbuf<char, std::char_traits<char>, std::allocator<char>>::swap, "C++: std::basic_stringbuf<char, std::char_traits<char>, std::allocator<char>>::swap(class std::basic_stringbuf<char> &) --> void", pybind11::arg("__rhs"));
		cl.def("str", (std::string (std::stringbuf::*)() const) &std::basic_stringbuf<char, std::char_traits<char>, std::allocator<char>>::str, "C++: std::basic_stringbuf<char, std::char_traits<char>, std::allocator<char>>::str() const --> std::string");
		cl.def("str", (void (std::stringbuf::*)(const std::string &)) &std::basic_stringbuf<char, std::char_traits<char>, std::allocator<char>>::str, "C++: std::basic_stringbuf<char, std::char_traits<char>, std::allocator<char>>::str(const std::string &) --> void", pybind11::arg("__s"));
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
	{ // std::basic_ostringstream file:bits/sstream.tcc line:293
		pybind11::class_<std::ostringstream, std::shared_ptr<std::ostringstream>, std::ostream> cl(M("std"), "ostringstream", "");
		cl.def( pybind11::init( [](){ return new std::ostringstream(); } ) );
		cl.def( pybind11::init<enum std::_Ios_Openmode>(), pybind11::arg("__mode") );

		cl.def( pybind11::init( [](const std::string & a0){ return new std::ostringstream(a0); } ), "doc" , pybind11::arg("__str"));
		cl.def( pybind11::init<const std::string &, enum std::_Ios_Openmode>(), pybind11::arg("__str"), pybind11::arg("__mode") );

		cl.def("swap", (void (std::ostringstream::*)(class std::basic_ostringstream<char> &)) &std::basic_ostringstream<char, std::char_traits<char>, std::allocator<char>>::swap, "C++: std::basic_ostringstream<char, std::char_traits<char>, std::allocator<char>>::swap(class std::basic_ostringstream<char> &) --> void", pybind11::arg("__rhs"));
		cl.def("rdbuf", (class std::basic_stringbuf<char> * (std::ostringstream::*)() const) &std::basic_ostringstream<char, std::char_traits<char>, std::allocator<char>>::rdbuf, "C++: std::basic_ostringstream<char, std::char_traits<char>, std::allocator<char>>::rdbuf() const --> class std::basic_stringbuf<char> *", pybind11::return_value_policy::reference_internal);
		cl.def("str", (std::string (std::ostringstream::*)() const) &std::basic_ostringstream<char, std::char_traits<char>, std::allocator<char>>::str, "C++: std::basic_ostringstream<char, std::char_traits<char>, std::allocator<char>>::str() const --> std::string");
		cl.def("str", (void (std::ostringstream::*)(const std::string &)) &std::basic_ostringstream<char, std::char_traits<char>, std::allocator<char>>::str, "C++: std::basic_ostringstream<char, std::char_traits<char>, std::allocator<char>>::str(const std::string &) --> void", pybind11::arg("__s"));
		cl.def("__lshift__", (std::ostream & (std::ostream::*)(long)) &std::basic_ostream<char, std::char_traits<char>>::operator<<, "C++: std::basic_ostream<char, std::char_traits<char>>::operator<<(long) --> std::ostream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__n"));
		cl.def("__lshift__", (std::ostream & (std::ostream::*)(unsigned long)) &std::basic_ostream<char, std::char_traits<char>>::operator<<, "C++: std::basic_ostream<char, std::char_traits<char>>::operator<<(unsigned long) --> std::ostream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__n"));
		cl.def("__lshift__", (std::ostream & (std::ostream::*)(bool)) &std::basic_ostream<char, std::char_traits<char>>::operator<<, "C++: std::basic_ostream<char, std::char_traits<char>>::operator<<(bool) --> std::ostream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__n"));
		cl.def("__lshift__", (std::ostream & (std::ostream::*)(short)) &std::basic_ostream<char, std::char_traits<char>>::operator<<, "C++: std::basic_ostream<char, std::char_traits<char>>::operator<<(short) --> std::ostream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__n"));
		cl.def("__lshift__", (std::ostream & (std::ostream::*)(unsigned short)) &std::basic_ostream<char, std::char_traits<char>>::operator<<, "C++: std::basic_ostream<char, std::char_traits<char>>::operator<<(unsigned short) --> std::ostream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__n"));
		cl.def("__lshift__", (std::ostream & (std::ostream::*)(int)) &std::basic_ostream<char, std::char_traits<char>>::operator<<, "C++: std::basic_ostream<char, std::char_traits<char>>::operator<<(int) --> std::ostream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__n"));
		cl.def("__lshift__", (std::ostream & (std::ostream::*)(unsigned int)) &std::basic_ostream<char, std::char_traits<char>>::operator<<, "C++: std::basic_ostream<char, std::char_traits<char>>::operator<<(unsigned int) --> std::ostream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__n"));
		cl.def("__lshift__", (std::ostream & (std::ostream::*)(long long)) &std::basic_ostream<char, std::char_traits<char>>::operator<<, "C++: std::basic_ostream<char, std::char_traits<char>>::operator<<(long long) --> std::ostream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__n"));
		cl.def("__lshift__", (std::ostream & (std::ostream::*)(unsigned long long)) &std::basic_ostream<char, std::char_traits<char>>::operator<<, "C++: std::basic_ostream<char, std::char_traits<char>>::operator<<(unsigned long long) --> std::ostream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__n"));
		cl.def("__lshift__", (std::ostream & (std::ostream::*)(double)) &std::basic_ostream<char, std::char_traits<char>>::operator<<, "C++: std::basic_ostream<char, std::char_traits<char>>::operator<<(double) --> std::ostream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__f"));
		cl.def("__lshift__", (std::ostream & (std::ostream::*)(float)) &std::basic_ostream<char, std::char_traits<char>>::operator<<, "C++: std::basic_ostream<char, std::char_traits<char>>::operator<<(float) --> std::ostream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__f"));
		cl.def("__lshift__", (std::ostream & (std::ostream::*)(long double)) &std::basic_ostream<char, std::char_traits<char>>::operator<<, "C++: std::basic_ostream<char, std::char_traits<char>>::operator<<(long double) --> std::ostream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__f"));
		cl.def("__lshift__", (std::ostream & (std::ostream::*)(const void *)) &std::basic_ostream<char, std::char_traits<char>>::operator<<, "C++: std::basic_ostream<char, std::char_traits<char>>::operator<<(const void *) --> std::ostream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__p"));
		cl.def("__lshift__", (std::ostream & (std::ostream::*)(class std::basic_streambuf<char> *)) &std::basic_ostream<char, std::char_traits<char>>::operator<<, "C++: std::basic_ostream<char, std::char_traits<char>>::operator<<(class std::basic_streambuf<char> *) --> std::ostream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__sb"));
		cl.def("put", (std::ostream & (std::ostream::*)(char)) &std::basic_ostream<char, std::char_traits<char>>::put, "C++: std::basic_ostream<char, std::char_traits<char>>::put(char) --> std::ostream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__c"));
		cl.def("_M_write", (void (std::ostream::*)(const char *, long)) &std::basic_ostream<char, std::char_traits<char>>::_M_write, "C++: std::basic_ostream<char, std::char_traits<char>>::_M_write(const char *, long) --> void", pybind11::arg("__s"), pybind11::arg("__n"));
		cl.def("write", (std::ostream & (std::ostream::*)(const char *, long)) &std::basic_ostream<char, std::char_traits<char>>::write, "C++: std::basic_ostream<char, std::char_traits<char>>::write(const char *, long) --> std::ostream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__s"), pybind11::arg("__n"));
		cl.def("flush", (std::ostream & (std::ostream::*)()) &std::basic_ostream<char, std::char_traits<char>>::flush, "C++: std::basic_ostream<char, std::char_traits<char>>::flush() --> std::ostream &", pybind11::return_value_policy::reference_internal);
		cl.def("seekp", (std::ostream & (std::ostream::*)(long, enum std::_Ios_Seekdir)) &std::basic_ostream<char, std::char_traits<char>>::seekp, "C++: std::basic_ostream<char, std::char_traits<char>>::seekp(long, enum std::_Ios_Seekdir) --> std::ostream &", pybind11::return_value_policy::reference_internal, pybind11::arg(""), pybind11::arg(""));
	}
}

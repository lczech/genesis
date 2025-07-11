#include <ios>
#include <istream>
#include <iterator>
#include <locale>
#include <memory>
#include <ostream>
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

void bind_std_basic_ios_tcc(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // std::basic_ios file:bits/basic_ios.tcc line:178
		pybind11::class_<std::basic_ios<char,std::char_traits<char>>, std::shared_ptr<std::basic_ios<char,std::char_traits<char>>>, std::ios_base> cl(M("std"), "basic_ios_char_std_char_traits_char_t", "");
		cl.def( pybind11::init<class std::basic_streambuf<char> *>(), pybind11::arg("__sb") );

		cl.def("rdstate", (enum std::_Ios_Iostate (std::basic_ios<char,std::char_traits<char>>::*)() const) &std::basic_ios<char, std::char_traits<char>>::rdstate, "C++: std::basic_ios<char, std::char_traits<char>>::rdstate() const --> enum std::_Ios_Iostate");
		cl.def("clear", [](std::basic_ios<char,std::char_traits<char>> &o) -> void { return o.clear(); }, "");
		cl.def("clear", (void (std::basic_ios<char,std::char_traits<char>>::*)(enum std::_Ios_Iostate)) &std::basic_ios<char, std::char_traits<char>>::clear, "C++: std::basic_ios<char, std::char_traits<char>>::clear(enum std::_Ios_Iostate) --> void", pybind11::arg("__state"));
		cl.def("setstate", (void (std::basic_ios<char,std::char_traits<char>>::*)(enum std::_Ios_Iostate)) &std::basic_ios<char, std::char_traits<char>>::setstate, "C++: std::basic_ios<char, std::char_traits<char>>::setstate(enum std::_Ios_Iostate) --> void", pybind11::arg("__state"));
		cl.def("_M_setstate", (void (std::basic_ios<char,std::char_traits<char>>::*)(enum std::_Ios_Iostate)) &std::basic_ios<char, std::char_traits<char>>::_M_setstate, "C++: std::basic_ios<char, std::char_traits<char>>::_M_setstate(enum std::_Ios_Iostate) --> void", pybind11::arg("__state"));
		cl.def("good", (bool (std::basic_ios<char,std::char_traits<char>>::*)() const) &std::basic_ios<char, std::char_traits<char>>::good, "C++: std::basic_ios<char, std::char_traits<char>>::good() const --> bool");
		cl.def("eof", (bool (std::basic_ios<char,std::char_traits<char>>::*)() const) &std::basic_ios<char, std::char_traits<char>>::eof, "C++: std::basic_ios<char, std::char_traits<char>>::eof() const --> bool");
		cl.def("fail", (bool (std::basic_ios<char,std::char_traits<char>>::*)() const) &std::basic_ios<char, std::char_traits<char>>::fail, "C++: std::basic_ios<char, std::char_traits<char>>::fail() const --> bool");
		cl.def("bad", (bool (std::basic_ios<char,std::char_traits<char>>::*)() const) &std::basic_ios<char, std::char_traits<char>>::bad, "C++: std::basic_ios<char, std::char_traits<char>>::bad() const --> bool");
		cl.def("exceptions", (enum std::_Ios_Iostate (std::basic_ios<char,std::char_traits<char>>::*)() const) &std::basic_ios<char, std::char_traits<char>>::exceptions, "C++: std::basic_ios<char, std::char_traits<char>>::exceptions() const --> enum std::_Ios_Iostate");
		cl.def("exceptions", (void (std::basic_ios<char,std::char_traits<char>>::*)(enum std::_Ios_Iostate)) &std::basic_ios<char, std::char_traits<char>>::exceptions, "C++: std::basic_ios<char, std::char_traits<char>>::exceptions(enum std::_Ios_Iostate) --> void", pybind11::arg("__except"));
		cl.def("tie", (std::ostream * (std::basic_ios<char,std::char_traits<char>>::*)() const) &std::basic_ios<char, std::char_traits<char>>::tie, "C++: std::basic_ios<char, std::char_traits<char>>::tie() const --> std::ostream *", pybind11::return_value_policy::reference_internal);
		cl.def("tie", (std::ostream * (std::basic_ios<char,std::char_traits<char>>::*)(std::ostream *)) &std::basic_ios<char, std::char_traits<char>>::tie, "C++: std::basic_ios<char, std::char_traits<char>>::tie(std::ostream *) --> std::ostream *", pybind11::return_value_policy::reference_internal, pybind11::arg("__tiestr"));
		cl.def("rdbuf", (class std::basic_streambuf<char> * (std::basic_ios<char,std::char_traits<char>>::*)() const) &std::basic_ios<char, std::char_traits<char>>::rdbuf, "C++: std::basic_ios<char, std::char_traits<char>>::rdbuf() const --> class std::basic_streambuf<char> *", pybind11::return_value_policy::reference_internal);
		cl.def("rdbuf", (class std::basic_streambuf<char> * (std::basic_ios<char,std::char_traits<char>>::*)(class std::basic_streambuf<char> *)) &std::basic_ios<char, std::char_traits<char>>::rdbuf, "C++: std::basic_ios<char, std::char_traits<char>>::rdbuf(class std::basic_streambuf<char> *) --> class std::basic_streambuf<char> *", pybind11::return_value_policy::reference_internal, pybind11::arg("__sb"));
		cl.def("copyfmt", (class std::basic_ios<char> & (std::basic_ios<char,std::char_traits<char>>::*)(const class std::basic_ios<char> &)) &std::basic_ios<char, std::char_traits<char>>::copyfmt, "C++: std::basic_ios<char, std::char_traits<char>>::copyfmt(const class std::basic_ios<char> &) --> class std::basic_ios<char> &", pybind11::return_value_policy::reference_internal, pybind11::arg("__rhs"));
		cl.def("fill", (char (std::basic_ios<char,std::char_traits<char>>::*)() const) &std::basic_ios<char, std::char_traits<char>>::fill, "C++: std::basic_ios<char, std::char_traits<char>>::fill() const --> char");
		cl.def("fill", (char (std::basic_ios<char,std::char_traits<char>>::*)(char)) &std::basic_ios<char, std::char_traits<char>>::fill, "C++: std::basic_ios<char, std::char_traits<char>>::fill(char) --> char", pybind11::arg("__ch"));
		cl.def("imbue", (class std::locale (std::basic_ios<char,std::char_traits<char>>::*)(const class std::locale &)) &std::basic_ios<char, std::char_traits<char>>::imbue, "C++: std::basic_ios<char, std::char_traits<char>>::imbue(const class std::locale &) --> class std::locale", pybind11::arg("__loc"));
		cl.def("narrow", (char (std::basic_ios<char,std::char_traits<char>>::*)(char, char) const) &std::basic_ios<char, std::char_traits<char>>::narrow, "C++: std::basic_ios<char, std::char_traits<char>>::narrow(char, char) const --> char", pybind11::arg("__c"), pybind11::arg("__dfault"));
		cl.def("widen", (char (std::basic_ios<char,std::char_traits<char>>::*)(char) const) &std::basic_ios<char, std::char_traits<char>>::widen, "C++: std::basic_ios<char, std::char_traits<char>>::widen(char) const --> char", pybind11::arg("__c"));
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
	}
	{ // std::basic_ostream file:bits/ostream.tcc line:359
		pybind11::class_<std::ostream, std::shared_ptr<std::ostream>, std::basic_ios<char,std::char_traits<char>>> cl(M("std"), "ostream", "");
		cl.def( pybind11::init<class std::basic_streambuf<char> *>(), pybind11::arg("__sb") );

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

		{ // std::basic_ostream<char, std::char_traits<char>>::sentry file:ostream line:96
			auto & enclosing_class = cl;
			pybind11::class_<std::basic_ostream<char, std::char_traits<char>>::sentry, std::shared_ptr<std::basic_ostream<char, std::char_traits<char>>::sentry>> cl(enclosing_class, "sentry", "");
			cl.def( pybind11::init<std::ostream &>(), pybind11::arg("__os") );

		}

	}
	{ // std::basic_istream file:bits/istream.tcc line:1077
		pybind11::class_<std::istream, std::shared_ptr<std::istream>, std::basic_ios<char,std::char_traits<char>>> cl(M("std"), "istream", "");
		cl.def( pybind11::init<class std::basic_streambuf<char> *>(), pybind11::arg("__sb") );

		cl.def("__rshift__", (std::istream & (std::istream::*)(bool &)) &std::basic_istream<char, std::char_traits<char>>::operator>>, "C++: std::basic_istream<char, std::char_traits<char>>::operator>>(bool &) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__n"));
		cl.def("__rshift__", (std::istream & (std::istream::*)(short &)) &std::basic_istream<char, std::char_traits<char>>::operator>>, "C++: std::basic_istream<char, std::char_traits<char>>::operator>>(short &) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__n"));
		cl.def("__rshift__", (std::istream & (std::istream::*)(unsigned short &)) &std::basic_istream<char, std::char_traits<char>>::operator>>, "C++: std::basic_istream<char, std::char_traits<char>>::operator>>(unsigned short &) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__n"));
		cl.def("__rshift__", (std::istream & (std::istream::*)(int &)) &std::basic_istream<char, std::char_traits<char>>::operator>>, "C++: std::basic_istream<char, std::char_traits<char>>::operator>>(int &) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__n"));
		cl.def("__rshift__", (std::istream & (std::istream::*)(unsigned int &)) &std::basic_istream<char, std::char_traits<char>>::operator>>, "C++: std::basic_istream<char, std::char_traits<char>>::operator>>(unsigned int &) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__n"));
		cl.def("__rshift__", (std::istream & (std::istream::*)(long &)) &std::basic_istream<char, std::char_traits<char>>::operator>>, "C++: std::basic_istream<char, std::char_traits<char>>::operator>>(long &) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__n"));
		cl.def("__rshift__", (std::istream & (std::istream::*)(unsigned long &)) &std::basic_istream<char, std::char_traits<char>>::operator>>, "C++: std::basic_istream<char, std::char_traits<char>>::operator>>(unsigned long &) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__n"));
		cl.def("__rshift__", (std::istream & (std::istream::*)(long long &)) &std::basic_istream<char, std::char_traits<char>>::operator>>, "C++: std::basic_istream<char, std::char_traits<char>>::operator>>(long long &) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__n"));
		cl.def("__rshift__", (std::istream & (std::istream::*)(unsigned long long &)) &std::basic_istream<char, std::char_traits<char>>::operator>>, "C++: std::basic_istream<char, std::char_traits<char>>::operator>>(unsigned long long &) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__n"));
		cl.def("__rshift__", (std::istream & (std::istream::*)(float &)) &std::basic_istream<char, std::char_traits<char>>::operator>>, "C++: std::basic_istream<char, std::char_traits<char>>::operator>>(float &) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__f"));
		cl.def("__rshift__", (std::istream & (std::istream::*)(double &)) &std::basic_istream<char, std::char_traits<char>>::operator>>, "C++: std::basic_istream<char, std::char_traits<char>>::operator>>(double &) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__f"));
		cl.def("__rshift__", (std::istream & (std::istream::*)(long double &)) &std::basic_istream<char, std::char_traits<char>>::operator>>, "C++: std::basic_istream<char, std::char_traits<char>>::operator>>(long double &) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__f"));
		cl.def("__rshift__", (std::istream & (std::istream::*)(void *&)) &std::basic_istream<char, std::char_traits<char>>::operator>>, "C++: std::basic_istream<char, std::char_traits<char>>::operator>>(void *&) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__p"));
		cl.def("__rshift__", (std::istream & (std::istream::*)(class std::basic_streambuf<char> *)) &std::basic_istream<char, std::char_traits<char>>::operator>>, "C++: std::basic_istream<char, std::char_traits<char>>::operator>>(class std::basic_streambuf<char> *) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__sb"));
		cl.def("gcount", (long (std::istream::*)() const) &std::basic_istream<char, std::char_traits<char>>::gcount, "C++: std::basic_istream<char, std::char_traits<char>>::gcount() const --> long");
		cl.def("get", (int (std::istream::*)()) &std::basic_istream<char, std::char_traits<char>>::get, "C++: std::basic_istream<char, std::char_traits<char>>::get() --> int");
		cl.def("get", (std::istream & (std::istream::*)(char &)) &std::basic_istream<char, std::char_traits<char>>::get, "C++: std::basic_istream<char, std::char_traits<char>>::get(char &) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__c"));
		cl.def("get", (std::istream & (std::istream::*)(char *, long, char)) &std::basic_istream<char, std::char_traits<char>>::get, "C++: std::basic_istream<char, std::char_traits<char>>::get(char *, long, char) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__s"), pybind11::arg("__n"), pybind11::arg("__delim"));
		cl.def("get", (std::istream & (std::istream::*)(char *, long)) &std::basic_istream<char, std::char_traits<char>>::get, "C++: std::basic_istream<char, std::char_traits<char>>::get(char *, long) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__s"), pybind11::arg("__n"));
		cl.def("get", (std::istream & (std::istream::*)(class std::basic_streambuf<char> &, char)) &std::basic_istream<char, std::char_traits<char>>::get, "C++: std::basic_istream<char, std::char_traits<char>>::get(class std::basic_streambuf<char> &, char) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__sb"), pybind11::arg("__delim"));
		cl.def("get", (std::istream & (std::istream::*)(class std::basic_streambuf<char> &)) &std::basic_istream<char, std::char_traits<char>>::get, "C++: std::basic_istream<char, std::char_traits<char>>::get(class std::basic_streambuf<char> &) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__sb"));
		cl.def("getline", (std::istream & (std::istream::*)(char *, long, char)) &std::basic_istream<char, std::char_traits<char>>::getline, "C++: std::basic_istream<char, std::char_traits<char>>::getline(char *, long, char) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__s"), pybind11::arg("__n"), pybind11::arg("__delim"));
		cl.def("getline", (std::istream & (std::istream::*)(char *, long)) &std::basic_istream<char, std::char_traits<char>>::getline, "C++: std::basic_istream<char, std::char_traits<char>>::getline(char *, long) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__s"), pybind11::arg("__n"));
		cl.def("ignore", (std::istream & (std::istream::*)(long, int)) &std::basic_istream<char, std::char_traits<char>>::ignore, "C++: std::basic_istream<char, std::char_traits<char>>::ignore(long, int) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__n"), pybind11::arg("__delim"));
		cl.def("ignore", (std::istream & (std::istream::*)(long)) &std::basic_istream<char, std::char_traits<char>>::ignore, "C++: std::basic_istream<char, std::char_traits<char>>::ignore(long) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__n"));
		cl.def("ignore", (std::istream & (std::istream::*)()) &std::basic_istream<char, std::char_traits<char>>::ignore, "C++: std::basic_istream<char, std::char_traits<char>>::ignore() --> std::istream &", pybind11::return_value_policy::reference_internal);
		cl.def("peek", (int (std::istream::*)()) &std::basic_istream<char, std::char_traits<char>>::peek, "C++: std::basic_istream<char, std::char_traits<char>>::peek() --> int");
		cl.def("read", (std::istream & (std::istream::*)(char *, long)) &std::basic_istream<char, std::char_traits<char>>::read, "C++: std::basic_istream<char, std::char_traits<char>>::read(char *, long) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__s"), pybind11::arg("__n"));
		cl.def("readsome", (long (std::istream::*)(char *, long)) &std::basic_istream<char, std::char_traits<char>>::readsome, "C++: std::basic_istream<char, std::char_traits<char>>::readsome(char *, long) --> long", pybind11::arg("__s"), pybind11::arg("__n"));
		cl.def("putback", (std::istream & (std::istream::*)(char)) &std::basic_istream<char, std::char_traits<char>>::putback, "C++: std::basic_istream<char, std::char_traits<char>>::putback(char) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg("__c"));
		cl.def("unget", (std::istream & (std::istream::*)()) &std::basic_istream<char, std::char_traits<char>>::unget, "C++: std::basic_istream<char, std::char_traits<char>>::unget() --> std::istream &", pybind11::return_value_policy::reference_internal);
		cl.def("sync", (int (std::istream::*)()) &std::basic_istream<char, std::char_traits<char>>::sync, "C++: std::basic_istream<char, std::char_traits<char>>::sync() --> int");
		cl.def("seekg", (std::istream & (std::istream::*)(long, enum std::_Ios_Seekdir)) &std::basic_istream<char, std::char_traits<char>>::seekg, "C++: std::basic_istream<char, std::char_traits<char>>::seekg(long, enum std::_Ios_Seekdir) --> std::istream &", pybind11::return_value_policy::reference_internal, pybind11::arg(""), pybind11::arg(""));

		{ // std::basic_istream<char, std::char_traits<char>>::sentry file:istream line:107
			auto & enclosing_class = cl;
			pybind11::class_<std::basic_istream<char, std::char_traits<char>>::sentry, std::shared_ptr<std::basic_istream<char, std::char_traits<char>>::sentry>> cl(enclosing_class, "sentry", "");
			cl.def( pybind11::init( [](std::istream & a0){ return new std::basic_istream<char, std::char_traits<char>>::sentry(a0); } ), "doc" , pybind11::arg("__is"));
			cl.def( pybind11::init<std::istream &, bool>(), pybind11::arg("__is"), pybind11::arg("__noskipws") );

		}

	}
}

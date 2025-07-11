#include <fstream>
#include <ios>
#include <iterator>
#include <memory>
#include <sstream> // __str__
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

void bind_std_fstream_tcc(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // std::basic_ofstream file:bits/fstream.tcc line:1088
		pybind11::class_<std::ofstream, std::shared_ptr<std::ofstream>, std::ostream> cl(M("std"), "ofstream", "");
		cl.def( pybind11::init( [](){ return new std::ofstream(); } ) );
		cl.def( pybind11::init( [](const char * a0){ return new std::ofstream(a0); } ), "doc" , pybind11::arg("__s"));
		cl.def( pybind11::init<const char *, enum std::_Ios_Openmode>(), pybind11::arg("__s"), pybind11::arg("__mode") );

		cl.def( pybind11::init( [](const std::string & a0){ return new std::ofstream(a0); } ), "doc" , pybind11::arg("__s"));
		cl.def( pybind11::init<const std::string &, enum std::_Ios_Openmode>(), pybind11::arg("__s"), pybind11::arg("__mode") );

		cl.def("swap", (void (std::ofstream::*)(class std::basic_ofstream<char> &)) &std::basic_ofstream<char, std::char_traits<char>>::swap, "C++: std::basic_ofstream<char, std::char_traits<char>>::swap(class std::basic_ofstream<char> &) --> void", pybind11::arg("__rhs"));
		cl.def("rdbuf", (class std::basic_filebuf<char> * (std::ofstream::*)() const) &std::basic_ofstream<char, std::char_traits<char>>::rdbuf, "C++: std::basic_ofstream<char, std::char_traits<char>>::rdbuf() const --> class std::basic_filebuf<char> *", pybind11::return_value_policy::reference_internal);
		cl.def("is_open", (bool (std::ofstream::*)()) &std::basic_ofstream<char, std::char_traits<char>>::is_open, "C++: std::basic_ofstream<char, std::char_traits<char>>::is_open() --> bool");
		cl.def("open", [](std::ofstream &o, const char * a0) -> void { return o.open(a0); }, "", pybind11::arg("__s"));
		cl.def("open", (void (std::ofstream::*)(const char *, enum std::_Ios_Openmode)) &std::basic_ofstream<char, std::char_traits<char>>::open, "C++: std::basic_ofstream<char, std::char_traits<char>>::open(const char *, enum std::_Ios_Openmode) --> void", pybind11::arg("__s"), pybind11::arg("__mode"));
		cl.def("open", [](std::ofstream &o, const std::string & a0) -> void { return o.open(a0); }, "", pybind11::arg("__s"));
		cl.def("open", (void (std::ofstream::*)(const std::string &, enum std::_Ios_Openmode)) &std::basic_ofstream<char, std::char_traits<char>>::open, "C++: std::basic_ofstream<char, std::char_traits<char>>::open(const std::string &, enum std::_Ios_Openmode) --> void", pybind11::arg("__s"), pybind11::arg("__mode"));
		cl.def("close", (void (std::ofstream::*)()) &std::basic_ofstream<char, std::char_traits<char>>::close, "C++: std::basic_ofstream<char, std::char_traits<char>>::close() --> void");
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
	{ // std::basic_fstream file:bits/fstream.tcc line:1089
		pybind11::class_<std::basic_fstream<char,std::char_traits<char>>, std::shared_ptr<std::basic_fstream<char,std::char_traits<char>>>, std::iostream> cl(M("std"), "basic_fstream_char_std_char_traits_char_t", "");
		cl.def( pybind11::init( [](){ return new std::basic_fstream<char,std::char_traits<char>>(); } ) );
		cl.def( pybind11::init( [](const char * a0){ return new std::basic_fstream<char,std::char_traits<char>>(a0); } ), "doc" , pybind11::arg("__s"));
		cl.def( pybind11::init<const char *, enum std::_Ios_Openmode>(), pybind11::arg("__s"), pybind11::arg("__mode") );

		cl.def( pybind11::init( [](const std::string & a0){ return new std::basic_fstream<char,std::char_traits<char>>(a0); } ), "doc" , pybind11::arg("__s"));
		cl.def( pybind11::init<const std::string &, enum std::_Ios_Openmode>(), pybind11::arg("__s"), pybind11::arg("__mode") );

		cl.def("swap", (void (std::basic_fstream<char,std::char_traits<char>>::*)(class std::basic_fstream<char> &)) &std::basic_fstream<char, std::char_traits<char>>::swap, "C++: std::basic_fstream<char, std::char_traits<char>>::swap(class std::basic_fstream<char> &) --> void", pybind11::arg("__rhs"));
		cl.def("rdbuf", (class std::basic_filebuf<char> * (std::basic_fstream<char,std::char_traits<char>>::*)() const) &std::basic_fstream<char, std::char_traits<char>>::rdbuf, "C++: std::basic_fstream<char, std::char_traits<char>>::rdbuf() const --> class std::basic_filebuf<char> *", pybind11::return_value_policy::reference_internal);
		cl.def("is_open", (bool (std::basic_fstream<char,std::char_traits<char>>::*)()) &std::basic_fstream<char, std::char_traits<char>>::is_open, "C++: std::basic_fstream<char, std::char_traits<char>>::is_open() --> bool");
		cl.def("open", [](std::basic_fstream<char,std::char_traits<char>> &o, const char * a0) -> void { return o.open(a0); }, "", pybind11::arg("__s"));
		cl.def("open", (void (std::basic_fstream<char,std::char_traits<char>>::*)(const char *, enum std::_Ios_Openmode)) &std::basic_fstream<char, std::char_traits<char>>::open, "C++: std::basic_fstream<char, std::char_traits<char>>::open(const char *, enum std::_Ios_Openmode) --> void", pybind11::arg("__s"), pybind11::arg("__mode"));
		cl.def("open", [](std::basic_fstream<char,std::char_traits<char>> &o, const std::string & a0) -> void { return o.open(a0); }, "", pybind11::arg("__s"));
		cl.def("open", (void (std::basic_fstream<char,std::char_traits<char>>::*)(const std::string &, enum std::_Ios_Openmode)) &std::basic_fstream<char, std::char_traits<char>>::open, "C++: std::basic_fstream<char, std::char_traits<char>>::open(const std::string &, enum std::_Ios_Openmode) --> void", pybind11::arg("__s"), pybind11::arg("__mode"));
		cl.def("close", (void (std::basic_fstream<char,std::char_traits<char>>::*)()) &std::basic_fstream<char, std::char_traits<char>>::close, "C++: std::basic_fstream<char, std::char_traits<char>>::close() --> void");
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

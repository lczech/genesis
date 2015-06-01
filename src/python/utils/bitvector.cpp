/**
 * @brief Definition of the genesis Python bindings for Bitvector class.
 *
 * @file
 * @ingroup python
 */

#include <boost/python.hpp>

#include "utils/bitvector.hpp"

void BoostPythonExport_Bitvector()
{

    boost::python::class_< ::genesis::Bitvector >( "Bitvector", boost::python::init< long unsigned int, boost::python::optional< bool > >(( boost::python::arg("size"), boost::python::arg("init")=(bool const)(false) )) )
        .def(
            "count"
            , (::size_t ( ::genesis::Bitvector::* )(  ) const)( &::genesis::Bitvector::count ) )
        .def(
            "dump"
            , (::std::string ( ::genesis::Bitvector::* )(  ) const)( &::genesis::Bitvector::dump ) )
        .def(
            "dump_int"
            , (::std::string ( ::genesis::Bitvector::* )( ::uint64_t ) const)( &::genesis::Bitvector::dump_int )
            , ( boost::python::arg("x") ) )
        .def(
            "flip"
            , (void ( ::genesis::Bitvector::* )( ::size_t ) )( &::genesis::Bitvector::flip )
            , ( boost::python::arg("index") ) )
        .def(
            "get"
            , (bool ( ::genesis::Bitvector::* )( ::size_t ) const)( &::genesis::Bitvector::get )
            , ( boost::python::arg("index") ) )
        .def(
            "hash"
            , (::size_t ( ::genesis::Bitvector::* )(  ) const)( &::genesis::Bitvector::hash ) )
        .def(
            "invert"
            , (void ( ::genesis::Bitvector::* )(  ) )( &::genesis::Bitvector::invert ) )
        .def(
            "normalize"
            , (void ( ::genesis::Bitvector::* )(  ) )( &::genesis::Bitvector::normalize ) )
        .def(
            "reset"
            , (void ( ::genesis::Bitvector::* )( bool ) )( &::genesis::Bitvector::reset )
            , ( boost::python::arg("value")=(bool)(false) ) )
        .def(
            "set"
            , (void ( ::genesis::Bitvector::* )( ::size_t ) )( &::genesis::Bitvector::set )
            , ( boost::python::arg("index") ) )
        .def(
            "set"
            , (void ( ::genesis::Bitvector::* )( ::size_t,bool ) )( &::genesis::Bitvector::set )
            , ( boost::python::arg("index"), boost::python::arg("value") ) )
        .def(
            "symmetric_difference"
            , (::genesis::Bitvector ( ::genesis::Bitvector::* )( ::genesis::Bitvector const & ) const)( &::genesis::Bitvector::symmetric_difference )
            , ( boost::python::arg("rhs") ) )
        .def(
            "symmetric_difference"
            , (::genesis::Bitvector (*)( ::genesis::Bitvector const &,::genesis::Bitvector const & ))( &::genesis::Bitvector::symmetric_difference )
            , ( boost::python::arg("lhs"), boost::python::arg("rhs") ) )
        .staticmethod( "symmetric_difference" )
        .def(
            "unset"
            , (void ( ::genesis::Bitvector::* )( ::size_t ) )( &::genesis::Bitvector::unset )
            , ( boost::python::arg("index") ) )
        .def(
            "x_hash"
            , (::uint64_t ( ::genesis::Bitvector::* )(  ) const)( &::genesis::Bitvector::x_hash ) )
        .def( boost::python::self != boost::python::self )
        .def( boost::python::self &= boost::python::self )
        .def( boost::python::self < boost::python::self )
        .def( boost::python::self <= boost::python::self )
        .def( boost::python::self == boost::python::self )
        .def( boost::python::self > boost::python::self )
        .def( boost::python::self >= boost::python::self )
        .def(
            "__getitem__"
            , (bool ( ::genesis::Bitvector::* )( ::size_t ) const)( &::genesis::Bitvector::operator[] )
            , ( boost::python::arg("index") ) )
        .def( boost::python::self ^= boost::python::self )
        .def( boost::python::self |= boost::python::self )
        .def( ~boost::python::self )
        .def(
            "size"
            , (::size_t ( ::genesis::Bitvector::* )(  ) const)( &::genesis::Bitvector::size ) )
        .def(
            "__len__"
            , (::size_t ( ::genesis::Bitvector::* )(  ) const)( &::genesis::Bitvector::size ) )
        .def( boost::python::self & boost::python::self )
        .def( boost::python::self - boost::python::self )
        .def( boost::python::self_ns::str( boost::python::self ) )
        .def( boost::python::self ^ boost::python::self )
        .def( boost::python::self | boost::python::self )
    ;

}

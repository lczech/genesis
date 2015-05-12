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
            "Count"
            , (::size_t ( ::genesis::Bitvector::* )(  ) const)( &::genesis::Bitvector::Count ) )
        .def(
            "Dump"
            , (::std::string ( ::genesis::Bitvector::* )(  ) const)( &::genesis::Bitvector::Dump ) )
        .def(
            "DumpInt"
            , (::std::string ( ::genesis::Bitvector::* )( ::uint64_t ) const)( &::genesis::Bitvector::DumpInt )
            , ( boost::python::arg("x") ) )
        .def(
            "Flip"
            , (void ( ::genesis::Bitvector::* )( ::size_t ) )( &::genesis::Bitvector::Flip )
            , ( boost::python::arg("index") ) )
        .def(
            "Get"
            , (bool ( ::genesis::Bitvector::* )( ::size_t ) const)( &::genesis::Bitvector::Get )
            , ( boost::python::arg("index") ) )
        .def(
            "Hash"
            , (::size_t ( ::genesis::Bitvector::* )(  ) const)( &::genesis::Bitvector::Hash ) )
        .def(
            "Invert"
            , (void ( ::genesis::Bitvector::* )(  ) )( &::genesis::Bitvector::Invert ) )
        .def(
            "Normalize"
            , (void ( ::genesis::Bitvector::* )(  ) )( &::genesis::Bitvector::Normalize ) )
        .def(
            "Reset"
            , (void ( ::genesis::Bitvector::* )( bool ) )( &::genesis::Bitvector::Reset )
            , ( boost::python::arg("value")=(bool)(false) ) )
        .def(
            "Set"
            , (void ( ::genesis::Bitvector::* )( ::size_t ) )( &::genesis::Bitvector::Set )
            , ( boost::python::arg("index") ) )
        .def(
            "Set"
            , (void ( ::genesis::Bitvector::* )( ::size_t,bool ) )( &::genesis::Bitvector::Set )
            , ( boost::python::arg("index"), boost::python::arg("value") ) )
        .def(
            "SymmetricDifference"
            , (::genesis::Bitvector ( ::genesis::Bitvector::* )( ::genesis::Bitvector const & ) const)( &::genesis::Bitvector::SymmetricDifference )
            , ( boost::python::arg("rhs") ) )
        .def(
            "SymmetricDifference"
            , (::genesis::Bitvector (*)( ::genesis::Bitvector const &,::genesis::Bitvector const & ))( &::genesis::Bitvector::SymmetricDifference )
            , ( boost::python::arg("lhs"), boost::python::arg("rhs") ) )
        .def(
            "Unset"
            , (void ( ::genesis::Bitvector::* )( ::size_t ) )( &::genesis::Bitvector::Unset )
            , ( boost::python::arg("index") ) )
        .def(
            "XHash"
            , (::uint64_t ( ::genesis::Bitvector::* )(  ) const)( &::genesis::Bitvector::XHash ) )
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
        .staticmethod( "SymmetricDifference" )
        .def( boost::python::self & boost::python::self )
        .def( boost::python::self - boost::python::self )
        .def( boost::python::self_ns::str( boost::python::self ) )
        .def( boost::python::self ^ boost::python::self )
        .def( boost::python::self | boost::python::self )
    ;

}

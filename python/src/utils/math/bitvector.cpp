/**
 * @brief Definition of the genesis Python bindings for Bitvector class.
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/utils/math/bitvector.hpp"

using namespace genesis::utils;

PYTHON_EXPORT_CLASS (Bitvector, "utils")
{
    boost::python::class_< ::genesis::utils::Bitvector >( "Bitvector", boost::python::init< long unsigned int, boost::python::optional< bool > >(( boost::python::arg("size"), boost::python::arg("init")=(bool const)(false) )) )

        // Public Member Functions

        .def(
            "count",
            ( size_t ( ::genesis::utils::Bitvector::* )(  ) const )( &::genesis::utils::Bitvector::count ),
            get_docstring("size_t ::genesis::utils::Bitvector::count () const")
        )
        .def(
            "dump",
            ( std::string ( ::genesis::utils::Bitvector::* )(  ) const )( &::genesis::utils::Bitvector::dump )
        )
        .def(
            "dump_int",
            ( std::string ( ::genesis::utils::Bitvector::* )( genesis::utils::Bitvector::IntType ) const )( &::genesis::utils::Bitvector::dump_int ),
            ( boost::python::arg("x") )
        )
        .def(
            "flip",
            ( void ( ::genesis::utils::Bitvector::* )( size_t ))( &::genesis::utils::Bitvector::flip ),
            ( boost::python::arg("index") ),
            get_docstring("void ::genesis::utils::Bitvector::flip (size_t index)")
        )
        .def(
            "get",
            ( bool ( ::genesis::utils::Bitvector::* )( size_t ) const )( &::genesis::utils::Bitvector::get ),
            ( boost::python::arg("index") ),
            get_docstring("bool ::genesis::utils::Bitvector::get (size_t index) const")
        )
        .def(
            "hash",
            ( size_t ( ::genesis::utils::Bitvector::* )(  ) const )( &::genesis::utils::Bitvector::hash ),
            get_docstring("size_t ::genesis::utils::Bitvector::hash () const")
        )
        .def(
            "invert",
            ( void ( ::genesis::utils::Bitvector::* )(  ))( &::genesis::utils::Bitvector::invert ),
            get_docstring("void ::genesis::utils::Bitvector::invert ()")
        )
        .def(
            "normalize",
            ( void ( ::genesis::utils::Bitvector::* )(  ))( &::genesis::utils::Bitvector::normalize ),
            get_docstring("void ::genesis::utils::Bitvector::normalize ()")
        )
        .def(
            "reset",
            ( void ( ::genesis::utils::Bitvector::* )( bool ))( &::genesis::utils::Bitvector::reset ),
            ( boost::python::arg("value")=(bool)(false) ),
            get_docstring("void ::genesis::utils::Bitvector::reset (bool value=false)")
        )
        .def(
            "set",
            ( void ( ::genesis::utils::Bitvector::* )( size_t ))( &::genesis::utils::Bitvector::set ),
            ( boost::python::arg("index") ),
            get_docstring("void ::genesis::utils::Bitvector::set (size_t index)")
        )
        .def(
            "set",
            ( void ( ::genesis::utils::Bitvector::* )( size_t, bool ))( &::genesis::utils::Bitvector::set ),
            ( boost::python::arg("index"), boost::python::arg("value") ),
            get_docstring("void ::genesis::utils::Bitvector::set (size_t index, bool value)")
        )
        .def(
            "size",
            ( size_t ( ::genesis::utils::Bitvector::* )(  ) const )( &::genesis::utils::Bitvector::size ),
            get_docstring("size_t ::genesis::utils::Bitvector::size () const")
        )
        .def(
            "symmetric_difference",
            ( ::genesis::utils::Bitvector ( * )( ::genesis::utils::Bitvector const &, ::genesis::utils::Bitvector const & ))( &::genesis::utils::Bitvector::symmetric_difference ),
            ( boost::python::arg("lhs"), boost::python::arg("rhs") )
        )
        .def(
            "symmetric_difference",
            ( ::genesis::utils::Bitvector ( ::genesis::utils::Bitvector::* )( ::genesis::utils::Bitvector const & ) const )( &::genesis::utils::Bitvector::symmetric_difference ),
            ( boost::python::arg("rhs") )
        )
        .staticmethod("symmetric_difference")
        .def(
            "unset",
            ( void ( ::genesis::utils::Bitvector::* )( size_t ))( &::genesis::utils::Bitvector::unset ),
            ( boost::python::arg("index") ),
            get_docstring("void ::genesis::utils::Bitvector::unset (size_t index)")
        )
        .def(
            "x_hash",
            ( ::genesis::utils::Bitvector::IntType ( ::genesis::utils::Bitvector::* )(  ) const )( &::genesis::utils::Bitvector::x_hash ),
            get_docstring("IntType ::genesis::utils::Bitvector::x_hash () const")
        )

        // Operators

        .def(
            "__len__",
            (::size_t ( ::genesis::utils::Bitvector::* )(  ) const)( &::genesis::utils::Bitvector::size )
        )
        .def(
            "__getitem__",
            ( bool ( ::genesis::utils::Bitvector::* )( size_t ) const )( &::genesis::utils::Bitvector::operator[] ),
            ( boost::python::arg("index") ),
            get_docstring("bool ::genesis::utils::Bitvector::operator[] (size_t index) const")
        )
        .def( boost::python::self != boost::python::self )
        .def( boost::python::self &= boost::python::self )
        .def( boost::python::self < boost::python::self )
        .def( boost::python::self <= boost::python::self )
        .def( boost::python::self == boost::python::self )
        .def( boost::python::self > boost::python::self )
        .def( boost::python::self >= boost::python::self )
        .def( boost::python::self ^= boost::python::self )
        .def( boost::python::self |= boost::python::self )
        .def( ~boost::python::self )
        .def( boost::python::self & boost::python::self )
        .def( boost::python::self - boost::python::self )
        .def( boost::python::self_ns::str( boost::python::self ) )
        .def( boost::python::self ^ boost::python::self )
        .def( boost::python::self | boost::python::self )
    ;
}

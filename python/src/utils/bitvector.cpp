/**
 * @brief Definition of the genesis Python bindings for Bitvector class.
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/utils/bitvector.hpp"

PYTHON_EXPORT_CLASS (Bitvector, "utils")
{
    boost::python::class_< ::genesis::Bitvector >( "Bitvector", boost::python::init< long unsigned int, boost::python::optional< bool > >(( boost::python::arg("size"), boost::python::arg("init")=(bool const)(false) )) )

        // Public Member Functions

        .def(
            "count",
            ( size_t ( ::genesis::Bitvector::* )(  ) const )( &::genesis::Bitvector::count ),
            get_docstring("size_t ::genesis::Bitvector::count () const")
        )
        .def(
            "dump",
            ( std::string ( ::genesis::Bitvector::* )(  ) const )( &::genesis::Bitvector::dump )
        )
        .def(
            "dump_int",
            ( std::string ( ::genesis::Bitvector::* )( genesis::Bitvector::IntType ) const )( &::genesis::Bitvector::dump_int ),
            ( boost::python::arg("x") )
        )
        .def(
            "flip",
            ( void ( ::genesis::Bitvector::* )( size_t ))( &::genesis::Bitvector::flip ),
            ( boost::python::arg("index") ),
            get_docstring("void ::genesis::Bitvector::flip (size_t index)")
        )
        .def(
            "get",
            ( bool ( ::genesis::Bitvector::* )( size_t ) const )( &::genesis::Bitvector::get ),
            ( boost::python::arg("index") ),
            get_docstring("bool ::genesis::Bitvector::get (size_t index) const")
        )
        .def(
            "hash",
            ( size_t ( ::genesis::Bitvector::* )(  ) const )( &::genesis::Bitvector::hash ),
            get_docstring("size_t ::genesis::Bitvector::hash () const")
        )
        .def(
            "invert",
            ( void ( ::genesis::Bitvector::* )(  ))( &::genesis::Bitvector::invert ),
            get_docstring("void ::genesis::Bitvector::invert ()")
        )
        .def(
            "normalize",
            ( void ( ::genesis::Bitvector::* )(  ))( &::genesis::Bitvector::normalize ),
            get_docstring("void ::genesis::Bitvector::normalize ()")
        )
        .def(
            "reset",
            ( void ( ::genesis::Bitvector::* )( bool ))( &::genesis::Bitvector::reset ),
            ( boost::python::arg("value")=(bool)(false) ),
            get_docstring("void ::genesis::Bitvector::reset (bool value=false)")
        )
        .def(
            "set",
            ( void ( ::genesis::Bitvector::* )( size_t ))( &::genesis::Bitvector::set ),
            ( boost::python::arg("index") ),
            get_docstring("void ::genesis::Bitvector::set (size_t index)")
        )
        .def(
            "set",
            ( void ( ::genesis::Bitvector::* )( size_t, bool ))( &::genesis::Bitvector::set ),
            ( boost::python::arg("index"), boost::python::arg("value") ),
            get_docstring("void ::genesis::Bitvector::set (size_t index, bool value)")
        )
        .def(
            "size",
            ( size_t ( ::genesis::Bitvector::* )(  ) const )( &::genesis::Bitvector::size ),
            get_docstring("size_t ::genesis::Bitvector::size () const")
        )
        .def(
            "symmetric_difference",
            ( ::genesis::Bitvector ( * )( ::genesis::Bitvector const &, ::genesis::Bitvector const & ))( &::genesis::Bitvector::symmetric_difference ),
            ( boost::python::arg("lhs"), boost::python::arg("rhs") )
        )
        .def(
            "symmetric_difference",
            ( ::genesis::Bitvector ( ::genesis::Bitvector::* )( ::genesis::Bitvector const & ) const )( &::genesis::Bitvector::symmetric_difference ),
            ( boost::python::arg("rhs") )
        )
        .staticmethod("symmetric_difference")
        .def(
            "unset",
            ( void ( ::genesis::Bitvector::* )( size_t ))( &::genesis::Bitvector::unset ),
            ( boost::python::arg("index") ),
            get_docstring("void ::genesis::Bitvector::unset (size_t index)")
        )
        .def(
            "x_hash",
            ( ::genesis::Bitvector::IntType ( ::genesis::Bitvector::* )(  ) const )( &::genesis::Bitvector::x_hash ),
            get_docstring("IntType ::genesis::Bitvector::x_hash () const")
        )

        // Operators

        .def(
            "__len__",
            (::size_t ( ::genesis::Bitvector::* )(  ) const)( &::genesis::Bitvector::size )
        )
        .def(
            "__getitem__",
            ( bool ( ::genesis::Bitvector::* )( size_t ) const )( &::genesis::Bitvector::operator[] ),
            ( boost::python::arg("index") ),
            get_docstring("bool ::genesis::Bitvector::operator[] (size_t index) const")
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

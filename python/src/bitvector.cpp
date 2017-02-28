/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>
#include <pybind11/operators.h>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS (Bitvector, "utils")
{
    // -------------------------------------------------------------------
    //     Class Bitvector
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::Bitvector, std::shared_ptr<::genesis::utils::Bitvector> > ( scope, "Bitvector" )
        .def( pybind11::init<  >() )
        .def( pybind11::init< const size_t, const bool >(), pybind11::arg("size"), pybind11::arg("init")=(const bool)(false) )
        // .def( pybind11::init< const size_t, const std::initializer_list< int > >(( pybind11::arg("size"), pybind11::arg("list") )) )

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
        // .def(
        //     "dump_int",
        //     ( std::string ( ::genesis::utils::Bitvector::* )( IntType ) const )( &::genesis::utils::Bitvector::dump_int ),
        //     ( pybind11::arg("x") )
        // )
        .def(
            "flip",
            ( void ( ::genesis::utils::Bitvector::* )( size_t ))( &::genesis::utils::Bitvector::flip ),
            ( pybind11::arg("index") ),
            get_docstring("void ::genesis::utils::Bitvector::flip (size_t index)")
        )
        .def(
            "get",
            ( bool ( ::genesis::utils::Bitvector::* )( size_t ) const )( &::genesis::utils::Bitvector::get ),
            ( pybind11::arg("index") ),
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
            ( void ( ::genesis::utils::Bitvector::* )( const bool ))( &::genesis::utils::Bitvector::reset ),
            ( pybind11::arg("value")=(const bool)(false) ),
            get_docstring("void ::genesis::utils::Bitvector::reset (const bool value=false)")
        )
        .def(
            "set",
            ( void ( ::genesis::utils::Bitvector::* )( size_t ))( &::genesis::utils::Bitvector::set ),
            pybind11::arg("index"),
            get_docstring("void ::genesis::utils::Bitvector::set (size_t index)")
        )
        .def(
            "set",
            ( void ( ::genesis::utils::Bitvector::* )( size_t, bool ))( &::genesis::utils::Bitvector::set ),
            pybind11::arg("index"), pybind11::arg("value"),
            get_docstring("void ::genesis::utils::Bitvector::set (size_t index, bool value)")
        )
        .def(
            "size",
            ( size_t ( ::genesis::utils::Bitvector::* )(  ) const )( &::genesis::utils::Bitvector::size ),
            get_docstring("size_t ::genesis::utils::Bitvector::size () const")
        )
        // .def_static(
        //     "symmetric_difference",
        //     ( Bitvector ( * )( Bitvector const &, Bitvector const & ))( &::genesis::utils::Bitvector::symmetric_difference ),
        //     ( pybind11::arg("lhs"), pybind11::arg("rhs") )
        // )
        // .staticmethod("symmetric_difference")
        .def(
            "symmetric_difference",
            ( Bitvector ( ::genesis::utils::Bitvector::* )( Bitvector const & ) const )( &::genesis::utils::Bitvector::symmetric_difference ),
            ( pybind11::arg("rhs") )
        )
        .def(
            "unset",
            ( void ( ::genesis::utils::Bitvector::* )( size_t ))( &::genesis::utils::Bitvector::unset ),
            ( pybind11::arg("index") ),
            get_docstring("void ::genesis::utils::Bitvector::unset (size_t index)")
        )
        // .def(
        //     "x_hash",
        //     ( IntType ( ::genesis::utils::Bitvector::* )(  ) const )( &::genesis::utils::Bitvector::x_hash ),
        //     get_docstring("IntType ::genesis::utils::Bitvector::x_hash () const")
        // )

        // Operators

        .def( pybind11::self != pybind11::self )
        .def( pybind11::self &= pybind11::self )
        .def( pybind11::self < pybind11::self )
        .def( pybind11::self <= pybind11::self )
        .def( pybind11::self == pybind11::self )
        .def( pybind11::self > pybind11::self )
        .def( pybind11::self >= pybind11::self )
        .def(
            "__getitem__",
            ( bool ( ::genesis::utils::Bitvector::* )( size_t ) const )( &::genesis::utils::Bitvector::operator[] ),
            ( pybind11::arg("index") ),
            get_docstring("bool ::genesis::utils::Bitvector::operator[] (size_t index) const")
        )
        .def( pybind11::self ^= pybind11::self )
        .def( pybind11::self |= pybind11::self )
        .def( ~pybind11::self )
    ;
}

// PYTHON_EXPORT_FUNCTIONS(utils_math_bitvector_export, "utils")
// {
//
//     scope.def(
//         "operator&",
//         ( Bitvector ( * )( Bitvector const &, Bitvector const & ))( &::genesis::utils::operator& ),
//         ( pybind11::arg("lhs"), pybind11::arg("rhs") )
//     );
//
//     scope.def(
//         "operator-",
//         ( Bitvector ( * )( Bitvector const &, Bitvector const & ))( &::genesis::utils::operator- ),
//         ( pybind11::arg("lhs"), pybind11::arg("rhs") ),
//         get_docstring("Bitvector ::genesis::utils::operator- (Bitvector const & lhs, Bitvector const & rhs)")
//     );
//
//     scope.def(
//         "operator^",
//         ( Bitvector ( * )( Bitvector const &, Bitvector const & ))( &::genesis::utils::operator^ ),
//         ( pybind11::arg("lhs"), pybind11::arg("rhs") )
//     );
//
//     scope.def(
//         "operator|",
//         ( Bitvector ( * )( Bitvector const &, Bitvector const & ))( &::genesis::utils::operator| ),
//         ( pybind11::arg("lhs"), pybind11::arg("rhs") )
//     );
//
//     scope.def(
//         "operator<<",
//         ( std::ostream & ( * )( std::ostream &, Bitvector const & ))( &::genesis::utils::operator<< ),
//         ( pybind11::arg("s"), pybind11::arg("rhs") )
//     );
// }

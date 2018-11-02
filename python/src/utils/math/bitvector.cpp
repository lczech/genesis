/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::Bitvector, scope )
{

    // -------------------------------------------------------------------
    //     Class Bitvector
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::Bitvector, std::shared_ptr<::genesis::utils::Bitvector> > ( scope, "Bitvector" )
        .def(
            pybind11::init<  >(),
            get_docstring("::genesis::utils::Bitvector::Bitvector ()")
        )
        .def(
            pybind11::init< const size_t, const bool >(),
            pybind11::arg("size"),
            pybind11::arg("initial_value")=(const bool)(false),
            get_docstring("::genesis::utils::Bitvector::Bitvector (const size_t size, const bool initial_value=false)")
        )
        .def(
            pybind11::init< const size_t, const std::initializer_list< int > >(),
            pybind11::arg("size"),
            pybind11::arg("list"),
            get_docstring("::genesis::utils::Bitvector::Bitvector (const size_t size, const std::initializer_list< int > list)")
        )
        .def(
            pybind11::init< Bitvector const &, size_t >(),
            pybind11::arg("other"),
            pybind11::arg("bits"),
            get_docstring("::genesis::utils::Bitvector::Bitvector (Bitvector const & other, size_t bits)")
        )
        .def(
            pybind11::init< Bitvector const & >(),
            pybind11::arg("arg")
        )

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
            ( std::string ( ::genesis::utils::Bitvector::* )( ::genesis::utils::Bitvector::IntType ) const )( &::genesis::utils::Bitvector::dump_int ),
            pybind11::arg("x")
        )
        .def(
            "flip",
            ( void ( ::genesis::utils::Bitvector::* )( size_t ))( &::genesis::utils::Bitvector::flip ),
            pybind11::arg("index"),
            get_docstring("void ::genesis::utils::Bitvector::flip (size_t index)")
        )
        .def(
            "get",
            ( bool ( ::genesis::utils::Bitvector::* )( size_t ) const )( &::genesis::utils::Bitvector::get ),
            pybind11::arg("index"),
            get_docstring("bool ::genesis::utils::Bitvector::get (size_t index) const")
        )
        .def(
            "hash",
            ( size_t ( ::genesis::utils::Bitvector::* )(  ) const )( &::genesis::utils::Bitvector::hash ),
            get_docstring("size_t ::genesis::utils::Bitvector::hash () const")
        )
        .def(
            "negate",
            ( void ( ::genesis::utils::Bitvector::* )(  ))( &::genesis::utils::Bitvector::negate ),
            get_docstring("void ::genesis::utils::Bitvector::negate ()")
        )
        .def(
            "normalize",
            ( void ( ::genesis::utils::Bitvector::* )(  ))( &::genesis::utils::Bitvector::normalize ),
            get_docstring("void ::genesis::utils::Bitvector::normalize ()")
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
            pybind11::arg("index"),
            pybind11::arg("value"),
            get_docstring("void ::genesis::utils::Bitvector::set (size_t index, bool value)")
        )
        .def(
            "set_all",
            ( void ( ::genesis::utils::Bitvector::* )( const bool ))( &::genesis::utils::Bitvector::set_all ),
            pybind11::arg("value")=(const bool)(false),
            get_docstring("void ::genesis::utils::Bitvector::set_all (const bool value=false)")
        )
        .def(
            "size",
            ( size_t ( ::genesis::utils::Bitvector::* )(  ) const )( &::genesis::utils::Bitvector::size ),
            get_docstring("size_t ::genesis::utils::Bitvector::size () const")
        )
        .def(
            "unset",
            ( void ( ::genesis::utils::Bitvector::* )( size_t ))( &::genesis::utils::Bitvector::unset ),
            pybind11::arg("index"),
            get_docstring("void ::genesis::utils::Bitvector::unset (size_t index)")
        )
        .def(
            "x_hash",
            ( ::genesis::utils::Bitvector::IntType ( ::genesis::utils::Bitvector::* )(  ) const )( &::genesis::utils::Bitvector::x_hash ),
            get_docstring("IntType ::genesis::utils::Bitvector::x_hash () const")
        )

        // Operators

        .def( pybind11::self != pybind11::self )
        .def( pybind11::self &= pybind11::self )
        .def( pybind11::self == pybind11::self )
        .def(
            "__getitem__",
            ( bool ( ::genesis::utils::Bitvector::* )( size_t ) const )( &::genesis::utils::Bitvector::operator[] ),
            pybind11::arg("index"),
            get_docstring("bool ::genesis::utils::Bitvector::operator[] (size_t index) const")
        )
        .def( pybind11::self ^= pybind11::self )
        .def( pybind11::self |= pybind11::self )
        .def( ~pybind11::self )
        .def(
            "__str__",
            []( ::genesis::utils::Bitvector const& obj ) -> std::string {
                std::ostringstream s;
                s << obj;
                return s.str();
            }
        )
    ;
}

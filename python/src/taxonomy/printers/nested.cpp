/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::taxonomy;

PYTHON_EXPORT_CLASS( ::genesis::taxonomy::PrinterNested, scope )
{

    // -------------------------------------------------------------------
    //     Class PrinterNested
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::taxonomy::PrinterNested, std::shared_ptr<::genesis::taxonomy::PrinterNested> > ( scope, "PrinterNested" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< PrinterNested const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "depth_limit",
            ( PrinterNested & ( ::genesis::taxonomy::PrinterNested::* )( int ))( &::genesis::taxonomy::PrinterNested::depth_limit ),
            pybind11::arg("value")
        )
        .def(
            "depth_limit",
            ( int ( ::genesis::taxonomy::PrinterNested::* )(  ) const )( &::genesis::taxonomy::PrinterNested::depth_limit )
        )
        .def(
            "line_limit",
            ( PrinterNested & ( ::genesis::taxonomy::PrinterNested::* )( int ))( &::genesis::taxonomy::PrinterNested::line_limit ),
            pybind11::arg("value")
        )
        .def(
            "line_limit",
            ( int ( ::genesis::taxonomy::PrinterNested::* )(  ) const )( &::genesis::taxonomy::PrinterNested::line_limit )
        )
        .def(
            "print",
            ( std::string ( ::genesis::taxonomy::PrinterNested::* )( Taxonomy const & ) const )( &::genesis::taxonomy::PrinterNested::print ),
            pybind11::arg("tax")
        )
        .def(
            "print",
            ( void ( ::genesis::taxonomy::PrinterNested::* )( std::ostream &, Taxonomy const & ) const )( &::genesis::taxonomy::PrinterNested::print ),
            pybind11::arg("out"),
            pybind11::arg("tax")
        )
        .def(
            "print_ranks",
            ( PrinterNested & ( ::genesis::taxonomy::PrinterNested::* )( bool ))( &::genesis::taxonomy::PrinterNested::print_ranks ),
            pybind11::arg("value")
        )
        .def(
            "print_ranks",
            ( bool ( ::genesis::taxonomy::PrinterNested::* )(  ) const )( &::genesis::taxonomy::PrinterNested::print_ranks )
        )
    ;
}

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::PrinterDetailed, scope )
{

    // -------------------------------------------------------------------
    //     Class PrinterDetailed
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::PrinterDetailed, std::shared_ptr<::genesis::tree::PrinterDetailed> > ( scope, "PrinterDetailed" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< PrinterDetailed const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "print",
            ( std::string ( ::genesis::tree::PrinterDetailed::* )( Tree const & ) const )( &::genesis::tree::PrinterDetailed::print ),
            pybind11::arg("tree")
        )
        .def(
            "print",
            ( void ( ::genesis::tree::PrinterDetailed::* )( std::ostream &, Tree const & ) const )( &::genesis::tree::PrinterDetailed::print ),
            pybind11::arg("out"),
            pybind11::arg("tree")
        )
        .def(
            "use_color",
            ( PrinterDetailed & ( ::genesis::tree::PrinterDetailed::* )( bool ))( &::genesis::tree::PrinterDetailed::use_color ),
            pybind11::arg("value")
        )
        .def(
            "use_color",
            ( bool ( ::genesis::tree::PrinterDetailed::* )(  ) const )( &::genesis::tree::PrinterDetailed::use_color )
        )
    ;
}

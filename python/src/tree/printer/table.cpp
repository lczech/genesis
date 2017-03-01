/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::PrinterTable, scope )
{

    // -------------------------------------------------------------------
    //     Class PrinterTable
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::PrinterTable, std::shared_ptr<::genesis::tree::PrinterTable> > ( scope, "PrinterTable" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< PrinterTable const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "print",
            ( std::string ( ::genesis::tree::PrinterTable::* )( Tree const & ))( &::genesis::tree::PrinterTable::print ),
            pybind11::arg("tree")
        )
        .def(
            "print",
            ( void ( ::genesis::tree::PrinterTable::* )( std::ostream &, Tree const & ))( &::genesis::tree::PrinterTable::print ),
            pybind11::arg("out"),
            pybind11::arg("tree")
        )
    ;
}

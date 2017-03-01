/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::PrinterCompact, scope )
{

    // -------------------------------------------------------------------
    //     Class PrinterCompact
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::PrinterCompact, std::shared_ptr<::genesis::tree::PrinterCompact> > ( scope, "PrinterCompact" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< PrinterCompact const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "print",
            ( std::string ( ::genesis::tree::PrinterCompact::* )( Tree const & ))( &::genesis::tree::PrinterCompact::print ),
            pybind11::arg("tree")
        )
        .def(
            "print",
            ( std::string ( ::genesis::tree::PrinterCompact::* )( Tree const &, std::function< std::string(TreeNode const &node, TreeEdge const &edge)> const ))( &::genesis::tree::PrinterCompact::print ),
            pybind11::arg("tree"),
            pybind11::arg("print_line")
        )
        .def(
            "print",
            ( void ( ::genesis::tree::PrinterCompact::* )( std::ostream &, Tree const &, std::function< std::string(TreeNode const &node, TreeEdge const &edge)> const ))( &::genesis::tree::PrinterCompact::print ),
            pybind11::arg("out"),
            pybind11::arg("tree"),
            pybind11::arg("print_line")
        )
    ;
}

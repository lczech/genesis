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
            pybind11::init<  >(),
            get_docstring("::genesis::tree::PrinterCompact::PrinterCompact ()")
        )
        .def(
            pybind11::init< PrinterCompact const & >(),
            pybind11::arg("arg"),
            get_docstring("::genesis::tree::PrinterCompact::PrinterCompact (PrinterCompact const & )")
        )

        // Public Member Functions

        .def(
            "limit",
            ( PrinterCompact & ( ::genesis::tree::PrinterCompact::* )( long ))( &::genesis::tree::PrinterCompact::limit ),
            pybind11::arg("value"),
            get_docstring("PrinterCompact & ::genesis::tree::PrinterCompact::limit (long value)")
        )
        .def(
            "limit",
            ( long ( ::genesis::tree::PrinterCompact::* )(  ) const )( &::genesis::tree::PrinterCompact::limit ),
            get_docstring("long ::genesis::tree::PrinterCompact::limit () const")
        )
        .def(
            "print",
            ( std::string ( ::genesis::tree::PrinterCompact::* )( Tree const & ))( &::genesis::tree::PrinterCompact::print ),
            pybind11::arg("tree"),
            get_docstring("std::string ::genesis::tree::PrinterCompact::print (Tree const & tree)")
        )
        .def(
            "print",
            ( std::string ( ::genesis::tree::PrinterCompact::* )( Tree const &, std::function< std::string(TreeNode const &node, TreeEdge const &edge)> const ))( &::genesis::tree::PrinterCompact::print ),
            pybind11::arg("tree"),
            pybind11::arg("print_line"),
            get_docstring("std::string ::genesis::tree::PrinterCompact::print (Tree const & tree, std::function< std::string(TreeNode const &node, TreeEdge const &edge)> const print_line)")
        )
        .def(
            "print",
            ( void ( ::genesis::tree::PrinterCompact::* )( std::ostream &, Tree const &, std::function< std::string(TreeNode const &node, TreeEdge const &edge)> const ))( &::genesis::tree::PrinterCompact::print ),
            pybind11::arg("out"),
            pybind11::arg("tree"),
            pybind11::arg("print_line"),
            get_docstring("void ::genesis::tree::PrinterCompact::print (std::ostream & out, Tree const & tree, std::function< std::string(TreeNode const &node, TreeEdge const &edge)> const print_line)")
        )
    ;
}

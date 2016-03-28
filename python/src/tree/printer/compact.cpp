/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::tree;

/*
PYTHON_EXPORT_CLASS (PrinterCompact, "tree")
{

    // -------------------------------------------------------------------
    //     Class PrinterCompact
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::tree::PrinterCompact > ( "PrinterCompact", boost::python::init<  >(  ) )
        .def( boost::python::init< PrinterCompact const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "print",
            ( std::string ( ::genesis::tree::PrinterCompact::* )( TreeType const & ))( &::genesis::tree::PrinterCompact::print ),
            ( boost::python::arg("tree") ),
            get_docstring("std::string ::genesis::tree::PrinterCompact::print (TreeType const & tree)")
        )
        .def(
            "print",
            ( std::string ( ::genesis::tree::PrinterCompact::* )( TreeType const &, std::function< std::string(typename TreeType::NodeType const &node, typename TreeType::EdgeType const &edge)> const ))( &::genesis::tree::PrinterCompact::print ),
            ( boost::python::arg("tree"), boost::python::arg("print_line") )
        )
        .def(
            "print",
            ( void ( ::genesis::tree::PrinterCompact::* )( std::ostream &, TreeType const &, std::function< std::string(typename TreeType::NodeType const &node, typename TreeType::EdgeType const &edge)> const ))( &::genesis::tree::PrinterCompact::print ),
            ( boost::python::arg("out"), boost::python::arg("tree"), boost::python::arg("print_line") ),
            get_docstring("void ::genesis::tree::PrinterCompact::print (std::ostream & out, TreeType const & tree, std::function< std::string(typename TreeType::NodeType const &node, typename TreeType::EdgeType const &edge)> const print_line)")
        )
    ;
}
*/

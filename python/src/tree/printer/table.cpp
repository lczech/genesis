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
PYTHON_EXPORT_CLASS (PrinterTable, "tree")
{

    // -------------------------------------------------------------------
    //     Class PrinterTable
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::tree::PrinterTable > ( "PrinterTable", boost::python::init<  >(  ) )
        .def( boost::python::init< PrinterTable const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "print",
            ( std::string ( ::genesis::tree::PrinterTable::* )( TreeType const & ))( &::genesis::tree::PrinterTable::print ),
            ( boost::python::arg("tree") )
        )
        .def(
            "print",
            ( void ( ::genesis::tree::PrinterTable::* )( std::ostream &, TreeType const & ))( &::genesis::tree::PrinterTable::print ),
            ( boost::python::arg("out"), boost::python::arg("tree") )
        )
    ;
}
*/

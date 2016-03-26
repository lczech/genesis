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
PYTHON_EXPORT_CLASS (PrinterDetailed, "tree")
{

    // -------------------------------------------------------------------
    //     Class PrinterDetailed
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::tree::PrinterDetailed > ( "PrinterDetailed", boost::python::init<  >(  ) )
        .def( boost::python::init< PrinterDetailed const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "print",
            ( std::string ( ::genesis::tree::PrinterDetailed::* )( TreeType const & ))( &::genesis::tree::PrinterDetailed::print ),
            ( boost::python::arg("tree") )
        )
        .def(
            "print",
            ( void ( ::genesis::tree::PrinterDetailed::* )( std::ostream &, TreeType const & ))( &::genesis::tree::PrinterDetailed::print ),
            ( boost::python::arg("out"), boost::python::arg("tree") )
        )
        .def(
            "use_color",
            ( PrinterDetailed & ( ::genesis::tree::PrinterDetailed::* )( bool ))( &::genesis::tree::PrinterDetailed::use_color ),
            ( boost::python::arg("value") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "use_color",
            ( bool ( ::genesis::tree::PrinterDetailed::* )(  ) const )( &::genesis::tree::PrinterDetailed::use_color )
        )
    ;
}
*/

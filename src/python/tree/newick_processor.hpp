/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <boost/python.hpp>

#include "tree/newick_processor.hpp"

// -------------------------------------------------------------------
//     Forward Declarations
// -------------------------------------------------------------------

boost::python::class_<genesis::NewickProcessor> BoostPythonExport_Get_NewickProcessor_Class();

// -------------------------------------------------------------------
//     Newick Processor Template Methods
// -------------------------------------------------------------------

template<class TreeType>
void BoostPythonExport_Overload_NewickProcessor()
{
    BoostPythonExport_Get_NewickProcessor_Class()
        .def(
            "from_file",
            ( bool ( * )( const std::string&, TreeType& ))( &::genesis::NewickProcessor::from_file ),
            ( boost::python::arg("fn"), boost::python::arg("tree") )
        )
        .def(
            "from_string",
            ( bool ( * )( const std::string&, TreeType& ))( &::genesis::NewickProcessor::from_string ),
            ( boost::python::arg("ts"), boost::python::arg("tree") )
        )
        .def(
            "to_file",
            ( bool ( * )( const TreeType&, const std::string ))( &::genesis::NewickProcessor::to_file ),
            ( boost::python::arg("fn"), boost::python::arg("tree") )
        )
        .def(
            "to_string",
            ( std::string ( * )( const TreeType& ))( &::genesis::NewickProcessor::to_string ),
            ( boost::python::arg("tree") )
        )
    ;
}

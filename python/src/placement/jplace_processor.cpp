/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/placement/jplace_processor.hpp"
#include "lib/placement/placement_map.hpp"

void BoostPythonExport_JplaceProcessor()
{
    using namespace genesis;

    boost::python::class_< ::genesis::JplaceProcessor > ( "JplaceProcessor" )

        // Public Member Functions

        .def(
            "check_version",
            ( bool ( * )( const std::string ))( &::genesis::JplaceProcessor::check_version ),
            ( boost::python::arg("version") ),
            get_docstring("static bool ::genesis::JplaceProcessor::check_version (const std::string version)")
        )
        .staticmethod("check_version")
        // .def(
        //     "from_document",
        //     ( bool ( * )( const JsonDocument &, PlacementMap & ))( &::genesis::JplaceProcessor::from_document ),
        //     ( boost::python::arg("doc"), boost::python::arg("placements") ),
        //     get_docstring("static bool ::genesis::JplaceProcessor::from_document (const JsonDocument & doc, PlacementMap & placements)")
        // )
        // .staticmethod("from_document")
        .def(
            "from_file",
            ( bool ( ::genesis::JplaceProcessor::* )( const std::string &, PlacementMap & ))( &::genesis::JplaceProcessor::from_file ),
            ( boost::python::arg("fn"), boost::python::arg("placements") ),
            get_docstring("static bool ::genesis::JplaceProcessor::from_file (const std::string & fn, PlacementMap & placements)")
        )
        .def(
            "from_string",
            ( bool ( ::genesis::JplaceProcessor::* )( const std::string &, PlacementMap & ))( &::genesis::JplaceProcessor::from_string ),
            ( boost::python::arg("jplace"), boost::python::arg("placements") ),
            get_docstring("static bool ::genesis::JplaceProcessor::from_string (const std::string & jplace, PlacementMap & placements)")
        )
        .def(
            "get_version",
            ( std::string ( * )(  ))( &::genesis::JplaceProcessor::get_version ),
            get_docstring("static std::string ::genesis::JplaceProcessor::get_version ()")
        )
        .staticmethod("get_version")
        // .def(
        //     "to_document",
        //     ( void ( * )( const PlacementMap &, JsonDocument & ))( &::genesis::JplaceProcessor::to_document ),
        //     ( boost::python::arg("placements"), boost::python::arg("doc") )
        // )
        // .staticmethod("to_document")
        .def(
            "to_file",
            ( bool ( ::genesis::JplaceProcessor::* )( const PlacementMap &, const std::string ))( &::genesis::JplaceProcessor::to_file ),
            ( boost::python::arg("placements"), boost::python::arg("fn") )
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::JplaceProcessor::* )( const PlacementMap & ))( &::genesis::JplaceProcessor::to_string ),
            ( boost::python::arg("placements") )
        )
        .def(
            "to_string",
            ( void ( ::genesis::JplaceProcessor::* )( const PlacementMap &, std::string & ))( &::genesis::JplaceProcessor::to_string ),
            ( boost::python::arg("placements"), boost::python::arg("jplace") )
        )

        // Public Member Variables

        .def_readwrite("report_invalid_numbers",  &::genesis::JplaceProcessor::report_invalid_numbers)
        .def_readwrite("correct_invalid_numbers", &::genesis::JplaceProcessor::correct_invalid_numbers)
    ;

}

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/tree/io/phyloxml_processor.hpp"

#include "lib/tree/default_tree.hpp"
#include "lib/placement/placement_tree.hpp"

// -------------------------------------------------------------------
//     Class Phyloxml Processor
// -------------------------------------------------------------------

template <typename AdapterType>
void BoostPythonExport_PhyloxmlProcessor (std::string name)
{
    typedef ::genesis::PhyloxmlProcessor<AdapterType> PhyloxmlProcessorType;
    typedef typename AdapterType::TreeType TreeType;

    boost::python::class_< PhyloxmlProcessorType > ( name.c_str(), boost::python::init<  >(  ) )
        .def( boost::python::init< AdapterType & >(( boost::python::arg("adapter") )) )

        // Public Member Functions

        .def(
            "to_file",
            ( bool ( PhyloxmlProcessorType::* )( const TreeType &, const std::string ))( &PhyloxmlProcessorType::to_file ),
            ( boost::python::arg("tree"), boost::python::arg("fn") ),
            get_docstring("bool ::genesis::PhyloxmlProcessor::to_file (const Tree< NDT, EDT > & tree, const std::string fn)")
        )
        .def(
            "to_string",
            ( std::string ( PhyloxmlProcessorType::* )( const TreeType & ))( &PhyloxmlProcessorType::to_string ),
            ( boost::python::arg("tree") ),
            get_docstring("std::string ::genesis::PhyloxmlProcessor::to_string (const Tree< NDT, EDT > & tree)")
        )
    ;
}

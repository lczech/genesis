/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <boost/python.hpp>

#include "placement/placement_tree.hpp"
#include "plausibility/plausibility_tree.hpp"

#include "tree/newick_processor.hpp"

#include "../src/python/tree/tree.hpp"

// -------------------------------------------------------------------
//     Newick Processor Template Methods
// -------------------------------------------------------------------

#define GENERATE_NEWICK_PROCESSOR_METHODS(classname) \
    .def(                                                                                                              \
        "from_file",                                                                                                    \
        ( bool ( * )( const std::string&, ::genesis::classname& ))( &::genesis::NewickProcessor::from_file ),           \
        ( boost::python::arg("fn"), boost::python::arg("tree") )                                                       \
    )                                                                                                                  \
    .def(                                                                                                              \
        "from_string",                                                                                                  \
        ( bool ( * )( const std::string&, ::genesis::classname& ))( &::genesis::NewickProcessor::from_string ),         \
        ( boost::python::arg("ts"), boost::python::arg("tree") )                                                       \
    )                                                                                                                  \
    .def(                                                                                                              \
        "to_file",                                                                                                      \
        ( bool ( * )( const ::genesis::classname&, const std::string ))( &::genesis::NewickProcessor::to_file ),        \
        ( boost::python::arg("fn"), boost::python::arg("tree") )                                                       \
    )                                                                                                                  \
    .def(                                                                                                              \
        "to_string",                                                                                                    \
        ( std::string ( * )( const ::genesis::classname& ))( &::genesis::NewickProcessor::to_string ),                  \
        ( boost::python::arg("tree") )                                                                                 \
    )

// -------------------------------------------------------------------
//     Class NewickProcessor
// -------------------------------------------------------------------

void BoostPythonExport_NewickProcessor()
{

    boost::python::class_< ::genesis::NewickProcessor > ( "NewickProcessor" )

        // Public Member Functions

        GENERATE_NEWICK_PROCESSOR_METHODS(PlacementTree)
        GENERATE_NEWICK_PROCESSOR_METHODS(PlausibilityTree)

        .staticmethod("from_file")
        .staticmethod("from_string")
        .staticmethod("to_file")
        .staticmethod("to_string")

        // Public Member Variables

        .def_readwrite("print_names",          &::genesis::NewickProcessor::print_names)
        .def_readwrite("print_branch_lengths", &::genesis::NewickProcessor::print_branch_lengths)
        .def_readwrite("print_comments",       &::genesis::NewickProcessor::print_comments)
        .def_readwrite("print_tags",           &::genesis::NewickProcessor::print_tags)

    ;
}

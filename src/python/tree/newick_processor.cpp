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
        "FromFile",                                                                                                    \
        ( bool ( * )( const std::string&, ::genesis::classname& ))( &::genesis::NewickProcessor::FromFile ),           \
        ( boost::python::arg("fn"), boost::python::arg("tree") )                                                       \
    )                                                                                                                  \
    .def(                                                                                                              \
        "FromString",                                                                                                  \
        ( bool ( * )( const std::string&, ::genesis::classname& ))( &::genesis::NewickProcessor::FromString ),         \
        ( boost::python::arg("ts"), boost::python::arg("tree") )                                                       \
    )                                                                                                                  \
    .def(                                                                                                              \
        "ToFile",                                                                                                      \
        ( bool ( * )( const std::string&, const ::genesis::classname& ))( &::genesis::NewickProcessor::ToFile ),       \
        ( boost::python::arg("fn"), boost::python::arg("tree") )                                                       \
    )                                                                                                                  \
    .def(                                                                                                              \
        "ToString",                                                                                                    \
        ( std::string ( * )( const ::genesis::classname& ))( &::genesis::NewickProcessor::ToString ),                  \
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

        .staticmethod("FromFile")
        .staticmethod("FromString")
        .staticmethod("ToFile")
        .staticmethod("ToString")

        // Public Member Variables

        .def_readwrite("print_names",          &::genesis::NewickProcessor::print_names)
        .def_readwrite("print_branch_lengths", &::genesis::NewickProcessor::print_branch_lengths)
        .def_readwrite("print_comments",       &::genesis::NewickProcessor::print_comments)
        .def_readwrite("print_tags",           &::genesis::NewickProcessor::print_tags)

    ;
}

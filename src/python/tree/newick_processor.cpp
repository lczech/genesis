/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <boost/python.hpp>

#include "tree/newick_processor.hpp"

// -------------------------------------------------------------------
//     Class NewickProcessor
// -------------------------------------------------------------------

static auto newick_processor_ = boost::python::class_< ::genesis::NewickProcessor > ( "NewickProcessor" )

    // Public Member Variables

    .def_readwrite("print_names",          &::genesis::NewickProcessor::print_names)
    .def_readwrite("print_branch_lengths", &::genesis::NewickProcessor::print_branch_lengths)
    .def_readwrite("print_comments",       &::genesis::NewickProcessor::print_comments)
    .def_readwrite("print_tags",           &::genesis::NewickProcessor::print_tags)
;

boost::python::class_<genesis::NewickProcessor> BoostPythonExport_Get_NewickProcessor_Class()
{
    return newick_processor_;
}

void BoostPythonExport_Finalize_NewickProcessor()
{
    newick_processor_
        .staticmethod("from_file")
        .staticmethod("from_string")
        .staticmethod("to_file")
        .staticmethod("to_string")
    ;
}

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <boost/python.hpp>

#include "tree/newick_processor.hpp"

#include "tree/default_tree.hpp"
#include "placement/placement_tree.hpp"

// -------------------------------------------------------------------
//     Class NewickProcessor
// -------------------------------------------------------------------

template<class TreeType>
void Overload_NewickProcessor(boost::python::class_<genesis::NewickProcessor>& newick_processor)
{
    newick_processor
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

void BoostPythonExport_NewickProcessor()
{
    auto newick_processor_ = boost::python::class_< ::genesis::NewickProcessor > ( "NewickProcessor" )

        // Public Member Variables

        .def_readwrite("print_names",          &::genesis::NewickProcessor::print_names)
        .def_readwrite("print_branch_lengths", &::genesis::NewickProcessor::print_branch_lengths)
        .def_readwrite("print_comments",       &::genesis::NewickProcessor::print_comments)
        .def_readwrite("print_tags",           &::genesis::NewickProcessor::print_tags)
    ;

    /*
        TODO newick tree and other tree related classes are initilized using those hard coded lists
        of classes. instead, a more flexible approach could be to use variadic templates to achieve this:
        the tree export function takes the class types of all trees to be exported, defines the python
        bindings for them, and then delegates to all the other tree related exporters using the same
        template paramters.
    */
    Overload_NewickProcessor<genesis::PlacementTree>(newick_processor_);
    Overload_NewickProcessor<genesis::DefaultTree>  (newick_processor_);

    newick_processor_
        .staticmethod("from_file")
        .staticmethod("from_string")
        .staticmethod("to_file")
        .staticmethod("to_string")
    ;
}

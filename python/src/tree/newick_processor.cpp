/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/tree/newick_processor.hpp"

#include "lib/tree/default_tree.hpp"
#include "lib/placement/placement_tree.hpp"

// -------------------------------------------------------------------
//     Class NewickProcessor
// -------------------------------------------------------------------

template<class TreeType>
void Overload_NewickProcessor(boost::python::class_<genesis::NewickProcessor>& newick_processor)
{
    newick_processor
        .def(
            "from_file",
            ( bool ( ::genesis::NewickProcessor::* )( const std::string&, TreeType& ))( &::genesis::NewickProcessor::from_file ),
            ( boost::python::arg("fn"), boost::python::arg("tree") )
        )
        .def(
            "from_string",
            ( bool ( ::genesis::NewickProcessor::* )( const std::string&, TreeType& ))( &::genesis::NewickProcessor::from_string ),
            ( boost::python::arg("ts"), boost::python::arg("tree") )
        )
        .def(
            "to_file",
            ( bool ( ::genesis::NewickProcessor::* )( const TreeType&, const std::string ))( &::genesis::NewickProcessor::to_file ),
            ( boost::python::arg("fn"), boost::python::arg("tree") )
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::NewickProcessor::* )( const TreeType& ))( &::genesis::NewickProcessor::to_string ),
            ( boost::python::arg("tree") )
        )
    ;
}

void BoostPythonExport_NewickProcessor()
{
    auto newick_processor_ = boost::python::class_< ::genesis::NewickProcessor > ( "NewickProcessor" )

        // Public Member Variables

        .def_readwrite("default_leaf_name",     &::genesis::NewickProcessor::default_leaf_name)
        .def_readwrite("default_internal_name", &::genesis::NewickProcessor::default_internal_name)
        .def_readwrite("default_root_name",     &::genesis::NewickProcessor::default_root_name)
        .def_readwrite("use_default_names",     &::genesis::NewickProcessor::use_default_names)

        .def_readwrite("print_names",           &::genesis::NewickProcessor::print_names)
        .def_readwrite("print_branch_lengths",  &::genesis::NewickProcessor::print_branch_lengths)
        .def_readwrite("print_comments",        &::genesis::NewickProcessor::print_comments)
        .def_readwrite("print_tags",            &::genesis::NewickProcessor::print_tags)
        .def_readwrite("precision",             &::genesis::NewickProcessor::precision)
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
}

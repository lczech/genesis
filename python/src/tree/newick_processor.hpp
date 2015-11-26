/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/tree/io/newick/processor.hpp"

#include "lib/tree/default/tree.hpp"
#include "lib/placement/placement_tree.hpp"

// -------------------------------------------------------------------
//     Class NewickProcessor
// -------------------------------------------------------------------

template <typename AdapterType>
void BoostPythonExport_NewickProcessor(std::string name)
{
    typedef ::genesis::NewickProcessor<AdapterType> NewickProcessorType;
    typedef typename AdapterType::TreeType TreeType;

    boost::python::class_< NewickProcessorType > ( name.c_str(), boost::python::init<  >(  ) )
        .def( boost::python::init< AdapterType & >(( boost::python::arg("adapter") )) )

        // Public Member Functions

        .def(
            "from_file",
            ( bool ( NewickProcessorType::* )( const std::string&, TreeType& ))( &NewickProcessorType::from_file ),
            ( boost::python::arg("fn"), boost::python::arg("tree") )
        )
        .def(
            "from_string",
            ( bool ( NewickProcessorType::* )( const std::string&, TreeType& ))( &NewickProcessorType::from_string ),
            ( boost::python::arg("ts"), boost::python::arg("tree") )
        )
        .def(
            "to_file",
            ( bool ( NewickProcessorType::* )( const TreeType&, const std::string ))( &NewickProcessorType::to_file ),
            ( boost::python::arg("fn"), boost::python::arg("tree") )
        )
        .def(
            "to_string",
            ( std::string ( NewickProcessorType::* )( const TreeType& ))( &NewickProcessorType::to_string ),
            ( boost::python::arg("tree") )
        )

        // Public Member Variables

        // .def_readwrite("default_leaf_name",     &NewickProcessorType::default_leaf_name)
        // .def_readwrite("default_internal_name", &NewickProcessorType::default_internal_name)
        // .def_readwrite("default_root_name",     &NewickProcessorType::default_root_name)
        // .def_readwrite("use_default_names",     &NewickProcessorType::use_default_names)

        .def_readwrite("print_names",           &NewickProcessorType::print_names)
        .def_readwrite("print_branch_lengths",  &NewickProcessorType::print_branch_lengths)
        .def_readwrite("print_comments",        &NewickProcessorType::print_comments)
        .def_readwrite("print_tags",            &NewickProcessorType::print_tags)
        .def_readwrite("precision",             &NewickProcessorType::precision)
    ;

}

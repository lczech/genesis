/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::tree;

template<class TreeType>
void python_export_function_tree_default_functions_TreeType ()
{
    boost::python::def(
        "find_node",
        ( TreeType::NodeType * ( * )( TreeType &, const std::string &, bool ))( &::genesis::tree::find_node ),
        ( boost::python::arg("tree"), boost::python::arg("name"), boost::python::arg("replace_underscores")=(bool)(false) ),
        boost::python::return_value_policy<boost::python::reference_existing_object>(),
        get_docstring("TreeType::NodeType * ::genesis::tree::find_node (TreeType & tree, const std::string & name, bool replace_underscores=false)")
    );

    boost::python::def(
        "node_names",
        ( std::vector< std::string > ( * )( TreeType &, bool ))( &::genesis::tree::node_names ),
        ( boost::python::arg("tree"), boost::python::arg("leaves_only")=(bool)(false) ),
        get_docstring("std::vector< std::string > ::genesis::tree::node_names (TreeType & tree, bool leaves_only=false)")
    );
}


/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::CommonTreeNewickWriter, scope )
{

    // -------------------------------------------------------------------
    //     Class CommonTreeNewickWriter
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::CommonTreeNewickWriter, std::shared_ptr<::genesis::tree::CommonTreeNewickWriter> > ( scope, "CommonTreeNewickWriter" )
        .def(
            pybind11::init<  >()
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::tree::CommonTreeNewickWriterPlugin, scope )
{

    // -------------------------------------------------------------------
    //     Class CommonTreeNewickWriterPlugin
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::CommonTreeNewickWriterPlugin, std::shared_ptr<::genesis::tree::CommonTreeNewickWriterPlugin> > ( scope, "CommonTreeNewickWriterPlugin" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< CommonTreeNewickWriterPlugin const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "branch_length_precision",
            ( int ( ::genesis::tree::CommonTreeNewickWriterPlugin::* )(  ) const )( &::genesis::tree::CommonTreeNewickWriterPlugin::branch_length_precision ),
            get_docstring("int ::genesis::tree::CommonTreeNewickWriterPlugin::branch_length_precision () const")
        )
        .def(
            "branch_length_precision",
            ( ::genesis::tree::CommonTreeNewickWriterPlugin & ( ::genesis::tree::CommonTreeNewickWriterPlugin::* )( int ))( &::genesis::tree::CommonTreeNewickWriterPlugin::branch_length_precision ),
            pybind11::arg("value"),
            get_docstring("self_type & ::genesis::tree::CommonTreeNewickWriterPlugin::branch_length_precision (int value)")
        )
        .def(
            "default_inner_name",
            ( ::genesis::tree::CommonTreeNewickWriterPlugin & ( ::genesis::tree::CommonTreeNewickWriterPlugin::* )( std::string const & ))( &::genesis::tree::CommonTreeNewickWriterPlugin::default_inner_name ),
            pybind11::arg("value"),
            get_docstring("self_type & ::genesis::tree::CommonTreeNewickWriterPlugin::default_inner_name (std::string const & value)")
        )
        .def(
            "default_inner_name",
            ( std::string const & ( ::genesis::tree::CommonTreeNewickWriterPlugin::* )(  ) const )( &::genesis::tree::CommonTreeNewickWriterPlugin::default_inner_name ),
            get_docstring("std::string const & ::genesis::tree::CommonTreeNewickWriterPlugin::default_inner_name () const")
        )
        .def(
            "default_leaf_name",
            ( ::genesis::tree::CommonTreeNewickWriterPlugin & ( ::genesis::tree::CommonTreeNewickWriterPlugin::* )( std::string const & ))( &::genesis::tree::CommonTreeNewickWriterPlugin::default_leaf_name ),
            pybind11::arg("value"),
            get_docstring("self_type & ::genesis::tree::CommonTreeNewickWriterPlugin::default_leaf_name (std::string const & value)")
        )
        .def(
            "default_leaf_name",
            ( std::string const & ( ::genesis::tree::CommonTreeNewickWriterPlugin::* )(  ) const )( &::genesis::tree::CommonTreeNewickWriterPlugin::default_leaf_name ),
            get_docstring("std::string const & ::genesis::tree::CommonTreeNewickWriterPlugin::default_leaf_name () const")
        )
        .def(
            "default_root_name",
            ( ::genesis::tree::CommonTreeNewickWriterPlugin & ( ::genesis::tree::CommonTreeNewickWriterPlugin::* )( std::string const & ))( &::genesis::tree::CommonTreeNewickWriterPlugin::default_root_name ),
            pybind11::arg("value"),
            get_docstring("self_type & ::genesis::tree::CommonTreeNewickWriterPlugin::default_root_name (std::string const & value)")
        )
        .def(
            "default_root_name",
            ( std::string const & ( ::genesis::tree::CommonTreeNewickWriterPlugin::* )(  ) const )( &::genesis::tree::CommonTreeNewickWriterPlugin::default_root_name ),
            get_docstring("std::string const & ::genesis::tree::CommonTreeNewickWriterPlugin::default_root_name () const")
        )
        .def(
            "edge_to_element",
            ( void ( ::genesis::tree::CommonTreeNewickWriterPlugin::* )( TreeEdge const &, NewickBrokerElement & ) const )( &::genesis::tree::CommonTreeNewickWriterPlugin::edge_to_element ),
            pybind11::arg("edge"),
            pybind11::arg("element")
        )
        .def(
            "enable_branch_lengths",
            ( bool ( ::genesis::tree::CommonTreeNewickWriterPlugin::* )(  ) const )( &::genesis::tree::CommonTreeNewickWriterPlugin::enable_branch_lengths ),
            get_docstring("bool ::genesis::tree::CommonTreeNewickWriterPlugin::enable_branch_lengths () const")
        )
        .def(
            "enable_branch_lengths",
            ( ::genesis::tree::CommonTreeNewickWriterPlugin & ( ::genesis::tree::CommonTreeNewickWriterPlugin::* )( bool ))( &::genesis::tree::CommonTreeNewickWriterPlugin::enable_branch_lengths ),
            pybind11::arg("value"),
            get_docstring("self_type & ::genesis::tree::CommonTreeNewickWriterPlugin::enable_branch_lengths (bool value)")
        )
        .def(
            "enable_names",
            ( bool ( ::genesis::tree::CommonTreeNewickWriterPlugin::* )(  ) const )( &::genesis::tree::CommonTreeNewickWriterPlugin::enable_names ),
            get_docstring("bool ::genesis::tree::CommonTreeNewickWriterPlugin::enable_names () const")
        )
        .def(
            "enable_names",
            ( ::genesis::tree::CommonTreeNewickWriterPlugin & ( ::genesis::tree::CommonTreeNewickWriterPlugin::* )( bool ))( &::genesis::tree::CommonTreeNewickWriterPlugin::enable_names ),
            pybind11::arg("value"),
            get_docstring("self_type & ::genesis::tree::CommonTreeNewickWriterPlugin::enable_names (bool value)")
        )
        .def(
            "node_to_element",
            ( void ( ::genesis::tree::CommonTreeNewickWriterPlugin::* )( TreeNode const &, NewickBrokerElement & ) const )( &::genesis::tree::CommonTreeNewickWriterPlugin::node_to_element ),
            pybind11::arg("node"),
            pybind11::arg("element")
        )
        .def(
            "register_with",
            ( void ( ::genesis::tree::CommonTreeNewickWriterPlugin::* )( NewickWriter & ) const )( &::genesis::tree::CommonTreeNewickWriterPlugin::register_with ),
            pybind11::arg("writer")
        )
        .def(
            "replace_name_spaces",
            ( bool ( ::genesis::tree::CommonTreeNewickWriterPlugin::* )(  ) const )( &::genesis::tree::CommonTreeNewickWriterPlugin::replace_name_spaces ),
            get_docstring("bool ::genesis::tree::CommonTreeNewickWriterPlugin::replace_name_spaces () const")
        )
        .def(
            "replace_name_spaces",
            ( ::genesis::tree::CommonTreeNewickWriterPlugin & ( ::genesis::tree::CommonTreeNewickWriterPlugin::* )( bool ))( &::genesis::tree::CommonTreeNewickWriterPlugin::replace_name_spaces ),
            pybind11::arg("value"),
            get_docstring("self_type & ::genesis::tree::CommonTreeNewickWriterPlugin::replace_name_spaces (bool value)")
        )
        .def(
            "set_default_names",
            ( ::genesis::tree::CommonTreeNewickWriterPlugin & ( ::genesis::tree::CommonTreeNewickWriterPlugin::* )( std::string const & ))( &::genesis::tree::CommonTreeNewickWriterPlugin::set_default_names ),
            pybind11::arg("value"),
            get_docstring("self_type & ::genesis::tree::CommonTreeNewickWriterPlugin::set_default_names (std::string const & value)")
        )
        .def(
            "use_default_names",
            ( bool ( ::genesis::tree::CommonTreeNewickWriterPlugin::* )(  ) const )( &::genesis::tree::CommonTreeNewickWriterPlugin::use_default_names ),
            get_docstring("bool ::genesis::tree::CommonTreeNewickWriterPlugin::use_default_names () const")
        )
        .def(
            "use_default_names",
            ( ::genesis::tree::CommonTreeNewickWriterPlugin & ( ::genesis::tree::CommonTreeNewickWriterPlugin::* )( bool ))( &::genesis::tree::CommonTreeNewickWriterPlugin::use_default_names ),
            pybind11::arg("value"),
            get_docstring("self_type & ::genesis::tree::CommonTreeNewickWriterPlugin::use_default_names (bool value)")
        )
    ;
}

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::CommonTreeNewickReader, scope )
{

    // -------------------------------------------------------------------
    //     Class CommonTreeNewickReader
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::CommonTreeNewickReader, std::shared_ptr<::genesis::tree::CommonTreeNewickReader> > ( scope, "CommonTreeNewickReader" )
        .def(
            pybind11::init<  >()
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::tree::CommonTreeNewickReaderPlugin, scope )
{

    // -------------------------------------------------------------------
    //     Class CommonTreeNewickReaderPlugin
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::CommonTreeNewickReaderPlugin, std::shared_ptr<::genesis::tree::CommonTreeNewickReaderPlugin> > ( scope, "CommonTreeNewickReaderPlugin" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< CommonTreeNewickReaderPlugin const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "default_branch_length",
            ( double ( ::genesis::tree::CommonTreeNewickReaderPlugin::* )(  ) const )( &::genesis::tree::CommonTreeNewickReaderPlugin::default_branch_length ),
            get_docstring("double ::genesis::tree::CommonTreeNewickReaderPlugin::default_branch_length () const")
        )
        .def(
            "default_branch_length",
            ( ::genesis::tree::CommonTreeNewickReaderPlugin & ( ::genesis::tree::CommonTreeNewickReaderPlugin::* )( double ))( &::genesis::tree::CommonTreeNewickReaderPlugin::default_branch_length ),
            pybind11::arg("value"),
            get_docstring("self_type & ::genesis::tree::CommonTreeNewickReaderPlugin::default_branch_length (double value)")
        )
        .def(
            "default_inner_name",
            ( ::genesis::tree::CommonTreeNewickReaderPlugin & ( ::genesis::tree::CommonTreeNewickReaderPlugin::* )( std::string const & ))( &::genesis::tree::CommonTreeNewickReaderPlugin::default_inner_name ),
            pybind11::arg("value"),
            get_docstring("self_type & ::genesis::tree::CommonTreeNewickReaderPlugin::default_inner_name (std::string const & value)")
        )
        .def(
            "default_inner_name",
            ( std::string const & ( ::genesis::tree::CommonTreeNewickReaderPlugin::* )(  ) const )( &::genesis::tree::CommonTreeNewickReaderPlugin::default_inner_name ),
            get_docstring("std::string const & ::genesis::tree::CommonTreeNewickReaderPlugin::default_inner_name () const")
        )
        .def(
            "default_leaf_name",
            ( ::genesis::tree::CommonTreeNewickReaderPlugin & ( ::genesis::tree::CommonTreeNewickReaderPlugin::* )( std::string const & ))( &::genesis::tree::CommonTreeNewickReaderPlugin::default_leaf_name ),
            pybind11::arg("value"),
            get_docstring("self_type & ::genesis::tree::CommonTreeNewickReaderPlugin::default_leaf_name (std::string const & value)")
        )
        .def(
            "default_leaf_name",
            ( std::string const & ( ::genesis::tree::CommonTreeNewickReaderPlugin::* )(  ) const )( &::genesis::tree::CommonTreeNewickReaderPlugin::default_leaf_name ),
            get_docstring("std::string const & ::genesis::tree::CommonTreeNewickReaderPlugin::default_leaf_name () const")
        )
        .def(
            "default_root_name",
            ( ::genesis::tree::CommonTreeNewickReaderPlugin & ( ::genesis::tree::CommonTreeNewickReaderPlugin::* )( std::string const & ))( &::genesis::tree::CommonTreeNewickReaderPlugin::default_root_name ),
            pybind11::arg("value"),
            get_docstring("self_type & ::genesis::tree::CommonTreeNewickReaderPlugin::default_root_name (std::string const & value)")
        )
        .def(
            "default_root_name",
            ( std::string const & ( ::genesis::tree::CommonTreeNewickReaderPlugin::* )(  ) const )( &::genesis::tree::CommonTreeNewickReaderPlugin::default_root_name ),
            get_docstring("std::string const & ::genesis::tree::CommonTreeNewickReaderPlugin::default_root_name () const")
        )
        .def(
            "element_to_edge",
            ( void ( ::genesis::tree::CommonTreeNewickReaderPlugin::* )( NewickBrokerElement const &, TreeEdge & ) const )( &::genesis::tree::CommonTreeNewickReaderPlugin::element_to_edge ),
            pybind11::arg("element"),
            pybind11::arg("edge")
        )
        .def(
            "element_to_node",
            ( void ( ::genesis::tree::CommonTreeNewickReaderPlugin::* )( NewickBrokerElement const &, TreeNode & ) const )( &::genesis::tree::CommonTreeNewickReaderPlugin::element_to_node ),
            pybind11::arg("element"),
            pybind11::arg("node")
        )
        .def(
            "register_with",
            ( void ( ::genesis::tree::CommonTreeNewickReaderPlugin::* )( NewickReader & ) const )( &::genesis::tree::CommonTreeNewickReaderPlugin::register_with ),
            pybind11::arg("reader")
        )
        .def(
            "replace_name_underscores",
            ( bool ( ::genesis::tree::CommonTreeNewickReaderPlugin::* )(  ) const )( &::genesis::tree::CommonTreeNewickReaderPlugin::replace_name_underscores ),
            get_docstring("bool ::genesis::tree::CommonTreeNewickReaderPlugin::replace_name_underscores () const")
        )
        .def(
            "replace_name_underscores",
            ( ::genesis::tree::CommonTreeNewickReaderPlugin & ( ::genesis::tree::CommonTreeNewickReaderPlugin::* )( bool ))( &::genesis::tree::CommonTreeNewickReaderPlugin::replace_name_underscores ),
            pybind11::arg("value"),
            get_docstring("self_type & ::genesis::tree::CommonTreeNewickReaderPlugin::replace_name_underscores (bool value)")
        )
        .def(
            "set_default_names",
            ( ::genesis::tree::CommonTreeNewickReaderPlugin & ( ::genesis::tree::CommonTreeNewickReaderPlugin::* )( std::string const & ))( &::genesis::tree::CommonTreeNewickReaderPlugin::set_default_names ),
            pybind11::arg("value"),
            get_docstring("self_type & ::genesis::tree::CommonTreeNewickReaderPlugin::set_default_names (std::string const & value)")
        )
        .def(
            "use_default_names",
            ( bool ( ::genesis::tree::CommonTreeNewickReaderPlugin::* )(  ) const )( &::genesis::tree::CommonTreeNewickReaderPlugin::use_default_names ),
            get_docstring("bool ::genesis::tree::CommonTreeNewickReaderPlugin::use_default_names () const")
        )
        .def(
            "use_default_names",
            ( ::genesis::tree::CommonTreeNewickReaderPlugin & ( ::genesis::tree::CommonTreeNewickReaderPlugin::* )( bool ))( &::genesis::tree::CommonTreeNewickReaderPlugin::use_default_names ),
            pybind11::arg("value"),
            get_docstring("self_type & ::genesis::tree::CommonTreeNewickReaderPlugin::use_default_names (bool value)")
        )
    ;
}

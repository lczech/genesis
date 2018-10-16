/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::KeyedAttributeTreeNewickReader, scope )
{

    // -------------------------------------------------------------------
    //     Class KeyedAttributeTreeNewickReader
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::KeyedAttributeTreeNewickReader, std::shared_ptr<::genesis::tree::KeyedAttributeTreeNewickReader> > ( scope, "KeyedAttributeTreeNewickReader" )
        .def(
            pybind11::init<  >()
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin, scope )
{

    // -------------------------------------------------------------------
    //     Class KeyedAttributeTreeNewickReaderPlugin
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin, std::shared_ptr<::genesis::tree::KeyedAttributeTreeNewickReaderPlugin> > ( scope, "KeyedAttributeTreeNewickReaderPlugin" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< KeyedAttributeTreeNewickReaderPlugin const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "add_attribute",
            ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::self_type & ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::* )( std::string const &, ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::Target ))( &::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::add_attribute ),
            pybind11::arg("key"),
            pybind11::arg("target"),
            get_docstring("self_type & ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::add_attribute (std::string const & key, Target target)")
        )
        .def(
            "add_attribute",
            ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::self_type & ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::* )( std::string const &, ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::Target, std::string const & ))( &::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::add_attribute ),
            pybind11::arg("source_key"),
            pybind11::arg("target"),
            pybind11::arg("target_key"),
            get_docstring("self_type & ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::add_attribute (std::string const & source_key, Target target, std::string const & target_key)")
        )
        .def(
            "add_attribute",
            ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::self_type & ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::* )( std::string const &, ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::Target, std::string const &, std::string const & ))( &::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::add_attribute ),
            pybind11::arg("source_key"),
            pybind11::arg("target"),
            pybind11::arg("target_key"),
            pybind11::arg("default_value"),
            get_docstring("self_type & ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::add_attribute (std::string const & source_key, Target target, std::string const & target_key, std::string const & default_value)")
        )
        .def(
            "add_catch_all",
            ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::self_type & ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::* )( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::Target ))( &::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::add_catch_all ),
            pybind11::arg("target"),
            get_docstring("self_type & ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::add_catch_all (Target target)")
        )
        .def(
            "add_nhx_attributes",
            ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::self_type & ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::* )(  ))( &::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::add_nhx_attributes ),
            get_docstring("self_type & ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::add_nhx_attributes ()")
        )
        .def(
            "assigner",
            ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::self_type & ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::* )( std::string const & ))( &::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::assigner ),
            pybind11::arg("value"),
            get_docstring("self_type & ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::assigner (std::string const & value)")
        )
        .def(
            "assigner",
            ( std::string ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::* )(  ) const )( &::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::assigner ),
            get_docstring("std::string ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::assigner () const")
        )
        .def(
            "clear",
            ( void ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::* )(  ))( &::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::clear ),
            get_docstring("void ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::clear ()")
        )
        .def(
            "element_to_edge",
            ( void ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::* )( NewickBrokerElement const &, TreeEdge & ) const )( &::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::element_to_edge ),
            pybind11::arg("element"),
            pybind11::arg("edge")
        )
        .def(
            "element_to_node",
            ( void ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::* )( NewickBrokerElement const &, TreeNode & ) const )( &::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::element_to_node ),
            pybind11::arg("element"),
            pybind11::arg("node")
        )
        .def(
            "prefix",
            ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::self_type & ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::* )( std::string const & ))( &::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::prefix ),
            pybind11::arg("value"),
            get_docstring("self_type & ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::prefix (std::string const & value)")
        )
        .def(
            "prefix",
            ( std::string ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::* )(  ) const )( &::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::prefix ),
            get_docstring("std::string ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::prefix () const")
        )
        .def(
            "register_with",
            ( void ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::* )( NewickReader & ) const )( &::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::register_with ),
            pybind11::arg("reader")
        )
        .def(
            "separator",
            ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::self_type & ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::* )( std::string const & ))( &::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::separator ),
            pybind11::arg("value"),
            get_docstring("self_type & ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::separator (std::string const & value)")
        )
        .def(
            "separator",
            ( std::string ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::* )(  ) const )( &::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::separator ),
            get_docstring("std::string ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::separator () const")
        )
        .def(
            "set_delimiters",
            ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::self_type & ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::* )( std::string const &, std::string const &, std::string const & ))( &::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::set_delimiters ),
            pybind11::arg("prefix"),
            pybind11::arg("separator"),
            pybind11::arg("assigner")=(std::string const &)("="),
            get_docstring("self_type & ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::set_delimiters (std::string const & prefix, std::string const & separator, std::string const & assigner=\"=\")")
        )
        .def(
            "set_nhx_delimiters",
            ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::self_type & ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::* )(  ))( &::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::set_nhx_delimiters ),
            get_docstring("self_type & ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::set_nhx_delimiters ()")
        )
        .def(
            "trim",
            ( bool ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::* )(  ) const )( &::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::trim ),
            get_docstring("bool ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::trim () const")
        )
        .def(
            "trim",
            ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::self_type & ( ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::* )( bool ))( &::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::trim ),
            pybind11::arg("value"),
            get_docstring("self_type & ::genesis::tree::KeyedAttributeTreeNewickReaderPlugin::trim (bool value)")
        )
    ;
}

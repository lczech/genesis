/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin, scope )
{

    // -------------------------------------------------------------------
    //     Class IndexedAttributeTreeNewickReaderPlugin
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin, std::shared_ptr<::genesis::tree::IndexedAttributeTreeNewickReaderPlugin> > ( scope, "IndexedAttributeTreeNewickReaderPlugin" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< IndexedAttributeTreeNewickReaderPlugin const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "add_attribute",
            ( ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::self_type & ( ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::* )( ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::Source, size_t, ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::Target, std::string const & ))( &::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::add_attribute ),
            pybind11::arg("source"),
            pybind11::arg("index"),
            pybind11::arg("target"),
            pybind11::arg("target_key"),
            get_docstring("self_type & ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::add_attribute (Source source, size_t index, Target target, std::string const & target_key)")
        )
        .def(
            "add_attribute",
            ( ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::self_type & ( ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::* )( ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::Source, size_t, ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::Target, std::string const &, std::string const & ))( &::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::add_attribute ),
            pybind11::arg("source"),
            pybind11::arg("index"),
            pybind11::arg("target"),
            pybind11::arg("target_key"),
            pybind11::arg("default_value"),
            get_docstring("self_type & ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::add_attribute (Source source, size_t index, Target target, std::string const & target_key, std::string const & default_value)")
        )
        .def(
            "add_catch_all",
            ( ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::self_type & ( ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::* )( ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::Source, ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::Target, std::string const & ))( &::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::add_catch_all ),
            pybind11::arg("source"),
            pybind11::arg("target"),
            pybind11::arg("target_key_prefix"),
            get_docstring("self_type & ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::add_catch_all (Source source, Target target, std::string const & target_key_prefix)")
        )
        .def(
            "add_catch_all",
            ( ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::self_type & ( ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::* )( ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::Target ))( &::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::add_catch_all ),
            pybind11::arg("target"),
            get_docstring("self_type & ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::add_catch_all (Target target)")
        )
        .def(
            "clear",
            ( void ( ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::* )(  ))( &::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::clear ),
            get_docstring("void ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::clear ()")
        )
        .def(
            "element_to_edge",
            ( void ( ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::* )( NewickBrokerElement const &, TreeEdge & ) const )( &::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::element_to_edge ),
            pybind11::arg("element"),
            pybind11::arg("edge")
        )
        .def(
            "element_to_node",
            ( void ( ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::* )( NewickBrokerElement const &, TreeNode & ) const )( &::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::element_to_node ),
            pybind11::arg("element"),
            pybind11::arg("node")
        )
        .def(
            "register_with",
            ( void ( ::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::* )( NewickReader & ) const )( &::genesis::tree::IndexedAttributeTreeNewickReaderPlugin::register_with ),
            pybind11::arg("reader")
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::tree::IndexedAttributeTreeNewickReader, scope )
{

    // -------------------------------------------------------------------
    //     Class IndexedAttributeTreeNewickReader
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::IndexedAttributeTreeNewickReader, std::shared_ptr<::genesis::tree::IndexedAttributeTreeNewickReader> > ( scope, "IndexedAttributeTreeNewickReader" )
        .def(
            pybind11::init<  >()
        )
    ;
}

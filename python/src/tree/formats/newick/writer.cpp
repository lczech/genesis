/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::NewickWriter, scope )
{

    // -------------------------------------------------------------------
    //     Class NewickWriter
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::NewickWriter, std::shared_ptr<::genesis::tree::NewickWriter> > ( scope, "NewickWriter" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< NewickWriter const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "broker_to_file",
            ( void ( ::genesis::tree::NewickWriter::* )( NewickBroker const &, std::string const & ) const )( &::genesis::tree::NewickWriter::broker_to_file ),
            pybind11::arg("broker"),
            pybind11::arg("filename"),
            get_docstring("void ::genesis::tree::NewickWriter::broker_to_file (NewickBroker const & broker, std::string const & filename) const")
        )
        .def(
            "broker_to_stream",
            ( void ( ::genesis::tree::NewickWriter::* )( NewickBroker const &, std::ostream & ) const )( &::genesis::tree::NewickWriter::broker_to_stream ),
            pybind11::arg("broker"),
            pybind11::arg("os"),
            get_docstring("void ::genesis::tree::NewickWriter::broker_to_stream (NewickBroker const & broker, std::ostream & os) const")
        )
        .def(
            "broker_to_string",
            ( std::string ( ::genesis::tree::NewickWriter::* )( NewickBroker const & ) const )( &::genesis::tree::NewickWriter::broker_to_string ),
            pybind11::arg("broker"),
            get_docstring("std::string ::genesis::tree::NewickWriter::broker_to_string (NewickBroker const & broker) const")
        )
        .def(
            "broker_to_string",
            ( void ( ::genesis::tree::NewickWriter::* )( NewickBroker const &, std::string & ) const )( &::genesis::tree::NewickWriter::broker_to_string ),
            pybind11::arg("broker"),
            pybind11::arg("ts"),
            get_docstring("void ::genesis::tree::NewickWriter::broker_to_string (NewickBroker const & broker, std::string & ts) const")
        )
        .def(
            "force_quotation_marks",
            ( NewickWriter & ( ::genesis::tree::NewickWriter::* )( bool ))( &::genesis::tree::NewickWriter::force_quotation_marks ),
            pybind11::arg("value"),
            get_docstring("NewickWriter & ::genesis::tree::NewickWriter::force_quotation_marks (bool value)")
        )
        .def(
            "force_quotation_marks",
            ( bool ( ::genesis::tree::NewickWriter::* )(  ) const )( &::genesis::tree::NewickWriter::force_quotation_marks ),
            get_docstring("bool ::genesis::tree::NewickWriter::force_quotation_marks () const")
        )
        .def(
            "quotation_marks",
            ( NewickWriter & ( ::genesis::tree::NewickWriter::* )( char ))( &::genesis::tree::NewickWriter::quotation_marks ),
            pybind11::arg("value"),
            get_docstring("NewickWriter & ::genesis::tree::NewickWriter::quotation_marks (char value)")
        )
        .def(
            "quotation_marks",
            ( char ( ::genesis::tree::NewickWriter::* )(  ) const )( &::genesis::tree::NewickWriter::quotation_marks ),
            get_docstring("char ::genesis::tree::NewickWriter::quotation_marks () const")
        )
        .def(
            "to_file",
            ( void ( ::genesis::tree::NewickWriter::* )( Tree const &, std::string const & ) const )( &::genesis::tree::NewickWriter::to_file ),
            pybind11::arg("tree"),
            pybind11::arg("filename"),
            get_docstring("void ::genesis::tree::NewickWriter::to_file (Tree const & tree, std::string const & filename) const")
        )
        .def(
            "to_stream",
            ( void ( ::genesis::tree::NewickWriter::* )( Tree const &, std::ostream & ) const )( &::genesis::tree::NewickWriter::to_stream ),
            pybind11::arg("tree"),
            pybind11::arg("os"),
            get_docstring("void ::genesis::tree::NewickWriter::to_stream (Tree const & tree, std::ostream & os) const")
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::tree::NewickWriter::* )( Tree const & ) const )( &::genesis::tree::NewickWriter::to_string ),
            pybind11::arg("tree"),
            get_docstring("std::string ::genesis::tree::NewickWriter::to_string (Tree const & tree) const")
        )
        .def(
            "to_string",
            ( void ( ::genesis::tree::NewickWriter::* )( Tree const &, std::string & ) const )( &::genesis::tree::NewickWriter::to_string ),
            pybind11::arg("tree"),
            pybind11::arg("ts"),
            get_docstring("void ::genesis::tree::NewickWriter::to_string (Tree const & tree, std::string & ts) const")
        )
        .def(
            "tree_to_broker",
            ( NewickBroker ( ::genesis::tree::NewickWriter::* )( Tree const & ) const )( &::genesis::tree::NewickWriter::tree_to_broker ),
            pybind11::arg("tree"),
            get_docstring("NewickBroker ::genesis::tree::NewickWriter::tree_to_broker (Tree const & tree) const")
        )
        .def(
            "write_comments",
            ( NewickWriter & ( ::genesis::tree::NewickWriter::* )( bool ))( &::genesis::tree::NewickWriter::write_comments ),
            pybind11::arg("value"),
            get_docstring("NewickWriter & ::genesis::tree::NewickWriter::write_comments (bool value)")
        )
        .def(
            "write_comments",
            ( bool ( ::genesis::tree::NewickWriter::* )(  ) const )( &::genesis::tree::NewickWriter::write_comments ),
            get_docstring("bool ::genesis::tree::NewickWriter::write_comments () const")
        )
        .def(
            "write_names",
            ( NewickWriter & ( ::genesis::tree::NewickWriter::* )( bool ))( &::genesis::tree::NewickWriter::write_names ),
            pybind11::arg("value"),
            get_docstring("NewickWriter & ::genesis::tree::NewickWriter::write_names (bool value)")
        )
        .def(
            "write_names",
            ( bool ( ::genesis::tree::NewickWriter::* )(  ) const )( &::genesis::tree::NewickWriter::write_names ),
            get_docstring("bool ::genesis::tree::NewickWriter::write_names () const")
        )
        .def(
            "write_tags",
            ( NewickWriter & ( ::genesis::tree::NewickWriter::* )( bool ))( &::genesis::tree::NewickWriter::write_tags ),
            pybind11::arg("value"),
            get_docstring("NewickWriter & ::genesis::tree::NewickWriter::write_tags (bool value)")
        )
        .def(
            "write_tags",
            ( bool ( ::genesis::tree::NewickWriter::* )(  ) const )( &::genesis::tree::NewickWriter::write_tags ),
            get_docstring("bool ::genesis::tree::NewickWriter::write_tags () const")
        )
        .def(
            "write_values",
            ( NewickWriter & ( ::genesis::tree::NewickWriter::* )( bool ))( &::genesis::tree::NewickWriter::write_values ),
            pybind11::arg("value"),
            get_docstring("NewickWriter & ::genesis::tree::NewickWriter::write_values (bool value)")
        )
        .def(
            "write_values",
            ( bool ( ::genesis::tree::NewickWriter::* )(  ) const )( &::genesis::tree::NewickWriter::write_values ),
            get_docstring("bool ::genesis::tree::NewickWriter::write_values () const")
        )
    ;
}

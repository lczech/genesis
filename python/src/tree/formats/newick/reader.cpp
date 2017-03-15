/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::NewickReader, scope )
{

    // -------------------------------------------------------------------
    //     Class NewickReader
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::NewickReader, std::shared_ptr<::genesis::tree::NewickReader> > ( scope, "NewickReader" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< NewickReader const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "enable_tags",
            ( NewickReader & ( ::genesis::tree::NewickReader::* )( bool ))( &::genesis::tree::NewickReader::enable_tags ),
            pybind11::arg("value"),
            get_docstring("NewickReader & ::genesis::tree::NewickReader::enable_tags (bool value)")
        )
        .def(
            "enable_tags",
            ( bool ( ::genesis::tree::NewickReader::* )(  ) const )( &::genesis::tree::NewickReader::enable_tags ),
            get_docstring("bool ::genesis::tree::NewickReader::enable_tags () const")
        )
        .def(
            "from_file",
            ( Tree ( ::genesis::tree::NewickReader::* )( std::string const & ) const )( &::genesis::tree::NewickReader::from_file ),
            pybind11::arg("filename"),
            get_docstring("Tree ::genesis::tree::NewickReader::from_file (std::string const & filename) const")
        )
        .def(
            "from_file",
            ( void ( ::genesis::tree::NewickReader::* )( std::string const &, TreeSet &, std::string const & ) const )( &::genesis::tree::NewickReader::from_file ),
            pybind11::arg("filename"),
            pybind11::arg("tree_set"),
            pybind11::arg("default_name")=(std::string const &)(""),
            get_docstring("void ::genesis::tree::NewickReader::from_file (std::string const & filename, TreeSet & tree_set, std::string const & default_name=\"\") const")
        )
        .def(
            "from_files",
            ( void ( ::genesis::tree::NewickReader::* )( std::vector< std::string > const &, TreeSet & ) const )( &::genesis::tree::NewickReader::from_files ),
            pybind11::arg("filenames"),
            pybind11::arg("tree_set"),
            get_docstring("void ::genesis::tree::NewickReader::from_files (std::vector< std::string > const & filenames, TreeSet & tree_set) const")
        )
        .def(
            "from_stream",
            ( Tree ( ::genesis::tree::NewickReader::* )( std::istream & ) const )( &::genesis::tree::NewickReader::from_stream ),
            pybind11::arg("input_stream"),
            get_docstring("Tree ::genesis::tree::NewickReader::from_stream (std::istream & input_stream) const")
        )
        .def(
            "from_stream",
            ( void ( ::genesis::tree::NewickReader::* )( std::istream &, TreeSet &, std::string const & ) const )( &::genesis::tree::NewickReader::from_stream ),
            pybind11::arg("input_stream"),
            pybind11::arg("tree_set"),
            pybind11::arg("default_name")=(std::string const &)(""),
            get_docstring("void ::genesis::tree::NewickReader::from_stream (std::istream & input_stream, TreeSet & tree_set, std::string const & default_name=\"\") const")
        )
        .def(
            "from_string",
            ( Tree ( ::genesis::tree::NewickReader::* )( std::string const & ) const )( &::genesis::tree::NewickReader::from_string ),
            pybind11::arg("tree_string"),
            get_docstring("Tree ::genesis::tree::NewickReader::from_string (std::string const & tree_string) const")
        )
        .def(
            "from_string",
            ( void ( ::genesis::tree::NewickReader::* )( std::string const &, TreeSet &, std::string const & ) const )( &::genesis::tree::NewickReader::from_string ),
            pybind11::arg("tree_string"),
            pybind11::arg("tree_set"),
            pybind11::arg("default_name")=(std::string const &)(""),
            get_docstring("void ::genesis::tree::NewickReader::from_string (std::string const & tree_string, TreeSet & tree_set, std::string const & default_name=\"\") const")
        )
        .def(
            "from_strings",
            ( void ( ::genesis::tree::NewickReader::* )( std::vector< std::string > const &, TreeSet &, std::string const & ) const )( &::genesis::tree::NewickReader::from_strings ),
            pybind11::arg("tree_strings"),
            pybind11::arg("tree_set"),
            pybind11::arg("default_name")=(std::string const &)(""),
            get_docstring("void ::genesis::tree::NewickReader::from_strings (std::vector< std::string > const & tree_strings, TreeSet & tree_set, std::string const & default_name=\"\") const")
        )
        .def(
            "stop_at_semicolon",
            ( NewickReader & ( ::genesis::tree::NewickReader::* )( bool ))( &::genesis::tree::NewickReader::stop_at_semicolon ),
            pybind11::arg("value"),
            get_docstring("NewickReader & ::genesis::tree::NewickReader::stop_at_semicolon (bool value)")
        )
        .def(
            "stop_at_semicolon",
            ( bool ( ::genesis::tree::NewickReader::* )(  ) const )( &::genesis::tree::NewickReader::stop_at_semicolon ),
            get_docstring("bool ::genesis::tree::NewickReader::stop_at_semicolon () const")
        )
    ;
}

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
            "from_file",
            ( bool ( ::genesis::tree::NewickReader::* )( const std::string &, Tree & ))( &::genesis::tree::NewickReader::from_file ),
            pybind11::arg("filename"),
            pybind11::arg("tree")
        )
        .def(
            "from_file",
            ( bool ( ::genesis::tree::NewickReader::* )( const std::string &, TreeSet & ))( &::genesis::tree::NewickReader::from_file ),
            pybind11::arg("filename"),
            pybind11::arg("tree_set")
        )
        .def(
            "from_files",
            ( bool ( ::genesis::tree::NewickReader::* )( const std::vector< std::string > &, TreeSet & ))( &::genesis::tree::NewickReader::from_files ),
            pybind11::arg("filenames"),
            pybind11::arg("tree_set")
        )
        .def(
            "from_string",
            ( bool ( ::genesis::tree::NewickReader::* )( const std::string &, Tree & ))( &::genesis::tree::NewickReader::from_string ),
            pybind11::arg("tree_string"),
            pybind11::arg("tree")
        )
        .def(
            "from_string",
            ( bool ( ::genesis::tree::NewickReader::* )( const std::string &, TreeSet &, const std::string & ))( &::genesis::tree::NewickReader::from_string ),
            pybind11::arg("tree_string"),
            pybind11::arg("tree_set"),
            pybind11::arg("default_name")=(const std::string &)("")
        )
        .def(
            "from_strings",
            ( bool ( ::genesis::tree::NewickReader::* )( const std::vector< std::string > &, TreeSet &, const std::string & ))( &::genesis::tree::NewickReader::from_strings ),
            pybind11::arg("tree_strings"),
            pybind11::arg("tree_set"),
            pybind11::arg("default_name")=(const std::string &)("")
        )
    ;
}

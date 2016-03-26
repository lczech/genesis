/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

template <typename TreeType_>
void PythonExportClass_NewickReader(std::string name)
{

    // -------------------------------------------------------------------
    //     Class NewickReader
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using NewickReaderType = NewickReader<typename TreeType_>;

    boost::python::class_< NewickReaderType > ( name.c_str(), boost::python::init<  >(  ) )
        .def( boost::python::init< NewickReader const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "from_file",
            ( bool ( NewickReaderType::* )( const std::string &, TreeSet< TreeType > & ))( &NewickReaderType::from_file ),
            ( boost::python::arg("filename"), boost::python::arg("tree_set") ),
            get_docstring("bool ::genesis::tree::NewickReader::from_file (const std::string & filename, TreeSet< TreeType > & tree_set)")
        )
        .def(
            "from_file",
            ( bool ( NewickReaderType::* )( const std::string &, TreeType & ))( &NewickReaderType::from_file ),
            ( boost::python::arg("filename"), boost::python::arg("tree") ),
            get_docstring("bool ::genesis::tree::NewickReader::from_file (const std::string & filename, TreeType & tree)")
        )
        .def(
            "from_files",
            ( bool ( NewickReaderType::* )( const std::vector< std::string > &, TreeSet< TreeType > & ))( &NewickReaderType::from_files ),
            ( boost::python::arg("filenames"), boost::python::arg("tree_set") ),
            get_docstring("bool ::genesis::tree::NewickReader::from_files (const std::vector< std::string > & filenames, TreeSet< TreeType > & tree_set)")
        )
        .def(
            "from_string",
            ( bool ( NewickReaderType::* )( const std::string &, TreeSet< TreeType > &, const std::string & ))( &NewickReaderType::from_string ),
            ( boost::python::arg("tree_string"), boost::python::arg("tree_set"), boost::python::arg("default_name")=(const std::string &)("") ),
            get_docstring("bool ::genesis::tree::NewickReader::from_string (const std::string & tree_string, TreeSet< TreeType > & tree_set, const std::string & default_name="")")
        )
        .def(
            "from_string",
            ( bool ( NewickReaderType::* )( const std::string &, TreeType & ))( &NewickReaderType::from_string ),
            ( boost::python::arg("tree_string"), boost::python::arg("tree") ),
            get_docstring("bool ::genesis::tree::NewickReader::from_string (const std::string & tree_string, TreeType & tree)")
        )
        .def(
            "from_strings",
            ( bool ( NewickReaderType::* )( const std::vector< std::string > &, TreeSet< TreeType > &, const std::string & ))( &NewickReaderType::from_strings ),
            ( boost::python::arg("tree_strings"), boost::python::arg("tree_set"), boost::python::arg("default_name")=(const std::string &)("") ),
            get_docstring("bool ::genesis::tree::NewickReader::from_strings (const std::vector< std::string > & tree_strings, TreeSet< TreeType > & tree_set, const std::string & default_name="")")
        )
    ;
}

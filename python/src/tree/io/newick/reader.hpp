/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

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

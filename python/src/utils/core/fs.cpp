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

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS(utils_core_fs_export, "utils")
{

    boost::python::def(
        "dir_exists",
        ( bool ( * )( std::string const & ))( &::genesis::utils::dir_exists ),
        ( boost::python::arg("dir") ),
        get_docstring("bool ::genesis::utils::dir_exists (std::string const & dir)")
    );

    boost::python::def(
        "file_exists",
        ( bool ( * )( std::string const & ))( &::genesis::utils::file_exists ),
        ( boost::python::arg("filename") ),
        get_docstring("bool ::genesis::utils::file_exists (std::string const & filename)")
    );

    boost::python::def(
        "file_size",
        ( size_t ( * )( std::string ))( &::genesis::utils::file_size ),
        ( boost::python::arg("filename") ),
        get_docstring("size_t ::genesis::utils::file_size (std::string filename)")
    );

    boost::python::def(
        "file_basename",
        ( std::string ( * )( std::string ))( &::genesis::utils::file_basename ),
        ( boost::python::arg("filename") ),
        get_docstring("std::string ::genesis::utils::file_basename (std::string filename)")
    );

    boost::python::def(
        "file_extension",
        ( std::string ( * )( std::string ))( &::genesis::utils::file_extension ),
        ( boost::python::arg("filename") ),
        get_docstring("std::string ::genesis::utils::file_extension (std::string filename)")
    );

    boost::python::def(
        "file_filename",
        ( std::string ( * )( std::string ))( &::genesis::utils::file_filename ),
        ( boost::python::arg("filename") ),
        get_docstring("std::string ::genesis::utils::file_filename (std::string filename)")
    );

    boost::python::def(
        "file_path",
        ( std::string ( * )( std::string ))( &::genesis::utils::file_path ),
        ( boost::python::arg("filename") ),
        get_docstring("std::string ::genesis::utils::file_path (std::string filename)")
    );

    boost::python::def(
        "file_read",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::file_read ),
        ( boost::python::arg("filename") ),
        get_docstring("std::string ::genesis::utils::file_read (std::string const & filename)")
    );

    boost::python::def(
        "file_info",
        ( std::unordered_map< std::string, std::string > ( * )( std::string const & ))( &::genesis::utils::file_info ),
        ( boost::python::arg("filename") ),
        get_docstring("std::unordered_map< std::string, std::string > ::genesis::utils::file_info (std::string const & filename)")
    );

    boost::python::def(
        "dir_list_files",
        ( std::vector< std::string > ( * )( std::string const & ))( &::genesis::utils::dir_list_files ),
        ( boost::python::arg("dir") ),
        get_docstring("std::vector< std::string > ::genesis::utils::dir_list_files (std::string const & dir)")
    );

    boost::python::def(
        "dir_create",
        ( void ( * )( std::string const & ))( &::genesis::utils::dir_create ),
        ( boost::python::arg("path") ),
        get_docstring("void ::genesis::utils::dir_create (std::string const & path)")
    );

    boost::python::def(
        "file_append",
        ( void ( * )( std::string const &, std::string const & ))( &::genesis::utils::file_append ),
        ( boost::python::arg("content"), boost::python::arg("filename") ),
        get_docstring("void ::genesis::utils::file_append (std::string const & content, std::string const & filename)")
    );

    boost::python::def(
        "file_write",
        ( void ( * )( std::string const &, std::string const & ))( &::genesis::utils::file_write ),
        ( boost::python::arg("content"), boost::python::arg("filename") ),
        get_docstring("void ::genesis::utils::file_write (std::string const & content, std::string const & filename)")
    );
}

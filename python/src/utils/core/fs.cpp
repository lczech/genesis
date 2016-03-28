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

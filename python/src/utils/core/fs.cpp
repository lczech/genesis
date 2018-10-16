/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_core_fs_export, ::genesis::utils, scope )
{

    scope.def(
        "dir_exists",
        ( bool ( * )( std::string const & ))( &::genesis::utils::dir_exists ),
            pybind11::arg("dir"),
        get_docstring("bool ::genesis::utils::dir_exists (std::string const & dir)")
    );

    scope.def(
        "file_exists",
        ( bool ( * )( std::string const & ))( &::genesis::utils::file_exists ),
            pybind11::arg("filename"),
        get_docstring("bool ::genesis::utils::file_exists (std::string const & filename)")
    );

    scope.def(
        "is_dir",
        ( bool ( * )( std::string const & ))( &::genesis::utils::is_dir ),
            pybind11::arg("path"),
        get_docstring("bool ::genesis::utils::is_dir (std::string const & path)")
    );

    scope.def(
        "is_file",
        ( bool ( * )( std::string const & ))( &::genesis::utils::is_file ),
            pybind11::arg("path"),
        get_docstring("bool ::genesis::utils::is_file (std::string const & path)")
    );

    scope.def(
        "is_valid_filname",
        ( bool ( * )( std::string const & ))( &::genesis::utils::is_valid_filname ),
            pybind11::arg("filename"),
        get_docstring("bool ::genesis::utils::is_valid_filname (std::string const & filename)")
    );

    scope.def(
        "path_exists",
        ( bool ( * )( std::string const & ))( &::genesis::utils::path_exists ),
            pybind11::arg("path"),
        get_docstring("bool ::genesis::utils::path_exists (std::string const & path)")
    );

    scope.def(
        "file_size",
        ( size_t ( * )( std::string const & ))( &::genesis::utils::file_size ),
            pybind11::arg("filename"),
        get_docstring("size_t ::genesis::utils::file_size (std::string const & filename)")
    );

    scope.def(
        "dir_normalize_path",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::dir_normalize_path ),
            pybind11::arg("path"),
        get_docstring("std::string ::genesis::utils::dir_normalize_path (std::string const & path)")
    );

    scope.def(
        "file_basename",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::file_basename ),
            pybind11::arg("filename"),
        get_docstring("std::string ::genesis::utils::file_basename (std::string const & filename)")
    );

    scope.def(
        "file_extension",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::file_extension ),
            pybind11::arg("filename"),
        get_docstring("std::string ::genesis::utils::file_extension (std::string const & filename)")
    );

    scope.def(
        "file_filename",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::file_filename ),
            pybind11::arg("filename"),
        get_docstring("std::string ::genesis::utils::file_filename (std::string const & filename)")
    );

    scope.def(
        "file_path",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::file_path ),
            pybind11::arg("filename"),
        get_docstring("std::string ::genesis::utils::file_path (std::string const & filename)")
    );

    scope.def(
        "file_read",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::file_read ),
            pybind11::arg("filename"),
        get_docstring("std::string ::genesis::utils::file_read (std::string const & filename)")
    );

    scope.def(
        "sanitize_filname",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::sanitize_filname ),
            pybind11::arg("filename"),
        get_docstring("std::string ::genesis::utils::sanitize_filname (std::string const & filename)")
    );

    scope.def(
        "file_info",
        ( std::unordered_map< std::string, std::string > ( * )( std::string const & ))( &::genesis::utils::file_info ),
            pybind11::arg("filename"),
        get_docstring("std::unordered_map< std::string, std::string > ::genesis::utils::file_info (std::string const & filename)")
    );

    scope.def(
        "dir_list_contents",
        ( std::vector< std::string > ( * )( std::string const &, bool, std::string const & ))( &::genesis::utils::dir_list_contents ),
            pybind11::arg("dir"),
            pybind11::arg("full_path")=(bool)(false),
            pybind11::arg("regex")=(std::string const &)(""),
        get_docstring("std::vector< std::string > ::genesis::utils::dir_list_contents (std::string const & dir, bool full_path=false, std::string const & regex=\"\")")
    );

    scope.def(
        "dir_list_directories",
        ( std::vector< std::string > ( * )( std::string const &, bool, std::string const & ))( &::genesis::utils::dir_list_directories ),
            pybind11::arg("dir"),
            pybind11::arg("full_path")=(bool)(false),
            pybind11::arg("regex")=(std::string const &)(""),
        get_docstring("std::vector< std::string > ::genesis::utils::dir_list_directories (std::string const & dir, bool full_path=false, std::string const & regex=\"\")")
    );

    scope.def(
        "dir_list_files",
        ( std::vector< std::string > ( * )( std::string const &, bool, std::string const & ))( &::genesis::utils::dir_list_files ),
            pybind11::arg("dir"),
            pybind11::arg("full_path")=(bool)(false),
            pybind11::arg("regex")=(std::string const &)(""),
        get_docstring("std::vector< std::string > ::genesis::utils::dir_list_files (std::string const & dir, bool full_path=false, std::string const & regex=\"\")")
    );

    scope.def(
        "dir_create",
        ( void ( * )( std::string const &, bool ))( &::genesis::utils::dir_create ),
            pybind11::arg("path"),
            pybind11::arg("with_parents")=(bool)(true),
        get_docstring("void ::genesis::utils::dir_create (std::string const & path, bool with_parents=true)")
    );

    scope.def(
        "file_append",
        ( void ( * )( std::string const &, std::string const & ))( &::genesis::utils::file_append ),
            pybind11::arg("content"),
            pybind11::arg("filename"),
        get_docstring("void ::genesis::utils::file_append (std::string const & content, std::string const & filename)")
    );

    scope.def(
        "file_write",
        ( void ( * )( std::string const &, std::string const & ))( &::genesis::utils::file_write ),
            pybind11::arg("content"),
            pybind11::arg("filename"),
        get_docstring("void ::genesis::utils::file_write (std::string const & content, std::string const & filename)")
    );
}

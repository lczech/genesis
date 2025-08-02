#include <functional>
#include <genesis/util/core/fs.hpp>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include <functional>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>

#ifndef BINDER_PYBIND11_TYPE_CASTER
#    define BINDER_PYBIND11_TYPE_CASTER
PYBIND11_DECLARE_HOLDER_TYPE( T, std::shared_ptr<T>, false )
PYBIND11_DECLARE_HOLDER_TYPE( T, T*, false )
PYBIND11_MAKE_OPAQUE( std::shared_ptr<void> )
#endif

void bind_genesis_util_core_fs(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    // genesis::util::core::file_append(const std::string &, const std::string &, bool)
    // file:genesis/util/core/fs.hpp line:131 function-signature: void
    // genesis::util::core::file_append(const std::string &, const std::string &, bool)(const
    // std::string &, const std::string &, bool) file:genesis/util/core/fs.hpp line:131
    M( "genesis::util::core" )
        .def(
            "file_append",
            []( const std::string& a0, const std::string& a1 ) -> void {
                return genesis::util::core::file_append( a0, a1 );
            },
            "",
            pybind11::arg( "content" ),
            pybind11::arg( "filename" )
        );
    M( "genesis::util::core" )
        .def(
            "file_append",
            ( void ( * )( const std::string&, const std::string&, bool ) ) &
                genesis::util::core::file_append,
            "Append the content of a string to a file.\n\n If the file is not writable, the "
            "function throws `std::runtime_error`.\n\nC++: genesis::util::core::file_append(const "
            "std::string &, const std::string &, bool) --> void",
            pybind11::arg( "content" ),
            pybind11::arg( "filename" ),
            pybind11::arg( "create_dirs" )
        );

    // genesis::util::core::is_dir(const std::string &) file:genesis/util/core/fs.hpp line:147
    // function-signature: bool genesis::util::core::is_dir(const std::string &)(const std::string
    // &) file:genesis/util/core/fs.hpp line:147
    M( "genesis::util::core" )
        .def(
            "is_dir",
            ( bool ( * )( const std::string& ) ) & genesis::util::core::is_dir,
            "Return true iff the provided path is a directory.\n\n Internally, this function "
            "simply return the value of dir_exists(), as this already does the\n needed check. "
            "Thus, it is an alias.\n\nC++: genesis::util::core::is_dir(const std::string &) --> "
            "bool",
            pybind11::arg( "path" )
        );

    // genesis::util::core::dir_exists(const std::string &) file:genesis/util/core/fs.hpp line:152
    // function-signature: bool genesis::util::core::dir_exists(const std::string &)(const
    // std::string &) file:genesis/util/core/fs.hpp line:152
    M( "genesis::util::core" )
        .def(
            "dir_exists",
            ( bool ( * )( const std::string& ) ) & genesis::util::core::dir_exists,
            "Return true iff the directory exists.\n\nC++: genesis::util::core::dir_exists(const "
            "std::string &) --> bool",
            pybind11::arg( "dir" )
        );

    // genesis::util::core::dir_create(const std::string &, bool) file:genesis/util/core/fs.hpp
    // line:161 function-signature: void genesis::util::core::dir_create(const std::string &,
    // bool)(const std::string &, bool) file:genesis/util/core/fs.hpp line:161
    M( "genesis::util::core" )
        .def(
            "dir_create",
            []( const std::string& a0 ) -> void { return genesis::util::core::dir_create( a0 ); },
            "",
            pybind11::arg( "path" )
        );
    M( "genesis::util::core" )
        .def(
            "dir_create",
            ( void ( * )( const std::string&, bool ) ) & genesis::util::core::dir_create,
            "Create a directory.\n\n If the directory already exists, nothing happens.\n If the "
            "path exists, but is not a directory, a `std::runtime_error` is thrown.\n If the "
            "creation fails for some other reason, also a `std::runtime_error` is thrown.\n\nC++: "
            "genesis::util::core::dir_create(const std::string &, bool) --> void",
            pybind11::arg( "path" ),
            pybind11::arg( "with_parents" )
        );

    // genesis::util::core::dir_normalize_path(const std::string &) file:genesis/util/core/fs.hpp
    // line:166 function-signature: std::string genesis::util::core::dir_normalize_path(const
    // std::string &)(const std::string &) file:genesis/util/core/fs.hpp line:166
    M( "genesis::util::core" )
        .def(
            "dir_normalize_path",
            ( std::string( * )( const std::string& ) ) & genesis::util::core::dir_normalize_path,
            "Normalize a dir name, i.e., make sure that the given path ends with exaclty one "
            "slash.\n\nC++: genesis::util::core::dir_normalize_path(const std::string &) --> "
            "std::string",
            pybind11::arg( "path" )
        );

    // genesis::util::core::dir_list_contents(const std::string &, bool, const std::string &)
    // file:genesis/util/core/fs.hpp line:176 function-signature: class std::vector<std::string >
    // genesis::util::core::dir_list_contents(const std::string &, bool, const std::string &)(const
    // std::string &, bool, const std::string &) file:genesis/util/core/fs.hpp line:176
    M( "genesis::util::core" )
        .def(
            "dir_list_contents",
            []( const std::string& a0 ) -> std::vector<std::string > {
                return genesis::util::core::dir_list_contents( a0 );
            },
            "",
            pybind11::arg( "dir" )
        );
    M( "genesis::util::core" )
        .def(
            "dir_list_contents",
            []( const std::string& a0, bool const& a1 ) -> std::vector<std::string > {
                return genesis::util::core::dir_list_contents( a0, a1 );
            },
            "",
            pybind11::arg( "dir" ),
            pybind11::arg( "full_path" )
        );
    M( "genesis::util::core" )
        .def(
            "dir_list_contents",
            ( class std::vector<std::string >( * )( const std::string&, bool, const std::string& )
            ) & genesis::util::core::dir_list_contents,
            "Get a list of files and directories in a directory.\n\n If  is set to `true`, the "
            "path of the provided  is prepended to the resulting\n list. If a non-empy  is "
            "provided, file names are filtered by this regular expression.\n\n If the directory is "
            "not readable, the function throws `std::runtime_error`.\n\nC++: "
            "genesis::util::core::dir_list_contents(const std::string &, bool, const std::string "
            "&) --> class std::vector<std::string >",
            pybind11::arg( "dir" ),
            pybind11::arg( "full_path" ),
            pybind11::arg( "regex" )
        );

    // genesis::util::core::dir_list_files(const std::string &, bool, const std::string &)
    // file:genesis/util/core/fs.hpp line:188 function-signature: class std::vector<std::string >
    // genesis::util::core::dir_list_files(const std::string &, bool, const std::string &)(const
    // std::string &, bool, const std::string &) file:genesis/util/core/fs.hpp line:188
    M( "genesis::util::core" )
        .def(
            "dir_list_files",
            []( const std::string& a0 ) -> std::vector<std::string > {
                return genesis::util::core::dir_list_files( a0 );
            },
            "",
            pybind11::arg( "dir" )
        );
    M( "genesis::util::core" )
        .def(
            "dir_list_files",
            []( const std::string& a0, bool const& a1 ) -> std::vector<std::string > {
                return genesis::util::core::dir_list_files( a0, a1 );
            },
            "",
            pybind11::arg( "dir" ),
            pybind11::arg( "full_path" )
        );
    M( "genesis::util::core" )
        .def(
            "dir_list_files",
            ( class std::vector<std::string >( * )( const std::string&, bool, const std::string& )
            ) & genesis::util::core::dir_list_files,
            "Get a list of files in a directory.\n\n Behaves the same as dir_list_contents(), "
            "except tat only files are listed (i.e., no directories).\n See there for an "
            "explanation of the arguments.\n\nC++: genesis::util::core::dir_list_files(const "
            "std::string &, bool, const std::string &) --> class std::vector<std::string >",
            pybind11::arg( "dir" ),
            pybind11::arg( "full_path" ),
            pybind11::arg( "regex" )
        );

    // genesis::util::core::dir_list_directories(const std::string &, bool, const std::string &)
    // file:genesis/util/core/fs.hpp line:200 function-signature: class std::vector<std::string >
    // genesis::util::core::dir_list_directories(const std::string &, bool, const std::string
    // &)(const std::string &, bool, const std::string &) file:genesis/util/core/fs.hpp line:200
    M( "genesis::util::core" )
        .def(
            "dir_list_directories",
            []( const std::string& a0 ) -> std::vector<std::string > {
                return genesis::util::core::dir_list_directories( a0 );
            },
            "",
            pybind11::arg( "dir" )
        );
    M( "genesis::util::core" )
        .def(
            "dir_list_directories",
            []( const std::string& a0, bool const& a1 ) -> std::vector<std::string > {
                return genesis::util::core::dir_list_directories( a0, a1 );
            },
            "",
            pybind11::arg( "dir" ),
            pybind11::arg( "full_path" )
        );
    M( "genesis::util::core" )
        .def(
            "dir_list_directories",
            ( class std::vector<std::string >( * )( const std::string&, bool, const std::string& )
            ) & genesis::util::core::dir_list_directories,
            "Get a list of directories in a directory.\n\n Behaves the same as "
            "dir_list_contents(), except tat only directories are listed (i.e., no files).\n See "
            "there for an explanation of the arguments.\n\nC++: "
            "genesis::util::core::dir_list_directories(const std::string &, bool, const "
            "std::string &) --> class std::vector<std::string >",
            pybind11::arg( "dir" ),
            pybind11::arg( "full_path" ),
            pybind11::arg( "regex" )
        );

    // genesis::util::core::current_path() file:genesis/util/core/fs.hpp line:218
    // function-signature: std::string genesis::util::core::current_path()()
    // file:genesis/util/core/fs.hpp line:218
    M( "genesis::util::core" )
        .def(
            "current_path",
            ( std::string( * )() ) & genesis::util::core::current_path,
            "Return the current (working) directory, simiar to getcwd().\n\n The function behaves "
            "similar to getcwd(). The path is returned with a trailing slash,\n for conveniently "
            "apppending file names to it.\n\n As this function changes the directory in the "
            "process, it is not thread safe.\n\nC++: genesis::util::core::current_path() --> "
            "std::string"
        );

    // genesis::util::core::real_path(const std::string &, bool) file:genesis/util/core/fs.hpp
    // line:228 function-signature: std::string genesis::util::core::real_path(const std::string &,
    // bool)(const std::string &, bool) file:genesis/util/core/fs.hpp line:228
    M( "genesis::util::core" )
        .def(
            "real_path",
            []( const std::string& a0 ) -> std::string {
                return genesis::util::core::real_path( a0 );
            },
            "",
            pybind11::arg( "path" )
        );
    M( "genesis::util::core" )
        .def(
            "real_path",
            ( std::string( * )( const std::string&, bool ) ) & genesis::util::core::real_path,
            "Return the real path of a file or directory, similar to realpath().\n\n The function "
            "behaves similar to realpath(), but (hopefully) offers better protability\n and error "
            "handling (long paths etc).\n\n As this function changes the directory in the process, "
            "it is not thread safe.\n\nC++: genesis::util::core::real_path(const std::string &, "
            "bool) --> std::string",
            pybind11::arg( "path" ),
            pybind11::arg( "resolve_link" )
        );

    // genesis::util::core::file_info(const std::string &) file:genesis/util/core/fs.hpp line:237
    // function-signature: class std::unordered_map<std::string, std::string >
    // genesis::util::core::file_info(const std::string &)(const std::string &)
    // file:genesis/util/core/fs.hpp line:237
    M( "genesis::util::core" )
        .def(
            "file_info",
            ( class std::unordered_map<std::string, std::string >( * )( const std::string& ) ) &
                genesis::util::core::file_info,
            "Return information about a file.\n\nC++: genesis::util::core::file_info(const "
            "std::string &) --> class std::unordered_map<std::string, std::string >",
            pybind11::arg( "filename" )
        );

    // genesis::util::core::file_size(const std::string &) file:genesis/util/core/fs.hpp line:242
    // function-signature: unsigned long genesis::util::core::file_size(const std::string &)(const
    // std::string &) file:genesis/util/core/fs.hpp line:242
    M( "genesis::util::core" )
        .def(
            "file_size",
            ( unsigned long ( * )( const std::string& ) ) & genesis::util::core::file_size,
            "Return the size of a file.\n\nC++: genesis::util::core::file_size(const std::string "
            "&) --> unsigned long",
            pybind11::arg( "filename" )
        );

    // genesis::util::core::file_path(const std::string &) file:genesis/util/core/fs.hpp line:250
    // function-signature: std::string genesis::util::core::file_path(const std::string &)(const
    // std::string &) file:genesis/util/core/fs.hpp line:250
    M( "genesis::util::core" )
        .def(
            "file_path",
            ( std::string( * )( const std::string& ) ) & genesis::util::core::file_path,
            "Return the path leading to a file.\n\n Does not resolve the path. Simply splits at "
            "the last directory separator.\n If there is no directory separator, it returns an "
            "empty string.\n\nC++: genesis::util::core::file_path(const std::string &) --> "
            "std::string",
            pybind11::arg( "filename" )
        );

    // genesis::util::core::file_basename(const std::string &) file:genesis/util/core/fs.hpp
    // line:255 function-signature: std::string genesis::util::core::file_basename(const std::string
    // &)(const std::string &) file:genesis/util/core/fs.hpp line:255
    M( "genesis::util::core" )
        .def(
            "file_basename",
            ( std::string( * )( const std::string& ) ) & genesis::util::core::file_basename,
            "Remove directory name from file name if present.\n\nC++: "
            "genesis::util::core::file_basename(const std::string &) --> std::string",
            pybind11::arg( "filename" )
        );

    // genesis::util::core::file_basename(const std::string &, const class std::vector<std::string >
    // &) file:genesis/util/core/fs.hpp line:274 function-signature: std::string
    // genesis::util::core::file_basename(const std::string &, const class std::vector<std::string >
    // &)(const std::string &, const class std::vector<std::string > &)
    // file:genesis/util/core/fs.hpp line:274
    M( "genesis::util::core" )
        .def(
            "file_basename",
            ( std::string( * )( const std::string&, const class std::vector<std::string >& ) ) &
                genesis::util::core::file_basename,
            "Remove the directory name from a file name if present, and remove a list of given "
            "extensions.\n\n After obtaining the file basename (the final rightmost segment of the "
            "file path after the last\n directory seperator `/`), the  are also removed from the "
            "file name.\n The given list is iterated regularly, and removed (from right to left, "
            "as these are suffixes)\n from the file name, if present.\n\n For example:\n\n     "
            "std::string path = \"/path/to/filename.txt.gz\";\n     std::cout << file_basename( "
            "path, { \".gz\", \".txt\" }) << \"\\n\";\n\n yields \"filename\". This is useful for "
            "example to remove a set of known extensions for a file\n type, for clean user output. "
            "Not all extensions have to be present; those that are not, are\n simply "
            "skipped.\n\nC++: genesis::util::core::file_basename(const std::string &, const class "
            "std::vector<std::string > &) --> std::string",
            pybind11::arg( "filename" ),
            pybind11::arg( "remove_extensions" )
        );

    // genesis::util::core::file_filename(const std::string &) file:genesis/util/core/fs.hpp
    // line:285 function-signature: std::string genesis::util::core::file_filename(const std::string
    // &)(const std::string &) file:genesis/util/core/fs.hpp line:285
    M( "genesis::util::core" )
        .def(
            "file_filename",
            ( std::string( * )( const std::string& ) ) & genesis::util::core::file_filename,
            "Remove extension if present.\n\n Caveat: Does not remove the path. So, if the "
            "filename itself does not contain an extension\n separator \".\", but the path does, "
            "this will yield an unwanted result. Call file_basename() first.\n\nC++: "
            "genesis::util::core::file_filename(const std::string &) --> std::string",
            pybind11::arg( "filename" )
        );

    // genesis::util::core::file_extension(const std::string &) file:genesis/util/core/fs.hpp
    // line:292 function-signature: std::string genesis::util::core::file_extension(const
    // std::string &)(const std::string &) file:genesis/util/core/fs.hpp line:292
    M( "genesis::util::core" )
        .def(
            "file_extension",
            ( std::string( * )( const std::string& ) ) & genesis::util::core::file_extension,
            "Return the extension name of a file.\n\n Also see file_filename().\n\nC++: "
            "genesis::util::core::file_extension(const std::string &) --> std::string",
            pybind11::arg( "filename" )
        );

    // genesis::util::core::is_valid_filename(const std::string &) file:genesis/util/core/fs.hpp
    // line:318 function-signature: bool genesis::util::core::is_valid_filename(const std::string
    // &)(const std::string &) file:genesis/util/core/fs.hpp line:318
    M( "genesis::util::core" )
        .def(
            "is_valid_filename",
            ( bool ( * )( const std::string& ) ) & genesis::util::core::is_valid_filename,
            "Check whether a file name is valid.\n\n Validating filenames depends on the operating "
            "system and file system of the disk. Thus, this is\n usually not an easy task. This "
            "function only checks some basics and is meant to catch the most\n common "
            "problems.\n\n The function is meant to be called on the file name itself, without the "
            "directory path leading\n to it. File extensions are allowed. Thus, you might need to "
            "call file_basename() before in order\n to get the file name without the path.\n\n "
            "Invalid filenames are:\n\n   * Those with spaces at the beginning or end, or only "
            "consisting of spaces (or empty).\n   * Those which contain any of the chars `< > : \" "
            "\\ / | ? *`.\n   * Those which contain any non-printable character, as determined via "
            "isprint().\n\n This might be too converative for some system, or allow too much for "
            "others. It however should\n return true for filenames that work on most "
            "systems.\n\nC++: genesis::util::core::is_valid_filename(const std::string &) --> bool",
            pybind11::arg( "filename" )
        );

    // genesis::util::core::sanitize_filename(const std::string &) file:genesis/util/core/fs.hpp
    // line:341 function-signature: std::string genesis::util::core::sanitize_filename(const
    // std::string &)(const std::string &) file:genesis/util/core/fs.hpp line:341
    M( "genesis::util::core" )
        .def(
            "sanitize_filename",
            ( std::string( * )( const std::string& ) ) & genesis::util::core::sanitize_filename,
            "Remove or replace all invalid parts of a filename.\n\n Similar to "
            "is_valid_filename(), this function is not meant to be an ultimate solution to valid\n "
            "filenames. See there for details.\n\n The function is meant to be called on the file "
            "name itself, without the directory path leading\n to it. File extensions are allowed. "
            "Thus, you might need to call file_basename() before in order\n to get the file name "
            "without the path.\n\n This function does the following:\n\n   * All non-printable "
            "characters are removed.\n   * Spaces at the beginning and end are removed.\n   * All "
            "invalid chars are replaced by an underscore. See is_valid_filename() for a list of "
            "those\n     chars.\n\n If after this procedure the filename is empty, an exception is "
            "thrown. This is meant to save the\n user from checking this, or from running into "
            "trouble when trying to write to this \"file\" -\n because an empty filename will "
            "point to a directory name.\n\nC++: genesis::util::core::sanitize_filename(const "
            "std::string &) --> std::string",
            pybind11::arg( "filename" )
        );
}

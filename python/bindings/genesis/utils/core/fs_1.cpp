#include <genesis/utils/core/fs.hpp>
#include <iterator>
#include <memory>
#include <string>
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

void bind_genesis_utils_core_fs_1(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    // genesis::utils::core::file_size(const std::string &) file:genesis/utils/core/fs.hpp line:242
    // function-signature: unsigned long genesis::utils::core::file_size(const std::string &)(const
    // std::string &) file:genesis/utils/core/fs.hpp line:242
    M( "genesis::utils::core" )
        .def(
            "file_size",
            ( unsigned long ( * )( const std::string& ) ) & genesis::utils::core::file_size,
            "Return the size of a file.\n\nC++: genesis::utils::core::file_size(const std::string "
            "&) --> unsigned long",
            pybind11::arg( "filename" )
        );

    // genesis::utils::core::file_path(const std::string &) file:genesis/utils/core/fs.hpp line:250
    // function-signature: std::string genesis::utils::core::file_path(const std::string &)(const
    // std::string &) file:genesis/utils/core/fs.hpp line:250
    M( "genesis::utils::core" )
        .def(
            "file_path",
            ( std::string( * )( const std::string& ) ) & genesis::utils::core::file_path,
            "Return the path leading to a file.\n\n Does not resolve the path. Simply splits at "
            "the last directory separator.\n If there is no directory separator, it returns an "
            "empty string.\n\nC++: genesis::utils::core::file_path(const std::string &) --> "
            "std::string",
            pybind11::arg( "filename" )
        );

    // genesis::utils::core::file_basename(const std::string &) file:genesis/utils/core/fs.hpp
    // line:255 function-signature: std::string genesis::utils::core::file_basename(const
    // std::string &)(const std::string &) file:genesis/utils/core/fs.hpp line:255
    M( "genesis::utils::core" )
        .def(
            "file_basename",
            ( std::string( * )( const std::string& ) ) & genesis::utils::core::file_basename,
            "Remove directory name from file name if present.\n\nC++: "
            "genesis::utils::core::file_basename(const std::string &) --> std::string",
            pybind11::arg( "filename" )
        );

    // genesis::utils::core::file_basename(const std::string &, const class std::vector<std::string
    // > &) file:genesis/utils/core/fs.hpp line:274 function-signature: std::string
    // genesis::utils::core::file_basename(const std::string &, const class std::vector<std::string
    // > &)(const std::string &, const class std::vector<std::string > &)
    // file:genesis/utils/core/fs.hpp line:274
    M( "genesis::utils::core" )
        .def(
            "file_basename",
            ( std::string( * )( const std::string&, const class std::vector<std::string >& ) ) &
                genesis::utils::core::file_basename,
            "Remove the directory name from a file name if present, and remove a list of given "
            "extensions.\n\n After obtaining the file basename (the final rightmost segment of the "
            "file path after the last\n directory seperator `/`), the  are also removed from the "
            "file name.\n The given list is iterated regularly, and removed (from right to left, "
            "as these are suffixes)\n from the file name, if present.\n\n For example:\n\n     "
            "std::string path = \"/path/to/filename.txt.gz\";\n     std::cout << file_basename( "
            "path, { \".gz\", \".txt\" }) << \"\\n\";\n\n yields \"filename\". This is useful for "
            "example to remove a set of known extensions for a file\n type, for clean user output. "
            "Not all extensions have to be present; those that are not, are\n simply "
            "skipped.\n\nC++: genesis::utils::core::file_basename(const std::string &, const class "
            "std::vector<std::string > &) --> std::string",
            pybind11::arg( "filename" ),
            pybind11::arg( "remove_extensions" )
        );

    // genesis::utils::core::file_filename(const std::string &) file:genesis/utils/core/fs.hpp
    // line:285 function-signature: std::string genesis::utils::core::file_filename(const
    // std::string &)(const std::string &) file:genesis/utils/core/fs.hpp line:285
    M( "genesis::utils::core" )
        .def(
            "file_filename",
            ( std::string( * )( const std::string& ) ) & genesis::utils::core::file_filename,
            "Remove extension if present.\n\n Caveat: Does not remove the path. So, if the "
            "filename itself does not contain an extension\n separator \".\", but the path does, "
            "this will yield an unwanted result. Call file_basename() first.\n\nC++: "
            "genesis::utils::core::file_filename(const std::string &) --> std::string",
            pybind11::arg( "filename" )
        );

    // genesis::utils::core::file_extension(const std::string &) file:genesis/utils/core/fs.hpp
    // line:292 function-signature: std::string genesis::utils::core::file_extension(const
    // std::string &)(const std::string &) file:genesis/utils/core/fs.hpp line:292
    M( "genesis::utils::core" )
        .def(
            "file_extension",
            ( std::string( * )( const std::string& ) ) & genesis::utils::core::file_extension,
            "Return the extension name of a file.\n\n Also see file_filename().\n\nC++: "
            "genesis::utils::core::file_extension(const std::string &) --> std::string",
            pybind11::arg( "filename" )
        );

    // genesis::utils::core::is_valid_filename(const std::string &) file:genesis/utils/core/fs.hpp
    // line:318 function-signature: bool genesis::utils::core::is_valid_filename(const std::string
    // &)(const std::string &) file:genesis/utils/core/fs.hpp line:318
    M( "genesis::utils::core" )
        .def(
            "is_valid_filename",
            ( bool ( * )( const std::string& ) ) & genesis::utils::core::is_valid_filename,
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
            "systems.\n\nC++: genesis::utils::core::is_valid_filename(const std::string &) --> "
            "bool",
            pybind11::arg( "filename" )
        );

    // genesis::utils::core::sanitize_filename(const std::string &) file:genesis/utils/core/fs.hpp
    // line:341 function-signature: std::string genesis::utils::core::sanitize_filename(const
    // std::string &)(const std::string &) file:genesis/utils/core/fs.hpp line:341
    M( "genesis::utils::core" )
        .def(
            "sanitize_filename",
            ( std::string( * )( const std::string& ) ) & genesis::utils::core::sanitize_filename,
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
            "point to a directory name.\n\nC++: genesis::utils::core::sanitize_filename(const "
            "std::string &) --> std::string",
            pybind11::arg( "filename" )
        );
}

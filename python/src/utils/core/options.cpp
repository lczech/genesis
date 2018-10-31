// /**
//  * @brief
//  *
//  * @file
//  * @ingroup python
//  */
//
// #include <src/common.hpp>
//
// #include "genesis/genesis.hpp"
//
// using namespace ::genesis::utils;
//
// PYTHON_EXPORT_CLASS( ::genesis::utils::Options, scope )
// {
//
//     // -------------------------------------------------------------------
//     //     Class Options
//     // -------------------------------------------------------------------
//
//     pybind11::class_< ::genesis::utils::Options, std::shared_ptr<::genesis::utils::Options> > ( scope, "Options" )
//
//         // Public Member Functions
//
//         .def(
//             "allow_file_overwriting",
//             ( bool ( ::genesis::utils::Options::* )(  ) const )( &::genesis::utils::Options::allow_file_overwriting ),
//             get_docstring("bool ::genesis::utils::Options::allow_file_overwriting () const")
//         )
//         .def(
//             "allow_file_overwriting",
//             ( void ( ::genesis::utils::Options::* )( bool ))( &::genesis::utils::Options::allow_file_overwriting ),
//             pybind11::arg("value"),
//             get_docstring("void ::genesis::utils::Options::allow_file_overwriting (bool value)")
//         )
//         .def(
//             "command_line",
//             ( std::vector< std::string > ( ::genesis::utils::Options::* )(  ) const )( &::genesis::utils::Options::command_line ),
//             get_docstring("std::vector< std::string > ::genesis::utils::Options::command_line () const")
//         )
//         .def(
//             "command_line",
//             ( void ( ::genesis::utils::Options::* )( int const, char const *const * ))( &::genesis::utils::Options::command_line ),
//             pybind11::arg("argc"),
//             pybind11::arg("argv"),
//             get_docstring("void ::genesis::utils::Options::command_line (int const argc, char const *const * argv)")
//         )
//         .def(
//             "command_line_string",
//             ( std::string ( ::genesis::utils::Options::* )(  ) const )( &::genesis::utils::Options::command_line_string ),
//             get_docstring("std::string ::genesis::utils::Options::command_line_string () const")
//         )
//         .def(
//             "info",
//             ( std::string ( ::genesis::utils::Options::* )(  ) const )( &::genesis::utils::Options::info ),
//             get_docstring("std::string ::genesis::utils::Options::info () const")
//         )
//         .def(
//             "number_of_threads",
//             ( unsigned int ( ::genesis::utils::Options::* )(  ) const )( &::genesis::utils::Options::number_of_threads ),
//             get_docstring("unsigned int ::genesis::utils::Options::number_of_threads () const")
//         )
//         .def(
//             "number_of_threads",
//             ( void ( ::genesis::utils::Options::* )( unsigned int ))( &::genesis::utils::Options::number_of_threads ),
//             pybind11::arg("number"),
//             get_docstring("void ::genesis::utils::Options::number_of_threads (unsigned int number)")
//         )
//         .def(
//             "print_object_gists",
//             ( long ( ::genesis::utils::Options::* )(  ) const )( &::genesis::utils::Options::print_object_gists ),
//             get_docstring("long ::genesis::utils::Options::print_object_gists () const")
//         )
//         .def(
//             "print_object_gists",
//             ( void ( ::genesis::utils::Options::* )( long ))( &::genesis::utils::Options::print_object_gists ),
//             pybind11::arg("value"),
//             get_docstring("void ::genesis::utils::Options::print_object_gists (long value)")
//         )
//         .def(
//             "print_object_infos",
//             ( bool ( ::genesis::utils::Options::* )(  ) const )( &::genesis::utils::Options::print_object_infos ),
//             get_docstring("bool ::genesis::utils::Options::print_object_infos () const")
//         )
//         .def(
//             "print_object_infos",
//             ( void ( ::genesis::utils::Options::* )( bool ))( &::genesis::utils::Options::print_object_infos ),
//             pybind11::arg("value"),
//             get_docstring("void ::genesis::utils::Options::print_object_infos (bool value)")
//         )
//         .def(
//             "random_engine",
//             ( std::default_random_engine & ( ::genesis::utils::Options::* )(  ))( &::genesis::utils::Options::random_engine ),
//             get_docstring("std::default_random_engine & ::genesis::utils::Options::random_engine ()")
//         )
//         .def(
//             "random_seed",
//             ( unsigned ( ::genesis::utils::Options::* )(  ) const )( &::genesis::utils::Options::random_seed ),
//             get_docstring("unsigned ::genesis::utils::Options::random_seed () const")
//         )
//         .def(
//             "random_seed",
//             ( void ( ::genesis::utils::Options::* )( const unsigned ))( &::genesis::utils::Options::random_seed ),
//             pybind11::arg("seed"),
//             get_docstring("void ::genesis::utils::Options::random_seed (const unsigned seed)")
//         )
//         .def_static(
//             "build_type",
//             ( std::string ( * )(  ))( &::genesis::utils::Options::build_type ),
//             get_docstring("static std::string ::genesis::utils::Options::build_type ()")
//         )
//         .def_static(
//             "compile_date_time",
//             ( std::string ( * )(  ))( &::genesis::utils::Options::compile_date_time ),
//             get_docstring("static std::string ::genesis::utils::Options::compile_date_time ()")
//         )
//         .def_static(
//             "compiler_family",
//             ( std::string ( * )(  ))( &::genesis::utils::Options::compiler_family ),
//             get_docstring("static std::string ::genesis::utils::Options::compiler_family ()")
//         )
//         .def_static(
//             "compiler_version",
//             ( std::string ( * )(  ))( &::genesis::utils::Options::compiler_version ),
//             get_docstring("static std::string ::genesis::utils::Options::compiler_version ()")
//         )
//         .def_static(
//             "cpp_version",
//             ( std::string ( * )(  ))( &::genesis::utils::Options::cpp_version ),
//             get_docstring("static std::string ::genesis::utils::Options::cpp_version ()")
//         )
//         .def_static(
//             "get",
//             ( Options & ( * )(  ))( &::genesis::utils::Options::get ),
//             get_docstring("static Options & ::genesis::utils::Options::get ()")
//         )
//         .def_static(
//             "is_big_endian",
//             ( bool ( * )(  ))( &::genesis::utils::Options::is_big_endian ),
//             get_docstring("static bool ::genesis::utils::Options::is_big_endian ()")
//         )
//         .def_static(
//             "is_debug",
//             ( bool ( * )(  ))( &::genesis::utils::Options::is_debug ),
//             get_docstring("static bool ::genesis::utils::Options::is_debug ()")
//         )
//         .def_static(
//             "is_little_endian",
//             ( bool ( * )(  ))( &::genesis::utils::Options::is_little_endian ),
//             get_docstring("static bool ::genesis::utils::Options::is_little_endian ()")
//         )
//         .def_static(
//             "is_release",
//             ( bool ( * )(  ))( &::genesis::utils::Options::is_release ),
//             get_docstring("static bool ::genesis::utils::Options::is_release ()")
//         )
//         .def_static(
//             "platform",
//             ( std::string ( * )(  ))( &::genesis::utils::Options::platform ),
//             get_docstring("static std::string ::genesis::utils::Options::platform ()")
//         )
//         .def_static(
//             "stderr_is_terminal",
//             ( bool ( * )(  ))( &::genesis::utils::Options::stderr_is_terminal ),
//             get_docstring("static bool ::genesis::utils::Options::stderr_is_terminal ()")
//         )
//         .def_static(
//             "stdin_is_terminal",
//             ( bool ( * )(  ))( &::genesis::utils::Options::stdin_is_terminal ),
//             get_docstring("static bool ::genesis::utils::Options::stdin_is_terminal ()")
//         )
//         .def_static(
//             "stdout_is_terminal",
//             ( bool ( * )(  ))( &::genesis::utils::Options::stdout_is_terminal ),
//             get_docstring("static bool ::genesis::utils::Options::stdout_is_terminal ()")
//         )
//         .def_static(
//             "terminal_size",
//             ( std::pair< int, int > ( * )(  ))( &::genesis::utils::Options::terminal_size ),
//             get_docstring("static std::pair< int, int > ::genesis::utils::Options::terminal_size ()")
//         )
//         .def_static(
//             "using_openmp",
//             ( bool ( * )(  ))( &::genesis::utils::Options::using_openmp ),
//             get_docstring("static bool ::genesis::utils::Options::using_openmp ()")
//         )
//         .def_static(
//             "using_pthreads",
//             ( bool ( * )(  ))( &::genesis::utils::Options::using_pthreads ),
//             get_docstring("static bool ::genesis::utils::Options::using_pthreads ()")
//         )
//         .def_static(
//             "using_zlib",
//             ( bool ( * )(  ))( &::genesis::utils::Options::using_zlib ),
//             get_docstring("static bool ::genesis::utils::Options::using_zlib ()")
//         )
//     ;
// }

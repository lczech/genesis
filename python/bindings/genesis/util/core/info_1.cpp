#include <genesis/util/core/info.hpp>
#include <iterator>
#include <memory>
#include <string>
#include <utility>

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

void bind_genesis_util_core_info_1(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    // genesis::util::core::info_get_pid() file:genesis/util/core/info.hpp line:367
    // function-signature: unsigned long genesis::util::core::info_get_pid()()
    // file:genesis/util/core/info.hpp line:367
    M( "genesis::util::core" )
        .def(
            "info_get_pid",
            ( unsigned long ( * )() ) & genesis::util::core::info_get_pid,
            "Get the process ID of the current process.\n\n This is a POSIX system call, using "
            "`getpid()`. See https://man7.org/linux/man-pages/man2/getpid.2.html\n and "
            "https://developer.apple.com/library/archive/documentation/System/Conceptual/"
            "ManPages_iPhoneOS/man2/getpid.2.html\n for details. We wrap this here to avoid having "
            "the system headers included. Furthermore, we\n convert to a size_t for convenience, "
            "although typically, the pid is an `int` (guaranteed to be\n non-negative for an "
            "actual process).\n\nC++: genesis::util::core::info_get_pid() --> unsigned long"
        );

    // genesis::util::core::info_stdin_is_terminal() file:genesis/util/core/info.hpp line:373
    // function-signature: bool genesis::util::core::info_stdin_is_terminal()()
    // file:genesis/util/core/info.hpp line:373
    M( "genesis::util::core" )
        .def(
            "info_stdin_is_terminal",
            ( bool ( * )() ) & genesis::util::core::info_stdin_is_terminal,
            "Return true iff the standard input stream is a terminal, and false if not, i.e., if\n "
            "it is a file or a pipe.\n\nC++: genesis::util::core::info_stdin_is_terminal() --> bool"
        );

    // genesis::util::core::info_stdout_is_terminal() file:genesis/util/core/info.hpp line:379
    // function-signature: bool genesis::util::core::info_stdout_is_terminal()()
    // file:genesis/util/core/info.hpp line:379
    M( "genesis::util::core" )
        .def(
            "info_stdout_is_terminal",
            ( bool ( * )() ) & genesis::util::core::info_stdout_is_terminal,
            "Return true iff the standard output stream is a terminal, and false if not, i.e., "
            "if\n it is a file or a pipe.\n\nC++: genesis::util::core::info_stdout_is_terminal() "
            "--> bool"
        );

    // genesis::util::core::info_stderr_is_terminal() file:genesis/util/core/info.hpp line:385
    // function-signature: bool genesis::util::core::info_stderr_is_terminal()()
    // file:genesis/util/core/info.hpp line:385
    M( "genesis::util::core" )
        .def(
            "info_stderr_is_terminal",
            ( bool ( * )() ) & genesis::util::core::info_stderr_is_terminal,
            "Return true iff the standard error stream is a terminal, and false if not, i.e., if\n "
            "it is a file or a pipe.\n\nC++: genesis::util::core::info_stderr_is_terminal() --> "
            "bool"
        );

    // genesis::util::core::info_terminal_size() file:genesis/util/core/info.hpp line:391
    // function-signature: struct std::pair<int, int> genesis::util::core::info_terminal_size()()
    // file:genesis/util/core/info.hpp line:391
    M( "genesis::util::core" )
        .def(
            "info_terminal_size",
            ( struct std::pair<int, int>( * )() ) & genesis::util::core::info_terminal_size,
            "Return the width and height of the terminal that is used to run the program,\n in "
            "number of columns and lines.\n\nC++: genesis::util::core::info_terminal_size() --> "
            "struct std::pair<int, int>"
        );

    // genesis::util::core::info_process_max_file_count() file:genesis/util/core/info.hpp line:401
    // function-signature: unsigned long genesis::util::core::info_process_max_file_count()()
    // file:genesis/util/core/info.hpp line:401
    M( "genesis::util::core" )
        .def(
            "info_process_max_file_count",
            ( unsigned long ( * )() ) & genesis::util::core::info_process_max_file_count,
            "Return the maximum number of files (i.e., file descriptors)\n that can be opened "
            "simultaneously in the current process.\n\nC++: "
            "genesis::util::core::info_process_max_file_count() --> unsigned long"
        );

    // genesis::util::core::info_process_current_file_count() file:genesis/util/core/info.hpp
    // line:407 function-signature: unsigned long
    // genesis::util::core::info_process_current_file_count()() file:genesis/util/core/info.hpp
    // line:407
    M( "genesis::util::core" )
        .def(
            "info_process_current_file_count",
            ( unsigned long ( * )() ) & genesis::util::core::info_process_current_file_count,
            "Return the number of files (i.e., file descriptors) that the current process\n (the "
            "process calling this function) has opened at the moment.\n\nC++: "
            "genesis::util::core::info_process_current_file_count() --> unsigned long"
        );

    // genesis::util::core::info_process_current_memory_usage() file:genesis/util/core/info.hpp
    // line:412 function-signature: unsigned long
    // genesis::util::core::info_process_current_memory_usage()() file:genesis/util/core/info.hpp
    // line:412
    M( "genesis::util::core" )
        .def(
            "info_process_current_memory_usage",
            ( unsigned long ( * )() ) & genesis::util::core::info_process_current_memory_usage,
            "Return the memory currently used by the current process, in bytes.\n\nC++: "
            "genesis::util::core::info_process_current_memory_usage() --> unsigned long"
        );

    // genesis::util::core::info_system_current_memory_usage() file:genesis/util/core/info.hpp
    // line:417 function-signature: unsigned long
    // genesis::util::core::info_system_current_memory_usage()() file:genesis/util/core/info.hpp
    // line:417
    M( "genesis::util::core" )
        .def(
            "info_system_current_memory_usage",
            ( unsigned long ( * )() ) & genesis::util::core::info_system_current_memory_usage,
            "Return the memory currently used across all running processes, in bytes.\n\nC++: "
            "genesis::util::core::info_system_current_memory_usage() --> unsigned long"
        );

    // genesis::util::core::info_system_current_memory_available() file:genesis/util/core/info.hpp
    // line:422 function-signature: unsigned long
    // genesis::util::core::info_system_current_memory_available()() file:genesis/util/core/info.hpp
    // line:422
    M( "genesis::util::core" )
        .def(
            "info_system_current_memory_available",
            ( unsigned long ( * )() ) & genesis::util::core::info_system_current_memory_available,
            "Return the memory currently available in the system, in bytes.\n\nC++: "
            "genesis::util::core::info_system_current_memory_available() --> unsigned long"
        );

    // genesis::util::core::info_process_current_cpu_usage(bool, bool)
    // file:genesis/util/core/info.hpp line:437 function-signature: double
    // genesis::util::core::info_process_current_cpu_usage(bool, bool)(bool, bool)
    // file:genesis/util/core/info.hpp line:437
    M( "genesis::util::core" )
        .def(
            "info_process_current_cpu_usage",
            []() -> double { return genesis::util::core::info_process_current_cpu_usage(); },
            ""
        );
    M( "genesis::util::core" )
        .def(
            "info_process_current_cpu_usage",
            []( bool const& a0 ) -> double {
                return genesis::util::core::info_process_current_cpu_usage( a0 );
            },
            "",
            pybind11::arg( "all_cores" )
        );
    M( "genesis::util::core" )
        .def(
            "info_process_current_cpu_usage",
            ( double ( * )( bool, bool ) ) & genesis::util::core::info_process_current_cpu_usage,
            "Return the CPU usage of the current process.\n\n The first time the function is "
            "called, it is being initialized, and will return 0.0, as there is\n no usage to be "
            "reported yet. Any subsequent call with then report the usage since the last call.\n\n "
            "By default, this reports the total usage across all cores. That is, for multi-core "
            "systems,\n this can report usage up to the number of cores. For instance, two cores "
            "fully running on a\n 4-core system would report 200% usage (using return value 200.0 "
            "by default, or 2.0 if \n is set to `false`). If however  is set to `false`, this is "
            "instead divided by the\n number of cores, and so would return 50% usage (using return "
            "value 50.0 or 0.5, depending on\n  instead.\n\nC++: "
            "genesis::util::core::info_process_current_cpu_usage(bool, bool) --> double",
            pybind11::arg( "all_cores" ),
            pybind11::arg( "percent" )
        );

    // genesis::util::core::info_system_current_cpu_usage(bool, bool)
    // file:genesis/util/core/info.hpp line:444 function-signature: double
    // genesis::util::core::info_system_current_cpu_usage(bool, bool)(bool, bool)
    // file:genesis/util/core/info.hpp line:444
    M( "genesis::util::core" )
        .def(
            "info_system_current_cpu_usage",
            []() -> double { return genesis::util::core::info_system_current_cpu_usage(); },
            ""
        );
    M( "genesis::util::core" )
        .def(
            "info_system_current_cpu_usage",
            []( bool const& a0 ) -> double {
                return genesis::util::core::info_system_current_cpu_usage( a0 );
            },
            "",
            pybind11::arg( "all_cores" )
        );
    M( "genesis::util::core" )
        .def(
            "info_system_current_cpu_usage",
            ( double ( * )( bool, bool ) ) & genesis::util::core::info_system_current_cpu_usage,
            "Return the CPU usage of the system, across all cores.\n\n \n See "
            "info_process_current_cpu_usage() for details on the parameters.\n\nC++: "
            "genesis::util::core::info_system_current_cpu_usage(bool, bool) --> double",
            pybind11::arg( "all_cores" ),
            pybind11::arg( "percent" )
        );

    // genesis::util::core::info_process_peak_memory_usage() file:genesis/util/core/info.hpp
    // line:453 function-signature: unsigned long
    // genesis::util::core::info_process_peak_memory_usage()() file:genesis/util/core/info.hpp
    // line:453
    M( "genesis::util::core" )
        .def(
            "info_process_peak_memory_usage",
            ( unsigned long ( * )() ) & genesis::util::core::info_process_peak_memory_usage,
            "Get the peak used memory, in bytes.\n\nC++: "
            "genesis::util::core::info_process_peak_memory_usage() --> unsigned long"
        );

    // genesis::util::core::info_process_total_cpu_time() file:genesis/util/core/info.hpp line:460
    // function-signature: struct std::pair<double, double>
    // genesis::util::core::info_process_total_cpu_time()() file:genesis/util/core/info.hpp line:460
    M( "genesis::util::core" )
        .def(
            "info_process_total_cpu_time",
            ( struct std::pair<double, double>( * )() ) &
                genesis::util::core::info_process_total_cpu_time,
            "Get the currently used cpu run time, similar to the Unix `time` command.\n\n Time is "
            "returned in seconds, with the first result the user time, and the second the system "
            "time.\n\nC++: genesis::util::core::info_process_total_cpu_time() --> struct "
            "std::pair<double, double>"
        );

    // genesis::util::core::info_process_total_energy_consumption() file:genesis/util/core/info.hpp
    // line:465 function-signature: double
    // genesis::util::core::info_process_total_energy_consumption()()
    // file:genesis/util/core/info.hpp line:465
    M( "genesis::util::core" )
        .def(
            "info_process_total_energy_consumption",
            ( double ( * )() ) & genesis::util::core::info_process_total_energy_consumption,
            "Get energy consumption of the program so far, in Wh.\n\nC++: "
            "genesis::util::core::info_process_total_energy_consumption() --> double"
        );

    // genesis::util::core::info_process_print_total_usage() file:genesis/util/core/info.hpp
    // line:470 function-signature: std::string
    // genesis::util::core::info_process_print_total_usage()() file:genesis/util/core/info.hpp
    // line:470
    M( "genesis::util::core" )
        .def(
            "info_process_print_total_usage",
            ( std::string( * )() ) & genesis::util::core::info_process_print_total_usage,
            "Print usage information to a string.\n\nC++: "
            "genesis::util::core::info_process_print_total_usage() --> std::string"
        );
}

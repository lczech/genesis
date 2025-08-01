#include <genesis/utils/tool/timer.hpp>
#include <sstream> // __str__

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

void bind_genesis_utils_tool_timer(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::utils::Timer file:genesis/utils/tool/timer.hpp line:49
        pybind11::class_<genesis::utils::Timer, std::shared_ptr<genesis::utils::Timer>> cl(
            M( "genesis::utils" ), "Timer", "Measure time."
        );
        // function-signature: genesis::utils::Timer::Timer(bool)(bool)
        // file:genesis/utils/tool/timer.hpp line:57
        cl.def( pybind11::init( []() { return new genesis::utils::Timer(); } ), "doc" );
        // function-signature: genesis::utils::Timer::Timer(bool)(bool)
        // file:genesis/utils/tool/timer.hpp line:57
        cl.def( pybind11::init<bool>(), pybind11::arg( "auto_start" ) );

        // function-signature: genesis::utils::Timer::Timer(const struct genesis::utils::Timer
        // &)(const struct genesis::utils::Timer &) file:genesis/utils/tool/timer.hpp line:69
        cl.def( pybind11::init( []( genesis::utils::Timer const& o ) {
            return new genesis::utils::Timer( o );
        } ) );
        // function-signature: struct genesis::utils::Timer & genesis::utils::Timer::operator=(const
        // struct genesis::utils::Timer &)(const struct genesis::utils::Timer &)
        // file:genesis/utils/tool/timer.hpp line:72
        cl.def(
            "assign",
            ( struct genesis::utils::Timer &
              (genesis::utils::Timer::*)(const struct genesis::utils::Timer&)) &
                genesis::utils::Timer::operator=,
            "C++: genesis::utils::Timer::operator=(const struct genesis::utils::Timer &) --> "
            "struct genesis::utils::Timer &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: void genesis::utils::Timer::start()()
        // file:genesis/utils/tool/timer.hpp line:79
        cl.def(
            "start",
            ( void( genesis::utils::Timer::* )() ) & genesis::utils::Timer::start,
            "C++: genesis::utils::Timer::start() --> void"
        );
        // function-signature: void genesis::utils::Timer::stop()()
        // file:genesis/utils/tool/timer.hpp line:87
        cl.def(
            "stop",
            ( void( genesis::utils::Timer::* )() ) & genesis::utils::Timer::stop,
            "C++: genesis::utils::Timer::stop() --> void"
        );
        // function-signature: void genesis::utils::Timer::pause()()
        // file:genesis/utils/tool/timer.hpp line:96
        cl.def(
            "pause",
            ( void( genesis::utils::Timer::* )() ) & genesis::utils::Timer::pause,
            "C++: genesis::utils::Timer::pause() --> void"
        );
        // function-signature: void genesis::utils::Timer::resume()()
        // file:genesis/utils/tool/timer.hpp line:101
        cl.def(
            "resume",
            ( void( genesis::utils::Timer::* )() ) & genesis::utils::Timer::resume,
            "C++: genesis::utils::Timer::resume() --> void"
        );
        // function-signature: void genesis::utils::Timer::reset()()
        // file:genesis/utils/tool/timer.hpp line:106
        cl.def(
            "reset",
            ( void( genesis::utils::Timer::* )() ) & genesis::utils::Timer::reset,
            "C++: genesis::utils::Timer::reset() --> void"
        );
        // function-signature: void genesis::utils::Timer::restart()()
        // file:genesis/utils/tool/timer.hpp line:112
        cl.def(
            "restart",
            ( void( genesis::utils::Timer::* )() ) & genesis::utils::Timer::restart,
            "C++: genesis::utils::Timer::restart() --> void"
        );
        // function-signature: double genesis::utils::Timer::elapsed() const()
        // file:genesis/utils/tool/timer.hpp line:123
        cl.def(
            "elapsed",
            ( double( genesis::utils::Timer::* )() const ) & genesis::utils::Timer::elapsed,
            "Return elapsed time in seconds.\n\n The timer keeps running, so this is essentially a "
            "\"lap\" function.\n\nC++: genesis::utils::Timer::elapsed() const --> double"
        );
        // function-signature: double genesis::utils::Timer::elapsed_and_restart()()
        // file:genesis/utils/tool/timer.hpp line:152
        cl.def(
            "elapsed_and_restart",
            ( double( genesis::utils::Timer::* )() ) & genesis::utils::Timer::elapsed_and_restart,
            "Return the elapsed time in seconds, and reset the timer.\n\nC++: "
            "genesis::utils::Timer::elapsed_and_restart() --> double"
        );
        // function-signature: double genesis::utils::Timer::elapsed_and_stop()()
        // file:genesis/utils/tool/timer.hpp line:162
        cl.def(
            "elapsed_and_stop",
            ( double( genesis::utils::Timer::* )() ) & genesis::utils::Timer::elapsed_and_stop,
            "Return the elapsed time in seconds, and stop the timer.\n\nC++: "
            "genesis::utils::Timer::elapsed_and_stop() --> double"
        );
    }
}

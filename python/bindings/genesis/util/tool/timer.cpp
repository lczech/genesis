#include <genesis/util/tool/timer.hpp>
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

void bind_genesis_util_tool_timer(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::util::Timer file:genesis/util/tool/timer.hpp line:49
        pybind11::class_<genesis::util::Timer, std::shared_ptr<genesis::util::Timer>> cl(
            M( "genesis::util" ), "Timer", "Measure time."
        );
        // function-signature: genesis::util::Timer::Timer(bool)(bool)
        // file:genesis/util/tool/timer.hpp line:57
        cl.def( pybind11::init( []() { return new genesis::util::Timer(); } ), "doc" );
        // function-signature: genesis::util::Timer::Timer(bool)(bool)
        // file:genesis/util/tool/timer.hpp line:57
        cl.def( pybind11::init<bool>(), pybind11::arg( "auto_start" ) );

        // function-signature: genesis::util::Timer::Timer(const struct genesis::util::Timer
        // &)(const struct genesis::util::Timer &) file:genesis/util/tool/timer.hpp line:69
        cl.def( pybind11::init( []( genesis::util::Timer const& o ) {
            return new genesis::util::Timer( o );
        } ) );
        // function-signature: struct genesis::util::Timer & genesis::util::Timer::operator=(const
        // struct genesis::util::Timer &)(const struct genesis::util::Timer &)
        // file:genesis/util/tool/timer.hpp line:72
        cl.def(
            "assign",
            ( struct genesis::util::Timer &
              (genesis::util::Timer::*)(const struct genesis::util::Timer&)) &
                genesis::util::Timer::operator=,
            "C++: genesis::util::Timer::operator=(const struct genesis::util::Timer &) --> struct "
            "genesis::util::Timer &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: void genesis::util::Timer::start()() file:genesis/util/tool/timer.hpp
        // line:79
        cl.def(
            "start",
            ( void( genesis::util::Timer::* )() ) & genesis::util::Timer::start,
            "C++: genesis::util::Timer::start() --> void"
        );
        // function-signature: void genesis::util::Timer::stop()() file:genesis/util/tool/timer.hpp
        // line:87
        cl.def(
            "stop",
            ( void( genesis::util::Timer::* )() ) & genesis::util::Timer::stop,
            "C++: genesis::util::Timer::stop() --> void"
        );
        // function-signature: void genesis::util::Timer::pause()() file:genesis/util/tool/timer.hpp
        // line:96
        cl.def(
            "pause",
            ( void( genesis::util::Timer::* )() ) & genesis::util::Timer::pause,
            "C++: genesis::util::Timer::pause() --> void"
        );
        // function-signature: void genesis::util::Timer::resume()()
        // file:genesis/util/tool/timer.hpp line:101
        cl.def(
            "resume",
            ( void( genesis::util::Timer::* )() ) & genesis::util::Timer::resume,
            "C++: genesis::util::Timer::resume() --> void"
        );
        // function-signature: void genesis::util::Timer::reset()() file:genesis/util/tool/timer.hpp
        // line:106
        cl.def(
            "reset",
            ( void( genesis::util::Timer::* )() ) & genesis::util::Timer::reset,
            "C++: genesis::util::Timer::reset() --> void"
        );
        // function-signature: void genesis::util::Timer::restart()()
        // file:genesis/util/tool/timer.hpp line:112
        cl.def(
            "restart",
            ( void( genesis::util::Timer::* )() ) & genesis::util::Timer::restart,
            "C++: genesis::util::Timer::restart() --> void"
        );
        // function-signature: double genesis::util::Timer::elapsed() const()
        // file:genesis/util/tool/timer.hpp line:123
        cl.def(
            "elapsed",
            ( double( genesis::util::Timer::* )() const ) & genesis::util::Timer::elapsed,
            "Return elapsed time in seconds.\n\n The timer keeps running, so this is essentially a "
            "\"lap\" function.\n\nC++: genesis::util::Timer::elapsed() const --> double"
        );
        // function-signature: double genesis::util::Timer::elapsed_and_restart()()
        // file:genesis/util/tool/timer.hpp line:152
        cl.def(
            "elapsed_and_restart",
            ( double( genesis::util::Timer::* )() ) & genesis::util::Timer::elapsed_and_restart,
            "Return the elapsed time in seconds, and reset the timer.\n\nC++: "
            "genesis::util::Timer::elapsed_and_restart() --> double"
        );
        // function-signature: double genesis::util::Timer::elapsed_and_stop()()
        // file:genesis/util/tool/timer.hpp line:162
        cl.def(
            "elapsed_and_stop",
            ( double( genesis::util::Timer::* )() ) & genesis::util::Timer::elapsed_and_stop,
            "Return the elapsed time in seconds, and stop the timer.\n\nC++: "
            "genesis::util::Timer::elapsed_and_stop() --> double"
        );
    }
}

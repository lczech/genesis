#include <genesis/utils/tools/date_time.hpp>
#include <iterator>
#include <memory>
#include <string>

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

void bind_genesis_utils_tools_date_time(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    // genesis::utils::current_date() file:genesis/utils/tools/date_time.hpp line:48
    // function-signature: std::string genesis::utils::current_date()()
    // file:genesis/utils/tools/date_time.hpp line:48
    M( "genesis::utils" )
        .def(
            "current_date",
            ( std::string( * )() ) & genesis::utils::current_date,
            "Returns the current date as a string in the format \"2014-12-31\".\n\nC++: "
            "genesis::utils::current_date() --> std::string"
        );

    // genesis::utils::current_time() file:genesis/utils/tools/date_time.hpp line:53
    // function-signature: std::string genesis::utils::current_time()()
    // file:genesis/utils/tools/date_time.hpp line:53
    M( "genesis::utils" )
        .def(
            "current_time",
            ( std::string( * )() ) & genesis::utils::current_time,
            "Returns the current time as a string in the format \"13:37:42\".\n\nC++: "
            "genesis::utils::current_time() --> std::string"
        );
}

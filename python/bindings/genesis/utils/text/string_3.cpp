#include <genesis/utils/text/string.hpp>
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

void bind_genesis_utils_text_string_3(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    // genesis::utils::text::to_string_byte_format(unsigned long) file:genesis/utils/text/string.hpp
    // line:883 function-signature: std::string genesis::utils::text::to_string_byte_format(unsigned
    // long)(unsigned long) file:genesis/utils/text/string.hpp line:883
    M( "genesis::utils::text" )
        .def(
            "to_string_byte_format",
            ( std::string( * )( unsigned long ) ) & genesis::utils::text::to_string_byte_format,
            "Produce a human readable formatting of a size in bytes, using the appropriate "
            "suffix.\n\n For instance, this function produces `123MB`.\n\nC++: "
            "genesis::utils::text::to_string_byte_format(unsigned long) --> std::string",
            pybind11::arg( "value" )
        );
}

#include <genesis/utils/io/base_input_source.hpp>
#include <genesis/utils/io/base_output_target.hpp>
#include <genesis/utils/io/function.hpp>
#include <genesis/utils/io/input_source_add_ons.hpp>
#include <genesis/utils/io/output_target_add_ons.hpp>
#include <iterator>
#include <memory>
#include <ostream>
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

void bind_genesis_utils_io_function(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    // genesis::utils::io::read_input_source(class std::shared_ptr<class
    // genesis::utils::io::BaseInputSource>) file:genesis/utils/io/function.hpp line:54
    // function-signature: std::string genesis::utils::io::read_input_source(class
    // std::shared_ptr<class genesis::utils::io::BaseInputSource>)(class std::shared_ptr<class
    // genesis::utils::io::BaseInputSource>) file:genesis/utils/io/function.hpp line:54
    M( "genesis::utils::io" )
        .def(
            "read_input_source",
            ( std::string( * )( class std::shared_ptr<class genesis::utils::io::BaseInputSource> )
            ) & genesis::utils::io::read_input_source,
            "Return the contents of an input  as a string.\n\n This simply reads all characters "
            "from the  into a `std::string`. It is similar to\n file_read(), but takes the more "
            "general BaseInputSource as input.\n\nC++: genesis::utils::io::read_input_source(class "
            "std::shared_ptr<class genesis::utils::io::BaseInputSource>) --> std::string",
            pybind11::arg( "source" )
        );

    // genesis::utils::io::read_input_source_lines(class std::shared_ptr<class
    // genesis::utils::io::BaseInputSource>) file:genesis/utils/io/function.hpp line:64
    // function-signature: class std::vector<std::string >
    // genesis::utils::io::read_input_source_lines(class std::shared_ptr<class
    // genesis::utils::io::BaseInputSource>)(class std::shared_ptr<class
    // genesis::utils::io::BaseInputSource>) file:genesis/utils/io/function.hpp line:64
    M( "genesis::utils::io" )
        .def(
            "read_input_source_lines",
            ( class std::vector<std::string >( * )( class std::shared_ptr<
                                                    class genesis::utils::io::BaseInputSource> ) ) &
                genesis::utils::io::read_input_source_lines,
            "Return the contents of an input  as a string, split into individual lines.\n\n This "
            "simply reads all lines from the  into a `std::string`. It is similar to\n "
            "file_read_lines(), but takes the more general BaseInputSource as input.\n\nC++: "
            "genesis::utils::io::read_input_source_lines(class std::shared_ptr<class "
            "genesis::utils::io::BaseInputSource>) --> class std::vector<std::string >",
            pybind11::arg( "source" )
        );

    // genesis::utils::io::write_output_target(const std::string &, class std::shared_ptr<class
    // genesis::utils::io::BaseOutputTarget>) file:genesis/utils/io/function.hpp line:77
    // function-signature: void genesis::utils::io::write_output_target(const std::string &, class
    // std::shared_ptr<class genesis::utils::io::BaseOutputTarget>)(const std::string &, class
    // std::shared_ptr<class genesis::utils::io::BaseOutputTarget>)
    // file:genesis/utils/io/function.hpp line:77
    M( "genesis::utils::io" )
        .def(
            "write_output_target",
            ( void ( * )( const std::string&, class std::shared_ptr<class genesis::utils::io::BaseOutputTarget> )
            ) & genesis::utils::io::write_output_target,
            "Write the contents of a string to an output \n\n This is similar to file_write(), but "
            "takes the more general BaseOutputTarget as output.\n\nC++: "
            "genesis::utils::io::write_output_target(const std::string &, class "
            "std::shared_ptr<class genesis::utils::io::BaseOutputTarget>) --> void",
            pybind11::arg( "content" ),
            pybind11::arg( "target" )
        );
}

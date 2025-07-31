#include <cstdio>
#include <genesis/utils/io/file_input_source.hpp>
#include <genesis/utils/io/input_source_add_ons.hpp>
#include <genesis/utils/io/output_target_add_ons.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
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

void bind_genesis_utils_io_file_input_source(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::utils::io::FileInputSource file:genesis/utils/io/file_input_source.hpp line:61
        pybind11::class_<
            genesis::utils::io::FileInputSource,
            std::shared_ptr<genesis::utils::io::FileInputSource>,
            genesis::utils::io::BaseInputSource>
            cl( M( "genesis::utils::io" ),
                "FileInputSource",
                "Input source for reading byte data from a file.\n\n The input file name is "
                "provided via the constructor.\n\n It is also possible to provide a `FILE` pointer "
                "directly. In this case, the ownership of the file\n pointer is taken by this "
                "class. Thus, closing the file is done when destructing this class." );
        // function-signature: genesis::utils::io::FileInputSource::FileInputSource(const
        // std::string &)(const std::string &) file:genesis/utils/io/file_input_source.hpp line:72
        cl.def( pybind11::init<const std::string&>(), pybind11::arg( "file_name" ) );
    }
}

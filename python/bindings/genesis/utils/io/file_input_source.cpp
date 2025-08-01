#include <cstdio>
#include <genesis/utils/io/base_input_source.hpp>
#include <genesis/utils/io/file_input_source.hpp>
#include <genesis/utils/io/gzip_input_source.hpp>
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
    { // genesis::utils::io::GzipInputSource file:genesis/utils/io/gzip_input_source.hpp line:57
        pybind11::class_<
            genesis::utils::io::GzipInputSource,
            std::shared_ptr<genesis::utils::io::GzipInputSource>,
            genesis::utils::io::BaseInputSource>
            cl( M( "genesis::utils::io" ),
                "GzipInputSource",
                "Input source for reading byte data from a gzip/zlib-compressed source.\n\n This "
                "input source is a wrapper that takes some other input source\n (FileInputSource, "
                "StringInputSource, StreamInputSource, etc),\n and de-compresses using gzip/zlib "
                "on the fly while reading.\n\n The class can be moved, but not copied, because of "
                "the internal state that is kept for\n decompression, and which would mess up the "
                "input source if copied." );
        // function-signature: genesis::utils::io::GzipInputSource::GzipInputSource(class
        // std::shared_ptr<class genesis::utils::io::BaseInputSource>, enum
        // genesis::utils::io::GzipInputSource::Format)(class std::shared_ptr<class
        // genesis::utils::io::BaseInputSource>, enum genesis::utils::io::GzipInputSource::Format)
        // file:genesis/utils/io/gzip_input_source.hpp line:100
        cl.def(
            pybind11::init(
                []( class std::shared_ptr<class genesis::utils::io::BaseInputSource> const& a0 ) {
                    return new genesis::utils::io::GzipInputSource( a0 );
                }
            ),
            "doc",
            pybind11::arg( "input_source" )
        );
        // function-signature: genesis::utils::io::GzipInputSource::GzipInputSource(class
        // std::shared_ptr<class genesis::utils::io::BaseInputSource>, enum
        // genesis::utils::io::GzipInputSource::Format)(class std::shared_ptr<class
        // genesis::utils::io::BaseInputSource>, enum genesis::utils::io::GzipInputSource::Format)
        // file:genesis/utils/io/gzip_input_source.hpp line:100
        cl.def(
            pybind11::init<
                class std::shared_ptr<class genesis::utils::io::BaseInputSource>,
                enum genesis::utils::io::GzipInputSource::Format>(),
            pybind11::arg( "input_source" ),
            pybind11::arg( "format" )
        );

        pybind11::enum_<genesis::utils::io::GzipInputSource::Format>(
            cl, "Format", "Format used by gzip/zlib for decompression."
        )
            .value( "kAutomatic", genesis::utils::io::GzipInputSource::Format::kAutomatic )
            .value( "kGzip", genesis::utils::io::GzipInputSource::Format::kGzip )
            .value( "kZlib", genesis::utils::io::GzipInputSource::Format::kZlib )
            .value( "kDeflate", genesis::utils::io::GzipInputSource::Format::kDeflate );
    }
}

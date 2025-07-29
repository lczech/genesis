#include <genesis/utils/io/gzip_stream.hpp>
#include <genesis/utils/io/input_source_add_ons.hpp>

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

void bind_genesis_utils_io_gzip_stream(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    // genesis::utils::io::GzipCompressionLevel file:genesis/utils/io/gzip_stream.hpp line:100
    pybind11::enum_<genesis::utils::io::GzipCompressionLevel>(
        M( "genesis::utils::io" ),
        "GzipCompressionLevel",
        "List of possible compression levels used for GzipOStream.\n\n The compression levels are "
        "handed over to zlib for compression, which currently allows all values\n between 1 (best "
        "speed) and 9 (best compression), with the special case 0 (no compression), as\n well as "
        "-1 for the default compression. Currently, the zlib default compression level "
        "corresponds\n to level 6, as this is a good compromise between speed and compression\n "
        "(it forms the \"elbow\" of the curve), hence we also use this as our default level.\n\n "
        "The enum only lists those four special levels. However, we use a fixed enum here (with "
        "the\n underlying type `int`), meaning that all values in between 1 and 9 are also allowed "
        "to be used.\n Values outside of the range [-1, 9] will lead to an exception being thrown "
        "when used in GzipOStream.\n\n \n GzipOStream\n \n\n GzipOFStream"
    )
        .value(
            "kDefaultCompression", genesis::utils::io::GzipCompressionLevel::kDefaultCompression
        )
        .value( "kNoCompression", genesis::utils::io::GzipCompressionLevel::kNoCompression )
        .value( "kBestSpeed", genesis::utils::io::GzipCompressionLevel::kBestSpeed )
        .value( "kBestCompression", genesis::utils::io::GzipCompressionLevel::kBestCompression );

    ;
}

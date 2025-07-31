#include <genesis/utils/io/base_input_source.hpp>
#include <genesis/utils/io/base_output_target.hpp>
#include <genesis/utils/io/functions.hpp>
#include <genesis/utils/io/gzip_stream.hpp>
#include <genesis/utils/io/input_source_add_ons.hpp>
#include <genesis/utils/io/output_target.hpp>
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

void bind_genesis_utils_io_output_target(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    // genesis::utils::io::to_gzip_block_file(const std::string &, enum
    // genesis::utils::io::GzipCompressionLevel, std::size_t, bool)
    // file:genesis/utils/io/output_target.hpp line:236 function-signature: class
    // std::shared_ptr<class genesis::utils::io::BaseOutputTarget>
    // genesis::utils::io::to_gzip_block_file(const std::string &, enum
    // genesis::utils::io::GzipCompressionLevel, std::size_t, bool)(const std::string &, enum
    // genesis::utils::io::GzipCompressionLevel, std::size_t, bool)
    // file:genesis/utils/io/output_target.hpp line:236
    M( "genesis::utils::io" )
        .def(
            "to_gzip_block_file",
            []( const std::string& a0
            ) -> std::shared_ptr<class genesis::utils::io::BaseOutputTarget> {
                return genesis::utils::io::to_gzip_block_file( a0 );
            },
            "",
            pybind11::arg( "file_name" )
        );
    M( "genesis::utils::io" )
        .def(
            "to_gzip_block_file",
            []( const std::string& a0, enum genesis::utils::io::GzipCompressionLevel const& a1
            ) -> std::shared_ptr<class genesis::utils::io::BaseOutputTarget> {
                return genesis::utils::io::to_gzip_block_file( a0, a1 );
            },
            "",
            pybind11::arg( "file_name" ),
            pybind11::arg( "compression_level" )
        );
    M( "genesis::utils::io" )
        .def(
            "to_gzip_block_file",
            []( const std::string& a0,
                enum genesis::utils::io::GzipCompressionLevel const& a1,
                std::size_t const& a2
            ) -> std::shared_ptr<class genesis::utils::io::BaseOutputTarget> {
                return genesis::utils::io::to_gzip_block_file( a0, a1, a2 );
            },
            "",
            pybind11::arg( "file_name" ),
            pybind11::arg( "compression_level" ),
            pybind11::arg( "block_size" )
        );
    M( "genesis::utils::io" )
        .def(
            "to_gzip_block_file",
            ( class std::shared_ptr<class genesis::utils::io::BaseOutputTarget>( * )(
                const std::string&, enum genesis::utils::io::GzipCompressionLevel, std::size_t, bool
            ) ) &
                genesis::utils::io::to_gzip_block_file,
            "Obtain an output target for writing gzip block compressed data to a file.\n\n This "
            "output target uses multithreaded gzip compression by block-compressing chunks of "
            "data.\n See GzipBlockOStream for an explanation and more details on this technique "
            "and the parameters\n offered here.\n\n For general details on using this output "
            "target for writing data, see to_file().\n This overload uses the global thread pool "
            "for the compression of blocks.\n See genesis::utils::core::Options for details.\n\n "
            "\n GzipBlockOStream\n \n\n GzipBlockOutputTarget\n \n\n to_file()\n\nC++: "
            "genesis::utils::io::to_gzip_block_file(const std::string &, enum "
            "genesis::utils::io::GzipCompressionLevel, std::size_t, bool) --> class "
            "std::shared_ptr<class genesis::utils::io::BaseOutputTarget>",
            pybind11::arg( "file_name" ),
            pybind11::arg( "compression_level" ),
            pybind11::arg( "block_size" ),
            pybind11::arg( "auto_adjust_filename" )
        );

    // genesis::utils::io::to_stdout() file:genesis/utils/io/output_target.hpp line:311
    // function-signature: class std::shared_ptr<class genesis::utils::io::BaseOutputTarget>
    // genesis::utils::io::to_stdout()() file:genesis/utils/io/output_target.hpp line:311
    M( "genesis::utils::io" )
        .def(
            "to_stdout",
            ( class std::shared_ptr<class genesis::utils::io::BaseOutputTarget>( * )() ) &
                genesis::utils::io::to_stdout,
            "Obtain an output target for writing to standard output (i.e., stdout or cout).\n\n "
            "The output target returned from this function can be used in the writer classes, "
            "e.g.,\n placement::JplaceWriter or sequence::FastaWriter.\n\nC++: "
            "genesis::utils::io::to_stdout() --> class std::shared_ptr<class "
            "genesis::utils::io::BaseOutputTarget>"
        );

    // genesis::utils::io::to_stderr() file:genesis/utils/io/output_target.hpp line:322
    // function-signature: class std::shared_ptr<class genesis::utils::io::BaseOutputTarget>
    // genesis::utils::io::to_stderr()() file:genesis/utils/io/output_target.hpp line:322
    M( "genesis::utils::io" )
        .def(
            "to_stderr",
            ( class std::shared_ptr<class genesis::utils::io::BaseOutputTarget>( * )() ) &
                genesis::utils::io::to_stderr,
            "Obtain an output target for writing to standard error (i.e., stderr or cerr).\n\n The "
            "output target returned from this function can be used in the writer classes, e.g.,\n "
            "placement::JplaceWriter or sequence::FastaWriter.\n\nC++: "
            "genesis::utils::io::to_stderr() --> class std::shared_ptr<class "
            "genesis::utils::io::BaseOutputTarget>"
        );

    // genesis::utils::io::read_input_source(class std::shared_ptr<class
    // genesis::utils::io::BaseInputSource>) file:genesis/utils/io/functions.hpp line:54
    // function-signature: std::string genesis::utils::io::read_input_source(class
    // std::shared_ptr<class genesis::utils::io::BaseInputSource>)(class std::shared_ptr<class
    // genesis::utils::io::BaseInputSource>) file:genesis/utils/io/functions.hpp line:54
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
    // genesis::utils::io::BaseInputSource>) file:genesis/utils/io/functions.hpp line:64
    // function-signature: class std::vector<std::string >
    // genesis::utils::io::read_input_source_lines(class std::shared_ptr<class
    // genesis::utils::io::BaseInputSource>)(class std::shared_ptr<class
    // genesis::utils::io::BaseInputSource>) file:genesis/utils/io/functions.hpp line:64
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
    // genesis::utils::io::BaseOutputTarget>) file:genesis/utils/io/functions.hpp line:77
    // function-signature: void genesis::utils::io::write_output_target(const std::string &, class
    // std::shared_ptr<class genesis::utils::io::BaseOutputTarget>)(const std::string &, class
    // std::shared_ptr<class genesis::utils::io::BaseOutputTarget>)
    // file:genesis/utils/io/functions.hpp line:77
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

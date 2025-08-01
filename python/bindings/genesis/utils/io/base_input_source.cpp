#include <cstdio>
#include <genesis/utils/io/base_input_source.hpp>
#include <genesis/utils/io/base_input_source_add_ons.hpp>
#include <genesis/utils/io/base_output_target.hpp>
#include <genesis/utils/io/base_output_target_add_ons.hpp>
#include <genesis/utils/io/file_input_source.hpp>
#include <genesis/utils/io/file_output_target.hpp>
#include <genesis/utils/io/gzip.hpp>
#include <genesis/utils/io/gzip_input_source.hpp>
#include <genesis/utils/io/gzip_output_target.hpp>
#include <genesis/utils/io/gzip_stream.hpp>
#include <genesis/utils/io/input_source.hpp>
#include <genesis/utils/io/input_source_add_ons.hpp>
#include <genesis/utils/io/output_target_add_ons.hpp>
#include <genesis/utils/io/string_input_source.hpp>
#include <ios>
#include <iterator>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
#include <string>
#include <string_view>
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

void bind_genesis_utils_io_base_input_source(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{

    genesis_utils_io_input_source_add_ons( M( "genesis::utils::io" ) );

    genesis_utils_io_output_target_add_ons( M( "genesis::utils::io" ) );
    { // genesis::utils::io::BaseInputSource file:genesis/utils/io/base_input_source.hpp line:51
        pybind11::class_<
            genesis::utils::io::BaseInputSource,
            std::shared_ptr<genesis::utils::io::BaseInputSource>>
            cl( M( "genesis::utils::io" ),
                "BaseInputSource",
                "Abstract base class for reading byte data from input sources.\n\n It offers to "
                "read() a certain amount of bytes into a char buffer." );
        // function-signature: std::string genesis::utils::io::BaseInputSource::source_name()
        // const() file:genesis/utils/io/base_input_source.hpp line:89
        cl.def(
            "source_name",
            ( std::string( genesis::utils::io::BaseInputSource::* )() const ) &
                genesis::utils::io::BaseInputSource::source_name,
            "Get a name of the input source. This is intended for user output.\n\nC++: "
            "genesis::utils::io::BaseInputSource::source_name() const --> std::string"
        );
        // function-signature: std::string genesis::utils::io::BaseInputSource::source_string()
        // const() file:genesis/utils/io/base_input_source.hpp line:99
        cl.def(
            "source_string",
            ( std::string( genesis::utils::io::BaseInputSource::* )() const ) &
                genesis::utils::io::BaseInputSource::source_string,
            "Get a string representing the input source. This is intended for the reader "
            "classes,\n which for example might want to examine the input file name.\n\nC++: "
            "genesis::utils::io::BaseInputSource::source_string() const --> std::string"
        );
        // function-signature: bool genesis::utils::io::BaseInputSource::is_trivial() const()
        // file:genesis/utils/io/base_input_source.hpp line:117
        cl.def(
            "is_trivial",
            ( bool( genesis::utils::io::BaseInputSource::* )() const ) &
                genesis::utils::io::BaseInputSource::is_trivial,
            "Return whether reading from this source is trivial, such as reading from a file,\n or "
            "involves more complex operations, such as reading from a compressed input that "
            "needs\n to be decompressed first.\n\n This helps downstream processes to decide if "
            "reading can be done in a cheap async thread,\n or needs to use the global thread pool "
            "in order to not oversubscribe the system when many\n such inputs (e.g., many gzipped "
            "files) are read in parallel. A thread that is just reading\n a block of data from a "
            "file is not using computation, and will spend its time in I/O wait,\n so it's okay to "
            "have many of those. However, compressed files need computation, and are\n hence not "
            "trivial.\n\nC++: genesis::utils::io::BaseInputSource::is_trivial() const --> bool"
        );

        genesis_utils_io_base_input_source_add_ons( cl );
    }
    { // genesis::utils::io::BaseOutputTarget file:genesis/utils/io/base_output_target.hpp line:60
        pybind11::class_<
            genesis::utils::io::BaseOutputTarget,
            std::shared_ptr<genesis::utils::io::BaseOutputTarget>>
            cl( M( "genesis::utils::io" ),
                "BaseOutputTarget",
                "Abstract base class for writing data to an output target.\n\n The class is an "
                "interface that allows writing to different targets, and adds a layer of "
                "abstraction\n around using simple `std::ostream` functionality. In particular, we "
                "want to add some checks,\n naming of the streams, etc. Internally however, the "
                "derived classes of this base class use\n `std::ostream`, and make it "
                "accessible.\n\n \n FileOutputTarget, GzipOutputTarget, StreamOutputTarget, "
                "StringOutputTarget for our derived\n output target classes.\n \n\n to_file(), "
                "to_gzip_block_file(), to_stream(), to_string() for helper functions to create\n "
                "these classes, to add some syntactic sugar and make it easy to use." );
        // function-signature: void genesis::utils::io::BaseOutputTarget::flush()()
        // file:genesis/utils/io/base_output_target.hpp line:118
        cl.def(
            "flush",
            ( void( genesis::utils::io::BaseOutputTarget::* )() ) &
                genesis::utils::io::BaseOutputTarget::flush,
            "Flush output stream buffer.\n\n Internally, the different output target derived "
            "classes use some variant of `std::ostream`\n to do the writing. Hence, the target "
            "might need flushing in cases where we want\n to synchronize it while writing, before "
            "closing the stream.\n\nC++: genesis::utils::io::BaseOutputTarget::flush() --> void"
        );
        // function-signature: std::string genesis::utils::io::BaseOutputTarget::target_name()
        // const() file:genesis/utils/io/base_output_target.hpp line:134
        cl.def(
            "target_name",
            ( std::string( genesis::utils::io::BaseOutputTarget::* )() const ) &
                genesis::utils::io::BaseOutputTarget::target_name,
            "Get a name of the output target. This is intended for user output.\n\n This will for "
            "example return something like \"output file (/path/to/file.txt)\", so that\n users "
            "know what type of output stream it is, and where it streams to.\n\nC++: "
            "genesis::utils::io::BaseOutputTarget::target_name() const --> std::string"
        );
        // function-signature: std::string genesis::utils::io::BaseOutputTarget::target_string()
        // const() file:genesis/utils/io/base_output_target.hpp line:146
        cl.def(
            "target_string",
            ( std::string( genesis::utils::io::BaseOutputTarget::* )() const ) &
                genesis::utils::io::BaseOutputTarget::target_string,
            "Get a string representing the output target.\n\n This is intended for the writer "
            "classes, which for example might want to examine the output\n file name. Hence, this "
            "function is meant to return just the file path (for a file target).\n\nC++: "
            "genesis::utils::io::BaseOutputTarget::target_string() const --> std::string"
        );

        genesis_utils_io_base_output_target_add_ons( cl );
    }
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
    // genesis::utils::io::is_gzip_compressed_file(const std::string &)
    // file:genesis/utils/io/gzip.hpp line:54 function-signature: bool
    // genesis::utils::io::is_gzip_compressed_file(const std::string &)(const std::string &)
    // file:genesis/utils/io/gzip.hpp line:54
    M( "genesis::utils::io" )
        .def(
            "is_gzip_compressed_file",
            ( bool ( * )( const std::string& ) ) & genesis::utils::io::is_gzip_compressed_file,
            "Return whether a given file is gzip-compressed.\n\n Returns true only iff the file "
            "exists and is gzip-compressed.\n If there is a mismatch between the gzip magic bytes "
            "and the file ending, a warning is issued\n via LOG_WARN.\n\nC++: "
            "genesis::utils::io::is_gzip_compressed_file(const std::string &) --> bool",
            pybind11::arg( "file_name" )
        );

    { // genesis::utils::io::StringInputSource file:genesis/utils/io/string_input_source.hpp line:60
        pybind11::class_<
            genesis::utils::io::StringInputSource,
            std::shared_ptr<genesis::utils::io::StringInputSource>,
            genesis::utils::io::BaseInputSource>
            cl( M( "genesis::utils::io" ),
                "StringInputSource",
                "Input source for reading byte data from a string.\n\n The input string is "
                "provided via the constructor. Unless the `std::string&&` constructor is used,\n "
                "the input is not owned by this class, thus the owner must keep it alive as long "
                "as reading\n from it is required, and is responsbile for destroying it. This "
                "class merely keeps a pointer.\n\n That also implies that the string shall not be "
                "modified while this input source is used,\n and thus, only const-members of the "
                "string shall be called." );
        // function-signature: genesis::utils::io::StringInputSource::StringInputSource(const
        // std::string &)(const std::string &) file:genesis/utils/io/string_input_source.hpp line:81
        cl.def( pybind11::init<const std::string&>(), pybind11::arg( "str" ) );
    }
    // genesis::utils::io::from_files(const class std::vector<std::string > &, bool)
    // file:genesis/utils/io/input_source.hpp line:97 function-signature: class std::vector<class
    // std::shared_ptr<class genesis::utils::io::BaseInputSource> >
    // genesis::utils::io::from_files(const class std::vector<std::string > &, bool)(const class
    // std::vector<std::string > &, bool) file:genesis/utils/io/input_source.hpp line:97
    M( "genesis::utils::io" )
        .def(
            "from_files",
            []( const class std::vector<std::string >& a0
            ) -> std::vector<class std::shared_ptr<class genesis::utils::io::BaseInputSource> > {
                return genesis::utils::io::from_files( a0 );
            },
            "",
            pybind11::arg( "file_names" )
        );
    M( "genesis::utils::io" )
        .def(
            "from_files",
            ( class std::vector<
                class std::shared_ptr<class genesis::utils::io::BaseInputSource> >( * )(
                const class std::vector<std::string >&, bool
            ) ) &
                genesis::utils::io::from_files,
            "Obtain a set of input sources for reading from files.\n\n See from_file() for "
            "details. This version returnes multiple input sources, which can be used\n for "
            "parallely reading from multiple files for speedup.\n This function can for example be "
            "used with the output of genesis::utils::dir_list_files().\n\n \n See from_file(), "
            "from_string(), from_strings(), and from_stream() for similar\n helper functions for "
            "other types of input sources.\n\nC++: genesis::utils::io::from_files(const class "
            "std::vector<std::string > &, bool) --> class std::vector<class std::shared_ptr<class "
            "genesis::utils::io::BaseInputSource> >",
            pybind11::arg( "file_names" ),
            pybind11::arg( "detect_compression" )
        );

    // genesis::utils::io::from_strings(const class std::vector<std::string > &)
    // file:genesis/utils/io/input_source.hpp line:217 function-signature: class std::vector<class
    // std::shared_ptr<class genesis::utils::io::BaseInputSource> >
    // genesis::utils::io::from_strings(const class std::vector<std::string > &)(const class
    // std::vector<std::string > &) file:genesis/utils/io/input_source.hpp line:217
    M( "genesis::utils::io" )
        .def(
            "from_strings",
            ( class std::vector<class std::shared_ptr<
                  class genesis::utils::io::BaseInputSource> >( * )( const class std::
                                                                         vector<std::string >& ) ) &
                genesis::utils::io::from_strings,
            "Obtain a set of input sources for reading from strings.\n\n See from_string() and "
            "from_files() for details.\n\n \n See from_file(), from_files(), from_string(), and "
            "from_stream() for similar\n helper functions for other types of input "
            "sources.\n\nC++: genesis::utils::io::from_strings(const class std::vector<std::string "
            "> &) --> class std::vector<class std::shared_ptr<class "
            "genesis::utils::io::BaseInputSource> >",
            pybind11::arg( "input_strings" )
        );

    // genesis::utils::io::from_stdin() file:genesis/utils/io/input_source.hpp line:278
    // function-signature: class std::shared_ptr<class genesis::utils::io::BaseInputSource>
    // genesis::utils::io::from_stdin()() file:genesis/utils/io/input_source.hpp line:278
    M( "genesis::utils::io" )
        .def(
            "from_stdin",
            ( class std::shared_ptr<class genesis::utils::io::BaseInputSource>( * )() ) &
                genesis::utils::io::from_stdin,
            "Obtain an input source for reading from standard input (i.e., stdin or cin).\n\n The "
            "input source returned from this function can be used in the reader classes, e.g.,\n "
            "placement::JplaceReader or sequence::FastaReader.\n\n \n See from_file(), "
            "from_files(), from_string(), and from_strings() for similar\n helper functions for "
            "other types of input sources, and see from_stream() for the underlying\n stream input "
            "helper function.\n\nC++: genesis::utils::io::from_stdin() --> class "
            "std::shared_ptr<class genesis::utils::io::BaseInputSource>"
        );

    { // genesis::utils::io::FileOutputTarget file:genesis/utils/io/file_output_target.hpp line:54
        pybind11::class_<
            genesis::utils::io::FileOutputTarget,
            std::shared_ptr<genesis::utils::io::FileOutputTarget>,
            genesis::utils::io::BaseOutputTarget>
            cl( M( "genesis::utils::io" ),
                "FileOutputTarget",
                "Output target for writing data to a file.\n\n The output file name is provided "
                "via the constructor." );
        // function-signature: genesis::utils::io::FileOutputTarget::FileOutputTarget(const
        // std::string &)(const std::string &) file:genesis/utils/io/file_output_target.hpp line:65
        cl.def( pybind11::init<const std::string&>(), pybind11::arg( "file_name" ) );
    }
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

    { // genesis::utils::io::GzipOutputTarget file:genesis/utils/io/gzip_output_target.hpp line:62
        pybind11::class_<
            genesis::utils::io::GzipOutputTarget,
            std::shared_ptr<genesis::utils::io::GzipOutputTarget>,
            genesis::utils::io::BaseOutputTarget>
            cl( M( "genesis::utils::io" ),
                "GzipOutputTarget",
                "Output target for writing byte data to a gzip/zlib-compressed target.\n\n This "
                "output target is a wrapper that takes some other output target\n "
                "(FileOutputTarget, StringOutputTarget, StreamOutputTarget, etc),\n and compresses "
                "using the gzip format on the fly while writing to that other target.\n\n The "
                "class can be moved, but not copied, because of the internal state that is kept "
                "for\n compression, and which would mess up the output if copied." );
        // function-signature: genesis::utils::io::GzipOutputTarget::GzipOutputTarget(class
        // std::shared_ptr<class genesis::utils::io::BaseOutputTarget>, enum
        // genesis::utils::io::GzipCompressionLevel)(class std::shared_ptr<class
        // genesis::utils::io::BaseOutputTarget>, enum genesis::utils::io::GzipCompressionLevel)
        // file:genesis/utils/io/gzip_output_target.hpp line:75
        cl.def(
            pybind11::init(
                []( class std::shared_ptr<class genesis::utils::io::BaseOutputTarget> const& a0 ) {
                    return new genesis::utils::io::GzipOutputTarget( a0 );
                }
            ),
            "doc",
            pybind11::arg( "output_target" )
        );
        // function-signature: genesis::utils::io::GzipOutputTarget::GzipOutputTarget(class
        // std::shared_ptr<class genesis::utils::io::BaseOutputTarget>, enum
        // genesis::utils::io::GzipCompressionLevel)(class std::shared_ptr<class
        // genesis::utils::io::BaseOutputTarget>, enum genesis::utils::io::GzipCompressionLevel)
        // file:genesis/utils/io/gzip_output_target.hpp line:75
        cl.def(
            pybind11::init<
                class std::shared_ptr<class genesis::utils::io::BaseOutputTarget>,
                enum genesis::utils::io::GzipCompressionLevel>(),
            pybind11::arg( "output_target" ),
            pybind11::arg( "compression_level" )
        );
    }
}

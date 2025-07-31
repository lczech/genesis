#include <genesis/utils/io/base_input_source.hpp>
#include <genesis/utils/io/base_output_target.hpp>
#include <genesis/utils/io/file_output_target.hpp>
#include <genesis/utils/io/gzip.hpp>
#include <genesis/utils/io/gzip_input_source.hpp>
#include <genesis/utils/io/gzip_output_target.hpp>
#include <genesis/utils/io/gzip_stream.hpp>
#include <genesis/utils/io/input_source.hpp>
#include <genesis/utils/io/input_source_add_ons.hpp>
#include <genesis/utils/io/output_target.hpp>
#include <genesis/utils/io/output_target_add_ons.hpp>
#include <genesis/utils/io/string_input_source.hpp>
#include <genesis/utils/io/string_output_target.hpp>
#include <genesis/utils/threading/thread_pool.hpp>
#include <ios>
#include <iterator>
#include <memory>
#include <ostream>
#include <sstream> // __str__
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

void bind_genesis_utils_io_file_output_target(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
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
    { // genesis::utils::io::GzipBlockOutputTarget file:genesis/utils/io/gzip_output_target.hpp
      // line:188
        pybind11::class_<
            genesis::utils::io::GzipBlockOutputTarget,
            std::shared_ptr<genesis::utils::io::GzipBlockOutputTarget>,
            genesis::utils::io::BaseOutputTarget>
            cl( M( "genesis::utils::io" ),
                "GzipBlockOutputTarget",
                "Output target for writing byte data to a gzip-compressed target in blocks of gzip "
                "data.\n\n This output target is a wrapper that takes some other output target\n "
                "(FileOutputTarget, StringOutputTarget, StreamOutputTarget, etc),\n and compresses "
                "using the gzip format on the fly while writing to that other target.\n\n Using "
                "gzip blocks allows us to compress in parallel using multiple threads. By default, "
                "if no\n `thread_pool` is provided, we use the global pool of "
                "Options::get().global_thread_pool().\n Furthermore, using gzip blocks should "
                "allow for downstream indexing and random access into the\n compressed file, "
                "similar to VCF tabix indexing, although we currently have not tested this.\n See "
                "the GzipBlockOStream class for details on gzip block compression.\n\n The class "
                "can be moved, but not copied, because of the internal state that is kept for\n "
                "compression, and which would mess up the output if copied.\n\n \n "
                "GzipBlockOStream\n \n\n to_gzip_block_file" );
    }
    { // genesis::utils::io::StringOutputTarget file:genesis/utils/io/string_output_target.hpp
      // line:69
        pybind11::class_<
            genesis::utils::io::StringOutputTarget,
            std::shared_ptr<genesis::utils::io::StringOutputTarget>,
            genesis::utils::io::BaseOutputTarget>
            cl( M( "genesis::utils::io" ),
                "StringOutputTarget",
                "Output target for writing data to a string.\n\n The target string to write to can "
                "be specified when constructing this class. In that case,\n it has to stay alive "
                "for the duration of the data writing process where this class is used.\n "
                "Internally, the data is buffered in a stringstream, and only written to the "
                "string\n on destruction of this class.\n\n     std::string target;\n     "
                "write_output_target( content, to_string( target ));\n     // use target\n\n "
                "Alternatively, when no target string is specified upon construction, the written "
                "results can\n be obtained later via the get() method. We use the io::to_string() "
                "method here, which returns\n a `std::shared_ptr` to the base class "
                "BaseOutputTarget, so usage is a bit tricky:\n\n     auto target = to_string();\n  "
                "   write_output_target( content, target );\n     // use "
                "static_cast<StringOutputTarget*>( target.get() )->get()\n\n See BaseOutputTarget "
                "for more information on the functionality." );
        // function-signature: genesis::utils::io::StringOutputTarget::StringOutputTarget()()
        // file:genesis/utils/io/string_output_target.hpp line:80
        cl.def( pybind11::init( []() { return new genesis::utils::io::StringOutputTarget(); } ) );
        // function-signature:
        // genesis::utils::io::StringOutputTarget::StringOutputTarget(std::string &)(std::string &)
        // file:genesis/utils/io/string_output_target.hpp line:86
        cl.def( pybind11::init<std::string&>(), pybind11::arg( "target" ) );

        // function-signature: std::string genesis::utils::io::StringOutputTarget::get() const()
        // file:genesis/utils/io/string_output_target.hpp line:111
        cl.def(
            "get",
            ( std::string( genesis::utils::io::StringOutputTarget::* )() const ) &
                genesis::utils::io::StringOutputTarget::get,
            "Get the string result.\n\nC++: genesis::utils::io::StringOutputTarget::get() const "
            "--> std::string"
        );
    }
    // genesis::utils::io::to_gzip_file(const std::string &, enum
    // genesis::utils::io::GzipCompressionLevel) file:genesis/utils/io/output_target.hpp line:154
    // function-signature: class std::shared_ptr<class genesis::utils::io::BaseOutputTarget>
    // genesis::utils::io::to_gzip_file(const std::string &, enum
    // genesis::utils::io::GzipCompressionLevel)(const std::string &, enum
    // genesis::utils::io::GzipCompressionLevel) file:genesis/utils/io/output_target.hpp line:154
    M( "genesis::utils::io" )
        .def(
            "to_gzip_file",
            ( class std::shared_ptr<class genesis::utils::io::BaseOutputTarget>( * )(
                const std::string&, enum genesis::utils::io::GzipCompressionLevel
            ) ) &
                genesis::utils::io::to_gzip_file,
            "Obtain an output target for writing to a gzip-compressed file.\n\n The output target "
            "returned from this function can be used in the writer classes, e.g.,\n "
            "placement::JplaceWriter or sequence::FastaWriter.\n\n \n to_file(), this is a wrapper "
            "that automatically sets GzipCompressionLevel::kDefaultCompression\n\nC++: "
            "genesis::utils::io::to_gzip_file(const std::string &, enum "
            "genesis::utils::io::GzipCompressionLevel) --> class std::shared_ptr<class "
            "genesis::utils::io::BaseOutputTarget>",
            pybind11::arg( "file_name" ),
            pybind11::arg( "compression_level" )
        );

    // genesis::utils::io::to_gzip_file(const std::string &) file:genesis/utils/io/output_target.hpp
    // line:169 function-signature: class std::shared_ptr<class
    // genesis::utils::io::BaseOutputTarget> genesis::utils::io::to_gzip_file(const std::string
    // &)(const std::string &) file:genesis/utils/io/output_target.hpp line:169
    M( "genesis::utils::io" )
        .def(
            "to_gzip_file",
            ( class std::shared_ptr<
                class genesis::utils::io::BaseOutputTarget>( * )( const std::string& ) ) &
                genesis::utils::io::to_gzip_file,
            "Obtain an output target for writing to a gzip-compressed file.\n\n The output target "
            "returned from this function can be used in the writer classes, e.g.,\n "
            "placement::JplaceWriter or sequence::FastaWriter.\n\n \n to_file(), this is a wrapper "
            "that automatically sets GzipCompressionLevel::kDefaultCompression\n\nC++: "
            "genesis::utils::io::to_gzip_file(const std::string &) --> class std::shared_ptr<class "
            "genesis::utils::io::BaseOutputTarget>",
            pybind11::arg( "file_name" )
        );
}

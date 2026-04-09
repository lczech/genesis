#include <genesis/util/io/base_output_target.hpp>
#include <genesis/util/io/base_output_target_add_ons.hpp>
#include <genesis/util/io/file_output_target.hpp>
#include <genesis/util/io/gzip_output_target.hpp>
#include <genesis/util/io/gzip_stream.hpp>
#include <genesis/util/io/input_source_add_ons.hpp>
#include <genesis/util/io/output_target.hpp>
#include <genesis/util/io/output_target_add_ons.hpp>
#include <genesis/util/io/string_output_target.hpp>
#include <genesis/util/threading/thread_pool.hpp>
#include <ios>
#include <iterator>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
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

void bind_genesis_util_io_base_output_target(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::util::io::BaseOutputTarget file:genesis/util/io/base_output_target.hpp line:60
        pybind11::class_<
            genesis::util::io::BaseOutputTarget,
            std::shared_ptr<genesis::util::io::BaseOutputTarget>>
            cl( M( "genesis::util::io" ),
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
        // function-signature: void genesis::util::io::BaseOutputTarget::flush()()
        // file:genesis/util/io/base_output_target.hpp line:118
        cl.def(
            "flush",
            ( void( genesis::util::io::BaseOutputTarget::* )() ) &
                genesis::util::io::BaseOutputTarget::flush,
            "Flush output stream buffer.\n\n Internally, the different output target derived "
            "classes use some variant of `std::ostream`\n to do the writing. Hence, the target "
            "might need flushing in cases where we want\n to synchronize it while writing, before "
            "closing the stream.\n\nC++: genesis::util::io::BaseOutputTarget::flush() --> void"
        );
        // function-signature: std::string genesis::util::io::BaseOutputTarget::target_name()
        // const() file:genesis/util/io/base_output_target.hpp line:134
        cl.def(
            "target_name",
            ( std::string( genesis::util::io::BaseOutputTarget::* )() const ) &
                genesis::util::io::BaseOutputTarget::target_name,
            "Get a name of the output target. This is intended for user output.\n\n This will for "
            "example return something like \"output file (/path/to/file.txt)\", so that\n users "
            "know what type of output stream it is, and where it streams to.\n\nC++: "
            "genesis::util::io::BaseOutputTarget::target_name() const --> std::string"
        );
        // function-signature: std::string genesis::util::io::BaseOutputTarget::target_string()
        // const() file:genesis/util/io/base_output_target.hpp line:146
        cl.def(
            "target_string",
            ( std::string( genesis::util::io::BaseOutputTarget::* )() const ) &
                genesis::util::io::BaseOutputTarget::target_string,
            "Get a string representing the output target.\n\n This is intended for the writer "
            "classes, which for example might want to examine the output\n file name. Hence, this "
            "function is meant to return just the file path (for a file target).\n\nC++: "
            "genesis::util::io::BaseOutputTarget::target_string() const --> std::string"
        );

        genesis_util_io_base_output_target_add_ons( cl );
    }
    { // genesis::util::io::FileOutputTarget file:genesis/util/io/file_output_target.hpp line:54
        pybind11::class_<
            genesis::util::io::FileOutputTarget,
            std::shared_ptr<genesis::util::io::FileOutputTarget>,
            genesis::util::io::BaseOutputTarget>
            cl( M( "genesis::util::io" ),
                "FileOutputTarget",
                "Output target for writing data to a file.\n\n The output file name is provided "
                "via the constructor." );
        // function-signature: genesis::util::io::FileOutputTarget::FileOutputTarget(const
        // std::string &)(const std::string &) file:genesis/util/io/file_output_target.hpp line:65
        cl.def( pybind11::init<const std::string&>(), pybind11::arg( "file_name" ) );
    }
    // genesis::util::io::GzipCompressionLevel file:genesis/util/io/gzip_stream.hpp line:100
    pybind11::enum_<genesis::util::io::GzipCompressionLevel>(
        M( "genesis::util::io" ),
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
            "kDefaultCompression", genesis::util::io::GzipCompressionLevel::kDefaultCompression
        )
        .value( "kNoCompression", genesis::util::io::GzipCompressionLevel::kNoCompression )
        .value( "kBestSpeed", genesis::util::io::GzipCompressionLevel::kBestSpeed )
        .value( "kBestCompression", genesis::util::io::GzipCompressionLevel::kBestCompression );

    ;

    { // genesis::util::io::GzipOutputTarget file:genesis/util/io/gzip_output_target.hpp line:62
        pybind11::class_<
            genesis::util::io::GzipOutputTarget,
            std::shared_ptr<genesis::util::io::GzipOutputTarget>,
            genesis::util::io::BaseOutputTarget>
            cl( M( "genesis::util::io" ),
                "GzipOutputTarget",
                "Output target for writing byte data to a gzip/zlib-compressed target.\n\n This "
                "output target is a wrapper that takes some other output target\n "
                "(FileOutputTarget, StringOutputTarget, StreamOutputTarget, etc),\n and compresses "
                "using the gzip format on the fly while writing to that other target.\n\n The "
                "class can be moved, but not copied, because of the internal state that is kept "
                "for\n compression, and which would mess up the output if copied." );
        // function-signature: genesis::util::io::GzipOutputTarget::GzipOutputTarget(class
        // std::shared_ptr<class genesis::util::io::BaseOutputTarget>, enum
        // genesis::util::io::GzipCompressionLevel)(class std::shared_ptr<class
        // genesis::util::io::BaseOutputTarget>, enum genesis::util::io::GzipCompressionLevel)
        // file:genesis/util/io/gzip_output_target.hpp line:75
        cl.def(
            pybind11::init(
                []( class std::shared_ptr<class genesis::util::io::BaseOutputTarget> const& a0 ) {
                    return new genesis::util::io::GzipOutputTarget( a0 );
                }
            ),
            "doc",
            pybind11::arg( "output_target" )
        );
        // function-signature: genesis::util::io::GzipOutputTarget::GzipOutputTarget(class
        // std::shared_ptr<class genesis::util::io::BaseOutputTarget>, enum
        // genesis::util::io::GzipCompressionLevel)(class std::shared_ptr<class
        // genesis::util::io::BaseOutputTarget>, enum genesis::util::io::GzipCompressionLevel)
        // file:genesis/util/io/gzip_output_target.hpp line:75
        cl.def(
            pybind11::init<
                class std::shared_ptr<class genesis::util::io::BaseOutputTarget>,
                enum genesis::util::io::GzipCompressionLevel>(),
            pybind11::arg( "output_target" ),
            pybind11::arg( "compression_level" )
        );
    }
    { // genesis::util::io::GzipBlockOutputTarget file:genesis/util/io/gzip_output_target.hpp
      // line:193
        pybind11::class_<
            genesis::util::io::GzipBlockOutputTarget,
            std::shared_ptr<genesis::util::io::GzipBlockOutputTarget>,
            genesis::util::io::BaseOutputTarget>
            cl( M( "genesis::util::io" ),
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
    { // genesis::util::io::StringOutputTarget file:genesis/util/io/string_output_target.hpp line:69
        pybind11::class_<
            genesis::util::io::StringOutputTarget,
            std::shared_ptr<genesis::util::io::StringOutputTarget>,
            genesis::util::io::BaseOutputTarget>
            cl( M( "genesis::util::io" ),
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
        // function-signature: genesis::util::io::StringOutputTarget::StringOutputTarget()()
        // file:genesis/util/io/string_output_target.hpp line:80
        cl.def( pybind11::init( []() { return new genesis::util::io::StringOutputTarget(); } ) );
        // function-signature: genesis::util::io::StringOutputTarget::StringOutputTarget(std::string
        // &)(std::string &) file:genesis/util/io/string_output_target.hpp line:86
        cl.def( pybind11::init<std::string&>(), pybind11::arg( "target" ) );

        // function-signature: std::string genesis::util::io::StringOutputTarget::get() const()
        // file:genesis/util/io/string_output_target.hpp line:111
        cl.def(
            "get",
            ( std::string( genesis::util::io::StringOutputTarget::* )() const ) &
                genesis::util::io::StringOutputTarget::get,
            "Get the string result.\n\nC++: genesis::util::io::StringOutputTarget::get() const --> "
            "std::string"
        );
    }
    // genesis::util::io::to_gzip_file(const std::string &, enum
    // genesis::util::io::GzipCompressionLevel) file:genesis/util/io/output_target.hpp line:154
    // function-signature: class std::shared_ptr<class genesis::util::io::BaseOutputTarget>
    // genesis::util::io::to_gzip_file(const std::string &, enum
    // genesis::util::io::GzipCompressionLevel)(const std::string &, enum
    // genesis::util::io::GzipCompressionLevel) file:genesis/util/io/output_target.hpp line:154
    M( "genesis::util::io" )
        .def(
            "to_gzip_file",
            ( class std::shared_ptr<class genesis::util::io::BaseOutputTarget>( * )(
                const std::string&, enum genesis::util::io::GzipCompressionLevel
            ) ) &
                genesis::util::io::to_gzip_file,
            "Obtain an output target for writing to a gzip-compressed file.\n\n The output target "
            "returned from this function can be used in the writer classes, e.g.,\n "
            "placement::JplaceWriter or sequence::FastaWriter.\n\n \n to_file(), this is a wrapper "
            "that automatically sets GzipCompressionLevel::kDefaultCompression\n\nC++: "
            "genesis::util::io::to_gzip_file(const std::string &, enum "
            "genesis::util::io::GzipCompressionLevel) --> class std::shared_ptr<class "
            "genesis::util::io::BaseOutputTarget>",
            pybind11::arg( "file_name" ),
            pybind11::arg( "compression_level" )
        );

    // genesis::util::io::to_gzip_file(const std::string &) file:genesis/util/io/output_target.hpp
    // line:169 function-signature: class std::shared_ptr<class genesis::util::io::BaseOutputTarget>
    // genesis::util::io::to_gzip_file(const std::string &)(const std::string &)
    // file:genesis/util/io/output_target.hpp line:169
    M( "genesis::util::io" )
        .def(
            "to_gzip_file",
            ( class std::shared_ptr<
                class genesis::util::io::BaseOutputTarget>( * )( const std::string& ) ) &
                genesis::util::io::to_gzip_file,
            "Obtain an output target for writing to a gzip-compressed file.\n\n The output target "
            "returned from this function can be used in the writer classes, e.g.,\n "
            "placement::JplaceWriter or sequence::FastaWriter.\n\n \n to_file(), this is a wrapper "
            "that automatically sets GzipCompressionLevel::kDefaultCompression\n\nC++: "
            "genesis::util::io::to_gzip_file(const std::string &) --> class std::shared_ptr<class "
            "genesis::util::io::BaseOutputTarget>",
            pybind11::arg( "file_name" )
        );

    // genesis::util::io::to_gzip_block_file(const std::string &, enum
    // genesis::util::io::GzipCompressionLevel, std::size_t, bool)
    // file:genesis/util/io/output_target.hpp line:236 function-signature: class
    // std::shared_ptr<class genesis::util::io::BaseOutputTarget>
    // genesis::util::io::to_gzip_block_file(const std::string &, enum
    // genesis::util::io::GzipCompressionLevel, std::size_t, bool)(const std::string &, enum
    // genesis::util::io::GzipCompressionLevel, std::size_t, bool)
    // file:genesis/util/io/output_target.hpp line:236
    M( "genesis::util::io" )
        .def(
            "to_gzip_block_file",
            []( const std::string& a0
            ) -> std::shared_ptr<class genesis::util::io::BaseOutputTarget> {
                return genesis::util::io::to_gzip_block_file( a0 );
            },
            "",
            pybind11::arg( "file_name" )
        );
    M( "genesis::util::io" )
        .def(
            "to_gzip_block_file",
            []( const std::string& a0, enum genesis::util::io::GzipCompressionLevel const& a1
            ) -> std::shared_ptr<class genesis::util::io::BaseOutputTarget> {
                return genesis::util::io::to_gzip_block_file( a0, a1 );
            },
            "",
            pybind11::arg( "file_name" ),
            pybind11::arg( "compression_level" )
        );
    M( "genesis::util::io" )
        .def(
            "to_gzip_block_file",
            []( const std::string& a0,
                enum genesis::util::io::GzipCompressionLevel const& a1,
                std::size_t const& a2
            ) -> std::shared_ptr<class genesis::util::io::BaseOutputTarget> {
                return genesis::util::io::to_gzip_block_file( a0, a1, a2 );
            },
            "",
            pybind11::arg( "file_name" ),
            pybind11::arg( "compression_level" ),
            pybind11::arg( "block_size" )
        );
    M( "genesis::util::io" )
        .def(
            "to_gzip_block_file",
            ( class std::shared_ptr<class genesis::util::io::BaseOutputTarget>( * )(
                const std::string&, enum genesis::util::io::GzipCompressionLevel, std::size_t, bool
            ) ) &
                genesis::util::io::to_gzip_block_file,
            "Obtain an output target for writing gzip block compressed data to a file.\n\n This "
            "output target uses multithreaded gzip compression by block-compressing chunks of "
            "data.\n See GzipBlockOStream for an explanation and more details on this technique "
            "and the parameters\n offered here.\n\n For general details on using this output "
            "target for writing data, see to_file().\n This overload uses the global thread pool "
            "for the compression of blocks.\n See genesis::util::core::Options for details.\n\n \n "
            "GzipBlockOStream\n \n\n GzipBlockOutputTarget\n \n\n to_file()\n\nC++: "
            "genesis::util::io::to_gzip_block_file(const std::string &, enum "
            "genesis::util::io::GzipCompressionLevel, std::size_t, bool) --> class "
            "std::shared_ptr<class genesis::util::io::BaseOutputTarget>",
            pybind11::arg( "file_name" ),
            pybind11::arg( "compression_level" ),
            pybind11::arg( "block_size" ),
            pybind11::arg( "auto_adjust_filename" )
        );

    // genesis::util::io::to_stdout() file:genesis/util/io/output_target.hpp line:311
    // function-signature: class std::shared_ptr<class genesis::util::io::BaseOutputTarget>
    // genesis::util::io::to_stdout()() file:genesis/util/io/output_target.hpp line:311
    M( "genesis::util::io" )
        .def(
            "to_stdout",
            ( class std::shared_ptr<class genesis::util::io::BaseOutputTarget>( * )() ) &
                genesis::util::io::to_stdout,
            "Obtain an output target for writing to standard output (i.e., stdout or cout).\n\n "
            "The output target returned from this function can be used in the writer classes, "
            "e.g.,\n placement::JplaceWriter or sequence::FastaWriter.\n\nC++: "
            "genesis::util::io::to_stdout() --> class std::shared_ptr<class "
            "genesis::util::io::BaseOutputTarget>"
        );

    // genesis::util::io::to_stderr() file:genesis/util/io/output_target.hpp line:322
    // function-signature: class std::shared_ptr<class genesis::util::io::BaseOutputTarget>
    // genesis::util::io::to_stderr()() file:genesis/util/io/output_target.hpp line:322
    M( "genesis::util::io" )
        .def(
            "to_stderr",
            ( class std::shared_ptr<class genesis::util::io::BaseOutputTarget>( * )() ) &
                genesis::util::io::to_stderr,
            "Obtain an output target for writing to standard error (i.e., stderr or cerr).\n\n The "
            "output target returned from this function can be used in the writer classes, e.g.,\n "
            "placement::JplaceWriter or sequence::FastaWriter.\n\nC++: "
            "genesis::util::io::to_stderr() --> class std::shared_ptr<class "
            "genesis::util::io::BaseOutputTarget>"
        );
}

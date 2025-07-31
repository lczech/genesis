#include <genesis/utils/io/base_input_source.hpp>
#include <genesis/utils/io/base_input_source_add_ons.hpp>
#include <genesis/utils/io/base_output_target.hpp>
#include <genesis/utils/io/base_output_target_add_ons.hpp>
#include <genesis/utils/io/input_source_add_ons.hpp>
#include <genesis/utils/io/output_target_add_ons.hpp>
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
}

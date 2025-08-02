#include <genesis/util/io/base_input_source.hpp>
#include <genesis/util/io/gzip.hpp>
#include <genesis/util/io/input_source.hpp>
#include <genesis/util/io/input_source_add_ons.hpp>
#include <genesis/util/io/output_target_add_ons.hpp>
#include <genesis/util/io/string_input_source.hpp>
#include <iterator>
#include <memory>
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

void bind_genesis_util_io_gzip(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    // genesis::util::io::is_gzip_compressed_file(const std::string &) file:genesis/util/io/gzip.hpp
    // line:54 function-signature: bool genesis::util::io::is_gzip_compressed_file(const std::string
    // &)(const std::string &) file:genesis/util/io/gzip.hpp line:54
    M( "genesis::util::io" )
        .def(
            "is_gzip_compressed_file",
            ( bool ( * )( const std::string& ) ) & genesis::util::io::is_gzip_compressed_file,
            "Return whether a given file is gzip-compressed.\n\n Returns true only iff the file "
            "exists and is gzip-compressed.\n If there is a mismatch between the gzip magic bytes "
            "and the file ending, a warning is issued\n via LOG_WARN.\n\nC++: "
            "genesis::util::io::is_gzip_compressed_file(const std::string &) --> bool",
            pybind11::arg( "file_name" )
        );

    { // genesis::util::io::StringInputSource file:genesis/util/io/string_input_source.hpp line:60
        pybind11::class_<
            genesis::util::io::StringInputSource,
            std::shared_ptr<genesis::util::io::StringInputSource>,
            genesis::util::io::BaseInputSource>
            cl( M( "genesis::util::io" ),
                "StringInputSource",
                "Input source for reading byte data from a string.\n\n The input string is "
                "provided via the constructor. Unless the `std::string&&` constructor is used,\n "
                "the input is not owned by this class, thus the owner must keep it alive as long "
                "as reading\n from it is required, and is responsbile for destroying it. This "
                "class merely keeps a pointer.\n\n That also implies that the string shall not be "
                "modified while this input source is used,\n and thus, only const-members of the "
                "string shall be called." );
        // function-signature: genesis::util::io::StringInputSource::StringInputSource(const
        // std::string &)(const std::string &) file:genesis/util/io/string_input_source.hpp line:81
        cl.def( pybind11::init<const std::string&>(), pybind11::arg( "str" ) );
    }
    // genesis::util::io::from_files(const class std::vector<std::string > &, bool)
    // file:genesis/util/io/input_source.hpp line:97 function-signature: class std::vector<class
    // std::shared_ptr<class genesis::util::io::BaseInputSource> >
    // genesis::util::io::from_files(const class std::vector<std::string > &, bool)(const class
    // std::vector<std::string > &, bool) file:genesis/util/io/input_source.hpp line:97
    M( "genesis::util::io" )
        .def(
            "from_files",
            []( const class std::vector<std::string >& a0
            ) -> std::vector<class std::shared_ptr<class genesis::util::io::BaseInputSource> > {
                return genesis::util::io::from_files( a0 );
            },
            "",
            pybind11::arg( "file_names" )
        );
    M( "genesis::util::io" )
        .def(
            "from_files",
            ( class std::vector<
                class std::shared_ptr<class genesis::util::io::BaseInputSource> >( * )(
                const class std::vector<std::string >&, bool
            ) ) &
                genesis::util::io::from_files,
            "Obtain a set of input sources for reading from files.\n\n See from_file() for "
            "details. This version returnes multiple input sources, which can be used\n for "
            "parallely reading from multiple files for speedup.\n This function can for example be "
            "used with the output of genesis::util::dir_list_files().\n\n \n See from_file(), "
            "from_string(), from_strings(), and from_stream() for similar\n helper functions for "
            "other types of input sources.\n\nC++: genesis::util::io::from_files(const class "
            "std::vector<std::string > &, bool) --> class std::vector<class std::shared_ptr<class "
            "genesis::util::io::BaseInputSource> >",
            pybind11::arg( "file_names" ),
            pybind11::arg( "detect_compression" )
        );

    // genesis::util::io::from_strings(const class std::vector<std::string > &)
    // file:genesis/util/io/input_source.hpp line:217 function-signature: class std::vector<class
    // std::shared_ptr<class genesis::util::io::BaseInputSource> >
    // genesis::util::io::from_strings(const class std::vector<std::string > &)(const class
    // std::vector<std::string > &) file:genesis/util/io/input_source.hpp line:217
    M( "genesis::util::io" )
        .def(
            "from_strings",
            ( class std::vector<class std::shared_ptr<
                  class genesis::util::io::BaseInputSource> >( * )( const class std::
                                                                        vector<std::string >& ) ) &
                genesis::util::io::from_strings,
            "Obtain a set of input sources for reading from strings.\n\n See from_string() and "
            "from_files() for details.\n\n \n See from_file(), from_files(), from_string(), and "
            "from_stream() for similar\n helper functions for other types of input "
            "sources.\n\nC++: genesis::util::io::from_strings(const class std::vector<std::string "
            "> &) --> class std::vector<class std::shared_ptr<class "
            "genesis::util::io::BaseInputSource> >",
            pybind11::arg( "input_strings" )
        );

    // genesis::util::io::from_stdin() file:genesis/util/io/input_source.hpp line:278
    // function-signature: class std::shared_ptr<class genesis::util::io::BaseInputSource>
    // genesis::util::io::from_stdin()() file:genesis/util/io/input_source.hpp line:278
    M( "genesis::util::io" )
        .def(
            "from_stdin",
            ( class std::shared_ptr<class genesis::util::io::BaseInputSource>( * )() ) &
                genesis::util::io::from_stdin,
            "Obtain an input source for reading from standard input (i.e., stdin or cin).\n\n The "
            "input source returned from this function can be used in the reader classes, e.g.,\n "
            "placement::JplaceReader or sequence::FastaReader.\n\n \n See from_file(), "
            "from_files(), from_string(), and from_strings() for similar\n helper functions for "
            "other types of input sources, and see from_stream() for the underlying\n stream input "
            "helper function.\n\nC++: genesis::util::io::from_stdin() --> class "
            "std::shared_ptr<class genesis::util::io::BaseInputSource>"
        );
}

#include <cstdio>
#include <genesis/utils/io/base_input_source.hpp>
#include <genesis/utils/io/file_input_source.hpp>
#include <genesis/utils/io/functions.hpp>
#include <genesis/utils/io/gzip.hpp>
#include <genesis/utils/io/gzip_input_source.hpp>
#include <genesis/utils/io/input_source.hpp>
#include <genesis/utils/io/input_source_bind.hpp>
#include <genesis/utils/io/string_input_source.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
#include <string>
#include <string_view>
#include <vector>

#include <functional>
#include <pybind11/pybind11.h>
#include <string>
#include <pybind11/stl.h>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>, false)
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*, false)
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>)
#endif

void bind_genesis_utils_io_file_input_source(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::utils::io::FileInputSource file:genesis/utils/io/file_input_source.hpp line:61
		pybind11::class_<genesis::utils::io::FileInputSource, std::shared_ptr<genesis::utils::io::FileInputSource>, genesis::utils::io::BaseInputSource> cl(M("genesis::utils::io"), "FileInputSource", "Input source for reading byte data from a file.\n\n The input file name is provided via the constructor.\n\n It is also possible to provide a `FILE` pointer directly. In this case, the ownership of the file\n pointer is taken by this class. Thus, closing the file is done when destructing this class.");
		// function-signature: genesis::utils::io::FileInputSource::FileInputSource(const std::string &)(const std::string &) file:genesis/utils/io/file_input_source.hpp line:72
		cl.def( pybind11::init<const std::string &>(), pybind11::arg("file_name") );

		// function-signature: genesis::utils::io::FileInputSource::FileInputSource(const class genesis::utils::io::FileInputSource &)(const class genesis::utils::io::FileInputSource &) file:genesis/utils/io/file_input_source.hpp line:95
		cl.def( pybind11::init( [](genesis::utils::io::FileInputSource const &o){ return new genesis::utils::io::FileInputSource(o); } ) );
		// function-signature: class genesis::utils::io::FileInputSource & genesis::utils::io::FileInputSource::operator=(const class genesis::utils::io::FileInputSource &)(const class genesis::utils::io::FileInputSource &) file:genesis/utils/io/file_input_source.hpp line:98
		cl.def("assign", (class genesis::utils::io::FileInputSource & (genesis::utils::io::FileInputSource::*)(const class genesis::utils::io::FileInputSource &)) &genesis::utils::io::FileInputSource::operator=, "C++: genesis::utils::io::FileInputSource::operator=(const class genesis::utils::io::FileInputSource &) --> class genesis::utils::io::FileInputSource &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	{ // genesis::utils::io::GzipInputSource file:genesis/utils/io/gzip_input_source.hpp line:57
		pybind11::class_<genesis::utils::io::GzipInputSource, std::shared_ptr<genesis::utils::io::GzipInputSource>, genesis::utils::io::BaseInputSource> cl(M("genesis::utils::io"), "GzipInputSource", "Input source for reading byte data from a gzip/zlib-compressed source.\n\n This input source is a wrapper that takes some other input source\n (FileInputSource, StringInputSource, StreamInputSource, etc),\n and de-compresses using gzip/zlib on the fly while reading.\n\n The class can be moved, but not copied, because of the internal state that is kept for\n decompression, and which would mess up the input source if copied.");
		// function-signature: genesis::utils::io::GzipInputSource::GzipInputSource(class std::shared_ptr<class genesis::utils::io::BaseInputSource>, enum genesis::utils::io::GzipInputSource::Format)(class std::shared_ptr<class genesis::utils::io::BaseInputSource>, enum genesis::utils::io::GzipInputSource::Format) file:genesis/utils/io/gzip_input_source.hpp line:100
		cl.def( pybind11::init( [](class std::shared_ptr<class genesis::utils::io::BaseInputSource> const & a0){ return new genesis::utils::io::GzipInputSource(a0); } ), "doc" , pybind11::arg("input_source"));
		// function-signature: genesis::utils::io::GzipInputSource::GzipInputSource(class std::shared_ptr<class genesis::utils::io::BaseInputSource>, enum genesis::utils::io::GzipInputSource::Format)(class std::shared_ptr<class genesis::utils::io::BaseInputSource>, enum genesis::utils::io::GzipInputSource::Format) file:genesis/utils/io/gzip_input_source.hpp line:100
		cl.def( pybind11::init<class std::shared_ptr<class genesis::utils::io::BaseInputSource>, enum genesis::utils::io::GzipInputSource::Format>(), pybind11::arg("input_source"), pybind11::arg("format") );


		pybind11::enum_<genesis::utils::io::GzipInputSource::Format>(cl, "Format", "Format used by gzip/zlib for decompression.")
			.value("kAutomatic", genesis::utils::io::GzipInputSource::Format::kAutomatic)
			.value("kGzip", genesis::utils::io::GzipInputSource::Format::kGzip)
			.value("kZlib", genesis::utils::io::GzipInputSource::Format::kZlib)
			.value("kDeflate", genesis::utils::io::GzipInputSource::Format::kDeflate);

	}
	// genesis::utils::io::is_gzip_compressed_file(const std::string &) file:genesis/utils/io/gzip.hpp line:54
	// function-signature: bool genesis::utils::io::is_gzip_compressed_file(const std::string &)(const std::string &) file:genesis/utils/io/gzip.hpp line:54
	M("genesis::utils::io").def("is_gzip_compressed_file", (bool (*)(const std::string &)) &genesis::utils::io::is_gzip_compressed_file, "Return whether a given file is gzip-compressed.\n\n Returns true only iff the file exists and is gzip-compressed.\n If there is a mismatch between the gzip magic bytes and the file ending, a warning is issued\n via LOG_WARN.\n\nC++: genesis::utils::io::is_gzip_compressed_file(const std::string &) --> bool", pybind11::arg("file_name"));

	{ // genesis::utils::io::StringInputSource file:genesis/utils/io/string_input_source.hpp line:60
		pybind11::class_<genesis::utils::io::StringInputSource, std::shared_ptr<genesis::utils::io::StringInputSource>, genesis::utils::io::BaseInputSource> cl(M("genesis::utils::io"), "StringInputSource", "Input source for reading byte data from a string.\n\n The input string is provided via the constructor. Unless the `std::string&&` constructor is used,\n the input is not owned by this class, thus the owner must keep it alive as long as reading\n from it is required, and is responsbile for destroying it. This class merely keeps a pointer.\n\n That also implies that the string shall not be modified while this input source is used,\n and thus, only const-members of the string shall be called.");
		// function-signature: genesis::utils::io::StringInputSource::StringInputSource(const std::string &)(const std::string &) file:genesis/utils/io/string_input_source.hpp line:81
		cl.def( pybind11::init<const std::string &>(), pybind11::arg("str") );

		// function-signature: genesis::utils::io::StringInputSource::StringInputSource(const class genesis::utils::io::StringInputSource &)(const class genesis::utils::io::StringInputSource &) file:genesis/utils/io/string_input_source.hpp line:109
		cl.def( pybind11::init( [](genesis::utils::io::StringInputSource const &o){ return new genesis::utils::io::StringInputSource(o); } ) );
		// function-signature: class genesis::utils::io::StringInputSource & genesis::utils::io::StringInputSource::operator=(const class genesis::utils::io::StringInputSource &)(const class genesis::utils::io::StringInputSource &) file:genesis/utils/io/string_input_source.hpp line:112
		cl.def("assign", (class genesis::utils::io::StringInputSource & (genesis::utils::io::StringInputSource::*)(const class genesis::utils::io::StringInputSource &)) &genesis::utils::io::StringInputSource::operator=, "C++: genesis::utils::io::StringInputSource::operator=(const class genesis::utils::io::StringInputSource &) --> class genesis::utils::io::StringInputSource &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	// genesis::utils::io::from_files(const class std::vector<std::string > &, bool) file:genesis/utils/io/input_source.hpp line:97
	// function-signature: class std::vector<class std::shared_ptr<class genesis::utils::io::BaseInputSource> > genesis::utils::io::from_files(const class std::vector<std::string > &, bool)(const class std::vector<std::string > &, bool) file:genesis/utils/io/input_source.hpp line:97
	M("genesis::utils::io").def("from_files", [](const class std::vector<std::string > & a0) -> std::vector<class std::shared_ptr<class genesis::utils::io::BaseInputSource> > { return genesis::utils::io::from_files(a0); }, "", pybind11::arg("file_names"));
	M("genesis::utils::io").def("from_files", (class std::vector<class std::shared_ptr<class genesis::utils::io::BaseInputSource> > (*)(const class std::vector<std::string > &, bool)) &genesis::utils::io::from_files, "Obtain a set of input sources for reading from files.\n\n See from_file() for details. This version returnes multiple input sources, which can be used\n for parallely reading from multiple files for speedup.\n This function can for example be used with the output of genesis::utils::dir_list_files().\n\n \n See from_file(), from_string(), from_strings(), and from_stream() for similar\n helper functions for other types of input sources.\n\nC++: genesis::utils::io::from_files(const class std::vector<std::string > &, bool) --> class std::vector<class std::shared_ptr<class genesis::utils::io::BaseInputSource> >", pybind11::arg("file_names"), pybind11::arg("detect_compression"));

	// genesis::utils::io::from_strings(const class std::vector<std::string > &) file:genesis/utils/io/input_source.hpp line:217
	// function-signature: class std::vector<class std::shared_ptr<class genesis::utils::io::BaseInputSource> > genesis::utils::io::from_strings(const class std::vector<std::string > &)(const class std::vector<std::string > &) file:genesis/utils/io/input_source.hpp line:217
	M("genesis::utils::io").def("from_strings", (class std::vector<class std::shared_ptr<class genesis::utils::io::BaseInputSource> > (*)(const class std::vector<std::string > &)) &genesis::utils::io::from_strings, "Obtain a set of input sources for reading from strings.\n\n See from_string() and from_files() for details.\n\n \n See from_file(), from_files(), from_string(), and from_stream() for similar\n helper functions for other types of input sources.\n\nC++: genesis::utils::io::from_strings(const class std::vector<std::string > &) --> class std::vector<class std::shared_ptr<class genesis::utils::io::BaseInputSource> >", pybind11::arg("input_strings"));

	// genesis::utils::io::from_stdin() file:genesis/utils/io/input_source.hpp line:278
	// function-signature: class std::shared_ptr<class genesis::utils::io::BaseInputSource> genesis::utils::io::from_stdin()() file:genesis/utils/io/input_source.hpp line:278
	M("genesis::utils::io").def("from_stdin", (class std::shared_ptr<class genesis::utils::io::BaseInputSource> (*)()) &genesis::utils::io::from_stdin, "Obtain an input source for reading from standard input (i.e., stdin or cin).\n\n The input source returned from this function can be used in the reader classes, e.g.,\n placement::JplaceReader or sequence::FastaReader.\n\n \n See from_file(), from_files(), from_string(), and from_strings() for similar\n helper functions for other types of input sources, and see from_stream() for the underlying\n stream input helper function.\n\nC++: genesis::utils::io::from_stdin() --> class std::shared_ptr<class genesis::utils::io::BaseInputSource>");

	// genesis::utils::io::read_input_source(class std::shared_ptr<class genesis::utils::io::BaseInputSource>) file:genesis/utils/io/functions.hpp line:53
	// function-signature: std::string genesis::utils::io::read_input_source(class std::shared_ptr<class genesis::utils::io::BaseInputSource>)(class std::shared_ptr<class genesis::utils::io::BaseInputSource>) file:genesis/utils/io/functions.hpp line:53
	M("genesis::utils::io").def("read_input_source", (std::string (*)(class std::shared_ptr<class genesis::utils::io::BaseInputSource>)) &genesis::utils::io::read_input_source, "Return the contents of an input  as a string.\n\n This simply reads all characters from the  into a `std::string`. It is similar to\n file_read(), but takes the more general BaseInputSource as input.\n\nC++: genesis::utils::io::read_input_source(class std::shared_ptr<class genesis::utils::io::BaseInputSource>) --> std::string", pybind11::arg("source"));

	// genesis::utils::io::read_input_source_lines(class std::shared_ptr<class genesis::utils::io::BaseInputSource>) file:genesis/utils/io/functions.hpp line:61
	// function-signature: class std::vector<std::string > genesis::utils::io::read_input_source_lines(class std::shared_ptr<class genesis::utils::io::BaseInputSource>)(class std::shared_ptr<class genesis::utils::io::BaseInputSource>) file:genesis/utils/io/functions.hpp line:61
	M("genesis::utils::io").def("read_input_source_lines", (class std::vector<std::string > (*)(class std::shared_ptr<class genesis::utils::io::BaseInputSource>)) &genesis::utils::io::read_input_source_lines, "Return the contents of an input  as a string, split into individual lines.\n\n This simply reads all lines from the  into a `std::string`. It is similar to\n file_read_lines(), but takes the more general BaseInputSource as input.\n\nC++: genesis::utils::io::read_input_source_lines(class std::shared_ptr<class genesis::utils::io::BaseInputSource>) --> class std::vector<std::string >", pybind11::arg("source"));

}

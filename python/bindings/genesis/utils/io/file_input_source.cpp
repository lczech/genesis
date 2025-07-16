#include <cstdio>
#include <custom/utils/extra_bindings.hpp>
#include <genesis/utils/io/base_input_source.hpp>
#include <genesis/utils/io/file_input_source.hpp>
#include <genesis/utils/io/functions.hpp>
#include <genesis/utils/io/gzip.hpp>
#include <genesis/utils/io/gzip_input_source.hpp>
#include <genesis/utils/io/input_source.hpp>
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
	{ // genesis::utils::FileInputSource file:genesis/utils/io/file_input_source.hpp line:60
		pybind11::class_<genesis::utils::FileInputSource, std::shared_ptr<genesis::utils::FileInputSource>, genesis::utils::BaseInputSource> cl(M("genesis::utils"), "FileInputSource", "Input source for reading byte data from a file.\n\n The input file name is provided via the constructor.\n\n It is also possible to provide a `FILE` pointer directly. In this case, the ownership of the file\n pointer is taken by this class. Thus, closing the file is done when destructing this class.");
		// function-signature: genesis::utils::FileInputSource::FileInputSource(const std::string &)(const std::string &) file:genesis/utils/io/file_input_source.hpp line:71
		cl.def( pybind11::init<const std::string &>(), pybind11::arg("file_name") );

		// function-signature: genesis::utils::FileInputSource::FileInputSource(const class genesis::utils::FileInputSource &)(const class genesis::utils::FileInputSource &) file:genesis/utils/io/file_input_source.hpp line:94
		cl.def( pybind11::init( [](genesis::utils::FileInputSource const &o){ return new genesis::utils::FileInputSource(o); } ) );
		// function-signature: class genesis::utils::FileInputSource & genesis::utils::FileInputSource::operator=(const class genesis::utils::FileInputSource &)(const class genesis::utils::FileInputSource &) file:genesis/utils/io/file_input_source.hpp line:97
		cl.def("assign", (class genesis::utils::FileInputSource & (genesis::utils::FileInputSource::*)(const class genesis::utils::FileInputSource &)) &genesis::utils::FileInputSource::operator=, "C++: genesis::utils::FileInputSource::operator=(const class genesis::utils::FileInputSource &) --> class genesis::utils::FileInputSource &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	{ // genesis::utils::GzipInputSource file:genesis/utils/io/gzip_input_source.hpp line:56
		pybind11::class_<genesis::utils::GzipInputSource, std::shared_ptr<genesis::utils::GzipInputSource>, genesis::utils::BaseInputSource> cl(M("genesis::utils"), "GzipInputSource", "Input source for reading byte data from a gzip/zlib-compressed source.\n\n This input source is a wrapper that takes some other input source\n (FileInputSource, StringInputSource, StreamInputSource, etc),\n and de-compresses using gzip/zlib on the fly while reading.\n\n The class can be moved, but not copied, because of the internal state that is kept for\n decompression, and which would mess up the input source if copied.");
		// function-signature: genesis::utils::GzipInputSource::GzipInputSource(class std::shared_ptr<class genesis::utils::BaseInputSource>, enum genesis::utils::GzipInputSource::Format)(class std::shared_ptr<class genesis::utils::BaseInputSource>, enum genesis::utils::GzipInputSource::Format) file:genesis/utils/io/gzip_input_source.hpp line:99
		cl.def( pybind11::init( [](class std::shared_ptr<class genesis::utils::BaseInputSource> const & a0){ return new genesis::utils::GzipInputSource(a0); } ), "doc" , pybind11::arg("input_source"));
		// function-signature: genesis::utils::GzipInputSource::GzipInputSource(class std::shared_ptr<class genesis::utils::BaseInputSource>, enum genesis::utils::GzipInputSource::Format)(class std::shared_ptr<class genesis::utils::BaseInputSource>, enum genesis::utils::GzipInputSource::Format) file:genesis/utils/io/gzip_input_source.hpp line:99
		cl.def( pybind11::init<class std::shared_ptr<class genesis::utils::BaseInputSource>, enum genesis::utils::GzipInputSource::Format>(), pybind11::arg("input_source"), pybind11::arg("format") );


		pybind11::enum_<genesis::utils::GzipInputSource::Format>(cl, "Format", "Format used by gzip/zlib for decompression.")
			.value("kAutomatic", genesis::utils::GzipInputSource::Format::kAutomatic)
			.value("kGzip", genesis::utils::GzipInputSource::Format::kGzip)
			.value("kZlib", genesis::utils::GzipInputSource::Format::kZlib)
			.value("kDeflate", genesis::utils::GzipInputSource::Format::kDeflate);

	}
	// genesis::utils::is_gzip_compressed_file(const std::string &) file:genesis/utils/io/gzip.hpp line:53
	// function-signature: bool genesis::utils::is_gzip_compressed_file(const std::string &)(const std::string &) file:genesis/utils/io/gzip.hpp line:53
	M("genesis::utils").def("is_gzip_compressed_file", (bool (*)(const std::string &)) &genesis::utils::is_gzip_compressed_file, "Return whether a given file is gzip-compressed.\n\n Returns true only iff the file exists and is gzip-compressed.\n If there is a mismatch between the gzip magic bytes and the file ending, a warning is issued\n via LOG_WARN.\n\nC++: genesis::utils::is_gzip_compressed_file(const std::string &) --> bool", pybind11::arg("file_name"));

	{ // genesis::utils::StringInputSource file:genesis/utils/io/string_input_source.hpp line:59
		pybind11::class_<genesis::utils::StringInputSource, std::shared_ptr<genesis::utils::StringInputSource>, genesis::utils::BaseInputSource> cl(M("genesis::utils"), "StringInputSource", "Input source for reading byte data from a string.\n\n The input string is provided via the constructor. Unless the `std::string&&` constructor is used,\n the input is not owned by this class, thus the owner must keep it alive as long as reading\n from it is required, and is responsbile for destroying it. This class merely keeps a pointer.\n\n That also implies that the string shall not be modified while this input source is used,\n and thus, only const-members of the string shall be called.");
		// function-signature: genesis::utils::StringInputSource::StringInputSource(const std::string &)(const std::string &) file:genesis/utils/io/string_input_source.hpp line:80
		cl.def( pybind11::init<const std::string &>(), pybind11::arg("str") );

		// function-signature: genesis::utils::StringInputSource::StringInputSource(const class genesis::utils::StringInputSource &)(const class genesis::utils::StringInputSource &) file:genesis/utils/io/string_input_source.hpp line:108
		cl.def( pybind11::init( [](genesis::utils::StringInputSource const &o){ return new genesis::utils::StringInputSource(o); } ) );
		// function-signature: class genesis::utils::StringInputSource & genesis::utils::StringInputSource::operator=(const class genesis::utils::StringInputSource &)(const class genesis::utils::StringInputSource &) file:genesis/utils/io/string_input_source.hpp line:111
		cl.def("assign", (class genesis::utils::StringInputSource & (genesis::utils::StringInputSource::*)(const class genesis::utils::StringInputSource &)) &genesis::utils::StringInputSource::operator=, "C++: genesis::utils::StringInputSource::operator=(const class genesis::utils::StringInputSource &) --> class genesis::utils::StringInputSource &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	// genesis::utils::from_files(const class std::vector<std::string > &, bool) file:genesis/utils/io/input_source.hpp line:96
	// function-signature: class std::vector<class std::shared_ptr<class genesis::utils::BaseInputSource> > genesis::utils::from_files(const class std::vector<std::string > &, bool)(const class std::vector<std::string > &, bool) file:genesis/utils/io/input_source.hpp line:96
	M("genesis::utils").def("from_files", [](const class std::vector<std::string > & a0) -> std::vector<class std::shared_ptr<class genesis::utils::BaseInputSource> > { return genesis::utils::from_files(a0); }, "", pybind11::arg("file_names"));
	M("genesis::utils").def("from_files", (class std::vector<class std::shared_ptr<class genesis::utils::BaseInputSource> > (*)(const class std::vector<std::string > &, bool)) &genesis::utils::from_files, "Obtain a set of input sources for reading from files.\n\n See from_file() for details. This version returnes multiple input sources, which can be used\n for parallely reading from multiple files for speedup.\n This function can for example be used with the output of utils::dir_list_files().\n\n \n See from_file(), from_string(), from_strings(), and from_stream() for similar\n helper functions for other types of input sources.\n\nC++: genesis::utils::from_files(const class std::vector<std::string > &, bool) --> class std::vector<class std::shared_ptr<class genesis::utils::BaseInputSource> >", pybind11::arg("file_names"), pybind11::arg("detect_compression"));

	// genesis::utils::from_string(const std::string &) file:genesis/utils/io/input_source.hpp line:143
	// function-signature: class std::shared_ptr<class genesis::utils::BaseInputSource> genesis::utils::from_string(const std::string &)(const std::string &) file:genesis/utils/io/input_source.hpp line:143
	M("genesis::utils").def("from_string", (class std::shared_ptr<class genesis::utils::BaseInputSource> (*)(const std::string &)) &genesis::utils::from_string, "Obtain an input source for reading from a `std::string`.\n\n The input source returned from this function can be used in the reader classes, e.g.,\n placement::JplaceReader or sequence::FastaReader.\n\n \n See from_file(), from_files(), from_strings(), and from_stream() for similar\n helper functions for other types of input sources.\n\nC++: genesis::utils::from_string(const std::string &) --> class std::shared_ptr<class genesis::utils::BaseInputSource>", pybind11::arg("input_string"));

	// genesis::utils::from_string(const char *) file:genesis/utils/io/input_source.hpp line:202
	// function-signature: class std::shared_ptr<class genesis::utils::BaseInputSource> genesis::utils::from_string(const char *)(const char *) file:genesis/utils/io/input_source.hpp line:202
	M("genesis::utils").def("from_string", (class std::shared_ptr<class genesis::utils::BaseInputSource> (*)(const char *)) &genesis::utils::from_string, "Obtain an input source for reading from a string literal.\n\n This overload is needed as otherwise, the other overloads of this function are ambiguous\n when called with a string literal.\n\n The input source returned from this function can be used in the reader classes, e.g.,\n placement::JplaceReader or sequence::FastaReader.\n\n \n See from_file(), from_files(), from_strings(), and from_stream() for similar\n helper functions for other types of input sources.\n\nC++: genesis::utils::from_string(const char *) --> class std::shared_ptr<class genesis::utils::BaseInputSource>", pybind11::arg("input_string"));

	// genesis::utils::from_strings(const class std::vector<std::string > &) file:genesis/utils/io/input_source.hpp line:216
	// function-signature: class std::vector<class std::shared_ptr<class genesis::utils::BaseInputSource> > genesis::utils::from_strings(const class std::vector<std::string > &)(const class std::vector<std::string > &) file:genesis/utils/io/input_source.hpp line:216
	M("genesis::utils").def("from_strings", (class std::vector<class std::shared_ptr<class genesis::utils::BaseInputSource> > (*)(const class std::vector<std::string > &)) &genesis::utils::from_strings, "Obtain a set of input sources for reading from strings.\n\n See from_string() and from_files() for details.\n\n \n See from_file(), from_files(), from_string(), and from_stream() for similar\n helper functions for other types of input sources.\n\nC++: genesis::utils::from_strings(const class std::vector<std::string > &) --> class std::vector<class std::shared_ptr<class genesis::utils::BaseInputSource> >", pybind11::arg("input_strings"));

	// genesis::utils::from_stdin() file:genesis/utils/io/input_source.hpp line:277
	// function-signature: class std::shared_ptr<class genesis::utils::BaseInputSource> genesis::utils::from_stdin()() file:genesis/utils/io/input_source.hpp line:277
	M("genesis::utils").def("from_stdin", (class std::shared_ptr<class genesis::utils::BaseInputSource> (*)()) &genesis::utils::from_stdin, "Obtain an input source for reading from standard input (i.e., stdin or cin).\n\n The input source returned from this function can be used in the reader classes, e.g.,\n placement::JplaceReader or sequence::FastaReader.\n\n \n See from_file(), from_files(), from_string(), and from_strings() for similar\n helper functions for other types of input sources, and see from_stream() for the underlying\n stream input helper function.\n\nC++: genesis::utils::from_stdin() --> class std::shared_ptr<class genesis::utils::BaseInputSource>");

	// genesis::utils::read_input_source(class std::shared_ptr<class genesis::utils::BaseInputSource>) file:genesis/utils/io/functions.hpp line:52
	// function-signature: std::string genesis::utils::read_input_source(class std::shared_ptr<class genesis::utils::BaseInputSource>)(class std::shared_ptr<class genesis::utils::BaseInputSource>) file:genesis/utils/io/functions.hpp line:52
	M("genesis::utils").def("read_input_source", (std::string (*)(class std::shared_ptr<class genesis::utils::BaseInputSource>)) &genesis::utils::read_input_source, "Return the contents of an input  as a string.\n\n This simply reads all characters from the  into a `std::string`. It is similar to\n file_read(), but takes the more general BaseInputSource as input.\n\nC++: genesis::utils::read_input_source(class std::shared_ptr<class genesis::utils::BaseInputSource>) --> std::string", pybind11::arg("source"));

	// genesis::utils::read_input_source_lines(class std::shared_ptr<class genesis::utils::BaseInputSource>) file:genesis/utils/io/functions.hpp line:60
	// function-signature: class std::vector<std::string > genesis::utils::read_input_source_lines(class std::shared_ptr<class genesis::utils::BaseInputSource>)(class std::shared_ptr<class genesis::utils::BaseInputSource>) file:genesis/utils/io/functions.hpp line:60
	M("genesis::utils").def("read_input_source_lines", (class std::vector<std::string > (*)(class std::shared_ptr<class genesis::utils::BaseInputSource>)) &genesis::utils::read_input_source_lines, "Return the contents of an input  as a string, split into individual lines.\n\n This simply reads all lines from the  into a `std::string`. It is similar to\n file_read_lines(), but takes the more general BaseInputSource as input.\n\nC++: genesis::utils::read_input_source_lines(class std::shared_ptr<class genesis::utils::BaseInputSource>) --> class std::vector<std::string >", pybind11::arg("source"));

}

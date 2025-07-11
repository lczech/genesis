#include <cstdio>
#include <genesis/utils/core/fs.hpp>
#include <genesis/utils/io/base_input_source.hpp>
#include <genesis/utils/io/file_input_source.hpp>
#include <genesis/utils/io/gzip.hpp>
#include <genesis/utils/io/gzip_input_source.hpp>
#include <genesis/utils/io/stream_input_source.hpp>
#include <genesis/utils/io/string_input_source.hpp>
#include <ios>
#include <istream>
#include <iterator>
#include <memory>
#include <sstream> // __str__
#include <streambuf>
#include <string>

#include <functional>
#include <pybind11/pybind11.h>
#include <string>
#include <pybind11/functional.h>
#include <../python/custom_bindings/extensions/utils/matrix.hpp>
#include <genesis/utils/color/functions.hpp>
#include <../python/custom_bindings/extensions/utils/bitvector.hpp>
#include <../python/custom_bindings/extensions/utils/range.hpp>
#include <../python/custom_bindings/extensions/string_target.hpp>
#include <../python/custom_bindings/extensions/sequence/quality.hpp>
#include <../python/custom_bindings/extensions/sequence/sequence_set.hpp>
#include <../python/custom_bindings/extensions/sequence/sequence_dict.hpp>
#include <../python/custom_bindings/extensions/sequence/fasta_input_iterator.hpp>
#include <../python/custom_bindings/extensions/sequence/fastq_input_iterator.hpp>
#include <../python/custom_bindings/extensions/sequence/reference_genome.hpp>
#include <../python/custom_bindings/extensions/taxonomy/taxopath.hpp>
#include <../python/custom_bindings/extensions/taxonomy/iterator.hpp>
#include <../python/custom_bindings/extensions/tree/tree.hpp>
#include <../python/custom_bindings/extensions/tree/functions_tree.hpp>
#include <../python/custom_bindings/extensions/placement/helper.hpp>
#include <pybind11/stl.h>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>)
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*)
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>)
#endif

// genesis::utils::Exception file: line:53
struct PyCallBack_genesis_utils_Exception : public genesis::utils::Exception {
	using genesis::utils::Exception::Exception;

	const char * what() const throw() override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const genesis::utils::Exception *>(this), "what");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<const char *>::value) {
				static pybind11::detail::override_caster_t<const char *> caster;
				return pybind11::detail::cast_ref<const char *>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<const char *>(std::move(o));
		}
		return Exception::what();
	}
};

// genesis::utils::IOError file: line:79
struct PyCallBack_genesis_utils_IOError : public genesis::utils::IOError {
	using genesis::utils::IOError::IOError;

	const char * what() const throw() override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const genesis::utils::IOError *>(this), "what");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<const char *>::value) {
				static pybind11::detail::override_caster_t<const char *> caster;
				return pybind11::detail::cast_ref<const char *>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<const char *>(std::move(o));
		}
		return Exception::what();
	}
};

// genesis::utils::ExistingFileError file: line:109
struct PyCallBack_genesis_utils_ExistingFileError : public genesis::utils::ExistingFileError {
	using genesis::utils::ExistingFileError::ExistingFileError;

	const char * what() const throw() override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const genesis::utils::ExistingFileError *>(this), "what");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<const char *>::value) {
				static pybind11::detail::override_caster_t<const char *> caster;
				return pybind11::detail::cast_ref<const char *>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<const char *>(std::move(o));
		}
		return Exception::what();
	}
};

// genesis::utils::GzipError file:genesis/utils/io/gzip.hpp line:64
struct PyCallBack_genesis_utils_GzipError : public genesis::utils::GzipError {
	using genesis::utils::GzipError::GzipError;

	const char * what() const throw() override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const genesis::utils::GzipError *>(this), "what");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<const char *>::value) {
				static pybind11::detail::override_caster_t<const char *> caster;
				return pybind11::detail::cast_ref<const char *>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<const char *>(std::move(o));
		}
		return Exception::what();
	}
};

void bind_genesis_utils_core_fs(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::utils::file_filename(const std::string &) file:genesis/utils/core/fs.hpp line:284
	M("genesis::utils").def("file_filename", (std::string (*)(const std::string &)) &genesis::utils::file_filename, "Remove extension if present.\n\n Caveat: Does not remove the path. So, if the filename itself does not contain an extension\n separator \".\", but the path does, this will yield an unwanted result. Call file_basename() first.\n\nC++: genesis::utils::file_filename(const std::string &) --> std::string", pybind11::arg("filename"));

	// genesis::utils::file_extension(const std::string &) file:genesis/utils/core/fs.hpp line:291
	M("genesis::utils").def("file_extension", (std::string (*)(const std::string &)) &genesis::utils::file_extension, "Return the extension name of a file.\n\n Also see file_filename().\n\nC++: genesis::utils::file_extension(const std::string &) --> std::string", pybind11::arg("filename"));

	// genesis::utils::is_valid_filename(const std::string &) file:genesis/utils/core/fs.hpp line:317
	M("genesis::utils").def("is_valid_filename", (bool (*)(const std::string &)) &genesis::utils::is_valid_filename, "Check whether a file name is valid.\n\n Validating filenames depends on the operating system and file system of the disk. Thus, this is\n usually not an easy task. This function only checks some basics and is meant to catch the most\n common problems.\n\n The function is meant to be called on the file name itself, without the directory path leading\n to it. File extensions are allowed. Thus, you might need to call file_basename() before in order\n to get the file name without the path.\n\n Invalid filenames are:\n\n   * Those with spaces at the beginning or end, or only consisting of spaces (or empty).\n   * Those which contain any of the chars `< > : \" \\ / | ? *`.\n   * Those which contain any non-printable character, as determined via isprint().\n\n This might be too converative for some system, or allow too much for others. It however should\n return true for filenames that work on most systems.\n\nC++: genesis::utils::is_valid_filename(const std::string &) --> bool", pybind11::arg("filename"));

	// genesis::utils::sanitize_filename(const std::string &) file:genesis/utils/core/fs.hpp line:340
	M("genesis::utils").def("sanitize_filename", (std::string (*)(const std::string &)) &genesis::utils::sanitize_filename, "Remove or replace all invalid parts of a filename.\n\n Similar to is_valid_filename(), this function is not meant to be an ultimate solution to valid\n filenames. See there for details.\n\n The function is meant to be called on the file name itself, without the directory path leading\n to it. File extensions are allowed. Thus, you might need to call file_basename() before in order\n to get the file name without the path.\n\n This function does the following:\n\n   * All non-printable characters are removed.\n   * Spaces at the beginning and end are removed.\n   * All invalid chars are replaced by an underscore. See is_valid_filename() for a list of those\n     chars.\n\n If after this procedure the filename is empty, an exception is thrown. This is meant to save the\n user from checking this, or from running into trouble when trying to write to this \"file\" -\n because an empty filename will point to a directory name.\n\nC++: genesis::utils::sanitize_filename(const std::string &) --> std::string", pybind11::arg("filename"));

	{ // genesis::utils::BaseInputSource file:genesis/utils/io/base_input_source.hpp line:50
		pybind11::class_<genesis::utils::BaseInputSource, std::shared_ptr<genesis::utils::BaseInputSource>> cl(M("genesis::utils"), "BaseInputSource", "Abstract base class for reading byte data from input sources.\n\n It offers to read() a certain amount of bytes into a char buffer.");
		cl.def("assign", (class genesis::utils::BaseInputSource & (genesis::utils::BaseInputSource::*)(const class genesis::utils::BaseInputSource &)) &genesis::utils::BaseInputSource::operator=, "C++: genesis::utils::BaseInputSource::operator=(const class genesis::utils::BaseInputSource &) --> class genesis::utils::BaseInputSource &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("read", (unsigned long (genesis::utils::BaseInputSource::*)(char *, unsigned long)) &genesis::utils::BaseInputSource::read, "Read  many bytes into the char \n\n Returns the number of bytes that have actually been read, which might be lower than\n the initial target  e.g., if the end of the input source was reached.\n\nC++: genesis::utils::BaseInputSource::read(char *, unsigned long) --> unsigned long", pybind11::arg("buffer"), pybind11::arg("size"));
		cl.def("source_name", (std::string (genesis::utils::BaseInputSource::*)() const) &genesis::utils::BaseInputSource::source_name, "Get a name of the input source. This is intended for user output.\n\nC++: genesis::utils::BaseInputSource::source_name() const --> std::string");
		cl.def("source_string", (std::string (genesis::utils::BaseInputSource::*)() const) &genesis::utils::BaseInputSource::source_string, "Get a string representing the input source. This is intended for the reader classes,\n which for example might want to examine the input file name.\n\nC++: genesis::utils::BaseInputSource::source_string() const --> std::string");
	}
	{ // genesis::utils::FileInputSource file:genesis/utils/io/file_input_source.hpp line:60
		pybind11::class_<genesis::utils::FileInputSource, std::shared_ptr<genesis::utils::FileInputSource>, genesis::utils::BaseInputSource> cl(M("genesis::utils"), "FileInputSource", "Input source for reading byte data from a file.\n\n The input file name is provided via the constructor.\n\n It is also possible to provide a `FILE` pointer directly. In this case, the ownership of the file\n pointer is taken by this class. Thus, closing the file is done when destructing this class.");
		cl.def( pybind11::init<const std::string &>(), pybind11::arg("file_name") );

		cl.def( pybind11::init<const std::string &, struct _IO_FILE *>(), pybind11::arg("file_name"), pybind11::arg("file") );

		cl.def( pybind11::init( [](genesis::utils::FileInputSource const &o){ return new genesis::utils::FileInputSource(o); } ) );
		cl.def("assign", (class genesis::utils::FileInputSource & (genesis::utils::FileInputSource::*)(const class genesis::utils::FileInputSource &)) &genesis::utils::FileInputSource::operator=, "C++: genesis::utils::FileInputSource::operator=(const class genesis::utils::FileInputSource &) --> class genesis::utils::FileInputSource &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
	{ // genesis::utils::GzipInputSource file:genesis/utils/io/gzip_input_source.hpp line:56
		pybind11::class_<genesis::utils::GzipInputSource, std::shared_ptr<genesis::utils::GzipInputSource>, genesis::utils::BaseInputSource> cl(M("genesis::utils"), "GzipInputSource", "Input source for reading byte data from a gzip/zlib-compressed source.\n\n This input source is a wrapper that takes some other input source\n (FileInputSource, StringInputSource, StreamInputSource, etc),\n and de-compresses using gzip/zlib on the fly while reading.\n\n The class can be moved, but not copied, because of the internal state that is kept for\n decompression, and which would mess up the input source if copied.");
		cl.def( pybind11::init( [](class std::shared_ptr<class genesis::utils::BaseInputSource> const & a0){ return new genesis::utils::GzipInputSource(a0); } ), "doc" , pybind11::arg("input_source"));
		cl.def( pybind11::init<class std::shared_ptr<class genesis::utils::BaseInputSource>, enum genesis::utils::GzipInputSource::Format>(), pybind11::arg("input_source"), pybind11::arg("format") );


		pybind11::enum_<genesis::utils::GzipInputSource::Format>(cl, "Format", "Format used by gzip/zlib for decompression.")
			.value("kAutomatic", genesis::utils::GzipInputSource::Format::kAutomatic)
			.value("kGzip", genesis::utils::GzipInputSource::Format::kGzip)
			.value("kZlib", genesis::utils::GzipInputSource::Format::kZlib)
			.value("kDeflate", genesis::utils::GzipInputSource::Format::kDeflate);

	}
	{ // genesis::utils::Exception file: line:53
		pybind11::class_<genesis::utils::Exception, std::shared_ptr<genesis::utils::Exception>, PyCallBack_genesis_utils_Exception, std::exception> cl(M("genesis::utils"), "Exception", "Base class for genesis exceptions.\n\n This class serves as the base from which all exceptions that are thrown from genesis shall\n be derived. We are far from having done this yet (unfortunately, we had this idea too late...),\n so for now, there are only a few exceptions that use this class as their base.\n But in the long term, we want to change that, so that developers using genesis can easily\n catch all genesis-related exceptions.");
		cl.def( pybind11::init<const std::string &>(), pybind11::arg("message") );

		cl.def( pybind11::init( [](PyCallBack_genesis_utils_Exception const &o){ return new PyCallBack_genesis_utils_Exception(o); } ) );
		cl.def( pybind11::init( [](genesis::utils::Exception const &o){ return new genesis::utils::Exception(o); } ) );
		cl.def("what", (const char * (genesis::utils::Exception::*)() const) &genesis::utils::Exception::what, "C++: genesis::utils::Exception::what() const --> const char *", pybind11::return_value_policy::reference_internal);
		cl.def("assign", (class genesis::utils::Exception & (genesis::utils::Exception::*)(const class genesis::utils::Exception &)) &genesis::utils::Exception::operator=, "C++: genesis::utils::Exception::operator=(const class genesis::utils::Exception &) --> class genesis::utils::Exception &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
	{ // genesis::utils::IOError file: line:79
		pybind11::class_<genesis::utils::IOError, std::shared_ptr<genesis::utils::IOError>, PyCallBack_genesis_utils_IOError, genesis::utils::Exception> cl(M("genesis::utils"), "IOError", "Exception class for general input/output errors.");
		cl.def( pybind11::init<const std::string &>(), pybind11::arg("message") );

		cl.def( pybind11::init<const std::string &, const std::string &>(), pybind11::arg("message"), pybind11::arg("filename") );

		cl.def( pybind11::init( [](PyCallBack_genesis_utils_IOError const &o){ return new PyCallBack_genesis_utils_IOError(o); } ) );
		cl.def( pybind11::init( [](genesis::utils::IOError const &o){ return new genesis::utils::IOError(o); } ) );
		cl.def("filename", (const std::string & (genesis::utils::IOError::*)() const) &genesis::utils::IOError::filename, "C++: genesis::utils::IOError::filename() const --> const std::string &", pybind11::return_value_policy::reference_internal);
		cl.def("assign", (class genesis::utils::IOError & (genesis::utils::IOError::*)(const class genesis::utils::IOError &)) &genesis::utils::IOError::operator=, "C++: genesis::utils::IOError::operator=(const class genesis::utils::IOError &) --> class genesis::utils::IOError &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
	{ // genesis::utils::ExistingFileError file: line:109
		pybind11::class_<genesis::utils::ExistingFileError, std::shared_ptr<genesis::utils::ExistingFileError>, PyCallBack_genesis_utils_ExistingFileError, genesis::utils::IOError> cl(M("genesis::utils"), "ExistingFileError", "Exception class that is thrown if trying to write to an existing file.\n\n See for example file_output_stream(). This exception is not thrown if\n Options::get().allow_file_overwriting() is set to true.");
		cl.def( pybind11::init<const std::string &, const std::string &>(), pybind11::arg("message"), pybind11::arg("filename") );

		cl.def( pybind11::init( [](PyCallBack_genesis_utils_ExistingFileError const &o){ return new PyCallBack_genesis_utils_ExistingFileError(o); } ) );
		cl.def( pybind11::init( [](genesis::utils::ExistingFileError const &o){ return new genesis::utils::ExistingFileError(o); } ) );
		cl.def("assign", (class genesis::utils::ExistingFileError & (genesis::utils::ExistingFileError::*)(const class genesis::utils::ExistingFileError &)) &genesis::utils::ExistingFileError::operator=, "C++: genesis::utils::ExistingFileError::operator=(const class genesis::utils::ExistingFileError &) --> class genesis::utils::ExistingFileError &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
	// genesis::utils::is_gzip_compressed_file(const std::string &) file:genesis/utils/io/gzip.hpp line:53
	M("genesis::utils").def("is_gzip_compressed_file", (bool (*)(const std::string &)) &genesis::utils::is_gzip_compressed_file, "Return whether a given file is gzip-compressed.\n\n Returns true only iff the file exists and is gzip-compressed.\n If there is a mismatch between the gzip magic bytes and the file ending, a warning is issued\n via LOG_WARN.\n\nC++: genesis::utils::is_gzip_compressed_file(const std::string &) --> bool", pybind11::arg("file_name"));

	{ // genesis::utils::GzipError file:genesis/utils/io/gzip.hpp line:64
		pybind11::class_<genesis::utils::GzipError, std::shared_ptr<genesis::utils::GzipError>, PyCallBack_genesis_utils_GzipError, genesis::utils::IOError> cl(M("genesis::utils"), "GzipError", "Exception class thrown by failed gzip/zlib operations.\n\n If compiled without zlib support, the exepction has no use and contains a dummy message.");
		cl.def( pybind11::init<const std::string &, int>(), pybind11::arg("z_stream_message"), pybind11::arg("error_code") );

		cl.def( pybind11::init<const std::string>(), pybind11::arg("msg") );

		cl.def("assign", (class genesis::utils::GzipError & (genesis::utils::GzipError::*)(const class genesis::utils::GzipError &)) &genesis::utils::GzipError::operator=, "C++: genesis::utils::GzipError::operator=(const class genesis::utils::GzipError &) --> class genesis::utils::GzipError &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
	{ // genesis::utils::StreamInputSource file:genesis/utils/io/stream_input_source.hpp line:56
		pybind11::class_<genesis::utils::StreamInputSource, std::shared_ptr<genesis::utils::StreamInputSource>, genesis::utils::BaseInputSource> cl(M("genesis::utils"), "StreamInputSource", "Input source for reading byte data from an istream.\n\n The input stream is provided via the constructor. It is not owned by this class, thus\n the owner must keep it alive as long as reading from it is required, and is responsbile for\n destroying it. This class merely keeps a reference to it.");
		cl.def( pybind11::init<std::istream &>(), pybind11::arg("in") );

		cl.def( pybind11::init( [](genesis::utils::StreamInputSource const &o){ return new genesis::utils::StreamInputSource(o); } ) );
		cl.def("assign", (class genesis::utils::StreamInputSource & (genesis::utils::StreamInputSource::*)(const class genesis::utils::StreamInputSource &)) &genesis::utils::StreamInputSource::operator=, "C++: genesis::utils::StreamInputSource::operator=(const class genesis::utils::StreamInputSource &) --> class genesis::utils::StreamInputSource &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
	{ // genesis::utils::StringInputSource file:genesis/utils/io/string_input_source.hpp line:58
		pybind11::class_<genesis::utils::StringInputSource, std::shared_ptr<genesis::utils::StringInputSource>, genesis::utils::BaseInputSource> cl(M("genesis::utils"), "StringInputSource", "Input source for reading byte data from a string.\n\n The input string is provided via the constructor. It is not owned by this class, thus\n the owner must keep it alive as long as reading from it is required, and is responsbile for\n destroying it. This class merely keeps a pointer to it.\n\n That implies that the string shall not be modified while this input source is used, thus,\n only const-members of the string can be called.");
		cl.def( pybind11::init<const char *, unsigned long>(), pybind11::arg("str"), pybind11::arg("size") );

		cl.def( pybind11::init<const std::string &>(), pybind11::arg("str") );

		cl.def( pybind11::init( [](genesis::utils::StringInputSource const &o){ return new genesis::utils::StringInputSource(o); } ) );
		cl.def("assign", (class genesis::utils::StringInputSource & (genesis::utils::StringInputSource::*)(const class genesis::utils::StringInputSource &)) &genesis::utils::StringInputSource::operator=, "C++: genesis::utils::StringInputSource::operator=(const class genesis::utils::StringInputSource &) --> class genesis::utils::StringInputSource &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
}

#include <functional>
#include <genesis/utils/io/base_input_source.hpp>
#include <genesis/utils/io/input_reader.hpp>
#include <genesis/utils/io/input_source.hpp>
#include <genesis/utils/io/input_stream.hpp>
#include <genesis/utils/text/char.hpp>
#include <ios>
#include <istream>
#include <iterator>
#include <locale>
#include <memory>
#include <sstream> // __str__
#include <streambuf>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

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

void bind_genesis_utils_io_input_source(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::utils::from_file(const std::string &, bool) file:genesis/utils/io/input_source.hpp line:67
	M("genesis::utils").def("from_file", [](const std::string & a0) -> std::shared_ptr<class genesis::utils::BaseInputSource> { return genesis::utils::from_file(a0); }, "", pybind11::arg("file_name"));
	M("genesis::utils").def("from_file", (class std::shared_ptr<class genesis::utils::BaseInputSource> (*)(const std::string &, bool)) &genesis::utils::from_file, "Obtain an input source for reading from a file.\n\n The input source returned from this function can be used in the reader classes, e.g.,\n placement::JplaceReader or sequence::FastaReader.\n\n If the parameter  is `true` (default), it is first determined whether the\n file is gzip compressed, and if so, a transparent decompression layer is added.\n That means, gzip-compressed files can be decompressed automatically and on the fly.\n\n \n from_files(), from_string(), from_strings(), and from_stream() for similar\n helper functions for other types of input sources.\n\nC++: genesis::utils::from_file(const std::string &, bool) --> class std::shared_ptr<class genesis::utils::BaseInputSource>", pybind11::arg("file_name"), pybind11::arg("detect_compression"));

	// genesis::utils::from_files(const class std::vector<std::string, class std::allocator<std::string > > &, bool) file:genesis/utils/io/input_source.hpp line:90
	M("genesis::utils").def("from_files", [](const class std::vector<std::string, class std::allocator<std::string > > & a0) -> std::vector<class std::shared_ptr<class genesis::utils::BaseInputSource>, class std::allocator<class std::shared_ptr<class genesis::utils::BaseInputSource> > > { return genesis::utils::from_files(a0); }, "", pybind11::arg("file_names"));
	M("genesis::utils").def("from_files", (class std::vector<class std::shared_ptr<class genesis::utils::BaseInputSource>, class std::allocator<class std::shared_ptr<class genesis::utils::BaseInputSource> > > (*)(const class std::vector<std::string, class std::allocator<std::string > > &, bool)) &genesis::utils::from_files, "Obtain a set of input sources for reading from files.\n\n See from_file() for details. This version returnes multiple input sources, which can be used\n for parallely reading from multiple files for speedup.\n This function can for example be used with the output of utils::dir_list_files().\n\n \n from_file(), from_string(), from_strings(), and from_stream() for similar\n helper functions for other types of input sources.\n\nC++: genesis::utils::from_files(const class std::vector<std::string, class std::allocator<std::string > > &, bool) --> class std::vector<class std::shared_ptr<class genesis::utils::BaseInputSource>, class std::allocator<class std::shared_ptr<class genesis::utils::BaseInputSource> > >", pybind11::arg("file_names"), pybind11::arg("detect_compression"));

	// genesis::utils::from_string(const std::string &) file:genesis/utils/io/input_source.hpp line:133
	M("genesis::utils").def("from_string", (class std::shared_ptr<class genesis::utils::BaseInputSource> (*)(const std::string &)) &genesis::utils::from_string, "Obtain an input source for reading from a string.\n\n The input source returned from this function can be used in the reader classes, e.g.,\n placement::JplaceReader or sequence::FastaReader.\n\n \n from_file(), from_files(), from_strings(), and from_stream() for similar\n helper functions for other types of input sources.\n\nC++: genesis::utils::from_string(const std::string &) --> class std::shared_ptr<class genesis::utils::BaseInputSource>", pybind11::arg("input_string"));

	// genesis::utils::from_strings(const class std::vector<std::string, class std::allocator<std::string > > &) file:genesis/utils/io/input_source.hpp line:147
	M("genesis::utils").def("from_strings", (class std::vector<class std::shared_ptr<class genesis::utils::BaseInputSource>, class std::allocator<class std::shared_ptr<class genesis::utils::BaseInputSource> > > (*)(const class std::vector<std::string, class std::allocator<std::string > > &)) &genesis::utils::from_strings, "Obtain a set of input sources for reading from strings.\n\n See from_string() and from_files() for details.\n\n \n from_file(), from_files(), from_string(), and from_stream() for similar\n helper functions for other types of input sources.\n\nC++: genesis::utils::from_strings(const class std::vector<std::string, class std::allocator<std::string > > &) --> class std::vector<class std::shared_ptr<class genesis::utils::BaseInputSource>, class std::allocator<class std::shared_ptr<class genesis::utils::BaseInputSource> > >", pybind11::arg("input_strings"));

	// genesis::utils::from_stream(std::istream &) file:genesis/utils/io/input_source.hpp line:187
	M("genesis::utils").def("from_stream", (class std::shared_ptr<class genesis::utils::BaseInputSource> (*)(std::istream &)) &genesis::utils::from_stream, "Obtain an input source for reading from a stream.\n\n The input source returned from this function can be used in the reader classes, e.g.,\n placement::JplaceReader or sequence::FastaReader.\n\n \n from_file(), from_files(), from_string(), and from_strings() for similar\n helper functions for other types of input sources.\n\nC++: genesis::utils::from_stream(std::istream &) --> class std::shared_ptr<class genesis::utils::BaseInputSource>", pybind11::arg("input_stream"));

	{ // genesis::utils::SynchronousReader file:genesis/utils/io/input_reader.hpp line:206
		pybind11::class_<genesis::utils::SynchronousReader, std::shared_ptr<genesis::utils::SynchronousReader>> cl(M("genesis::utils"), "SynchronousReader", "Read bytes from an \n\n\n\n\n\n\n\n");
		cl.def( pybind11::init( [](){ return new genesis::utils::SynchronousReader(); } ) );
		cl.def("init", (void (genesis::utils::SynchronousReader::*)(class std::shared_ptr<class genesis::utils::BaseInputSource>)) &genesis::utils::SynchronousReader::init, "C++: genesis::utils::SynchronousReader::init(class std::shared_ptr<class genesis::utils::BaseInputSource>) --> void", pybind11::arg("input_source"));
		cl.def("valid", (bool (genesis::utils::SynchronousReader::*)() const) &genesis::utils::SynchronousReader::valid, "C++: genesis::utils::SynchronousReader::valid() const --> bool");
		cl.def("input_source", (const class genesis::utils::BaseInputSource * (genesis::utils::SynchronousReader::*)() const) &genesis::utils::SynchronousReader::input_source, "C++: genesis::utils::SynchronousReader::input_source() const --> const class genesis::utils::BaseInputSource *", pybind11::return_value_policy::reference_internal);
		cl.def("class_name", (std::string (genesis::utils::SynchronousReader::*)() const) &genesis::utils::SynchronousReader::class_name, "C++: genesis::utils::SynchronousReader::class_name() const --> std::string");
		cl.def("start_reading", (void (genesis::utils::SynchronousReader::*)(char *, unsigned long)) &genesis::utils::SynchronousReader::start_reading, "C++: genesis::utils::SynchronousReader::start_reading(char *, unsigned long) --> void", pybind11::arg("target_buffer"), pybind11::arg("target_size"));
		cl.def("finish_reading", (unsigned long (genesis::utils::SynchronousReader::*)()) &genesis::utils::SynchronousReader::finish_reading, "C++: genesis::utils::SynchronousReader::finish_reading() --> unsigned long");
	}
	// genesis::utils::is_ascii(struct std::integral_constant<bool, true>, char) file:genesis/utils/text/char.hpp line:50
	M("genesis::utils").def("is_ascii", (bool (*)(struct std::integral_constant<bool, true>, char)) &genesis::utils::is_ascii<char>, "C++: genesis::utils::is_ascii(struct std::integral_constant<bool, true>, char) --> bool", pybind11::arg(""), pybind11::arg("c"));

	// genesis::utils::is_ascii(char) file:genesis/utils/text/char.hpp line:69
	M("genesis::utils").def("is_ascii", (bool (*)(char)) &genesis::utils::is_ascii, "Return whether a char is pure ASCII, that is, in the range [0, 127].\n\nC++: genesis::utils::is_ascii(char) --> bool", pybind11::arg("c"));

	// genesis::utils::is_cntrl(char) file:genesis/utils/text/char.hpp line:79
	M("genesis::utils").def("is_cntrl", (bool (*)(char)) &genesis::utils::is_cntrl, "Return whether a char is a control character, according to `iscntrl` of the cctype> `heade`\n but ASCII-only.\n\nC++: genesis::utils::is_cntrl(char) --> bool", pybind11::arg("c"));

	// genesis::utils::is_control(char) file:genesis/utils/text/char.hpp line:87
	M("genesis::utils").def("is_control", (bool (*)(char)) &genesis::utils::is_control, "Return whether a char is a control character, excluding white spaces, ASCII-only.\n\nC++: genesis::utils::is_control(char) --> bool", pybind11::arg("c"));

	// genesis::utils::is_digit(char) file:genesis/utils/text/char.hpp line:95
	M("genesis::utils").def("is_digit", (bool (*)(char)) &genesis::utils::is_digit, "Return whether a char is a digit (0-9), ASCII-only.\n\nC++: genesis::utils::is_digit(char) --> bool", pybind11::arg("c"));

	// genesis::utils::is_xdigit(char) file:genesis/utils/text/char.hpp line:105
	M("genesis::utils").def("is_xdigit", (bool (*)(char)) &genesis::utils::is_xdigit, "Return whether a char is a hexadecimal digit (0-9 or A-F or a-f), ASCII-only.\n\nC++: genesis::utils::is_xdigit(char) --> bool", pybind11::arg("c"));

	// genesis::utils::is_lower(char) file:genesis/utils/text/char.hpp line:115
	M("genesis::utils").def("is_lower", (bool (*)(char)) &genesis::utils::is_lower, "Return whether a char is a lower case letter (a-z), ASCII-only.\n\nC++: genesis::utils::is_lower(char) --> bool", pybind11::arg("c"));

	// genesis::utils::is_upper(char) file:genesis/utils/text/char.hpp line:125
	M("genesis::utils").def("is_upper", (bool (*)(char)) &genesis::utils::is_upper, "Return whether a char is an upper case letter (A-Z), ASCII-only.\n\nC++: genesis::utils::is_upper(char) --> bool", pybind11::arg("c"));

	// genesis::utils::is_alpha(char) file:genesis/utils/text/char.hpp line:135
	M("genesis::utils").def("is_alpha", (bool (*)(char)) &genesis::utils::is_alpha, "Return whether a char is a letter (a-z or A-Z), ASCII-only.\n\nC++: genesis::utils::is_alpha(char) --> bool", pybind11::arg("c"));

	// genesis::utils::is_alnum(char) file:genesis/utils/text/char.hpp line:143
	M("genesis::utils").def("is_alnum", (bool (*)(char)) &genesis::utils::is_alnum, "Return whether a char is a letter (a-z or A-Z) or a digit (0-9), ASCII-only.\n\nC++: genesis::utils::is_alnum(char) --> bool", pybind11::arg("c"));

	// genesis::utils::is_punct(char) file:genesis/utils/text/char.hpp line:152
	M("genesis::utils").def("is_punct", (bool (*)(char)) &genesis::utils::is_punct, "Return whether a char is a punctuation mark, according to `ispunct` of the `cctype`\n header, but ASCII-only.\n\nC++: genesis::utils::is_punct(char) --> bool", pybind11::arg("c"));

	// genesis::utils::is_graph(char) file:genesis/utils/text/char.hpp line:166
	M("genesis::utils").def("is_graph", (bool (*)(char)) &genesis::utils::is_graph, "Return whether a char is a character with graphical representation, according to `isgraph`\n of the `cctype` header, but ASCII-only.\n\nC++: genesis::utils::is_graph(char) --> bool", pybind11::arg("c"));

	// genesis::utils::is_blank(char) file:genesis/utils/text/char.hpp line:174
	M("genesis::utils").def("is_blank", (bool (*)(char)) &genesis::utils::is_blank, "Return whether a char is either a space or a tab character.\n\nC++: genesis::utils::is_blank(char) --> bool", pybind11::arg("c"));

	// genesis::utils::is_newline(char) file:genesis/utils/text/char.hpp line:182
	M("genesis::utils").def("is_newline", (bool (*)(char)) &genesis::utils::is_newline, "Return whether a char is either a new line or a carriage return character.\n\nC++: genesis::utils::is_newline(char) --> bool", pybind11::arg("c"));

	// genesis::utils::is_other_space(char) file:genesis/utils/text/char.hpp line:191
	M("genesis::utils").def("is_other_space", (bool (*)(char)) &genesis::utils::is_other_space, "Return whether a char is some other white space charater that is neither space, tab,\n new line, or carriage return - that is, whether it is a form feed or a vertical tab.\n\nC++: genesis::utils::is_other_space(char) --> bool", pybind11::arg("c"));

	// genesis::utils::is_space(char) file:genesis/utils/text/char.hpp line:200
	M("genesis::utils").def("is_space", (bool (*)(char)) &genesis::utils::is_space, "Return whether a char is some form of white space charater, so either space, tab,\n new line, carriage return, form feed, or vertical tab.\n\nC++: genesis::utils::is_space(char) --> bool", pybind11::arg("c"));

	// genesis::utils::is_print(char) file:genesis/utils/text/char.hpp line:209
	M("genesis::utils").def("is_print", (bool (*)(char)) &genesis::utils::is_print, "Return whether a char is a printable character, according to `isprint` of the `cctype`\n header, but ASCII-only.\n\nC++: genesis::utils::is_print(char) --> bool", pybind11::arg("c"));

	// genesis::utils::to_lower(char) file:genesis/utils/text/char.hpp line:221
	M("genesis::utils").def("to_lower", (char (*)(char)) &genesis::utils::to_lower, "Return the lower case version of a letter, ASCII-only.\n\nC++: genesis::utils::to_lower(char) --> char", pybind11::arg("c"));

	// genesis::utils::to_upper(char) file:genesis/utils/text/char.hpp line:230
	M("genesis::utils").def("to_upper", (char (*)(char)) &genesis::utils::to_upper, "Return the upper case version of a letter, ASCII-only.\n\nC++: genesis::utils::to_upper(char) --> char", pybind11::arg("c"));

	// genesis::utils::char_match_ci(char, char) file:genesis/utils/text/char.hpp line:243
	M("genesis::utils").def("char_match_ci", (bool (*)(char, char)) &genesis::utils::char_match_ci, "Return whether two chars are the same, case insensitive, and ASCII-only.\n\nC++: genesis::utils::char_match_ci(char, char) --> bool", pybind11::arg("c1"), pybind11::arg("c2"));

	// genesis::utils::char_to_hex(char, bool) file:genesis/utils/text/char.hpp line:268
	M("genesis::utils").def("char_to_hex", [](char const & a0) -> std::string { return genesis::utils::char_to_hex(a0); }, "", pybind11::arg("c"));
	M("genesis::utils").def("char_to_hex", (std::string (*)(char, bool)) &genesis::utils::char_to_hex, "Return the name and hex representation of a `char`.\n\n This is meant for user output, for example to warn about invalid input characters.\n Hence, by default, a text-representation is returned, using the form\n\n     LF (0x0A; control: NL line feed, new line)\n     'N' (0x4E; letter: capital N)\n\n where non-printable characters are abbreviated by their symbol, and printable\n characters are included in single quotation marks.\n\n Non-ASCII characters (those outside of the 0-127 interval) are written in the form\n\n     non-ASCII char (0xF7)\n\n If  is set to `false`, just the two-byte hex presentation is returned (e.g., `4E` for `N`).\n\nC++: genesis::utils::char_to_hex(char, bool) --> std::string", pybind11::arg("c"), pybind11::arg("full"));

	// genesis::utils::char_to_hex(unsigned char, bool) file:genesis/utils/text/char.hpp line:276
	M("genesis::utils").def("char_to_hex", [](unsigned char const & a0) -> std::string { return genesis::utils::char_to_hex(a0); }, "", pybind11::arg("c"));
	M("genesis::utils").def("char_to_hex", (std::string (*)(unsigned char, bool)) &genesis::utils::char_to_hex, "Return the name and hex representation of a `char`, given as an `unsigned char`.\n\n This overload is meant for situations were a text input is read from file as `unsigned char`.\n The rest of the function is identical to char_to_hex( char, bool ), see there for details.\n\nC++: genesis::utils::char_to_hex(unsigned char, bool) --> std::string", pybind11::arg("c"), pybind11::arg("full"));

	{ // genesis::utils::InputStream file:genesis/utils/io/input_stream.hpp line:81
		pybind11::class_<genesis::utils::InputStream, std::shared_ptr<genesis::utils::InputStream>> cl(M("genesis::utils"), "InputStream", "Stream interface for reading data from an \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
		cl.def( pybind11::init( [](){ return new genesis::utils::InputStream(); } ) );
		cl.def( pybind11::init<class std::shared_ptr<class genesis::utils::BaseInputSource>>(), pybind11::arg("input_source") );

		cl.def("parse_unsigned_integer", (unsigned long (genesis::utils::InputStream::*)()) &genesis::utils::InputStream::parse_unsigned_integer<unsigned long>, "Read an unsigned integer from a stream into a size_t and return it.\n\n This template specialization is mean as yet another speedup for the case of this data type,\n as we can then work without casting and overflow check. It is also the base function that\n is called from the others that do the overflow check.\n\nC++: genesis::utils::InputStream::parse_unsigned_integer() --> unsigned long");
		cl.def("dereference", (char (genesis::utils::InputStream::*)() const) &genesis::utils::InputStream::operator*, "Dereference operator. Return the current char.\n\nC++: genesis::utils::InputStream::operator*() const --> char");
		cl.def("current", (char (genesis::utils::InputStream::*)() const) &genesis::utils::InputStream::current, "Return the current char, with some checks.\n\n This function is similar to the dereference operator, but additionally performs two checks:\n\n  *  End of input: If this function is called when there is no more data left in the input,\n     it throws an `runtime_error`.\n  *  Current char: This iterator is meant for ASCII (or similar) text format encodings with\n     single bytes, and its output should be usable for lookup tables etc. Thus, this function\n     ensures that the char is in the range [0, 127]. If not, an `std::domain_error` is thrown.\n\n Usually, those two conditions are checked in the parser anyway, so in most cases it is\n preferred to use the \n\n\n\nC++: genesis::utils::InputStream::current() const --> char");
		cl.def("advance", (class genesis::utils::InputStream & (genesis::utils::InputStream::*)()) &genesis::utils::InputStream::advance, "Move to the next char in the stream and advance the counters.\n\nC++: genesis::utils::InputStream::advance() --> class genesis::utils::InputStream &", pybind11::return_value_policy::reference_internal);
		cl.def("pre_increment", (class genesis::utils::InputStream & (genesis::utils::InputStream::*)()) &genesis::utils::InputStream::operator++, "Move to the next char in the stream. Shortcut for advance().\n\nC++: genesis::utils::InputStream::operator++() --> class genesis::utils::InputStream &", pybind11::return_value_policy::reference_internal);
		cl.def("get_char", (char (genesis::utils::InputStream::*)()) &genesis::utils::InputStream::get_char, "Extract a single char from the input.\n\n Return the current char and move to the next one.\n\nC++: genesis::utils::InputStream::get_char() --> char");
		cl.def("get_line", (void (genesis::utils::InputStream::*)(std::string &)) &genesis::utils::InputStream::get_line, "Read the current line, append it to the  and move to the beginning of the\n next line.\n\n The function finds the end of the current line, starting from the current position,\n and appends the content to the given  excluding the trailing new line char(s).\n The stream is left at the first char of the next line.\n\nC++: genesis::utils::InputStream::get_line(std::string &) --> void", pybind11::arg("target"));
		cl.def("get_line", (std::string (genesis::utils::InputStream::*)()) &genesis::utils::InputStream::get_line, "Read the current line and move to the beginning of the next.\n\n The function finds the end of the current line, starting from the current position,\n and returns the content, excluding the trailing new line char(s).\n The stream is left at the first char of the next line.\n\nC++: genesis::utils::InputStream::get_line() --> std::string");
		cl.def("read_char_or_throw", (char (genesis::utils::InputStream::*)(const char)) &genesis::utils::InputStream::read_char_or_throw, "Lexing function that reads a single char from the stream and checks whether it equals\n the provided one.\n\n If not, the function throws `std::runtime_error`. The stream is advanced by one position and\n the char is returned. For a similar function that checks the value of the current char but\n does not advance in the stream, see affirm_char_or_throw().\n\nC++: genesis::utils::InputStream::read_char_or_throw(const char) --> char", pybind11::arg("criterion"));
		cl.def("read_char_or_throw", (char (genesis::utils::InputStream::*)(class std::function<bool (char)>)) &genesis::utils::InputStream::read_char_or_throw, "Lexing function that reads a single char from the stream and checks whether it\n fulfills the provided criterion.\n\n If not, the function throws `std::runtime_error`. The stream is advanced by one position and\n the char is returned. For a similar function that checks the value of the current char but\n does not advance in the stream, see affirm_char_or_throw().\n\nC++: genesis::utils::InputStream::read_char_or_throw(class std::function<bool (char)>) --> char", pybind11::arg("criterion"));
		cl.def("line", (unsigned long (genesis::utils::InputStream::*)() const) &genesis::utils::InputStream::line, "Return the current line of the input stream.\n\n The counter starts with line 1 for input stream.\n\nC++: genesis::utils::InputStream::line() const --> unsigned long");
		cl.def("column", (unsigned long (genesis::utils::InputStream::*)() const) &genesis::utils::InputStream::column, "Return the current column of the input stream.\n\n The counter starts with column 1 for each line of the input stream.\n New line characters `` are included in counting and count as the last character of a line.\n\nC++: genesis::utils::InputStream::column() const --> unsigned long");
		cl.def("at", (std::string (genesis::utils::InputStream::*)() const) &genesis::utils::InputStream::at, "Return a textual representation of the current input position in the form\n \"line:column\".\n\nC++: genesis::utils::InputStream::at() const --> std::string");
		cl.def("good", (bool (genesis::utils::InputStream::*)() const) &genesis::utils::InputStream::good, "Return true iff the input is good (not end of data) and can be read from.\n\nC++: genesis::utils::InputStream::good() const --> bool");
		cl.def("eof", (bool (genesis::utils::InputStream::*)() const) &genesis::utils::InputStream::eof, "Return true iff the input reached its end.\n\nC++: genesis::utils::InputStream::eof() const --> bool");
		cl.def("source_name", (std::string (genesis::utils::InputStream::*)() const) &genesis::utils::InputStream::source_name, "Get the input source name where this stream reads from.\n\n Depending on the type of input, this is either\n\n  *  \"input string\",\n  *  \"input stream\" or\n  *  \"input file <filename>\"\n\n This is mainly useful for user output like log and error messages.\n\nC++: genesis::utils::InputStream::source_name() const --> std::string");
		cl.def("buffer", (struct std::pair<const char *, unsigned long> (genesis::utils::InputStream::*)()) &genesis::utils::InputStream::buffer, "Direct access to the internal buffer.\n\n This function returns a pointer to the internal buffer, as well as the length (past the end)\n that is currently buffered. This is meant for special file parsers that can optimize better\n when using this - but it is highly dangerous to use if you do not know what you are doing!\n\n The idea is as follows: With access to the buffer, parse data as needed, keeping track of\n how many chars have been processed. Then, use the jump_unchecked() function to update this\n stream to the new position of the stream (the char after the last one being processed by\n the parsing).\n\n Caveat: Never parse and jump across new line characters (or, for that matter, carriage return\n characters, which won't be automatically converted when using the buffer directly)! This\n would invalidate the line counting!\n\n Caveat: Never read after the end of the buffer, that is, never access the char at the\n returned last position `buffer + length` or after!\n\nC++: genesis::utils::InputStream::buffer() --> struct std::pair<const char *, unsigned long>");
		cl.def("jump_unchecked", (void (genesis::utils::InputStream::*)(unsigned long)) &genesis::utils::InputStream::jump_unchecked, "Jump forward in the stream by a certain amount of chars.\n\n This is meant to update the stream position after using buffer() for direct parsing.\n See the caveats there!\n\n In particular, we can never jump behind the current buffer end, and shall not jump across\n new lines. That is, this function is not meant as a way to jump to an arbitrary (later)\n position in a file!\n\nC++: genesis::utils::InputStream::jump_unchecked(unsigned long) --> void", pybind11::arg("n"));
	}
}

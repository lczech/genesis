#include <genesis/utils/text/char.hpp>
#include <iterator>
#include <memory>
#include <string>

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

void bind_genesis_utils_text_char(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::utils::text::is_ascii(char) file:genesis/utils/text/char.hpp line:70
	// function-signature: bool genesis::utils::text::is_ascii(char)(char) file:genesis/utils/text/char.hpp line:70
	M("genesis::utils::text").def("is_ascii", (bool (*)(char)) &genesis::utils::text::is_ascii, "Return whether a char is pure ASCII, that is, in the range [0, 127].\n\nC++: genesis::utils::text::is_ascii(char) --> bool", pybind11::arg("c"));

	// genesis::utils::text::is_cntrl(char) file:genesis/utils/text/char.hpp line:80
	// function-signature: bool genesis::utils::text::is_cntrl(char)(char) file:genesis/utils/text/char.hpp line:80
	M("genesis::utils::text").def("is_cntrl", (bool (*)(char)) &genesis::utils::text::is_cntrl, "Return whether a char is a control character, according to `iscntrl` of the cctype> `heade`\n but ASCII-only.\n\nC++: genesis::utils::text::is_cntrl(char) --> bool", pybind11::arg("c"));

	// genesis::utils::text::is_control(char) file:genesis/utils/text/char.hpp line:88
	// function-signature: bool genesis::utils::text::is_control(char)(char) file:genesis/utils/text/char.hpp line:88
	M("genesis::utils::text").def("is_control", (bool (*)(char)) &genesis::utils::text::is_control, "Return whether a char is a control character, excluding white spaces, ASCII-only.\n\nC++: genesis::utils::text::is_control(char) --> bool", pybind11::arg("c"));

	// genesis::utils::text::is_digit(char) file:genesis/utils/text/char.hpp line:96
	// function-signature: bool genesis::utils::text::is_digit(char)(char) file:genesis/utils/text/char.hpp line:96
	M("genesis::utils::text").def("is_digit", (bool (*)(char)) &genesis::utils::text::is_digit, "Return whether a char is a digit (0-9), ASCII-only.\n\nC++: genesis::utils::text::is_digit(char) --> bool", pybind11::arg("c"));

	// genesis::utils::text::is_xdigit(char) file:genesis/utils/text/char.hpp line:106
	// function-signature: bool genesis::utils::text::is_xdigit(char)(char) file:genesis/utils/text/char.hpp line:106
	M("genesis::utils::text").def("is_xdigit", (bool (*)(char)) &genesis::utils::text::is_xdigit, "Return whether a char is a hexadecimal digit (0-9 or A-F or a-f), ASCII-only.\n\nC++: genesis::utils::text::is_xdigit(char) --> bool", pybind11::arg("c"));

	// genesis::utils::text::is_lower(char) file:genesis/utils/text/char.hpp line:116
	// function-signature: bool genesis::utils::text::is_lower(char)(char) file:genesis/utils/text/char.hpp line:116
	M("genesis::utils::text").def("is_lower", (bool (*)(char)) &genesis::utils::text::is_lower, "Return whether a char is a lower case letter (a-z), ASCII-only.\n\nC++: genesis::utils::text::is_lower(char) --> bool", pybind11::arg("c"));

	// genesis::utils::text::is_upper(char) file:genesis/utils/text/char.hpp line:126
	// function-signature: bool genesis::utils::text::is_upper(char)(char) file:genesis/utils/text/char.hpp line:126
	M("genesis::utils::text").def("is_upper", (bool (*)(char)) &genesis::utils::text::is_upper, "Return whether a char is an upper case letter (A-Z), ASCII-only.\n\nC++: genesis::utils::text::is_upper(char) --> bool", pybind11::arg("c"));

	// genesis::utils::text::is_alpha(char) file:genesis/utils/text/char.hpp line:136
	// function-signature: bool genesis::utils::text::is_alpha(char)(char) file:genesis/utils/text/char.hpp line:136
	M("genesis::utils::text").def("is_alpha", (bool (*)(char)) &genesis::utils::text::is_alpha, "Return whether a char is a letter (a-z or A-Z), ASCII-only.\n\nC++: genesis::utils::text::is_alpha(char) --> bool", pybind11::arg("c"));

	// genesis::utils::text::is_alnum(char) file:genesis/utils/text/char.hpp line:144
	// function-signature: bool genesis::utils::text::is_alnum(char)(char) file:genesis/utils/text/char.hpp line:144
	M("genesis::utils::text").def("is_alnum", (bool (*)(char)) &genesis::utils::text::is_alnum, "Return whether a char is a letter (a-z or A-Z) or a digit (0-9), ASCII-only.\n\nC++: genesis::utils::text::is_alnum(char) --> bool", pybind11::arg("c"));

	// genesis::utils::text::is_punct(char) file:genesis/utils/text/char.hpp line:153
	// function-signature: bool genesis::utils::text::is_punct(char)(char) file:genesis/utils/text/char.hpp line:153
	M("genesis::utils::text").def("is_punct", (bool (*)(char)) &genesis::utils::text::is_punct, "Return whether a char is a punctuation mark, according to `ispunct` of the `cctype`\n header, but ASCII-only.\n\nC++: genesis::utils::text::is_punct(char) --> bool", pybind11::arg("c"));

	// genesis::utils::text::is_graph(char) file:genesis/utils/text/char.hpp line:167
	// function-signature: bool genesis::utils::text::is_graph(char)(char) file:genesis/utils/text/char.hpp line:167
	M("genesis::utils::text").def("is_graph", (bool (*)(char)) &genesis::utils::text::is_graph, "Return whether a char is a character with graphical representation, according to `isgraph`\n of the `cctype` header, but ASCII-only.\n\nC++: genesis::utils::text::is_graph(char) --> bool", pybind11::arg("c"));

	// genesis::utils::text::is_blank(char) file:genesis/utils/text/char.hpp line:175
	// function-signature: bool genesis::utils::text::is_blank(char)(char) file:genesis/utils/text/char.hpp line:175
	M("genesis::utils::text").def("is_blank", (bool (*)(char)) &genesis::utils::text::is_blank, "Return whether a char is either a space or a tab character.\n\nC++: genesis::utils::text::is_blank(char) --> bool", pybind11::arg("c"));

	// genesis::utils::text::is_newline(char) file:genesis/utils/text/char.hpp line:183
	// function-signature: bool genesis::utils::text::is_newline(char)(char) file:genesis/utils/text/char.hpp line:183
	M("genesis::utils::text").def("is_newline", (bool (*)(char)) &genesis::utils::text::is_newline, "Return whether a char is either a new line or a carriage return character.\n\nC++: genesis::utils::text::is_newline(char) --> bool", pybind11::arg("c"));

	// genesis::utils::text::is_other_space(char) file:genesis/utils/text/char.hpp line:192
	// function-signature: bool genesis::utils::text::is_other_space(char)(char) file:genesis/utils/text/char.hpp line:192
	M("genesis::utils::text").def("is_other_space", (bool (*)(char)) &genesis::utils::text::is_other_space, "Return whether a char is some other white space charater that is neither space, tab,\n new line, or carriage return - that is, whether it is a form feed or a vertical tab.\n\nC++: genesis::utils::text::is_other_space(char) --> bool", pybind11::arg("c"));

	// genesis::utils::text::is_space(char) file:genesis/utils/text/char.hpp line:201
	// function-signature: bool genesis::utils::text::is_space(char)(char) file:genesis/utils/text/char.hpp line:201
	M("genesis::utils::text").def("is_space", (bool (*)(char)) &genesis::utils::text::is_space, "Return whether a char is some form of white space charater, so either space, tab,\n new line, carriage return, form feed, or vertical tab.\n\nC++: genesis::utils::text::is_space(char) --> bool", pybind11::arg("c"));

	// genesis::utils::text::is_print(char) file:genesis/utils/text/char.hpp line:210
	// function-signature: bool genesis::utils::text::is_print(char)(char) file:genesis/utils/text/char.hpp line:210
	M("genesis::utils::text").def("is_print", (bool (*)(char)) &genesis::utils::text::is_print, "Return whether a char is a printable character, according to `isprint` of the `cctype`\n header, but ASCII-only.\n\nC++: genesis::utils::text::is_print(char) --> bool", pybind11::arg("c"));

	// genesis::utils::text::to_lower(char) file:genesis/utils/text/char.hpp line:222
	// function-signature: char genesis::utils::text::to_lower(char)(char) file:genesis/utils/text/char.hpp line:222
	M("genesis::utils::text").def("to_lower", (char (*)(char)) &genesis::utils::text::to_lower, "Return the lower case version of a letter, ASCII-only.\n\nC++: genesis::utils::text::to_lower(char) --> char", pybind11::arg("c"));

	// genesis::utils::text::to_upper(char) file:genesis/utils/text/char.hpp line:231
	// function-signature: char genesis::utils::text::to_upper(char)(char) file:genesis/utils/text/char.hpp line:231
	M("genesis::utils::text").def("to_upper", (char (*)(char)) &genesis::utils::text::to_upper, "Return the upper case version of a letter, ASCII-only.\n\nC++: genesis::utils::text::to_upper(char) --> char", pybind11::arg("c"));

	// genesis::utils::text::char_match_ci(char, char) file:genesis/utils/text/char.hpp line:244
	// function-signature: bool genesis::utils::text::char_match_ci(char, char)(char, char) file:genesis/utils/text/char.hpp line:244
	M("genesis::utils::text").def("char_match_ci", (bool (*)(char, char)) &genesis::utils::text::char_match_ci, "Return whether two chars are the same, case insensitive, and ASCII-only.\n\nC++: genesis::utils::text::char_match_ci(char, char) --> bool", pybind11::arg("c1"), pybind11::arg("c2"));

	// genesis::utils::text::char_to_hex(char, bool) file:genesis/utils/text/char.hpp line:269
	// function-signature: std::string genesis::utils::text::char_to_hex(char, bool)(char, bool) file:genesis/utils/text/char.hpp line:269
	M("genesis::utils::text").def("char_to_hex", [](char const & a0) -> std::string { return genesis::utils::text::char_to_hex(a0); }, "", pybind11::arg("c"));
	M("genesis::utils::text").def("char_to_hex", (std::string (*)(char, bool)) &genesis::utils::text::char_to_hex, "Return the name and hex representation of a `char`.\n\n This is meant for user output, for example to warn about invalid input characters.\n Hence, by default, a text-representation is returned, using the form\n\n     LF (0x0A; control: NL line feed, new line)\n     'N' (0x4E; letter: capital N)\n\n where non-printable characters are abbreviated by their symbol, and printable\n characters are included in single quotation marks.\n\n Non-ASCII characters (those outside of the 0-127 interval) are written in the form\n\n     non-ASCII char (0xF7)\n\n If  is set to `false`, just the two-byte hex presentation is returned (e.g., `4E` for `N`).\n\nC++: genesis::utils::text::char_to_hex(char, bool) --> std::string", pybind11::arg("c"), pybind11::arg("full"));

	// genesis::utils::text::char_to_hex(unsigned char, bool) file:genesis/utils/text/char.hpp line:277
	// function-signature: std::string genesis::utils::text::char_to_hex(unsigned char, bool)(unsigned char, bool) file:genesis/utils/text/char.hpp line:277
	M("genesis::utils::text").def("char_to_hex", [](unsigned char const & a0) -> std::string { return genesis::utils::text::char_to_hex(a0); }, "", pybind11::arg("c"));
	M("genesis::utils::text").def("char_to_hex", (std::string (*)(unsigned char, bool)) &genesis::utils::text::char_to_hex, "Return the name and hex representation of a `char`, given as an `unsigned char`.\n\n This overload is meant for situations were a text input is read from file as `unsigned char`.\n The rest of the function is identical to char_to_hex( char, bool ), see there for details.\n\nC++: genesis::utils::text::char_to_hex(unsigned char, bool) --> std::string", pybind11::arg("c"), pybind11::arg("full"));

}

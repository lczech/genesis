#include <functional>
#include <genesis/utils/text/string.hpp>
#include <iterator>
#include <memory>
#include <string>
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

void bind_genesis_utils_text_string_1(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::utils::split(const std::string &, class std::function<bool (char)>, const bool) file:genesis/utils/text/string.hpp line:321
	M("genesis::utils").def("split", [](const std::string & a0, class std::function<bool (char)> const & a1) -> std::vector<std::string, class std::allocator<std::string > > { return genesis::utils::split(a0, a1); }, "", pybind11::arg("str"), pybind11::arg("delimiter_predicate"));
	M("genesis::utils").def("split", (class std::vector<std::string, class std::allocator<std::string > > (*)(const std::string &, class std::function<bool (char)>, const bool)) &genesis::utils::split, "Spilt a  into parts, given a \n that returns `true` for delimiters chars.\n\n The  is split using any of the chars for which  is `true`,\n and returned as a vector of strings.  If `trim_empty` is set, empty strings resulting from\n adjacent delimiter chars are excluded from the output.\n\nC++: genesis::utils::split(const std::string &, class std::function<bool (char)>, const bool) --> class std::vector<std::string, class std::allocator<std::string > >", pybind11::arg("str"), pybind11::arg("delimiter_predicate"), pybind11::arg("trim_empty"));

	// genesis::utils::split_at(const std::string &, const std::string &, const bool) file:genesis/utils/text/string.hpp line:334
	M("genesis::utils").def("split_at", [](const std::string & a0, const std::string & a1) -> std::vector<std::string, class std::allocator<std::string > > { return genesis::utils::split_at(a0, a1); }, "", pybind11::arg("str"), pybind11::arg("delimiter"));
	M("genesis::utils").def("split_at", (class std::vector<std::string, class std::allocator<std::string > > (*)(const std::string &, const std::string &, const bool)) &genesis::utils::split_at, "Spilt a  into parts, given a  string.\n\n The  is split where the whole  string is found, and returned as a vector\n of strings. If `trim_empty` is set, empty strings resulting from adjacent delimiters are\n excluded from the output.\n\nC++: genesis::utils::split_at(const std::string &, const std::string &, const bool) --> class std::vector<std::string, class std::allocator<std::string > >", pybind11::arg("str"), pybind11::arg("delimiter"), pybind11::arg("trim_empty"));

	// genesis::utils::split_range_list(const std::string &) file:genesis/utils/text/string.hpp line:346
	M("genesis::utils").def("split_range_list", (class std::vector<unsigned long, class std::allocator<unsigned long> > (*)(const std::string &)) &genesis::utils::split_range_list, "Split a string containing positive interger numbers into its parts and resolve ranges.\n\n For example, the string `1, 3, 5-7, 10` results in a vector of all listed numbers and the\n ranges in between, that is `1, 3, 5, 6, 7, 10`. Whitespace is ignored; the range is sorted.\n\nC++: genesis::utils::split_range_list(const std::string &) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("str"));

	// genesis::utils::wrap(const std::string &, unsigned long) file:genesis/utils/text/string.hpp line:355
	M("genesis::utils").def("wrap", [](const std::string & a0) -> std::string { return genesis::utils::wrap(a0); }, "", pybind11::arg("text"));
	M("genesis::utils").def("wrap", (std::string (*)(const std::string &, unsigned long)) &genesis::utils::wrap, "Wrap a  at a given \n \n\nC++: genesis::utils::wrap(const std::string &, unsigned long) --> std::string", pybind11::arg("text"), pybind11::arg("line_length"));

	// genesis::utils::indent(const std::string &, const std::string &) file:genesis/utils/text/string.hpp line:367
	M("genesis::utils").def("indent", [](const std::string & a0) -> std::string { return genesis::utils::indent(a0); }, "", pybind11::arg("text"));
	M("genesis::utils").def("indent", (std::string (*)(const std::string &, const std::string &)) &genesis::utils::indent, "Indent each line of `text` with `indentation` and return the result.\n\n By default, four spaces are used to indent. Whether the text ends with a new line or not is not\n changed. Any trailing indentation chars are trimmed, in order to not have trailing whitespaces\n in the result (except for the new line, if the text ends in one).\n\nC++: genesis::utils::indent(const std::string &, const std::string &) --> std::string", pybind11::arg("text"), pybind11::arg("indentation"));

	// genesis::utils::replace_all(const std::string &, const std::string &, const std::string &) file:genesis/utils/text/string.hpp line:376
	M("genesis::utils").def("replace_all", (std::string (*)(const std::string &, const std::string &, const std::string &)) &genesis::utils::replace_all, "Return a copy of a string, where all occurrences of a search string\n are replaced by a replace string.\n\nC++: genesis::utils::replace_all(const std::string &, const std::string &, const std::string &) --> std::string", pybind11::arg("text"), pybind11::arg("search"), pybind11::arg("replace"));

	// genesis::utils::remove_all(const std::string &, const std::string &) file:genesis/utils/text/string.hpp line:385
	M("genesis::utils").def("remove_all", (std::string (*)(const std::string &, const std::string &)) &genesis::utils::remove_all, "Return a copy of a string, where all occurrences of a search string are removed.\n\nC++: genesis::utils::remove_all(const std::string &, const std::string &) --> std::string", pybind11::arg("text"), pybind11::arg("search"));

	// genesis::utils::replace_all_chars(const std::string &, const std::string &, char) file:genesis/utils/text/string.hpp line:393
	M("genesis::utils").def("replace_all_chars", (std::string (*)(const std::string &, const std::string &, char)) &genesis::utils::replace_all_chars, "Replace all occurrences of the  in  by the  char.\n\nC++: genesis::utils::replace_all_chars(const std::string &, const std::string &, char) --> std::string", pybind11::arg("text"), pybind11::arg("search_chars"), pybind11::arg("replace"));

	// genesis::utils::remove_all_chars(const std::string &, const std::string &) file:genesis/utils/text/string.hpp line:421
	M("genesis::utils").def("remove_all_chars", (std::string (*)(const std::string &, const std::string &)) &genesis::utils::remove_all_chars, "Remove all occurrences of the  in \n \n\nC++: genesis::utils::remove_all_chars(const std::string &, const std::string &) --> std::string", pybind11::arg("text"), pybind11::arg("search_chars"));

	// genesis::utils::remove_all_non_alnum(const std::string &) file:genesis/utils/text/string.hpp line:442
	M("genesis::utils").def("remove_all_non_alnum", (std::string (*)(const std::string &)) &genesis::utils::remove_all_non_alnum, "Remove all non-alphanumerical characters from a string.\n\nC++: genesis::utils::remove_all_non_alnum(const std::string &) --> std::string", pybind11::arg("text"));

	// genesis::utils::trim_right(const std::string &, const std::string &) file:genesis/utils/text/string.hpp line:448
	M("genesis::utils").def("trim_right", [](const std::string & a0) -> std::string { return genesis::utils::trim_right(a0); }, "", pybind11::arg("s"));
	M("genesis::utils").def("trim_right", (std::string (*)(const std::string &, const std::string &)) &genesis::utils::trim_right, "Return a copy of the input string, with left trimmed white spaces\n (or any other \n \n\nC++: genesis::utils::trim_right(const std::string &, const std::string &) --> std::string", pybind11::arg("s"), pybind11::arg("delimiters"));

	// genesis::utils::trim_left(const std::string &, const std::string &) file:genesis/utils/text/string.hpp line:457
	M("genesis::utils").def("trim_left", [](const std::string & a0) -> std::string { return genesis::utils::trim_left(a0); }, "", pybind11::arg("s"));
	M("genesis::utils").def("trim_left", (std::string (*)(const std::string &, const std::string &)) &genesis::utils::trim_left, "Return a copy of the input string, with right trimmed white spaces\n (or any other \n \n\nC++: genesis::utils::trim_left(const std::string &, const std::string &) --> std::string", pybind11::arg("s"), pybind11::arg("delimiters"));

	// genesis::utils::trim(const std::string &, const std::string &) file:genesis/utils/text/string.hpp line:466
	M("genesis::utils").def("trim", [](const std::string & a0) -> std::string { return genesis::utils::trim(a0); }, "", pybind11::arg("s"));
	M("genesis::utils").def("trim", (std::string (*)(const std::string &, const std::string &)) &genesis::utils::trim, "Return a copy of the input string, with trimmed white spaces\n (or any other \n \n\nC++: genesis::utils::trim(const std::string &, const std::string &) --> std::string", pybind11::arg("s"), pybind11::arg("delimiters"));

	// genesis::utils::to_lower(const std::string &) file:genesis/utils/text/string.hpp line:478
	M("genesis::utils").def("to_lower", (std::string (*)(const std::string &)) &genesis::utils::to_lower, "Return an all-lowercase copy of the given string, locale-aware.\n\nC++: genesis::utils::to_lower(const std::string &) --> std::string", pybind11::arg("str"));

	// genesis::utils::to_lower_inplace(std::string &) file:genesis/utils/text/string.hpp line:492
	M("genesis::utils").def("to_lower_inplace", (void (*)(std::string &)) &genesis::utils::to_lower_inplace, "Turn the given string to all-lowercase, locale-aware.\n\nC++: genesis::utils::to_lower_inplace(std::string &) --> void", pybind11::arg("str"));

	// genesis::utils::to_upper(const std::string &) file:genesis/utils/text/string.hpp line:502
	M("genesis::utils").def("to_upper", (std::string (*)(const std::string &)) &genesis::utils::to_upper, "Return an all-uppercase copy of the given string, locale-aware.\n\nC++: genesis::utils::to_upper(const std::string &) --> std::string", pybind11::arg("str"));

	// genesis::utils::to_upper_inplace(std::string &) file:genesis/utils/text/string.hpp line:514
	M("genesis::utils").def("to_upper_inplace", (void (*)(std::string &)) &genesis::utils::to_upper_inplace, "Turn the given string to all-uppercase, locale-aware.\n\nC++: genesis::utils::to_upper_inplace(std::string &) --> void", pybind11::arg("str"));

	// genesis::utils::to_lower_ascii_inplace(std::string &) file:genesis/utils/text/string.hpp line:524
	M("genesis::utils").def("to_lower_ascii_inplace", (void (*)(std::string &)) &genesis::utils::to_lower_ascii_inplace, "Turn the given string to all-lowercase, ASCII-only.\n\nC++: genesis::utils::to_lower_ascii_inplace(std::string &) --> void", pybind11::arg("str"));

	// genesis::utils::to_lower_ascii(const std::string &) file:genesis/utils/text/string.hpp line:529
	M("genesis::utils").def("to_lower_ascii", (std::string (*)(const std::string &)) &genesis::utils::to_lower_ascii, "Return an all-lowercase copy of the given string, ASCII-only.\n\nC++: genesis::utils::to_lower_ascii(const std::string &) --> std::string", pybind11::arg("str"));

	// genesis::utils::to_upper_ascii_inplace(std::string &) file:genesis/utils/text/string.hpp line:536
	M("genesis::utils").def("to_upper_ascii_inplace", (void (*)(std::string &)) &genesis::utils::to_upper_ascii_inplace, "Turn the given string to all-uppercase, ASCII-only, inline.\n\n Uses AVX2 if available.\n\nC++: genesis::utils::to_upper_ascii_inplace(std::string &) --> void", pybind11::arg("str"));

	// genesis::utils::to_upper_ascii(const std::string &) file:genesis/utils/text/string.hpp line:541
	M("genesis::utils").def("to_upper_ascii", (std::string (*)(const std::string &)) &genesis::utils::to_upper_ascii, "Return an all-uppercase copy of the given string, ASCII-only.\n\nC++: genesis::utils::to_upper_ascii(const std::string &) --> std::string", pybind11::arg("str"));

	// genesis::utils::escape(const std::string &) file:genesis/utils/text/string.hpp line:554
	M("genesis::utils").def("escape", (std::string (*)(const std::string &)) &genesis::utils::escape, "Return a string where special chars are replaces by their escape sequence.\n\n All new lines are transformed into either \\r or \\n, tabs into \\t.\n Double quotation marks are preceeded by a backslash, also the backslash itself will be escaped,\n so that `\"` becomes `\"` and `\\` becomes `\\`.\n\nC++: genesis::utils::escape(const std::string &) --> std::string", pybind11::arg("text"));

	// genesis::utils::deescape(const std::string &) file:genesis/utils/text/string.hpp line:567
	M("genesis::utils").def("deescape", (std::string (*)(const std::string &)) &genesis::utils::deescape, "Return a string where backslash-escaped characters are transformed into\n their respective string form.\n\n All occurrences of `backslash + char` in the string are de-escaped. That is, all ``, `` and\n `` are turned into their respective control sequences, while all other chars folloing a\n backslash are translated into the char itself (so that e.g., quotation marks or backslashes\n themself can be escaped).\n\n Also see deescape( char c ).\n\nC++: genesis::utils::deescape(const std::string &) --> std::string", pybind11::arg("text"));

	// genesis::utils::deescape(char) file:genesis/utils/text/string.hpp line:577
	M("genesis::utils").def("deescape", (char (*)(char)) &genesis::utils::deescape, "Return the de-escaped char for a backslash-escaped char.\n\n The function takes the char that follows a backslash in an escaped string and returns its\n de-escaped char. That is, `n` is turned into a new line (``), `t` is turned into a tab (``)\n and `r` is turned into a carrier return (``). All other chars (e.g., quotation marks or\n the backslash itself) are simply returned as-is.\n\nC++: genesis::utils::deescape(char) --> char", pybind11::arg("c"));

	// genesis::utils::repeat(const std::string &, unsigned long) file:genesis/utils/text/string.hpp line:586
	M("genesis::utils").def("repeat", (std::string (*)(const std::string &, unsigned long)) &genesis::utils::repeat, "Take a string and repeat it a given number of times.\n\nC++: genesis::utils::repeat(const std::string &, unsigned long) --> std::string", pybind11::arg("word"), pybind11::arg("times"));

	// genesis::utils::to_string_leading_zeros(unsigned long, unsigned long) file:genesis/utils/text/string.hpp line:594
	M("genesis::utils").def("to_string_leading_zeros", [](unsigned long const & a0) -> std::string { return genesis::utils::to_string_leading_zeros(a0); }, "", pybind11::arg("value"));
	M("genesis::utils").def("to_string_leading_zeros", (std::string (*)(unsigned long, unsigned long)) &genesis::utils::to_string_leading_zeros, "Return a string representation of a `size_t`  with a fixed length, that is,\n by adding leading zeros.\n\n If  is already longer than  the result will also be longer.\n\nC++: genesis::utils::to_string_leading_zeros(unsigned long, unsigned long) --> std::string", pybind11::arg("value"), pybind11::arg("length"));

	// genesis::utils::to_string_precise(double, int) file:genesis/utils/text/string.hpp line:607
	M("genesis::utils").def("to_string_precise", [](double const & a0) -> std::string { return genesis::utils::to_string_precise(a0); }, "", pybind11::arg("value"));
	M("genesis::utils").def("to_string_precise", (std::string (*)(double, int)) &genesis::utils::to_string_precise, "Return a precise string representation of the input value, using the provided precision\n value (determining its decimal places).\n\n This function rounds the value to the given precision, and then returns its string representation\n with possible trailing zeros. Thus, it uses fixed precision. This is useful for e.g., output\n in a table format.\n\n For a version of this function that truncates trailing zeros, see to_string_rounded().\n Also, see to_string() if you simply want to output a string representation of a double.\n\nC++: genesis::utils::to_string_precise(double, int) --> std::string", pybind11::arg("value"), pybind11::arg("precision"));

	// genesis::utils::to_string_rounded(double, int) file:genesis/utils/text/string.hpp line:620
	M("genesis::utils").def("to_string_rounded", [](double const & a0) -> std::string { return genesis::utils::to_string_rounded(a0); }, "", pybind11::arg("value"));
	M("genesis::utils").def("to_string_rounded", (std::string (*)(double, int)) &genesis::utils::to_string_rounded, "Return a string representation of the input value, using the provided precision value\n (determining its decimal places) to round, and truncate trailing zeros.\n\n This function rounds the value to the given precision, and then returns its string representation\n without trailing zeros. This is useful for output that keeps a certain amount of significant\n decimal digits, while making the output as short as possible.\n\n If you want to round, but also keep trailing zeros, see to_string_precise().\n Also, see to_string() if you simply want to output a string representation of a double.\n\nC++: genesis::utils::to_string_rounded(double, int) --> std::string", pybind11::arg("value"), pybind11::arg("precision"));

}

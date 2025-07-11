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

void bind_genesis_utils_text_string(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::utils::contains_ci(const class std::vector<std::string, class std::allocator<std::string > > &, const std::string &) file:genesis/utils/text/string.hpp line:55
	M("genesis::utils").def("contains_ci", (bool (*)(const class std::vector<std::string, class std::allocator<std::string > > &, const std::string &)) &genesis::utils::contains_ci, "Return whether a vector of strings contains a given string, case insensitive.\n\nC++: genesis::utils::contains_ci(const class std::vector<std::string, class std::allocator<std::string > > &, const std::string &) --> bool", pybind11::arg("haystack"), pybind11::arg("needle"));

	// genesis::utils::contains_ci_alnum(const class std::vector<std::string, class std::allocator<std::string > > &, const std::string &) file:genesis/utils/text/string.hpp line:61
	M("genesis::utils").def("contains_ci_alnum", (bool (*)(const class std::vector<std::string, class std::allocator<std::string > > &, const std::string &)) &genesis::utils::contains_ci_alnum, "Return whether a vector of strings contains a given string, case insensitive,\n and ignoring all non-alphanumerical characters.\n\nC++: genesis::utils::contains_ci_alnum(const class std::vector<std::string, class std::allocator<std::string > > &, const std::string &) --> bool", pybind11::arg("haystack"), pybind11::arg("needle"));

	// genesis::utils::strcasecmp(const char *, const char *) file:genesis/utils/text/string.hpp line:73
	M("genesis::utils").def("strcasecmp", (int (*)(const char *, const char *)) &genesis::utils::strcasecmp, "Compares two strings, ignoring case differences.\n\n Compares the null-terminated strings  and  returning -1, 0, or 1\n if  is lexicographically less than, equal to, or greater than  over those characters,\n respectively.\n\n This is equivalent to the POSIX function of the same name, found in `strings.h`,\n but included here for portability.\n\nC++: genesis::utils::strcasecmp(const char *, const char *) --> int", pybind11::arg("s1"), pybind11::arg("s2"));

	// genesis::utils::strncasecmp(const char *, const char *, unsigned long) file:genesis/utils/text/string.hpp line:85
	M("genesis::utils").def("strncasecmp", (int (*)(const char *, const char *, unsigned long)) &genesis::utils::strncasecmp, "Compares up to  chars of two strings, ignoring case differences.\n\n Compares up to  chars of the null-terminated strings  and  returning -1, 0, or 1\n if  is lexicographically less than, equal to, or greater than  over those characters,\n respectively.\n\n This is equivalent to the POSIX function of the same name, found in `strings.h`,\n but included here for portability.\n\nC++: genesis::utils::strncasecmp(const char *, const char *, unsigned long) --> int", pybind11::arg("s1"), pybind11::arg("s2"), pybind11::arg("n"));

	// genesis::utils::equals_ci(const std::string &, const std::string &) file:genesis/utils/text/string.hpp line:90
	M("genesis::utils").def("equals_ci", (bool (*)(const std::string &, const std::string &)) &genesis::utils::equals_ci, "Compare two strings, case insensitive.\n\nC++: genesis::utils::equals_ci(const std::string &, const std::string &) --> bool", pybind11::arg("lhs"), pybind11::arg("rhs"));

	// genesis::utils::equals_ci_alnum(const std::string &, const std::string &) file:genesis/utils/text/string.hpp line:95
	M("genesis::utils").def("equals_ci_alnum", (bool (*)(const std::string &, const std::string &)) &genesis::utils::equals_ci_alnum, "Compare two strings, case insensitive, and ignoring all non-alphanumerical characters.\n\nC++: genesis::utils::equals_ci_alnum(const std::string &, const std::string &) --> bool", pybind11::arg("lhs"), pybind11::arg("rhs"));

	// genesis::utils::starts_with(const std::string &, const std::string &) file:genesis/utils/text/string.hpp line:100
	M("genesis::utils").def("starts_with", (bool (*)(const std::string &, const std::string &)) &genesis::utils::starts_with, "Return whether a string starts with another string, i.e., check for a \n \n\nC++: genesis::utils::starts_with(const std::string &, const std::string &) --> bool", pybind11::arg("text"), pybind11::arg("prefix"));

	// genesis::utils::starts_with(const std::string &, const std::string &, std::string &) file:genesis/utils/text/string.hpp line:109
	M("genesis::utils").def("starts_with", (bool (*)(const std::string &, const std::string &, std::string &)) &genesis::utils::starts_with, "Return whether a string starts with another string, i.e., check for a \n\n This version of the function then stores the  of the matched \n containing all remaining characters that did not match the  pattern.\n If there is no match, the  is left unchanged.\n\nC++: genesis::utils::starts_with(const std::string &, const std::string &, std::string &) --> bool", pybind11::arg("text"), pybind11::arg("prefix"), pybind11::arg("suffix"));

	// genesis::utils::starts_with_ci(const std::string &, const std::string &) file:genesis/utils/text/string.hpp line:115
	M("genesis::utils").def("starts_with_ci", (bool (*)(const std::string &, const std::string &)) &genesis::utils::starts_with_ci, "Return whether a string starts with another string, i.e., check for a \n case insensitive.\n\nC++: genesis::utils::starts_with_ci(const std::string &, const std::string &) --> bool", pybind11::arg("text"), pybind11::arg("prefix"));

	// genesis::utils::starts_with_ci(const std::string &, const std::string &, std::string &) file:genesis/utils/text/string.hpp line:123
	M("genesis::utils").def("starts_with_ci", (bool (*)(const std::string &, const std::string &, std::string &)) &genesis::utils::starts_with_ci, "Return whether a string starts with another string, i.e., check for a \n case insensitive.\n\n  starts_with( std::string const&, std::string const&, std::string& )\n\nC++: genesis::utils::starts_with_ci(const std::string &, const std::string &, std::string &) --> bool", pybind11::arg("text"), pybind11::arg("prefix"), pybind11::arg("suffix"));

	// genesis::utils::starts_with_ci_alnum(const std::string &, const std::string &) file:genesis/utils/text/string.hpp line:131
	M("genesis::utils").def("starts_with_ci_alnum", (bool (*)(const std::string &, const std::string &)) &genesis::utils::starts_with_ci_alnum, "Return whether a string starts with another string (prefix), comparing case-independent,\n and ignoring all non-alphanumerical characters.\n\n For example, `\"REF_CNT.S2-1.sorted:1\"` starts with `\"[ref-cnt]\"`, according to this function.\n\nC++: genesis::utils::starts_with_ci_alnum(const std::string &, const std::string &) --> bool", pybind11::arg("text"), pybind11::arg("prefix"));

	// genesis::utils::starts_with_ci_alnum(const std::string &, const std::string &, std::string &, bool) file:genesis/utils/text/string.hpp line:145
	M("genesis::utils").def("starts_with_ci_alnum", [](const std::string & a0, const std::string & a1, std::string & a2) -> bool { return genesis::utils::starts_with_ci_alnum(a0, a1, a2); }, "", pybind11::arg("text"), pybind11::arg("prefix"), pybind11::arg("suffix"));
	M("genesis::utils").def("starts_with_ci_alnum", (bool (*)(const std::string &, const std::string &, std::string &, bool)) &genesis::utils::starts_with_ci_alnum, "starts_with_ci_alnum( std::string const& , std::string const& )\n\n This version of the function then stores the  of the matched \n containing all remaining characters that did not match the  pattern.\n If there is no match, the  is left unchanged.\n\n If additionally  is set, all leading non-alphanumerical characters of the\n  are removed.\n For example, `\"REF_CNT.S2-1.sorted:1\"` starts with `\"[ref-cnt]\"`, and then contains the suffix\n `\".S2-1.sorted:1\"` - with  this is reduced to `\"S2-1.sorted:1\"` instead.\n\nC++: genesis::utils::starts_with_ci_alnum(const std::string &, const std::string &, std::string &, bool) --> bool", pybind11::arg("text"), pybind11::arg("prefix"), pybind11::arg("suffix"), pybind11::arg("trim_suffix"));

	// genesis::utils::ends_with(const std::string &, const std::string &) file:genesis/utils/text/string.hpp line:155
	M("genesis::utils").def("ends_with", (bool (*)(const std::string &, const std::string &)) &genesis::utils::ends_with, "Return whether a string ends with another string, i.e., check for a \n \n\nC++: genesis::utils::ends_with(const std::string &, const std::string &) --> bool", pybind11::arg("text"), pybind11::arg("suffix"));

	// genesis::utils::ends_with(const std::string &, const std::string &, std::string &) file:genesis/utils/text/string.hpp line:164
	M("genesis::utils").def("ends_with", (bool (*)(const std::string &, const std::string &, std::string &)) &genesis::utils::ends_with, "Return whether a string ends with another string, i.e., check for a \n\n This version of the function then stores the  of the matched \n containing all leading characters that did not match the  pattern.\n If there is no match, the  is left unchanged.\n\nC++: genesis::utils::ends_with(const std::string &, const std::string &, std::string &) --> bool", pybind11::arg("text"), pybind11::arg("suffix"), pybind11::arg("prefix"));

	// genesis::utils::ends_with_ci(const std::string &, const std::string &) file:genesis/utils/text/string.hpp line:170
	M("genesis::utils").def("ends_with_ci", (bool (*)(const std::string &, const std::string &)) &genesis::utils::ends_with_ci, "Return whether a string ends with another string, i.e., check for a suffix,\n case insensitive.\n\nC++: genesis::utils::ends_with_ci(const std::string &, const std::string &) --> bool", pybind11::arg("text"), pybind11::arg("suffix"));

	// genesis::utils::ends_with_ci(const std::string &, const std::string &, std::string &) file:genesis/utils/text/string.hpp line:178
	M("genesis::utils").def("ends_with_ci", (bool (*)(const std::string &, const std::string &, std::string &)) &genesis::utils::ends_with_ci, "Return whether a string ends with another string, i.e., check for a suffix,\n case insensitive.\n\n  ends_with( std::string const&, std::string const&, std::string& )\n\nC++: genesis::utils::ends_with_ci(const std::string &, const std::string &, std::string &) --> bool", pybind11::arg("text"), pybind11::arg("suffix"), pybind11::arg("prefix"));

	// genesis::utils::ends_with_ci_alnum(const std::string &, const std::string &) file:genesis/utils/text/string.hpp line:186
	M("genesis::utils").def("ends_with_ci_alnum", (bool (*)(const std::string &, const std::string &)) &genesis::utils::ends_with_ci_alnum, "Return whether a string ends with another string (suffix), comparing case-independent,\n and ignoring all non-alphanumerical characters.\n\n For example, `\"S2-1.sorted:1.REF_CNT\"` ends with `\"[ref-cnt]\"`, according to this function.\n\nC++: genesis::utils::ends_with_ci_alnum(const std::string &, const std::string &) --> bool", pybind11::arg("text"), pybind11::arg("suffix"));

	// genesis::utils::ends_with_ci_alnum(const std::string &, const std::string &, std::string &, bool) file:genesis/utils/text/string.hpp line:200
	M("genesis::utils").def("ends_with_ci_alnum", [](const std::string & a0, const std::string & a1, std::string & a2) -> bool { return genesis::utils::ends_with_ci_alnum(a0, a1, a2); }, "", pybind11::arg("text"), pybind11::arg("suffix"), pybind11::arg("prefix"));
	M("genesis::utils").def("ends_with_ci_alnum", (bool (*)(const std::string &, const std::string &, std::string &, bool)) &genesis::utils::ends_with_ci_alnum, "ends_with_ci_alnum( std::string const& , std::string const& )\n\n This version of the function then stores the  of the matched \n containing all leading characters that did not match the  pattern.\n If there is no match, the  is left unchanged.\n\n If additionally  is set, all trailing non-alphanumerical characters of the\n  are removed.\n For example, `\"S2-1.sorted:1.REF_CNT\"` ends with `\"[ref-cnt]\"`, and then contains the prefix\n `\"S2-1.sorted:1.\"` - with  this is reduced to `\"S2-1.sorted:1\"` instead.\n\nC++: genesis::utils::ends_with_ci_alnum(const std::string &, const std::string &, std::string &, bool) --> bool", pybind11::arg("text"), pybind11::arg("suffix"), pybind11::arg("prefix"), pybind11::arg("trim_prefix"));

	// genesis::utils::match_wildcards(const std::string &, const std::string &) file:genesis/utils/text/string.hpp line:211
	M("genesis::utils").def("match_wildcards", (bool (*)(const std::string &, const std::string &)) &genesis::utils::match_wildcards, "Return whether a string is matched by a wildcard pattern containing `?` and `*`\n for single and mutliple (0 or more) wildcard characters, respectively.\n\nC++: genesis::utils::match_wildcards(const std::string &, const std::string &) --> bool", pybind11::arg("str"), pybind11::arg("pattern"));

	// genesis::utils::compare_natural(const std::string &, const std::string &) file:genesis/utils/text/string.hpp line:219
	M("genesis::utils").def("compare_natural", (int (*)(const std::string &, const std::string &)) &genesis::utils::compare_natural, "Compare two strings with natural human sorting, that is \"A1\", \"A2\", \"A100\", instead of the\n standard sort by ASCII value \"A1\", \"A100\", \"A2\".\n\n Returns negavie if  <  `0` if they are equal, and positive if  > \n \n\nC++: genesis::utils::compare_natural(const std::string &, const std::string &) --> int", pybind11::arg("lhs"), pybind11::arg("rhs"));

	// genesis::utils::head(const std::string &, unsigned long) file:genesis/utils/text/string.hpp line:271
	M("genesis::utils").def("head", [](const std::string & a0) -> std::string { return genesis::utils::head(a0); }, "", pybind11::arg("text"));
	M("genesis::utils").def("head", (std::string (*)(const std::string &, unsigned long)) &genesis::utils::head, "Return the first lines of the text.\n\nC++: genesis::utils::head(const std::string &, unsigned long) --> std::string", pybind11::arg("text"), pybind11::arg("lines"));

	// genesis::utils::tail(const std::string &, unsigned long) file:genesis/utils/text/string.hpp line:276
	M("genesis::utils").def("tail", [](const std::string & a0) -> std::string { return genesis::utils::tail(a0); }, "", pybind11::arg("text"));
	M("genesis::utils").def("tail", (std::string (*)(const std::string &, unsigned long)) &genesis::utils::tail, "Return the last lines of the text.\n\nC++: genesis::utils::tail(const std::string &, unsigned long) --> std::string", pybind11::arg("text"), pybind11::arg("lines"));

	// genesis::utils::count_substring_occurrences(const std::string &, const std::string &) file:genesis/utils/text/string.hpp line:285
	M("genesis::utils").def("count_substring_occurrences", (unsigned long (*)(const std::string &, const std::string &)) &genesis::utils::count_substring_occurrences, "Return the number of (possibly overlapping) occurrences of a substring in a string.\n\nC++: genesis::utils::count_substring_occurrences(const std::string &, const std::string &) --> unsigned long", pybind11::arg("str"), pybind11::arg("sub"));

	// genesis::utils::split(const std::string &, char, const bool) file:genesis/utils/text/string.hpp line:294
	M("genesis::utils").def("split", [](const std::string & a0) -> std::vector<std::string, class std::allocator<std::string > > { return genesis::utils::split(a0); }, "", pybind11::arg("str"));
	M("genesis::utils").def("split", [](const std::string & a0, char const & a1) -> std::vector<std::string, class std::allocator<std::string > > { return genesis::utils::split(a0, a1); }, "", pybind11::arg("str"), pybind11::arg("delimiter"));
	M("genesis::utils").def("split", (class std::vector<std::string, class std::allocator<std::string > > (*)(const std::string &, char, const bool)) &genesis::utils::split, "Spilt a  into parts, given a  char.\n\n The  is split using the  char, and returned as a vector\n of strings. If `trim_empty` is set, empty strings resulting from adjacent delimiter chars are\n excluded from the output.\n\nC++: genesis::utils::split(const std::string &, char, const bool) --> class std::vector<std::string, class std::allocator<std::string > >", pybind11::arg("str"), pybind11::arg("delimiter"), pybind11::arg("trim_empty"));

	// genesis::utils::split(const std::string &, const std::string &, const bool) file:genesis/utils/text/string.hpp line:307
	M("genesis::utils").def("split", [](const std::string & a0, const std::string & a1) -> std::vector<std::string, class std::allocator<std::string > > { return genesis::utils::split(a0, a1); }, "", pybind11::arg("str"), pybind11::arg("delimiters"));
	M("genesis::utils").def("split", (class std::vector<std::string, class std::allocator<std::string > > (*)(const std::string &, const std::string &, const bool)) &genesis::utils::split, "Spilt a  into parts, given a  set of chars.\n\n The  is split using any of the chars in  and returned as a vector\n of strings. If `trim_empty` is set, empty strings resulting from adjacent delimiter chars are\n excluded from the output.\n\nC++: genesis::utils::split(const std::string &, const std::string &, const bool) --> class std::vector<std::string, class std::allocator<std::string > >", pybind11::arg("str"), pybind11::arg("delimiters"), pybind11::arg("trim_empty"));

}

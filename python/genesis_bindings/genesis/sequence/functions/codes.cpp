#include <functional>
#include <genesis/sequence/functions/codes.hpp>
#include <genesis/utils/color/color.hpp>
#include <iterator>
#include <map>
#include <memory>
#include <string>
#include <utility>

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

void bind_genesis_sequence_functions_codes(std::function< pybind11::module &(std::string const &namespace_) > &M)
{

	extend_quality(M("genesis::sequence"));
	// genesis::sequence::nucleic_acid_codes_plain() file:genesis/sequence/functions/codes.hpp line:63
	M("genesis::sequence").def("nucleic_acid_codes_plain", (std::string (*)()) &genesis::sequence::nucleic_acid_codes_plain, "Return all plain nucleic acid codes. Those are `ACGTU`.\n\nC++: genesis::sequence::nucleic_acid_codes_plain() --> std::string");

	// genesis::sequence::nucleic_acid_codes_degenerated() file:genesis/sequence/functions/codes.hpp line:68
	M("genesis::sequence").def("nucleic_acid_codes_degenerated", (std::string (*)()) &genesis::sequence::nucleic_acid_codes_degenerated, "Return all degenerated nucleic acid codes. Those are `WSMKRYBDHV`.\n\nC++: genesis::sequence::nucleic_acid_codes_degenerated() --> std::string");

	// genesis::sequence::nucleic_acid_codes_undetermined() file:genesis/sequence/functions/codes.hpp line:73
	M("genesis::sequence").def("nucleic_acid_codes_undetermined", (std::string (*)()) &genesis::sequence::nucleic_acid_codes_undetermined, "Return all undetermined nucleic acid codes. Those are `NOX.-?`.\n\nC++: genesis::sequence::nucleic_acid_codes_undetermined() --> std::string");

	// genesis::sequence::nucleic_acid_codes_undetermined_letters() file:genesis/sequence/functions/codes.hpp line:79
	M("genesis::sequence").def("nucleic_acid_codes_undetermined_letters", (std::string (*)()) &genesis::sequence::nucleic_acid_codes_undetermined_letters, "Return all undetermined nucleic acid codes that are letters. Those are `NOX`, that is,\n excluding `.-?` as compared to nucleic_acid_codes_undetermined().\n\nC++: genesis::sequence::nucleic_acid_codes_undetermined_letters() --> std::string");

	// genesis::sequence::nucleic_acid_codes_all() file:genesis/sequence/functions/codes.hpp line:84
	M("genesis::sequence").def("nucleic_acid_codes_all", (std::string (*)()) &genesis::sequence::nucleic_acid_codes_all, "Return all valid nucleic acid codes. Those are `ACGTUWSMKRYBDHVNOX.-?`.\n\nC++: genesis::sequence::nucleic_acid_codes_all() --> std::string");

	// genesis::sequence::nucleic_acid_codes_all_letters() file:genesis/sequence/functions/codes.hpp line:90
	M("genesis::sequence").def("nucleic_acid_codes_all_letters", (std::string (*)()) &genesis::sequence::nucleic_acid_codes_all_letters, "Return all valid nucleic acid codes. Those are `ACGTUWSMKRYBDHVNOX`, that is,\n excluding `.-?` as compared to nucleic_acid_codes_all().\n\nC++: genesis::sequence::nucleic_acid_codes_all_letters() --> std::string");

	// genesis::sequence::amino_acid_codes_plain() file:genesis/sequence/functions/codes.hpp line:99
	M("genesis::sequence").def("amino_acid_codes_plain", (std::string (*)()) &genesis::sequence::amino_acid_codes_plain, "Return all plain amino acid codes. Those are `ACDEFGHIKLMNOPQRSTUVWY`.\n\nC++: genesis::sequence::amino_acid_codes_plain() --> std::string");

	// genesis::sequence::amino_acid_codes_degenerated() file:genesis/sequence/functions/codes.hpp line:104
	M("genesis::sequence").def("amino_acid_codes_degenerated", (std::string (*)()) &genesis::sequence::amino_acid_codes_degenerated, "Return all degenerated amino acid codes. Those are `BJZ`.\n\nC++: genesis::sequence::amino_acid_codes_degenerated() --> std::string");

	// genesis::sequence::amino_acid_codes_undetermined() file:genesis/sequence/functions/codes.hpp line:113
	M("genesis::sequence").def("amino_acid_codes_undetermined", (std::string (*)()) &genesis::sequence::amino_acid_codes_undetermined, "Return all undetermined amino acid codes, and `.`. Those are `X*-?.`.\n\n The typical undetermined codes for amino acids are `X*-?`, but some programs (e.g., hmmalign)\n also use the `.`, borrowed from nucleotide codes, as an undetermined character. Hence, we also\n support this here.\n\nC++: genesis::sequence::amino_acid_codes_undetermined() --> std::string");

	// genesis::sequence::amino_acid_codes_all() file:genesis/sequence/functions/codes.hpp line:121
	M("genesis::sequence").def("amino_acid_codes_all", (std::string (*)()) &genesis::sequence::amino_acid_codes_all, "Return all valid amino acid codes, and `.`. Those are `ACDEFGHIKLMNOPQRSTUVWYBJZX*-?.`.\n\n See amino_acid_codes_undetermined() for the reason that `.` is included here. Basically,\n bioinformatics standards are a mess.\n\nC++: genesis::sequence::amino_acid_codes_all() --> std::string");

	// genesis::sequence::normalize_code_alphabet(const std::string &) file:genesis/sequence/functions/codes.hpp line:134
	M("genesis::sequence").def("normalize_code_alphabet", (std::string (*)(const std::string &)) &genesis::sequence::normalize_code_alphabet, "Normalize an alphabet set of Sequence codes, i.e., make them upper case, sort them,\n and remove duplicates.\n\n For example, when given a set of nucleic acid codes like `aGtc`, the function returns `ACGT`.\n This is useful to get consistent codes in functions that accept a user defined code alphabet.\n\nC++: genesis::sequence::normalize_code_alphabet(const std::string &) --> std::string", pybind11::arg("alphabet"));

	// genesis::sequence::normalize_nucleic_acid_code(char, bool) file:genesis/sequence/functions/codes.hpp line:149
	M("genesis::sequence").def("normalize_nucleic_acid_code", [](char const & a0) -> char { return genesis::sequence::normalize_nucleic_acid_code(a0); }, "", pybind11::arg("code"));
	M("genesis::sequence").def("normalize_nucleic_acid_code", (char (*)(char, bool)) &genesis::sequence::normalize_nucleic_acid_code, "Normalize a nucleic acide code.\n\n That is, make it upper case, replace `U` by `T`, replace all undetermined chars by `-`.\n See nucleic_acid_codes_undetermined() for a list of the latter.\n\n If  is set to `true` (default), degenerated chars are just put to\n upper case, but otherwise left as they are. If set to `false`, an exception is thrown\n if a degenerated char is encountered. See nucleic_acid_codes_degenerated() for their list.\n\n Lastly, an exception is also thrown for non nucleic acid codes,\n that is all chars that are not part of nucleic_acid_codes_all().\n\nC++: genesis::sequence::normalize_nucleic_acid_code(char, bool) --> char", pybind11::arg("code"), pybind11::arg("accept_degenerated"));

	// genesis::sequence::normalize_amino_acid_code(char, bool) file:genesis/sequence/functions/codes.hpp line:164
	M("genesis::sequence").def("normalize_amino_acid_code", [](char const & a0) -> char { return genesis::sequence::normalize_amino_acid_code(a0); }, "", pybind11::arg("code"));
	M("genesis::sequence").def("normalize_amino_acid_code", (char (*)(char, bool)) &genesis::sequence::normalize_amino_acid_code, "Normalize an amino acid code.\n\n That is, make it upper case and replace all undetermined chars by `-`.\n See amino_acid_codes_undetermined() for a list of the latter.\n\n If  is set to `true` (default), degenerated chars are just put to\n upper case, but otherwise left as they are. If set to `false`, an exception is thrown\n if a degenerated char is encountered. See amino_acid_codes_degenerated() for their list.\n\n Lastly, an exception is also thrown for non amino acid codes,\n that is all chars that are not part of amino_acid_codes_all().\n\nC++: genesis::sequence::normalize_amino_acid_code(char, bool) --> char", pybind11::arg("code"), pybind11::arg("accept_degenerated"));

	// genesis::sequence::reverse_complement(const std::string &, bool) file:genesis/sequence/functions/codes.hpp line:176
	M("genesis::sequence").def("reverse_complement", [](const std::string & a0) -> std::string { return genesis::sequence::reverse_complement(a0); }, "", pybind11::arg("sequence"));
	M("genesis::sequence").def("reverse_complement", (std::string (*)(const std::string &, bool)) &genesis::sequence::reverse_complement, "Get the reverse complement of a nucleic acid sequence.\n\n That is, reverse the string and flip `A` with `T` and `C` with `G`.\n Gap characters are normalized to `-`, and an exception is thrown for invalid characters.\n\n If furthermore  is `true` (default), degenerated codes are also flipped.\n For example `M == AC` becomes `K == TG`, `W == AT` stays the same, and `B == CGT` becomes\n `V = GCA`. If set to `false`, an exception is thrown when degenerated chars are found.\n\nC++: genesis::sequence::reverse_complement(const std::string &, bool) --> std::string", pybind11::arg("sequence"), pybind11::arg("accept_degenerated"));

	// genesis::sequence::nucleic_acid_transition(char) file:genesis/sequence/functions/codes.hpp line:184
	M("genesis::sequence").def("nucleic_acid_transition", (char (*)(char)) &genesis::sequence::nucleic_acid_transition, "Return the transition base for the given base.\n\n This yields the transition bases, that is `A <-> G` and `C <-> T`, respecitvely.\n Any other char will throw an exception.\n\nC++: genesis::sequence::nucleic_acid_transition(char) --> char", pybind11::arg("code"));

	// genesis::sequence::nucleic_acid_code_containment(char, char, bool) file:genesis/sequence/functions/codes.hpp line:195
	M("genesis::sequence").def("nucleic_acid_code_containment", [](char const & a0, char const & a1) -> bool { return genesis::sequence::nucleic_acid_code_containment(a0, a1); }, "", pybind11::arg("a"), pybind11::arg("b"));
	M("genesis::sequence").def("nucleic_acid_code_containment", (bool (*)(char, char, bool)) &genesis::sequence::nucleic_acid_code_containment, "Compare two nucleic acid codes and check if they are equal, taking degenerated/ambiguous\n characters into account.\n\n That is, 'A' and 'W' yield `true`, as 'W' contains 'A' and 'T'.\n The order and casing of the input does not matter.\n The parameter  selects how undetermined characters (\"NOX.-?\") are\n treated: if set to `true` (default), they match ALL other chars, if set to `false`, they match none.\n\nC++: genesis::sequence::nucleic_acid_code_containment(char, char, bool) --> bool", pybind11::arg("a"), pybind11::arg("b"), pybind11::arg("undetermined_matches_all"));

	// genesis::sequence::nucleic_acid_text_colors() file:genesis/sequence/functions/codes.hpp line:207
	M("genesis::sequence").def("nucleic_acid_text_colors", (class std::map<char, std::string, struct std::less<char>, class std::allocator<struct std::pair<const char, std::string > > > (*)()) &genesis::sequence::nucleic_acid_text_colors, "Return a map of text colors for each nucleic acid code.\n\n This function gives a color name usable for utils::Style for each nucleic acid code.\n The return value of this function can for example be used in sequence::print_color() function.\n\nC++: genesis::sequence::nucleic_acid_text_colors() --> class std::map<char, std::string, struct std::less<char>, class std::allocator<struct std::pair<const char, std::string > > >");

	// genesis::sequence::amino_acid_text_colors() file:genesis/sequence/functions/codes.hpp line:215
	M("genesis::sequence").def("amino_acid_text_colors", (class std::map<char, std::string, struct std::less<char>, class std::allocator<struct std::pair<const char, std::string > > > (*)()) &genesis::sequence::amino_acid_text_colors, "Return a map of text colors for each amino acid code.\n\n This function gives a color name usable for utils::Style for each amino acid code.\n The return value of this function can for example be used in sequence::print_color() function.\n\nC++: genesis::sequence::amino_acid_text_colors() --> class std::map<char, std::string, struct std::less<char>, class std::allocator<struct std::pair<const char, std::string > > >");

	// genesis::sequence::nucleic_acid_colors() file:genesis/sequence/functions/codes.hpp line:222
	M("genesis::sequence").def("nucleic_acid_colors", (class std::map<char, class genesis::utils::Color, struct std::less<char>, class std::allocator<struct std::pair<const char, class genesis::utils::Color> > > (*)()) &genesis::sequence::nucleic_acid_colors, "Return a map of Color%s for each nucleic acid code.\n\n This function gives a Color for each nucleic acid code.\n\nC++: genesis::sequence::nucleic_acid_colors() --> class std::map<char, class genesis::utils::Color, struct std::less<char>, class std::allocator<struct std::pair<const char, class genesis::utils::Color> > >");

	// genesis::sequence::amino_acid_colors() file:genesis/sequence/functions/codes.hpp line:229
	M("genesis::sequence").def("amino_acid_colors", (class std::map<char, class genesis::utils::Color, struct std::less<char>, class std::allocator<struct std::pair<const char, class genesis::utils::Color> > > (*)()) &genesis::sequence::amino_acid_colors, "Return a map of Color%s for each amino acid code.\n\n This function gives a Color for each amino acid code.\n\nC++: genesis::sequence::amino_acid_colors() --> class std::map<char, class genesis::utils::Color, struct std::less<char>, class std::allocator<struct std::pair<const char, class genesis::utils::Color> > >");

	// genesis::sequence::nucleic_acid_name(char) file:genesis/sequence/functions/codes.hpp line:265
	M("genesis::sequence").def("nucleic_acid_name", (std::string (*)(char)) &genesis::sequence::nucleic_acid_name, "Get the name of a nucleic acid given its IUPAC code.\n\n The codes are translated as follows:\n\n     A Adenine\n     C Cytosine\n     G Guanine\n     T Thymine\n     U Uracil\n     W Weak\n     S Strong\n     M aMino\n     K Keto\n     R puRine\n     Y pYrimidine\n     B not A\n     D not C\n     H not G\n     V not T\n     N any\n     O omitted\n     X masked\n     . gap\n     - gap\n     ? gap\n\n The code char is treated case-insensitive. If the given code char is not valid, an\n `std::out_of_range` exception is thrown.\n\nC++: genesis::sequence::nucleic_acid_name(char) --> std::string", pybind11::arg("code"));

	// genesis::sequence::amino_acid_name(char) file:genesis/sequence/functions/codes.hpp line:305
	M("genesis::sequence").def("amino_acid_name", (std::string (*)(char)) &genesis::sequence::amino_acid_name, "Get the name of a amino acid given its IUPAC code.\n\n The codes are translated as follows:\n\n     A Alanine\n     B Aspartic acid or Asparagine\n     C Cysteine\n     D Aspartic acid\n     E Glutamic acid\n     F Phenylalanine\n     G Glycine\n     H Histidine\n     I Isoleucine\n     J Leucine or Isoleucine\n     K Lysine\n     L Leucine\n     M Methionine\n     N Asparagine\n     O Pyrrolysine\n     P Proline\n     Q Glutamine\n     R Arginine\n     S Serine\n     T Threonine\n     U Selenocysteine\n     V Valine\n     W Tryptophan\n     Y Tyrosine\n     Z Glutamic acid or Glutamine\n     X any\n     * translation stop\n     - gap\n     ? gap\n\n The code char is treated case-insensitive. If the given code char is not valid, an\n `std::out_of_range` exception is thrown.\n\nC++: genesis::sequence::amino_acid_name(char) --> std::string", pybind11::arg("code"));

	// genesis::sequence::nucleic_acid_ambiguities(char) file:genesis/sequence/functions/codes.hpp line:344
	M("genesis::sequence").def("nucleic_acid_ambiguities", (std::string (*)(char)) &genesis::sequence::nucleic_acid_ambiguities, "Return the possible ambiguous nucleic acid codes for a given code char.\n\n The codes are resolved as follows:\n\n     'A' ==> \"A\"\n     'C' ==> \"C\"\n     'G' ==> \"G\"\n     'T' ==> \"T\"\n     'U' ==> \"T\"\n\n     'W' ==> \"AT\"\n     'S' ==> \"CG\"\n     'M' ==> \"AC\"\n     'K' ==> \"GT\"\n     'R' ==> \"AG\"\n     'Y' ==> \"CT\"\n\n     'B' ==> \"CGT\"\n     'D' ==> \"AGT\"\n     'H' ==> \"ACT\"\n     'V' ==> \"ACG\"\n\n     'N' ==> \"ACGT\"\n     'O' ==> \"-\"\n     'X' ==> \"-\"\n     '.' ==> \"-\"\n     '-' ==> \"-\"\n     '?' ==> \"-\"\n\n The code char is treated case-insensitive. If the given code char is not valid, an\n `std::out_of_range` exception is thrown.\n\n See nucleic_acid_ambiguity_code() for a reverse version of this function. It is however not\n exactly the reverse, as some degenerated codes are mapped to the gap char. Thus, this function\n is not injective.\n\nC++: genesis::sequence::nucleic_acid_ambiguities(char) --> std::string", pybind11::arg("code"));

	// genesis::sequence::nucleic_acid_ambiguity_code(std::string) file:genesis/sequence/functions/codes.hpp line:377
	M("genesis::sequence").def("nucleic_acid_ambiguity_code", (char (*)(std::string)) &genesis::sequence::nucleic_acid_ambiguity_code, "Return the nucleic acid code that represents all given `codes`.\n\n The codes are resolved as follows:\n\n     \"A\"    ==> 'A'\n     \"C\"    ==> 'C'\n     \"G\"    ==> 'G'\n     \"T\"    ==> 'T'\n\n     \"AT\"   ==> 'W'\n     \"CG\"   ==> 'S'\n     \"AC\"   ==> 'M'\n     \"GT\"   ==> 'K'\n     \"AG\"   ==> 'R'\n     \"CT\"   ==> 'Y'\n\n     \"CGT\"  ==> 'B'\n     \"AGT\"  ==> 'D'\n     \"ACT\"  ==> 'H'\n     \"ACG\"  ==> 'V'\n\n     \"ACGT\" ==> 'N'\n     \"-\"    ==> '-'\n\n The given codes are treated case-insensitive and order-independent. For example, given `\"tCgG\"`,\n the function still returns `'B'`. However, if any of the given codes is not valid, an\n `std::out_of_range` exception is thrown.\n\n See nucleic_acid_ambiguities() for the reverse of this function.\n\nC++: genesis::sequence::nucleic_acid_ambiguity_code(std::string) --> char", pybind11::arg("codes"));

}

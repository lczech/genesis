#include <genesis/utils/color/color.hpp>
#include <genesis/utils/color/functions.hpp>
#include <genesis/utils/math/bitvector.hpp>
#include <genesis/utils/math/bitvector/helper.hpp>
#include <genesis/utils/math/bitvector/operators.hpp>
#include <ios>
#include <iterator>
#include <locale>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
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

void bind_genesis_utils_math_bitvector_helper(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::utils::make_bool_vector_from_indices(const class std::vector<unsigned long, class std::allocator<unsigned long> > &, unsigned long) file:genesis/utils/math/bitvector/helper.hpp line:51
	M("genesis::utils").def("make_bool_vector_from_indices", [](const class std::vector<unsigned long, class std::allocator<unsigned long> > & a0) -> std::vector<bool, class std::allocator<bool> > { return genesis::utils::make_bool_vector_from_indices(a0); }, "", pybind11::arg("indices"));
	M("genesis::utils").def("make_bool_vector_from_indices", (class std::vector<bool, class std::allocator<bool> > (*)(const class std::vector<unsigned long, class std::allocator<unsigned long> > &, unsigned long)) &genesis::utils::make_bool_vector_from_indices, "Helper function to create a bool vector from a set of indices to be set to `true`.\n\n The function expectes a list of indices. It returns a bool vector with the size of the largest\n index, or the provided  (if set to a value > 0), where all positions of these indices are\n `true`, and all other positions are `false`.\n\nC++: genesis::utils::make_bool_vector_from_indices(const class std::vector<unsigned long, class std::allocator<unsigned long> > &, unsigned long) --> class std::vector<bool, class std::allocator<bool> >", pybind11::arg("indices"), pybind11::arg("size"));

	// genesis::utils::bitwise_and(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &, bool) file:genesis/utils/math/bitvector/operators.hpp line:54
	M("genesis::utils").def("bitwise_and", [](const class genesis::utils::Bitvector & a0, const class genesis::utils::Bitvector & a1) -> genesis::utils::Bitvector { return genesis::utils::bitwise_and(a0, a1); }, "", pybind11::arg("lhs"), pybind11::arg("rhs"));
	M("genesis::utils").def("bitwise_and", (class genesis::utils::Bitvector (*)(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &, bool)) &genesis::utils::bitwise_and, "Take the bitwise `and` of two Bitvector%s of potentially different size.\n\n The function is the same as the normal version, but allows to use Bitvector%s of different sizes.\n By default (`use_larger == false`), we use the number of bits of the shorter Bitvector,\n that is, it operates on bits `[ 0 , m )` with `m = min( lhs.size(), rhs.size() )`.\n If  is set however, the resulting Bitvector has the max size of the two inputs,\n with the bits at the end (the ones after `m`) being `false`.\n\nC++: genesis::utils::bitwise_and(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &, bool) --> class genesis::utils::Bitvector", pybind11::arg("lhs"), pybind11::arg("rhs"), pybind11::arg("use_larger"));

	// genesis::utils::bitwise_or(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &, bool) file:genesis/utils/math/bitvector/operators.hpp line:61
	M("genesis::utils").def("bitwise_or", [](const class genesis::utils::Bitvector & a0, const class genesis::utils::Bitvector & a1) -> genesis::utils::Bitvector { return genesis::utils::bitwise_or(a0, a1); }, "", pybind11::arg("lhs"), pybind11::arg("rhs"));
	M("genesis::utils").def("bitwise_or", (class genesis::utils::Bitvector (*)(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &, bool)) &genesis::utils::bitwise_or, "Take the bitwise `or` of two Bitvector%s of potentially different size.\n\n  bitwise_and()\n\nC++: genesis::utils::bitwise_or(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &, bool) --> class genesis::utils::Bitvector", pybind11::arg("lhs"), pybind11::arg("rhs"), pybind11::arg("use_larger"));

	// genesis::utils::bitwise_xor(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &, bool) file:genesis/utils/math/bitvector/operators.hpp line:68
	M("genesis::utils").def("bitwise_xor", [](const class genesis::utils::Bitvector & a0, const class genesis::utils::Bitvector & a1) -> genesis::utils::Bitvector { return genesis::utils::bitwise_xor(a0, a1); }, "", pybind11::arg("lhs"), pybind11::arg("rhs"));
	M("genesis::utils").def("bitwise_xor", (class genesis::utils::Bitvector (*)(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &, bool)) &genesis::utils::bitwise_xor, "Take the bitwise `xor` of two Bitvector%s of potentially different size.\n\n  bitwise_and()\n\nC++: genesis::utils::bitwise_xor(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &, bool) --> class genesis::utils::Bitvector", pybind11::arg("lhs"), pybind11::arg("rhs"), pybind11::arg("use_larger"));

	// genesis::utils::set_minus(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &) file:genesis/utils/math/bitvector/operators.hpp line:70
	M("genesis::utils").def("set_minus", (class genesis::utils::Bitvector (*)(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &)) &genesis::utils::set_minus, "C++: genesis::utils::set_minus(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &) --> class genesis::utils::Bitvector", pybind11::arg("lhs"), pybind11::arg("rhs"));

	// genesis::utils::symmetric_difference(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &) file:genesis/utils/math/bitvector/operators.hpp line:71
	M("genesis::utils").def("symmetric_difference", (class genesis::utils::Bitvector (*)(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &)) &genesis::utils::symmetric_difference, "C++: genesis::utils::symmetric_difference(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &) --> class genesis::utils::Bitvector", pybind11::arg("lhs"), pybind11::arg("rhs"));

	// genesis::utils::is_strict_subset(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &) file:genesis/utils/math/bitvector/operators.hpp line:76
	M("genesis::utils").def("is_strict_subset", (bool (*)(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &)) &genesis::utils::is_strict_subset, "Strict subset.\n\nC++: genesis::utils::is_strict_subset(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &) --> bool", pybind11::arg("sub"), pybind11::arg("super"));

	// genesis::utils::is_strict_superset(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &) file:genesis/utils/math/bitvector/operators.hpp line:81
	M("genesis::utils").def("is_strict_superset", (bool (*)(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &)) &genesis::utils::is_strict_superset, "Strict superset.\n\nC++: genesis::utils::is_strict_superset(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &) --> bool", pybind11::arg("super"), pybind11::arg("sub"));

	// genesis::utils::is_subset(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &) file:genesis/utils/math/bitvector/operators.hpp line:86
	M("genesis::utils").def("is_subset", (bool (*)(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &)) &genesis::utils::is_subset, "Subset or equal.\n\nC++: genesis::utils::is_subset(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &) --> bool", pybind11::arg("sub"), pybind11::arg("super"));

	// genesis::utils::is_superset(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &) file:genesis/utils/math/bitvector/operators.hpp line:91
	M("genesis::utils").def("is_superset", (bool (*)(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &)) &genesis::utils::is_superset, "Superset or equal.\n\nC++: genesis::utils::is_superset(const class genesis::utils::Bitvector &, const class genesis::utils::Bitvector &) --> bool", pybind11::arg("super"), pybind11::arg("sub"));

	{ // genesis::utils::Color file:genesis/utils/color/color.hpp line:47
		pybind11::class_<genesis::utils::Color, std::shared_ptr<genesis::utils::Color>> cl(M("genesis::utils"), "Color", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::Color(); } ) );
		cl.def( pybind11::init<double, double, double>(), pybind11::arg("r"), pybind11::arg("g"), pybind11::arg("b") );

		cl.def( pybind11::init<double, double, double, double>(), pybind11::arg("r"), pybind11::arg("g"), pybind11::arg("b"), pybind11::arg("a") );

		cl.def( pybind11::init( [](genesis::utils::Color const &o){ return new genesis::utils::Color(o); } ) );
		cl.def("assign", (class genesis::utils::Color & (genesis::utils::Color::*)(const class genesis::utils::Color &)) &genesis::utils::Color::operator=, "C++: genesis::utils::Color::operator=(const class genesis::utils::Color &) --> class genesis::utils::Color &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("swap", (void (genesis::utils::Color::*)(class genesis::utils::Color &)) &genesis::utils::Color::swap, "C++: genesis::utils::Color::swap(class genesis::utils::Color &) --> void", pybind11::arg("other"));
		cl.def_static("from_bytes", [](unsigned char const & a0, unsigned char const & a1, unsigned char const & a2) -> genesis::utils::Color { return genesis::utils::Color::from_bytes(a0, a1, a2); }, "", pybind11::arg("r"), pybind11::arg("g"), pybind11::arg("b"));
		cl.def_static("from_bytes", (class genesis::utils::Color (*)(unsigned char, unsigned char, unsigned char, unsigned char)) &genesis::utils::Color::from_bytes, "color_from_bytes()\n\nC++: genesis::utils::Color::from_bytes(unsigned char, unsigned char, unsigned char, unsigned char) --> class genesis::utils::Color", pybind11::arg("r"), pybind11::arg("g"), pybind11::arg("b"), pybind11::arg("a"));
		cl.def_static("from_hex", [](const std::string & a0) -> genesis::utils::Color { return genesis::utils::Color::from_hex(a0); }, "", pybind11::arg("hex_color"));
		cl.def_static("from_hex", (class genesis::utils::Color (*)(const std::string &, const std::string &)) &genesis::utils::Color::from_hex, "color_from_hex()\n\nC++: genesis::utils::Color::from_hex(const std::string &, const std::string &) --> class genesis::utils::Color", pybind11::arg("hex_color"), pybind11::arg("prefix"));
		cl.def("r", (double (genesis::utils::Color::*)() const) &genesis::utils::Color::r, "C++: genesis::utils::Color::r() const --> double");
		cl.def("g", (double (genesis::utils::Color::*)() const) &genesis::utils::Color::g, "C++: genesis::utils::Color::g() const --> double");
		cl.def("b", (double (genesis::utils::Color::*)() const) &genesis::utils::Color::b, "C++: genesis::utils::Color::b() const --> double");
		cl.def("a", (double (genesis::utils::Color::*)() const) &genesis::utils::Color::a, "C++: genesis::utils::Color::a() const --> double");
		cl.def("r_byte", (unsigned char (genesis::utils::Color::*)() const) &genesis::utils::Color::r_byte, "C++: genesis::utils::Color::r_byte() const --> unsigned char");
		cl.def("g_byte", (unsigned char (genesis::utils::Color::*)() const) &genesis::utils::Color::g_byte, "C++: genesis::utils::Color::g_byte() const --> unsigned char");
		cl.def("b_byte", (unsigned char (genesis::utils::Color::*)() const) &genesis::utils::Color::b_byte, "C++: genesis::utils::Color::b_byte() const --> unsigned char");
		cl.def("a_byte", (unsigned char (genesis::utils::Color::*)() const) &genesis::utils::Color::a_byte, "C++: genesis::utils::Color::a_byte() const --> unsigned char");
		cl.def("r", (void (genesis::utils::Color::*)(double)) &genesis::utils::Color::r, "C++: genesis::utils::Color::r(double) --> void", pybind11::arg("value"));
		cl.def("g", (void (genesis::utils::Color::*)(double)) &genesis::utils::Color::g, "C++: genesis::utils::Color::g(double) --> void", pybind11::arg("value"));
		cl.def("b", (void (genesis::utils::Color::*)(double)) &genesis::utils::Color::b, "C++: genesis::utils::Color::b(double) --> void", pybind11::arg("value"));
		cl.def("a", (void (genesis::utils::Color::*)(double)) &genesis::utils::Color::a, "C++: genesis::utils::Color::a(double) --> void", pybind11::arg("value"));
		cl.def("r_byte", (void (genesis::utils::Color::*)(unsigned char)) &genesis::utils::Color::r_byte, "C++: genesis::utils::Color::r_byte(unsigned char) --> void", pybind11::arg("value"));
		cl.def("g_byte", (void (genesis::utils::Color::*)(unsigned char)) &genesis::utils::Color::g_byte, "C++: genesis::utils::Color::g_byte(unsigned char) --> void", pybind11::arg("value"));
		cl.def("b_byte", (void (genesis::utils::Color::*)(unsigned char)) &genesis::utils::Color::b_byte, "C++: genesis::utils::Color::b_byte(unsigned char) --> void", pybind11::arg("value"));
		cl.def("a_byte", (void (genesis::utils::Color::*)(unsigned char)) &genesis::utils::Color::a_byte, "C++: genesis::utils::Color::a_byte(unsigned char) --> void", pybind11::arg("value"));

		cl.def("__str__", [](genesis::utils::Color const &o) -> std::string { std::ostringstream s; genesis::utils::operator<<(s, o); return s.str(); } );
	}
	// genesis::utils::color_from_bytes(unsigned char, unsigned char, unsigned char, unsigned char) file:genesis/utils/color/functions.hpp line:56
	M("genesis::utils").def("color_from_bytes", [](unsigned char const & a0, unsigned char const & a1, unsigned char const & a2) -> genesis::utils::Color { return genesis::utils::color_from_bytes(a0, a1, a2); }, "", pybind11::arg("r"), pybind11::arg("g"), pybind11::arg("b"));
	M("genesis::utils").def("color_from_bytes", (class genesis::utils::Color (*)(unsigned char, unsigned char, unsigned char, unsigned char)) &genesis::utils::color_from_bytes, "Create a Color given three or four values in the range `[ 0, 255 ]` for each\n of the components red, green and blue, and optionally alpha.\n\nC++: genesis::utils::color_from_bytes(unsigned char, unsigned char, unsigned char, unsigned char) --> class genesis::utils::Color", pybind11::arg("r"), pybind11::arg("g"), pybind11::arg("b"), pybind11::arg("a"));

	// genesis::utils::color_from_hex(const std::string &, const std::string &) file:genesis/utils/color/functions.hpp line:65
	M("genesis::utils").def("color_from_hex", [](const std::string & a0) -> genesis::utils::Color { return genesis::utils::color_from_hex(a0); }, "", pybind11::arg("hex_color"));
	M("genesis::utils").def("color_from_hex", (class genesis::utils::Color (*)(const std::string &, const std::string &)) &genesis::utils::color_from_hex, "Create a Color given a hex color string in the format \"#003366[ff]\".\n\n The hash sign in the beginning can be replaced by any given prefix.\n If the string is not correctly formatted, an std::invalid_argument exception is thrown.\n If the string contains only RGB, alpha is set to `1.0`.\n\nC++: genesis::utils::color_from_hex(const std::string &, const std::string &) --> class genesis::utils::Color", pybind11::arg("hex_color"), pybind11::arg("prefix"));

	// genesis::utils::color_to_hex(const class genesis::utils::Color &, const std::string &, bool, bool) file:genesis/utils/color/functions.hpp line:75
	M("genesis::utils").def("color_to_hex", [](const class genesis::utils::Color & a0) -> std::string { return genesis::utils::color_to_hex(a0); }, "", pybind11::arg("c"));
	M("genesis::utils").def("color_to_hex", [](const class genesis::utils::Color & a0, const std::string & a1) -> std::string { return genesis::utils::color_to_hex(a0, a1); }, "", pybind11::arg("c"), pybind11::arg("prefix"));
	M("genesis::utils").def("color_to_hex", [](const class genesis::utils::Color & a0, const std::string & a1, bool const & a2) -> std::string { return genesis::utils::color_to_hex(a0, a1, a2); }, "", pybind11::arg("c"), pybind11::arg("prefix"), pybind11::arg("uppercase"));
	M("genesis::utils").def("color_to_hex", (std::string (*)(const class genesis::utils::Color &, const std::string &, bool, bool)) &genesis::utils::color_to_hex, "Return a hex string representation of a Color in the format \"#003366[ff]\".\n\n The hash sign in the beginning can be replaced by any given \n If  is set to `true`, any outputted alphabetical chars\n (that is, A to F for hex strings) are uppercase.\n if  is set to `true`, two additional hex digits are printed for the alpha channel.\n\nC++: genesis::utils::color_to_hex(const class genesis::utils::Color &, const std::string &, bool, bool) --> std::string", pybind11::arg("c"), pybind11::arg("prefix"), pybind11::arg("uppercase"), pybind11::arg("with_alpha"));

	// genesis::utils::resolve_color_string(const std::string &) file:genesis/utils/color/functions.hpp line:97
	M("genesis::utils").def("resolve_color_string", (class genesis::utils::Color (*)(const std::string &)) &genesis::utils::resolve_color_string, "Resolve a string representing a color.\n\n The string can either be a hex color as accepted by color_from_hex(), e.g., \"#003366[ff]\",\n or one of the named colors, see is_color_name() and color_from_name().\n\nC++: genesis::utils::resolve_color_string(const std::string &) --> class genesis::utils::Color", pybind11::arg("color_str"));

}

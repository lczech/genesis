#include <genesis/utils/math/bitvector.hpp>
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

void bind_genesis_utils_math_bitvector(std::function< pybind11::module &(std::string const &namespace_) > &M)
{

	extend_string_target(M("genesis::utils"));
	{ // genesis::utils::Bitvector file:genesis/utils/math/bitvector.hpp line:49
		pybind11::class_<genesis::utils::Bitvector, std::shared_ptr<genesis::utils::Bitvector>> cl(M("genesis::utils"), "Bitvector", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::Bitvector(); } ) );
		cl.def( pybind11::init( [](unsigned long const & a0){ return new genesis::utils::Bitvector(a0); } ), "doc" , pybind11::arg("size"));
		cl.def( pybind11::init<unsigned long, bool>(), pybind11::arg("size"), pybind11::arg("initial_value") );

		cl.def( pybind11::init<unsigned long, const class genesis::utils::Bitvector &>(), pybind11::arg("size"), pybind11::arg("other") );

		cl.def( pybind11::init<const std::string &>(), pybind11::arg("values") );

		cl.def( pybind11::init( [](genesis::utils::Bitvector const &o){ return new genesis::utils::Bitvector(o); } ) );
		cl.def("assign", (class genesis::utils::Bitvector & (genesis::utils::Bitvector::*)(const class genesis::utils::Bitvector &)) &genesis::utils::Bitvector::operator=, "C++: genesis::utils::Bitvector::operator=(const class genesis::utils::Bitvector &) --> class genesis::utils::Bitvector &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("__getitem__", (bool (genesis::utils::Bitvector::*)(unsigned long) const) &genesis::utils::Bitvector::operator[], "Return the value of a single bit, without boundary check.\n\nC++: genesis::utils::Bitvector::operator[](unsigned long) const --> bool", pybind11::arg("index"));
		cl.def("get", (bool (genesis::utils::Bitvector::*)(unsigned long) const) &genesis::utils::Bitvector::get, "Return the value of a single bit, with boundary check.\n\nC++: genesis::utils::Bitvector::get(unsigned long) const --> bool", pybind11::arg("index"));
		cl.def("set", (void (genesis::utils::Bitvector::*)(unsigned long)) &genesis::utils::Bitvector::set, "Set the value of a single bit to true, with boundary check.\n\nC++: genesis::utils::Bitvector::set(unsigned long) --> void", pybind11::arg("index"));
		cl.def("unset", (void (genesis::utils::Bitvector::*)(unsigned long)) &genesis::utils::Bitvector::unset, "Set the value of a single bit to false, with boundary check.\n\nC++: genesis::utils::Bitvector::unset(unsigned long) --> void", pybind11::arg("index"));
		cl.def("set", (void (genesis::utils::Bitvector::*)(unsigned long, bool)) &genesis::utils::Bitvector::set, "Set the value of a single bit to a given bool value, with boundary check.\n\nC++: genesis::utils::Bitvector::set(unsigned long, bool) --> void", pybind11::arg("index"), pybind11::arg("value"));
		cl.def("set", [](genesis::utils::Bitvector &o, unsigned long const & a0, unsigned long const & a1) -> void { return o.set(a0, a1); }, "", pybind11::arg("first"), pybind11::arg("last"));
		cl.def("set", (void (genesis::utils::Bitvector::*)(unsigned long, unsigned long, bool)) &genesis::utils::Bitvector::set, "Set the value of a contiguous range of bits in the Bitvector.\n\n This function takes the  (inclusive) and  (past-the-end) indices into\n the Bitvector (with boundary check), and sets them to the given value (`true` by default).\n\n The range  to  is zero-based, with last being the past-the-end index.\n Hence, this is the same as\n\n     for( size_t i = first; i < last; ++i ) {\n         bitvector.set( i, value );\n     }\n\n but faster for anything beyond a few bits, as we operate on whole words internally.\n\nC++: genesis::utils::Bitvector::set(unsigned long, unsigned long, bool) --> void", pybind11::arg("first"), pybind11::arg("last"), pybind11::arg("value"));
		cl.def("flip", (void (genesis::utils::Bitvector::*)(unsigned long)) &genesis::utils::Bitvector::flip, "Flip (negate) the value of a single bit, with boundary check.\n\nC++: genesis::utils::Bitvector::flip(unsigned long) --> void", pybind11::arg("index"));
		cl.def("toggle", (void (genesis::utils::Bitvector::*)(unsigned long)) &genesis::utils::Bitvector::toggle, "Alias for flip(), see there for details.\n\nC++: genesis::utils::Bitvector::toggle(unsigned long) --> void", pybind11::arg("index"));
		cl.def("__iand__", (class genesis::utils::Bitvector & (genesis::utils::Bitvector::*)(const class genesis::utils::Bitvector &)) &genesis::utils::Bitvector::operator&=, "C++: genesis::utils::Bitvector::operator&=(const class genesis::utils::Bitvector &) --> class genesis::utils::Bitvector &", pybind11::return_value_policy::reference_internal, pybind11::arg("rhs"));
		cl.def("__ior__", (class genesis::utils::Bitvector & (genesis::utils::Bitvector::*)(const class genesis::utils::Bitvector &)) &genesis::utils::Bitvector::operator|=, "C++: genesis::utils::Bitvector::operator|=(const class genesis::utils::Bitvector &) --> class genesis::utils::Bitvector &", pybind11::return_value_policy::reference_internal, pybind11::arg("rhs"));
		cl.def("__ixor__", (class genesis::utils::Bitvector & (genesis::utils::Bitvector::*)(const class genesis::utils::Bitvector &)) &genesis::utils::Bitvector::operator^=, "C++: genesis::utils::Bitvector::operator^=(const class genesis::utils::Bitvector &) --> class genesis::utils::Bitvector &", pybind11::return_value_policy::reference_internal, pybind11::arg("rhs"));
		cl.def("__invert__", (class genesis::utils::Bitvector (genesis::utils::Bitvector::*)() const) &genesis::utils::Bitvector::operator~, "C++: genesis::utils::Bitvector::operator~() const --> class genesis::utils::Bitvector");
		cl.def("__eq__", (bool (genesis::utils::Bitvector::*)(const class genesis::utils::Bitvector &) const) &genesis::utils::Bitvector::operator==, "C++: genesis::utils::Bitvector::operator==(const class genesis::utils::Bitvector &) const --> bool", pybind11::arg("other"));
		cl.def("__ne__", (bool (genesis::utils::Bitvector::*)(const class genesis::utils::Bitvector &) const) &genesis::utils::Bitvector::operator!=, "C++: genesis::utils::Bitvector::operator!=(const class genesis::utils::Bitvector &) const --> bool", pybind11::arg("other"));
		cl.def("empty", (bool (genesis::utils::Bitvector::*)() const) &genesis::utils::Bitvector::empty, "Return whether the Bitvector is empty, that is, has size() == 0.\n\n Note that this function does _not_ count the number of bits that are set to `true`.\n It simply returns whether the Bitvector has any size (`false`),\n or was default constructed (`true`).\n\nC++: genesis::utils::Bitvector::empty() const --> bool");
		cl.def("size", (unsigned long (genesis::utils::Bitvector::*)() const) &genesis::utils::Bitvector::size, "Return the size (number of bits) of this Bitvector.\n\nC++: genesis::utils::Bitvector::size() const --> unsigned long");
		cl.def("count", (unsigned long (genesis::utils::Bitvector::*)() const) &genesis::utils::Bitvector::count, "Count the number of set bits in the Bitvector, that is, its Hamming weight,\n or population count (popcnt).\n\nC++: genesis::utils::Bitvector::count() const --> unsigned long");
		cl.def("count", (unsigned long (genesis::utils::Bitvector::*)(unsigned long, unsigned long) const) &genesis::utils::Bitvector::count, "Count the number of set bits between a range of indices in the Bitvector,\n that is, its Hamming weight, or population count (popcnt), for that range.\n\n The range  to  is zero-based, with last being the past-the-end index.\n Hence, this is the same as\n\n     size_t count = 0;\n     for( size_t i = first; i < last; ++i ) {\n         if( bitvector.get( i )) {\n             ++count;\n         }\n     }\n\n but faster, as we use whole-word counting internally.\n\nC++: genesis::utils::Bitvector::count(unsigned long, unsigned long) const --> unsigned long", pybind11::arg("first"), pybind11::arg("last"));
		cl.def("find_next_set", (unsigned long (genesis::utils::Bitvector::*)(unsigned long) const) &genesis::utils::Bitvector::find_next_set, "Return the index of the next position in the Bitvector that is set.\n\n This returns the first position after  including  itself, that is set.\n If no such position exists (because all following bits are `false`), or if  is beyond\n the length of the vector `std::numeric_limits<size_t>::max()` is returned instead.\n\nC++: genesis::utils::Bitvector::find_next_set(unsigned long) const --> unsigned long", pybind11::arg("start"));
		cl.def("hash", (unsigned long (genesis::utils::Bitvector::*)() const) &genesis::utils::Bitvector::hash, "Return an std::hash value for the Bitvector.\n\nC++: genesis::utils::Bitvector::hash() const --> unsigned long");
		cl.def("x_hash", (unsigned long (genesis::utils::Bitvector::*)() const) &genesis::utils::Bitvector::x_hash, "Return a hash value of type IntType that is quicker to calculate than hash().\n\n This can be used for obtaining a simple hash using xor of the words.\n The avalanche effect is of course not present, but for many applications, this hash is\n good enough and quite useful.\n\nC++: genesis::utils::Bitvector::x_hash() const --> unsigned long");
		cl.def("negate", (void (genesis::utils::Bitvector::*)()) &genesis::utils::Bitvector::negate, "Flip all bits.\n\nC++: genesis::utils::Bitvector::negate() --> void");
		cl.def("normalize", (void (genesis::utils::Bitvector::*)()) &genesis::utils::Bitvector::normalize, "Bring the Bitvector in a normalized form, where the first bit is always zero.\n\n If the first bit is zero, nothing happens. However, if is is one, the whole Bitvector is flipped\n using negate().\n\nC++: genesis::utils::Bitvector::normalize() --> void");
		cl.def("set_all", [](genesis::utils::Bitvector &o) -> void { return o.set_all(); }, "");
		cl.def("set_all", (void (genesis::utils::Bitvector::*)(const bool)) &genesis::utils::Bitvector::set_all, "Set all the bits to a specified \n     \n\nC++: genesis::utils::Bitvector::set_all(const bool) --> void", pybind11::arg("value"));
		cl.def("dump", (std::string (genesis::utils::Bitvector::*)() const) &genesis::utils::Bitvector::dump, "C++: genesis::utils::Bitvector::dump() const --> std::string");
		cl.def("dump_int", (std::string (genesis::utils::Bitvector::*)(unsigned long) const) &genesis::utils::Bitvector::dump_int, "C++: genesis::utils::Bitvector::dump_int(unsigned long) const --> std::string", pybind11::arg("x"));
		cl.def("data", (const class std::vector<unsigned long, class std::allocator<unsigned long> > & (genesis::utils::Bitvector::*)() const) &genesis::utils::Bitvector::data, "C++: genesis::utils::Bitvector::data() const --> const class std::vector<unsigned long, class std::allocator<unsigned long> > &", pybind11::return_value_policy::reference_internal);

		cl.def("__str__", [](genesis::utils::Bitvector const &o) -> std::string { std::ostringstream s; genesis::utils::operator<<(s, o); return s.str(); } );

		extend_bitvector(cl);
	}
	{ // genesis::utils::BitvectorHash file:genesis/utils/math/bitvector.hpp line:453
		pybind11::class_<genesis::utils::BitvectorHash, std::shared_ptr<genesis::utils::BitvectorHash>> cl(M("genesis::utils"), "BitvectorHash", "Helper structure that yields the hash of a given Bitvector.\n\n It is meant to be used in containers such as `unordered_set` or `unordered_map`\n that can make use of custom hash functions for the key objects. By default,\n these containers use a specialization of the `std::hash` template, which we also offer,\n and that also uses the Bitvector::hash() function.\n\n Hence, this class here is slightly redundant, as it gives the same result as just using\n the `std::hash` specialization. Still, it might be useful to have.\n\n See also BitvectorXhash for an alternative version that uses Bitvector::x_hash() instead.");
		cl.def( pybind11::init( [](){ return new genesis::utils::BitvectorHash(); } ) );
		cl.def("__call__", (unsigned long (genesis::utils::BitvectorHash::*)(const class genesis::utils::Bitvector &) const) &genesis::utils::BitvectorHash::operator(), "C++: genesis::utils::BitvectorHash::operator()(const class genesis::utils::Bitvector &) const --> unsigned long", pybind11::arg("value"));
	}
	{ // genesis::utils::BitvectorXhash file:genesis/utils/math/bitvector.hpp line:476
		pybind11::class_<genesis::utils::BitvectorXhash, std::shared_ptr<genesis::utils::BitvectorXhash>> cl(M("genesis::utils"), "BitvectorXhash", "Helper structer that yields the x_hash of a given Bitvector.\n\n It is meant to be used in containers such as `unordered_set` or `unordered_map`\n that can make use of custom hash functions for the key objects. Using this class instead\n of the standard `std::hash` specialization, the Bitvector::x_hash() function is used instead\n of the standard hash() function. It is hence faster to compute, but without avalanche effect.\n\n In some use cases, this might be preferrable - we however recommend to test this, in order to\n make sure that colliding hashes do not slow down the performance in the end.\n\n Note that the function needs to cast from Bitvector::IntType to std::size_t.\n On most modern systems, these are expecte to be the same, i.e., 64 bit unsigned integers.\n However, this might cause problem on systems where this is not the case.");
		cl.def( pybind11::init( [](){ return new genesis::utils::BitvectorXhash(); } ) );
		cl.def("__call__", (unsigned long (genesis::utils::BitvectorXhash::*)(const class genesis::utils::Bitvector &) const) &genesis::utils::BitvectorXhash::operator(), "C++: genesis::utils::BitvectorXhash::operator()(const class genesis::utils::Bitvector &) const --> unsigned long", pybind11::arg("value"));
	}
}

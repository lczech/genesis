#include <genesis/utils/bit/bitvector.hpp>
#include <genesis/utils/bit/bitvector/operators.hpp>
#include <genesis/utils/bit/bitvector_add_ons.hpp>
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
#include <pybind11/stl.h>
#include <string>

#ifndef BINDER_PYBIND11_TYPE_CASTER
#    define BINDER_PYBIND11_TYPE_CASTER
PYBIND11_DECLARE_HOLDER_TYPE( T, std::shared_ptr<T>, false )
PYBIND11_DECLARE_HOLDER_TYPE( T, T*, false )
PYBIND11_MAKE_OPAQUE( std::shared_ptr<void> )
#endif

void bind_genesis_utils_bit_bitvector(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::utils::bit::Bitvector file:genesis/utils/bit/bitvector.hpp line:67
        pybind11::
            class_<genesis::utils::bit::Bitvector, std::shared_ptr<genesis::utils::bit::Bitvector>>
                cl( M( "genesis::utils::bit" ), "Bitvector", "" );
        // function-signature: genesis::utils::bit::Bitvector::Bitvector()()
        // file:genesis/utils/bit/bitvector.hpp line:131
        cl.def( pybind11::init( []() { return new genesis::utils::bit::Bitvector(); } ) );
        // function-signature: genesis::utils::bit::Bitvector::Bitvector(unsigned long,
        // bool)(unsigned long, bool) file:genesis/utils/bit/bitvector.hpp line:137
        cl.def(
            pybind11::init( []( unsigned long const& a0 ) {
                return new genesis::utils::bit::Bitvector( a0 );
            } ),
            "doc",
            pybind11::arg( "size" )
        );
        // function-signature: genesis::utils::bit::Bitvector::Bitvector(unsigned long,
        // bool)(unsigned long, bool) file:genesis/utils/bit/bitvector.hpp line:137
        cl.def(
            pybind11::init<unsigned long, bool>(),
            pybind11::arg( "size" ),
            pybind11::arg( "initial_value" )
        );

        // function-signature: genesis::utils::bit::Bitvector::Bitvector(unsigned long, class
        // std::vector<unsigned long>)(unsigned long, class std::vector<unsigned long>)
        // file:genesis/utils/bit/bitvector.hpp line:164
        cl.def(
            pybind11::init<unsigned long, class std::vector<unsigned long>>(),
            pybind11::arg( "size" ),
            pybind11::arg( "index_list" )
        );

        // function-signature: genesis::utils::bit::Bitvector::Bitvector(unsigned long, const class
        // genesis::utils::bit::Bitvector &)(unsigned long, const class
        // genesis::utils::bit::Bitvector &) file:genesis/utils/bit/bitvector.hpp line:195
        cl.def(
            pybind11::init<unsigned long, const class genesis::utils::bit::Bitvector&>(),
            pybind11::arg( "size" ),
            pybind11::arg( "other" )
        );

        // function-signature: genesis::utils::bit::Bitvector::Bitvector(const std::string &)(const
        // std::string &) file:genesis/utils/bit/bitvector.hpp line:203
        cl.def( pybind11::init<const std::string&>(), pybind11::arg( "values" ) );

        // function-signature: genesis::utils::bit::Bitvector::Bitvector(const class
        // genesis::utils::bit::Bitvector &)(const class genesis::utils::bit::Bitvector &)
        // file:genesis/utils/bit/bitvector.hpp line:214
        cl.def( pybind11::init( []( genesis::utils::bit::Bitvector const& o ) {
            return new genesis::utils::bit::Bitvector( o );
        } ) );
        // function-signature: class genesis::utils::bit::Bitvector &
        // genesis::utils::bit::Bitvector::operator=(const class genesis::utils::bit::Bitvector
        // &)(const class genesis::utils::bit::Bitvector &) file:genesis/utils/bit/bitvector.hpp
        // line:217
        cl.def(
            "assign",
            ( class genesis::utils::bit::Bitvector &
              (genesis::utils::bit::Bitvector::*)(const class genesis::utils::bit::Bitvector&)) &
                genesis::utils::bit::Bitvector::operator=,
            "C++: genesis::utils::bit::Bitvector::operator=(const class "
            "genesis::utils::bit::Bitvector &) --> class genesis::utils::bit::Bitvector &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: bool genesis::utils::bit::Bitvector::operator[](unsigned long)
        // const(unsigned long) file:genesis/utils/bit/bitvector.hpp line:227
        cl.def(
            "__getitem__",
            ( bool( genesis::utils::bit::Bitvector::* )( unsigned long ) const ) &
                genesis::utils::bit::Bitvector::operator[],
            "Return the value of a single bit, without boundary check.\n\nC++: "
            "genesis::utils::bit::Bitvector::operator[](unsigned long) const --> bool",
            pybind11::arg( "index" )
        );
        // function-signature: bool genesis::utils::bit::Bitvector::get(unsigned long)
        // const(unsigned long) file:genesis/utils/bit/bitvector.hpp line:236
        cl.def(
            "get",
            ( bool( genesis::utils::bit::Bitvector::* )( unsigned long ) const ) &
                genesis::utils::bit::Bitvector::get,
            "Return the value of a single bit, with boundary check.\n\nC++: "
            "genesis::utils::bit::Bitvector::get(unsigned long) const --> bool",
            pybind11::arg( "index" )
        );
        // function-signature: void genesis::utils::bit::Bitvector::set(unsigned long)(unsigned
        // long) file:genesis/utils/bit/bitvector.hpp line:253
        cl.def(
            "set",
            ( void( genesis::utils::bit::Bitvector::* )( unsigned long ) ) &
                genesis::utils::bit::Bitvector::set,
            "Set the value of a single bit to true, with boundary check.\n\nC++: "
            "genesis::utils::bit::Bitvector::set(unsigned long) --> void",
            pybind11::arg( "index" )
        );
        // function-signature: void genesis::utils::bit::Bitvector::set(unsigned long,
        // bool)(unsigned long, bool) file:genesis/utils/bit/bitvector.hpp line:270
        cl.def(
            "set",
            ( void( genesis::utils::bit::Bitvector::* )( unsigned long, bool ) ) &
                genesis::utils::bit::Bitvector::set,
            "Set the value of a single bit to a given bool value, with boundary check.\n\nC++: "
            "genesis::utils::bit::Bitvector::set(unsigned long, bool) --> void",
            pybind11::arg( "index" ),
            pybind11::arg( "value" )
        );
        // function-signature: void genesis::utils::bit::Bitvector::unset(unsigned long)(unsigned
        // long) file:genesis/utils/bit/bitvector.hpp line:282
        cl.def(
            "unset",
            ( void( genesis::utils::bit::Bitvector::* )( unsigned long ) ) &
                genesis::utils::bit::Bitvector::unset,
            "Set the value of a single bit to false, with boundary check.\n\nC++: "
            "genesis::utils::bit::Bitvector::unset(unsigned long) --> void",
            pybind11::arg( "index" )
        );
        // function-signature: void genesis::utils::bit::Bitvector::set_range(unsigned long,
        // unsigned long, bool)(unsigned long, unsigned long, bool)
        // file:genesis/utils/bit/bitvector.hpp line:311
        cl.def(
            "set_range",
            []( genesis::utils::bit::Bitvector& o, unsigned long const& a0, unsigned long const& a1
            ) -> void { return o.set_range( a0, a1 ); },
            "",
            pybind11::arg( "first" ),
            pybind11::arg( "last" )
        );
        cl.def(
            "set_range",
            ( void( genesis::utils::bit::Bitvector::* )( unsigned long, unsigned long, bool ) ) &
                genesis::utils::bit::Bitvector::set_range,
            "Set the value of a contiguous range of bits in the Bitvector.\n\n This function takes "
            "the  (inclusive) and  (past-the-end) indices into\n the Bitvector (with boundary "
            "check), and sets them to the given value (`true` by default).\n\n The range  to  is "
            "zero-based, with last being the past-the-end index.\n Hence, this is the same as\n\n  "
            "   for( size_t i = first; i < last; ++i ) {\n         bitvector.set( i, value );\n    "
            " }\n\n but faster for anything beyond a few bits, as we operate on whole words "
            "internally.\n\nC++: genesis::utils::bit::Bitvector::set_range(unsigned long, unsigned "
            "long, bool) --> void",
            pybind11::arg( "first" ),
            pybind11::arg( "last" ),
            pybind11::arg( "value" )
        );
        // function-signature: void genesis::utils::bit::Bitvector::set_all(bool)(bool)
        // file:genesis/utils/bit/bitvector.hpp line:316
        cl.def(
            "set_all",
            ( void( genesis::utils::bit::Bitvector::* )( bool ) ) &
                genesis::utils::bit::Bitvector::set_all,
            "Set all the bits to a specified \n     \n\nC++: "
            "genesis::utils::bit::Bitvector::set_all(bool) --> void",
            pybind11::arg( "value" )
        );
        // function-signature: void genesis::utils::bit::Bitvector::flip(unsigned long)(unsigned
        // long) file:genesis/utils/bit/bitvector.hpp line:321
        cl.def(
            "flip",
            ( void( genesis::utils::bit::Bitvector::* )( unsigned long ) ) &
                genesis::utils::bit::Bitvector::flip,
            "Flip (negate) the value of a single bit, with boundary check.\n\nC++: "
            "genesis::utils::bit::Bitvector::flip(unsigned long) --> void",
            pybind11::arg( "index" )
        );
        // function-signature: void genesis::utils::bit::Bitvector::toggle(unsigned long)(unsigned
        // long) file:genesis/utils/bit/bitvector.hpp line:338
        cl.def(
            "toggle",
            ( void( genesis::utils::bit::Bitvector::* )( unsigned long ) ) &
                genesis::utils::bit::Bitvector::toggle,
            "Alias for flip(), see there for details.\n\nC++: "
            "genesis::utils::bit::Bitvector::toggle(unsigned long) --> void",
            pybind11::arg( "index" )
        );
        // function-signature: void genesis::utils::bit::Bitvector::negate()()
        // file:genesis/utils/bit/bitvector.hpp line:346
        cl.def(
            "negate",
            ( void( genesis::utils::bit::Bitvector::* )() ) &
                genesis::utils::bit::Bitvector::negate,
            "Flip all bits. Alias for invert().\n\nC++: genesis::utils::bit::Bitvector::negate() "
            "--> void"
        );
        // function-signature: void genesis::utils::bit::Bitvector::invert()()
        // file:genesis/utils/bit/bitvector.hpp line:351
        cl.def(
            "invert",
            ( void( genesis::utils::bit::Bitvector::* )() ) &
                genesis::utils::bit::Bitvector::invert,
            "Flip all bits. Alias for negate().\n\nC++: genesis::utils::bit::Bitvector::invert() "
            "--> void"
        );
        // function-signature: bool genesis::utils::bit::Bitvector::empty() const()
        // file:genesis/utils/bit/bitvector.hpp line:364
        cl.def(
            "empty",
            ( bool( genesis::utils::bit::Bitvector::* )() const ) &
                genesis::utils::bit::Bitvector::empty,
            "Return whether the Bitvector is empty, that is, has size() == 0.\n\n Note that this "
            "function does _not_ count the number of bits that are set to `true`.\n It simply "
            "returns whether the Bitvector has any size (`false`),\n or was default constructed "
            "(`true`).\n\nC++: genesis::utils::bit::Bitvector::empty() const --> bool"
        );
        // function-signature: unsigned long genesis::utils::bit::Bitvector::size() const()
        // file:genesis/utils/bit/bitvector.hpp line:372
        cl.def(
            "size",
            ( unsigned long ( genesis::utils::bit::Bitvector::* )() const ) &
                genesis::utils::bit::Bitvector::size,
            "Return the size (number of bits) of this Bitvector.\n\nC++: "
            "genesis::utils::bit::Bitvector::size() const --> unsigned long"
        );
        // function-signature: class std::vector<unsigned long> &
        // genesis::utils::bit::Bitvector::data()() file:genesis/utils/bit/bitvector.hpp line:402
        cl.def(
            "data",
            ( class std::vector<unsigned long> & (genesis::utils::bit::Bitvector::*)() ) &
                genesis::utils::bit::Bitvector::data,
            "Get a non-const reference to the underlying data, i.e., the vector of units.\n\n This "
            "is meant to allow external functions such as serialization/deserialization to "
            "access\n the underlying data. It is hence important that any function uses this data "
            "with care,\n and does not break the assumptions and invariants expected in the "
            "bitvector. In particular,\n the size of the used data returned here needs to match "
            "the number of bits of the bitvector\n via the size() function, plus the needed "
            "padding to get to a full word size, and any padding\n bits shall be 0.\n\n We do not "
            "allow for resizing an existing instance. Similarly, we hence recommend to not\n "
            "change the size of the data from the outside, and always call unset_padding_bits() "
            "should\n any operation on the data cause the padding to be non-zero (such as "
            "inverting words).\n\nC++: genesis::utils::bit::Bitvector::data() --> class "
            "std::vector<unsigned long> &",
            pybind11::return_value_policy::reference_internal
        );
        // function-signature: void genesis::utils::bit::Bitvector::unset_padding_bits()()
        // file:genesis/utils/bit/bitvector.hpp line:418
        cl.def(
            "unset_padding_bits",
            ( void( genesis::utils::bit::Bitvector::* )() ) &
                genesis::utils::bit::Bitvector::unset_padding_bits,
            "Unset all bits to zero that are not actively used at the end of the data vector.\n\n "
            "The data buffer always contains a multiple of IntSize many bits, thus there might be "
            "surplus\n bits at its end for padding. In case we do operations with Bitvectors of "
            "different size, or\n when negating bits, these might be affected, so we need to reset "
            "them to zero in these cases.\n\n This function, similar to the non-const data() "
            "function, is meant for external operations\n that need access to internal data. This "
            "unset function is idempotent, and it is fine to call\n it on any bitvector.\n\nC++: "
            "genesis::utils::bit::Bitvector::unset_padding_bits() --> void"
        );
        // function-signature: unsigned long genesis::utils::bit::Bitvector::get_padding_mask()
        // const() file:genesis/utils/bit/bitvector.hpp line:427
        cl.def(
            "get_padding_mask",
            ( unsigned long ( genesis::utils::bit::Bitvector::* )() const ) &
                genesis::utils::bit::Bitvector::get_padding_mask,
            "Get the mask used for unset_padding_bits()\n\n This can be useful when checking "
            "certain properties, such as if all bits are set.\n If the Bitvector has a size that "
            "exactly matches the underlying int type (64 bits typically),\n the mask is all-zero! "
            "It shall not be applied in that case.\n\nC++: "
            "genesis::utils::bit::Bitvector::get_padding_mask() const --> unsigned long"
        );
        // function-signature: unsigned long
        // genesis::utils::bit::Bitvector::get_vector_size(unsigned long)(unsigned long)
        // file:genesis/utils/bit/bitvector.hpp line:434
        cl.def_static(
            "get_vector_size",
            ( unsigned long ( * )( unsigned long ) ) &
                genesis::utils::bit::Bitvector::get_vector_size,
            "For a given numer of bits, compute the size of the internally used vector.\n\n This "
            "is mostly meant as a helper for data operations such as serialization and "
            "deserialization.\n\nC++: genesis::utils::bit::Bitvector::get_vector_size(unsigned "
            "long) --> unsigned long",
            pybind11::arg( "bit_size" )
        );
        // function-signature: class genesis::utils::bit::Bitvector &
        // genesis::utils::bit::Bitvector::operator&=(const class genesis::utils::bit::Bitvector
        // &)(const class genesis::utils::bit::Bitvector &) file:genesis/utils/bit/bitvector.hpp
        // line:443
        cl.def(
            "__iand__",
            ( class genesis::utils::bit::Bitvector &
              (genesis::utils::bit::Bitvector::*)(const class genesis::utils::bit::Bitvector&)) &
                genesis::utils::bit::Bitvector::operator&=,
            "C++: genesis::utils::bit::Bitvector::operator&=(const class "
            "genesis::utils::bit::Bitvector &) --> class genesis::utils::bit::Bitvector &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "rhs" )
        );
        // function-signature: class genesis::utils::bit::Bitvector &
        // genesis::utils::bit::Bitvector::operator|=(const class genesis::utils::bit::Bitvector
        // &)(const class genesis::utils::bit::Bitvector &) file:genesis/utils/bit/bitvector.hpp
        // line:444
        cl.def(
            "__ior__",
            ( class genesis::utils::bit::Bitvector &
              (genesis::utils::bit::Bitvector::*)(const class genesis::utils::bit::Bitvector&)) &
                genesis::utils::bit::Bitvector::operator|=,
            "C++: genesis::utils::bit::Bitvector::operator|=(const class "
            "genesis::utils::bit::Bitvector &) --> class genesis::utils::bit::Bitvector &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "rhs" )
        );
        // function-signature: class genesis::utils::bit::Bitvector &
        // genesis::utils::bit::Bitvector::operator^=(const class genesis::utils::bit::Bitvector
        // &)(const class genesis::utils::bit::Bitvector &) file:genesis/utils/bit/bitvector.hpp
        // line:445
        cl.def(
            "__ixor__",
            ( class genesis::utils::bit::Bitvector &
              (genesis::utils::bit::Bitvector::*)(const class genesis::utils::bit::Bitvector&)) &
                genesis::utils::bit::Bitvector::operator^=,
            "C++: genesis::utils::bit::Bitvector::operator^=(const class "
            "genesis::utils::bit::Bitvector &) --> class genesis::utils::bit::Bitvector &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "rhs" )
        );
        // function-signature: class genesis::utils::bit::Bitvector
        // genesis::utils::bit::Bitvector::operator~() const() file:genesis/utils/bit/bitvector.hpp
        // line:446
        cl.def(
            "__invert__",
            ( class genesis::utils::bit::Bitvector( genesis::utils::bit::Bitvector::* )() const ) &
                genesis::utils::bit::Bitvector::operator~,
            "C++: genesis::utils::bit::Bitvector::operator~() const --> class "
            "genesis::utils::bit::Bitvector"
        );
        // function-signature: bool genesis::utils::bit::Bitvector::operator==(const class
        // genesis::utils::bit::Bitvector &) const(const class genesis::utils::bit::Bitvector &)
        // file:genesis/utils/bit/bitvector.hpp line:452
        cl.def(
            "__eq__",
            ( bool( genesis::utils::bit::Bitvector::* )( const class genesis::utils::bit::
                                                             Bitvector& ) const ) &
                genesis::utils::bit::Bitvector::operator==,
            "C++: genesis::utils::bit::Bitvector::operator==(const class "
            "genesis::utils::bit::Bitvector &) const --> bool",
            pybind11::arg( "other" )
        );
        // function-signature: bool genesis::utils::bit::Bitvector::operator!=(const class
        // genesis::utils::bit::Bitvector &) const(const class genesis::utils::bit::Bitvector &)
        // file:genesis/utils/bit/bitvector.hpp line:453
        cl.def(
            "__ne__",
            ( bool( genesis::utils::bit::Bitvector::* )( const class genesis::utils::bit::
                                                             Bitvector& ) const ) &
                genesis::utils::bit::Bitvector::operator!=,
            "C++: genesis::utils::bit::Bitvector::operator!=(const class "
            "genesis::utils::bit::Bitvector &) const --> bool",
            pybind11::arg( "other" )
        );

        cl.def( "__str__", []( genesis::utils::bit::Bitvector const& o ) -> std::string {
            std::ostringstream s;
            using namespace genesis::utils::bit;
            s << o;
            return s.str();
        } );

        genesis_utils_bit_bitvector_add_ons( cl );
    }
}

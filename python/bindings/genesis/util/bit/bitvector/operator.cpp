#include <genesis/util/bit/bitvector.hpp>
#include <genesis/util/bit/bitvector/operator.hpp>
#include <genesis/util/bit/twobit_vector_add_ons.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
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

void bind_genesis_util_bit_bitvector_operator(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    // genesis::util::bit::BitwiseOperatorLengthPolicy file:genesis/util/bit/bitvector/operator.hpp
    // line:61
    pybind11::enum_<genesis::util::bit::BitwiseOperatorLengthPolicy>(
        M( "genesis::util::bit" ),
        "BitwiseOperatorLengthPolicy",
        "Policy to decide how to combine Bitvector%s of different lengths.\n\n This is used in "
        "bitwise_and(), bitwise_or(), and bitwise_xor(). For these functions, when\n applying them "
        "to Bitvector%s of different lengths, we need to decide which length to use."
    )
        .value( "kExpectEqual", genesis::util::bit::BitwiseOperatorLengthPolicy::kExpectEqual )
        .value( "kUseShorter", genesis::util::bit::BitwiseOperatorLengthPolicy::kUseShorter )
        .value( "kUseLonger", genesis::util::bit::BitwiseOperatorLengthPolicy::kUseLonger )
        .value( "kUseFirst", genesis::util::bit::BitwiseOperatorLengthPolicy::kUseFirst )
        .value( "kUseSecond", genesis::util::bit::BitwiseOperatorLengthPolicy::kUseSecond );

    ;

    // genesis::util::bit::bitwise_and(const class genesis::util::bit::Bitvector &, const class
    // genesis::util::bit::Bitvector &, enum genesis::util::bit::BitwiseOperatorLengthPolicy)
    // file:genesis/util/bit/bitvector/operator.hpp line:108 function-signature: class
    // genesis::util::bit::Bitvector genesis::util::bit::bitwise_and(const class
    // genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &, enum
    // genesis::util::bit::BitwiseOperatorLengthPolicy)(const class genesis::util::bit::Bitvector &,
    // const class genesis::util::bit::Bitvector &, enum
    // genesis::util::bit::BitwiseOperatorLengthPolicy) file:genesis/util/bit/bitvector/operator.hpp
    // line:108
    M( "genesis::util::bit" )
        .def(
            "bitwise_and",
            []( const class genesis::util::bit::Bitvector& a0,
                const class genesis::util::bit::Bitvector& a1 ) -> genesis::util::bit::Bitvector {
                return genesis::util::bit::bitwise_and( a0, a1 );
            },
            "",
            pybind11::arg( "lhs" ),
            pybind11::arg( "rhs" )
        );
    M( "genesis::util::bit" )
        .def(
            "bitwise_and",
            ( class genesis::util::bit::Bitvector( * )(
                const class genesis::util::bit::Bitvector&,
                const class genesis::util::bit::Bitvector&,
                enum genesis::util::bit::BitwiseOperatorLengthPolicy
            ) ) &
                genesis::util::bit::bitwise_and,
            "Take the bitwise `and` of two Bitvector%s of potentially different size.\n\n The "
            "function is the same as the normal version, but allows to use Bitvector%s of "
            "different sizes.\n By default (`policty == kExpectEqual`), the vectors are expected "
            "to be of equal length.\n See BitwiseOperatorLengthPolicy for other choices of the "
            "policy.\n\nC++: genesis::util::bit::bitwise_and(const class "
            "genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &, enum "
            "genesis::util::bit::BitwiseOperatorLengthPolicy) --> class "
            "genesis::util::bit::Bitvector",
            pybind11::arg( "lhs" ),
            pybind11::arg( "rhs" ),
            pybind11::arg( "length_policy" )
        );

    // genesis::util::bit::bitwise_or(const class genesis::util::bit::Bitvector &, const class
    // genesis::util::bit::Bitvector &, enum genesis::util::bit::BitwiseOperatorLengthPolicy)
    // file:genesis/util/bit/bitvector/operator.hpp line:120 function-signature: class
    // genesis::util::bit::Bitvector genesis::util::bit::bitwise_or(const class
    // genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &, enum
    // genesis::util::bit::BitwiseOperatorLengthPolicy)(const class genesis::util::bit::Bitvector &,
    // const class genesis::util::bit::Bitvector &, enum
    // genesis::util::bit::BitwiseOperatorLengthPolicy) file:genesis/util/bit/bitvector/operator.hpp
    // line:120
    M( "genesis::util::bit" )
        .def(
            "bitwise_or",
            []( const class genesis::util::bit::Bitvector& a0,
                const class genesis::util::bit::Bitvector& a1 ) -> genesis::util::bit::Bitvector {
                return genesis::util::bit::bitwise_or( a0, a1 );
            },
            "",
            pybind11::arg( "lhs" ),
            pybind11::arg( "rhs" )
        );
    M( "genesis::util::bit" )
        .def(
            "bitwise_or",
            ( class genesis::util::bit::Bitvector( * )(
                const class genesis::util::bit::Bitvector&,
                const class genesis::util::bit::Bitvector&,
                enum genesis::util::bit::BitwiseOperatorLengthPolicy
            ) ) &
                genesis::util::bit::bitwise_or,
            "Take the bitwise `or` of two Bitvector%s of potentially different size.\n\n The "
            "function is the same as the normal version, but allows to use Bitvector%s of "
            "different sizes.\n By default (`policty == kExpectEqual`), the vectors are expected "
            "to be of equal length.\n See BitwiseOperatorLengthPolicy for other choices of the "
            "policy.\n\nC++: genesis::util::bit::bitwise_or(const class "
            "genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &, enum "
            "genesis::util::bit::BitwiseOperatorLengthPolicy) --> class "
            "genesis::util::bit::Bitvector",
            pybind11::arg( "lhs" ),
            pybind11::arg( "rhs" ),
            pybind11::arg( "length_policy" )
        );

    // genesis::util::bit::bitwise_xor(const class genesis::util::bit::Bitvector &, const class
    // genesis::util::bit::Bitvector &, enum genesis::util::bit::BitwiseOperatorLengthPolicy)
    // file:genesis/util/bit/bitvector/operator.hpp line:130 function-signature: class
    // genesis::util::bit::Bitvector genesis::util::bit::bitwise_xor(const class
    // genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &, enum
    // genesis::util::bit::BitwiseOperatorLengthPolicy)(const class genesis::util::bit::Bitvector &,
    // const class genesis::util::bit::Bitvector &, enum
    // genesis::util::bit::BitwiseOperatorLengthPolicy) file:genesis/util/bit/bitvector/operator.hpp
    // line:130
    M( "genesis::util::bit" )
        .def(
            "bitwise_xor",
            []( const class genesis::util::bit::Bitvector& a0,
                const class genesis::util::bit::Bitvector& a1 ) -> genesis::util::bit::Bitvector {
                return genesis::util::bit::bitwise_xor( a0, a1 );
            },
            "",
            pybind11::arg( "lhs" ),
            pybind11::arg( "rhs" )
        );
    M( "genesis::util::bit" )
        .def(
            "bitwise_xor",
            ( class genesis::util::bit::Bitvector( * )(
                const class genesis::util::bit::Bitvector&,
                const class genesis::util::bit::Bitvector&,
                enum genesis::util::bit::BitwiseOperatorLengthPolicy
            ) ) &
                genesis::util::bit::bitwise_xor,
            "Take the bitwise `xor` of two Bitvector%s of potentially different size.\n\n \n\nC++: "
            "genesis::util::bit::bitwise_xor(const class genesis::util::bit::Bitvector &, const "
            "class genesis::util::bit::Bitvector &, enum "
            "genesis::util::bit::BitwiseOperatorLengthPolicy) --> class "
            "genesis::util::bit::Bitvector",
            pybind11::arg( "lhs" ),
            pybind11::arg( "rhs" ),
            pybind11::arg( "length_policy" )
        );

    // genesis::util::bit::bit_string_header(unsigned long, bool)
    // file:genesis/util/bit/bitvector/operator.hpp line:143 function-signature: std::string
    // genesis::util::bit::bit_string_header(unsigned long, bool)(unsigned long, bool)
    // file:genesis/util/bit/bitvector/operator.hpp line:143
    M( "genesis::util::bit" )
        .def(
            "bit_string_header",
            []( unsigned long const& a0 ) -> std::string {
                return genesis::util::bit::bit_string_header( a0 );
            },
            "",
            pybind11::arg( "n" )
        );
    M( "genesis::util::bit" )
        .def(
            "bit_string_header",
            ( std::string( * )( unsigned long, bool ) ) & genesis::util::bit::bit_string_header,
            "Helper for to_bit_string() to print decimals as a header\n for easier readability of "
            "bit positions.\n\nC++: genesis::util::bit::bit_string_header(unsigned long, bool) --> "
            "std::string",
            pybind11::arg( "n" ),
            pybind11::arg( "with_dec_line" )
        );

    // genesis::util::bit::to_bit_string(const class genesis::util::bit::Bitvector &, bool, char,
    // char) file:genesis/util/bit/bitvector/operator.hpp line:148 function-signature: std::string
    // genesis::util::bit::to_bit_string(const class genesis::util::bit::Bitvector &, bool, char,
    // char)(const class genesis::util::bit::Bitvector &, bool, char, char)
    // file:genesis/util/bit/bitvector/operator.hpp line:148
    M( "genesis::util::bit" )
        .def(
            "to_bit_string",
            []( const class genesis::util::bit::Bitvector& a0 ) -> std::string {
                return genesis::util::bit::to_bit_string( a0 );
            },
            "",
            pybind11::arg( "bv" )
        );
    M( "genesis::util::bit" )
        .def(
            "to_bit_string",
            []( const class genesis::util::bit::Bitvector& a0, bool const& a1 ) -> std::string {
                return genesis::util::bit::to_bit_string( a0, a1 );
            },
            "",
            pybind11::arg( "bv" ),
            pybind11::arg( "with_line_breaks" )
        );
    M( "genesis::util::bit" )
        .def(
            "to_bit_string",
            []( const class genesis::util::bit::Bitvector& a0, bool const& a1, char const& a2
            ) -> std::string { return genesis::util::bit::to_bit_string( a0, a1, a2 ); },
            "",
            pybind11::arg( "bv" ),
            pybind11::arg( "with_line_breaks" ),
            pybind11::arg( "zero" )
        );
    M( "genesis::util::bit" )
        .def(
            "to_bit_string",
            ( std::string( * )( const class genesis::util::bit::Bitvector&, bool, char, char ) ) &
                genesis::util::bit::to_bit_string,
            "Print the bits of a Bitvector to a string.\n\nC++: "
            "genesis::util::bit::to_bit_string(const class genesis::util::bit::Bitvector &, bool, "
            "char, char) --> std::string",
            pybind11::arg( "bv" ),
            pybind11::arg( "with_line_breaks" ),
            pybind11::arg( "zero" ),
            pybind11::arg( "one" )
        );

    // genesis::util::bit::serialized_bitvector_size(unsigned long)
    // file:genesis/util/bit/bitvector/operator.hpp line:183 function-signature: unsigned long
    // genesis::util::bit::serialized_bitvector_size(unsigned long)(unsigned long)
    // file:genesis/util/bit/bitvector/operator.hpp line:183
    M( "genesis::util::bit" )
        .def(
            "serialized_bitvector_size",
            ( unsigned long ( * )( unsigned long ) ) &
                genesis::util::bit::serialized_bitvector_size,
            "Get the size in the binary output of a serialized Bitvector using Serializer,\n given "
            "the number of bits being serialized.\n\nC++: "
            "genesis::util::bit::serialized_bitvector_size(unsigned long) --> unsigned long",
            pybind11::arg( "bit_size" )
        );

    // genesis::util::bit::serialized_bitvector_size(const class genesis::util::bit::Bitvector &)
    // file:genesis/util/bit/bitvector/operator.hpp line:194 function-signature: unsigned long
    // genesis::util::bit::serialized_bitvector_size(const class genesis::util::bit::Bitvector
    // &)(const class genesis::util::bit::Bitvector &) file:genesis/util/bit/bitvector/operator.hpp
    // line:194
    M( "genesis::util::bit" )
        .def(
            "serialized_bitvector_size",
            ( unsigned long ( * )( const class genesis::util::bit::Bitvector& ) ) &
                genesis::util::bit::serialized_bitvector_size,
            "Get the size in the binary output of a serialized Bitvector using Serializer.\n\nC++: "
            "genesis::util::bit::serialized_bitvector_size(const class "
            "genesis::util::bit::Bitvector &) --> unsigned long",
            pybind11::arg( "bv" )
        );

    { // genesis::util::bit::BitvectorHash file:genesis/util/bit/bitvector/operator.hpp line:216
        pybind11::class_<
            genesis::util::bit::BitvectorHash,
            std::shared_ptr<genesis::util::bit::BitvectorHash>>
            cl( M( "genesis::util::bit" ),
                "BitvectorHash",
                "Helper structure that yields the hash of a given Bitvector.\n\n It is meant to be "
                "used in containers such as `unordered_set` or `unordered_map`\n that can make use "
                "of custom hash functions for the key objects. By default,\n these containers use "
                "a specialization of the `std::hash` template, which we also offer,\n and that "
                "also uses the Bitvector::hash() function.\n\n Hence, this class here is slightly "
                "redundant, as it gives the same result as just using\n the `std::hash` "
                "specialization. Still, it might be useful to have.\n\n See also BitvectorXhash "
                "for an alternative version that uses Bitvector::x_hash() instead." );
        // function-signature: genesis::util::bit::BitvectorHash()
        cl.def( pybind11::init( []() { return new genesis::util::bit::BitvectorHash(); } ) );
        // function-signature: std::size_t genesis::util::bit::BitvectorHash::operator()(const class
        // genesis::util::bit::Bitvector &) const(const class genesis::util::bit::Bitvector &)
        // file:genesis/util/bit/bitvector/operator.hpp line:218
        cl.def(
            "__call__",
            ( std::size_t( genesis::util::bit::BitvectorHash::* )( const class genesis::util::bit::
                                                                       Bitvector& ) const ) &
                genesis::util::bit::BitvectorHash::operator(),
            "C++: genesis::util::bit::BitvectorHash::operator()(const class "
            "genesis::util::bit::Bitvector &) const --> std::size_t",
            pybind11::arg( "value" )
        );
    }
    { // genesis::util::bit::BitvectorXhash file:genesis/util/bit/bitvector/operator.hpp line:239
        pybind11::class_<
            genesis::util::bit::BitvectorXhash,
            std::shared_ptr<genesis::util::bit::BitvectorXhash>>
            cl( M( "genesis::util::bit" ),
                "BitvectorXhash",
                "Helper structure that yields the x_hash of a given Bitvector.\n\n It is meant to "
                "be used in containers such as `unordered_set` or `unordered_map`\n that can make "
                "use of custom hash functions for the key objects. Using this class instead\n of "
                "the standard `std::hash` specialization, the Bitvector::x_hash() function is used "
                "instead\n of the standard hash() function. It is hence faster to compute, but "
                "without avalanche effect.\n\n In some use cases, this might be preferrable - we "
                "however recommend to test this, in order to\n make sure that colliding hashes do "
                "not slow down the performance in the end.\n\n Note that the function needs to "
                "cast from Bitvector::IntType to std::size_t.\n On most modern systems, these are "
                "expecte to be the same, i.e., 64 bit unsigned integers.\n However, this might "
                "cause problem on systems where this is not the case." );
        // function-signature: genesis::util::bit::BitvectorXhash()
        cl.def( pybind11::init( []() { return new genesis::util::bit::BitvectorXhash(); } ) );
        // function-signature: std::size_t genesis::util::bit::BitvectorXhash::operator()(const
        // class genesis::util::bit::Bitvector &) const(const class genesis::util::bit::Bitvector &)
        // file:genesis/util/bit/bitvector/operator.hpp line:241
        cl.def(
            "__call__",
            ( std::size_t( genesis::util::bit::BitvectorXhash::* )( const class genesis::util::bit::
                                                                        Bitvector& ) const ) &
                genesis::util::bit::BitvectorXhash::operator(),
            "C++: genesis::util::bit::BitvectorXhash::operator()(const class "
            "genesis::util::bit::Bitvector &) const --> std::size_t",
            pybind11::arg( "value" )
        );
    }
}

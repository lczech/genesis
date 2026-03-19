#include <functional>
#include <genesis/util/bit/bitvector.hpp>
#include <genesis/util/bit/bitvector/function.hpp>
#include <genesis/util/bit/twobit_vector_add_ons.hpp>
#include <iterator>
#include <memory>
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

void bind_genesis_util_bit_bitvector_function(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    // genesis::util::bit::make_bool_vector_from_indices(const class std::vector<unsigned long> &,
    // unsigned long) file:genesis/util/bit/bitvector/function.hpp line:62 function-signature: class
    // std::vector<bool> genesis::util::bit::make_bool_vector_from_indices(const class
    // std::vector<unsigned long> &, unsigned long)(const class std::vector<unsigned long> &,
    // unsigned long) file:genesis/util/bit/bitvector/function.hpp line:62
    M( "genesis::util::bit" )
        .def(
            "make_bool_vector_from_indices",
            []( const class std::vector<unsigned long>& a0 ) -> std::vector<bool> {
                return genesis::util::bit::make_bool_vector_from_indices( a0 );
            },
            "",
            pybind11::arg( "indices" )
        );
    M( "genesis::util::bit" )
        .def(
            "make_bool_vector_from_indices",
            ( class std::vector<bool>( * )( const class std::vector<unsigned long>&, unsigned long )
            ) & genesis::util::bit::make_bool_vector_from_indices,
            "Helper function to create a bool vector from a set of indices to be set to "
            "`true`.\n\n The function expectes a list of indices. It returns a bool vector with "
            "the size of the largest\n index, or the provided  (if set to a value > 0), where all "
            "positions of these indices are\n `true`, and all other positions are `false`.\n\nC++: "
            "genesis::util::bit::make_bool_vector_from_indices(const class std::vector<unsigned "
            "long> &, unsigned long) --> class std::vector<bool>",
            pybind11::arg( "indices" ),
            pybind11::arg( "size" )
        );

    // genesis::util::bit::make_random_bitvector(unsigned long)
    // file:genesis/util/bit/bitvector/function.hpp line:70 function-signature: class
    // genesis::util::bit::Bitvector genesis::util::bit::make_random_bitvector(unsigned
    // long)(unsigned long) file:genesis/util/bit/bitvector/function.hpp line:70
    M( "genesis::util::bit" )
        .def(
            "make_random_bitvector",
            ( class genesis::util::bit::Bitvector( * )( unsigned long ) ) &
                genesis::util::bit::make_random_bitvector,
            "Create a Bitvector of a given  with randomly initialized bits, mostly for "
            "testing.\n\nC++: genesis::util::bit::make_random_bitvector(unsigned long) --> class "
            "genesis::util::bit::Bitvector",
            pybind11::arg( "size" )
        );

    // genesis::util::bit::negate(class genesis::util::bit::Bitvector &)
    // file:genesis/util/bit/bitvector/function.hpp line:79 function-signature: void
    // genesis::util::bit::negate(class genesis::util::bit::Bitvector &)(class
    // genesis::util::bit::Bitvector &) file:genesis/util/bit/bitvector/function.hpp line:79
    M( "genesis::util::bit" )
        .def(
            "negate",
            ( void ( * )( class genesis::util::bit::Bitvector& ) ) & genesis::util::bit::negate,
            "Flip all bits. Alias for invert().\n\nC++: genesis::util::bit::negate(class "
            "genesis::util::bit::Bitvector &) --> void",
            pybind11::arg( "bv" )
        );

    // genesis::util::bit::invert(class genesis::util::bit::Bitvector &)
    // file:genesis/util/bit/bitvector/function.hpp line:84 function-signature: void
    // genesis::util::bit::invert(class genesis::util::bit::Bitvector &)(class
    // genesis::util::bit::Bitvector &) file:genesis/util/bit/bitvector/function.hpp line:84
    M( "genesis::util::bit" )
        .def(
            "invert",
            ( void ( * )( class genesis::util::bit::Bitvector& ) ) & genesis::util::bit::invert,
            "Flip all bits. Alias for negate().\n\nC++: genesis::util::bit::invert(class "
            "genesis::util::bit::Bitvector &) --> void",
            pybind11::arg( "bv" )
        );

    // genesis::util::bit::normalize(class genesis::util::bit::Bitvector &)
    // file:genesis/util/bit/bitvector/function.hpp line:92 function-signature: void
    // genesis::util::bit::normalize(class genesis::util::bit::Bitvector &)(class
    // genesis::util::bit::Bitvector &) file:genesis/util/bit/bitvector/function.hpp line:92
    M( "genesis::util::bit" )
        .def(
            "normalize",
            ( void ( * )( class genesis::util::bit::Bitvector& ) ) & genesis::util::bit::normalize,
            "Bring the Bitvector in a normalized form, where the first bit is always zero.\n\n If "
            "the first bit is zero, nothing happens. However, if is is one, the whole Bitvector is "
            "flipped\n using negate().\n\nC++: genesis::util::bit::normalize(class "
            "genesis::util::bit::Bitvector &) --> void",
            pybind11::arg( "bv" )
        );

    // genesis::util::bit::bitvector_hash(const class genesis::util::bit::Bitvector &)
    // file:genesis/util/bit/bitvector/function.hpp line:101 function-signature: unsigned long
    // genesis::util::bit::bitvector_hash(const class genesis::util::bit::Bitvector &)(const class
    // genesis::util::bit::Bitvector &) file:genesis/util/bit/bitvector/function.hpp line:101
    M( "genesis::util::bit" )
        .def(
            "bitvector_hash",
            ( unsigned long ( * )( const class genesis::util::bit::Bitvector& ) ) &
                genesis::util::bit::bitvector_hash,
            "Return an std::hash value for the Bitvector.\n\nC++: "
            "genesis::util::bit::bitvector_hash(const class genesis::util::bit::Bitvector &) --> "
            "unsigned long",
            pybind11::arg( "bv" )
        );

    // genesis::util::bit::bitvector_x_hash(const class genesis::util::bit::Bitvector &)
    // file:genesis/util/bit/bitvector/function.hpp line:110 function-signature: unsigned long
    // genesis::util::bit::bitvector_x_hash(const class genesis::util::bit::Bitvector &)(const class
    // genesis::util::bit::Bitvector &) file:genesis/util/bit/bitvector/function.hpp line:110
    M( "genesis::util::bit" )
        .def(
            "bitvector_x_hash",
            ( unsigned long ( * )( const class genesis::util::bit::Bitvector& ) ) &
                genesis::util::bit::bitvector_x_hash,
            "Return a hash value of type IntType that is quicker to calculate than hash().\n\n "
            "This can be used for obtaining a simple hash using xor of the words.\n The avalanche "
            "effect is of course not present, but for many applications, this hash is\n good "
            "enough and quite useful.\n\nC++: genesis::util::bit::bitvector_x_hash(const class "
            "genesis::util::bit::Bitvector &) --> unsigned long",
            pybind11::arg( "bv" )
        );

    // genesis::util::bit::pop_count(const class genesis::util::bit::Bitvector &)
    // file:genesis/util/bit/bitvector/function.hpp line:120 function-signature: unsigned long
    // genesis::util::bit::pop_count(const class genesis::util::bit::Bitvector &)(const class
    // genesis::util::bit::Bitvector &) file:genesis/util/bit/bitvector/function.hpp line:120
    M( "genesis::util::bit" )
        .def(
            "pop_count",
            ( unsigned long ( * )( const class genesis::util::bit::Bitvector& ) ) &
                genesis::util::bit::pop_count,
            "Count the number of set bits in the Bitvector, that is, its Hamming weight,\n or "
            "population count (popcnt).\n\nC++: genesis::util::bit::pop_count(const class "
            "genesis::util::bit::Bitvector &) --> unsigned long",
            pybind11::arg( "bv" )
        );

    // genesis::util::bit::pop_count(const class genesis::util::bit::Bitvector &, unsigned long,
    // unsigned long) file:genesis/util/bit/bitvector/function.hpp line:138 function-signature:
    // unsigned long genesis::util::bit::pop_count(const class genesis::util::bit::Bitvector &,
    // unsigned long, unsigned long)(const class genesis::util::bit::Bitvector &, unsigned long,
    // unsigned long) file:genesis/util/bit/bitvector/function.hpp line:138
    M( "genesis::util::bit" )
        .def(
            "pop_count",
            ( unsigned long ( * )(
                const class genesis::util::bit::Bitvector&, unsigned long, unsigned long
            ) ) &
                genesis::util::bit::pop_count,
            "Count the number of set bits between a range of indices in the Bitvector,\n that is, "
            "its Hamming weight, or population count (popcnt), for that range.\n\n The range  to  "
            "is zero-based, with last being the past-the-end index.\n Hence, this is the same "
            "as\n\n     size_t count = 0;\n     for( size_t i = first; i < last; ++i ) {\n         "
            "if( bitvector.get( i )) {\n             ++count;\n         }\n     }\n\n but faster, "
            "as we use whole-word counting internally.\n\nC++: genesis::util::bit::pop_count(const "
            "class genesis::util::bit::Bitvector &, unsigned long, unsigned long) --> unsigned "
            "long",
            pybind11::arg( "bv" ),
            pybind11::arg( "first" ),
            pybind11::arg( "last" )
        );

    // genesis::util::bit::all_set(const class genesis::util::bit::Bitvector &)
    // file:genesis/util/bit/bitvector/function.hpp line:147 function-signature: bool
    // genesis::util::bit::all_set(const class genesis::util::bit::Bitvector &)(const class
    // genesis::util::bit::Bitvector &) file:genesis/util/bit/bitvector/function.hpp line:147
    M( "genesis::util::bit" )
        .def(
            "all_set",
            ( bool ( * )( const class genesis::util::bit::Bitvector& ) ) &
                genesis::util::bit::all_set,
            "Return if all bits are set, i.e., the Bitvector is all one.\n\nC++: "
            "genesis::util::bit::all_set(const class genesis::util::bit::Bitvector &) --> bool",
            pybind11::arg( "bv" )
        );

    // genesis::util::bit::all_unset(const class genesis::util::bit::Bitvector &)
    // file:genesis/util/bit/bitvector/function.hpp line:154 function-signature: bool
    // genesis::util::bit::all_unset(const class genesis::util::bit::Bitvector &)(const class
    // genesis::util::bit::Bitvector &) file:genesis/util/bit/bitvector/function.hpp line:154
    M( "genesis::util::bit" )
        .def(
            "all_unset",
            ( bool ( * )( const class genesis::util::bit::Bitvector& ) ) &
                genesis::util::bit::all_unset,
            "Return if all bits are unset, i.e., the Bitvector is all zero.\n\n Alias for "
            "none_set()\n\nC++: genesis::util::bit::all_unset(const class "
            "genesis::util::bit::Bitvector &) --> bool",
            pybind11::arg( "bv" )
        );

    // genesis::util::bit::any_set(const class genesis::util::bit::Bitvector &)
    // file:genesis/util/bit/bitvector/function.hpp line:159 function-signature: bool
    // genesis::util::bit::any_set(const class genesis::util::bit::Bitvector &)(const class
    // genesis::util::bit::Bitvector &) file:genesis/util/bit/bitvector/function.hpp line:159
    M( "genesis::util::bit" )
        .def(
            "any_set",
            ( bool ( * )( const class genesis::util::bit::Bitvector& ) ) &
                genesis::util::bit::any_set,
            "Return if any bits are set, i.e., the Bitvector is not all zero.\n\nC++: "
            "genesis::util::bit::any_set(const class genesis::util::bit::Bitvector &) --> bool",
            pybind11::arg( "bv" )
        );

    // genesis::util::bit::any_unset(const class genesis::util::bit::Bitvector &)
    // file:genesis/util/bit/bitvector/function.hpp line:164 function-signature: bool
    // genesis::util::bit::any_unset(const class genesis::util::bit::Bitvector &)(const class
    // genesis::util::bit::Bitvector &) file:genesis/util/bit/bitvector/function.hpp line:164
    M( "genesis::util::bit" )
        .def(
            "any_unset",
            ( bool ( * )( const class genesis::util::bit::Bitvector& ) ) &
                genesis::util::bit::any_unset,
            "Return if any bits are unset, i.e., the Bitvector is not all one.\n\nC++: "
            "genesis::util::bit::any_unset(const class genesis::util::bit::Bitvector &) --> bool",
            pybind11::arg( "bv" )
        );

    // genesis::util::bit::none_set(const class genesis::util::bit::Bitvector &)
    // file:genesis/util/bit/bitvector/function.hpp line:171 function-signature: bool
    // genesis::util::bit::none_set(const class genesis::util::bit::Bitvector &)(const class
    // genesis::util::bit::Bitvector &) file:genesis/util/bit/bitvector/function.hpp line:171
    M( "genesis::util::bit" )
        .def(
            "none_set",
            ( bool ( * )( const class genesis::util::bit::Bitvector& ) ) &
                genesis::util::bit::none_set,
            "Return if no bits are set, i.e., the Bitvector is all zero.\n\n Alias for "
            "all_unset()\n\nC++: genesis::util::bit::none_set(const class "
            "genesis::util::bit::Bitvector &) --> bool",
            pybind11::arg( "bv" )
        );

    // genesis::util::bit::find_first_set(const class genesis::util::bit::Bitvector &)
    // file:genesis/util/bit/bitvector/function.hpp line:179 function-signature: unsigned long
    // genesis::util::bit::find_first_set(const class genesis::util::bit::Bitvector &)(const class
    // genesis::util::bit::Bitvector &) file:genesis/util/bit/bitvector/function.hpp line:179
    M( "genesis::util::bit" )
        .def(
            "find_first_set",
            ( unsigned long ( * )( const class genesis::util::bit::Bitvector& ) ) &
                genesis::util::bit::find_first_set,
            "Return the index of the first bit in the Bitvector that is set.\n\n If no such "
            "position exists (because all bits are `false`), Bitvector::npos\n is "
            "returned.\n\nC++: genesis::util::bit::find_first_set(const class "
            "genesis::util::bit::Bitvector &) --> unsigned long",
            pybind11::arg( "bv" )
        );

    // genesis::util::bit::find_last_set(const class genesis::util::bit::Bitvector &)
    // file:genesis/util/bit/bitvector/function.hpp line:187 function-signature: unsigned long
    // genesis::util::bit::find_last_set(const class genesis::util::bit::Bitvector &)(const class
    // genesis::util::bit::Bitvector &) file:genesis/util/bit/bitvector/function.hpp line:187
    M( "genesis::util::bit" )
        .def(
            "find_last_set",
            ( unsigned long ( * )( const class genesis::util::bit::Bitvector& ) ) &
                genesis::util::bit::find_last_set,
            "Return the index of the last bit in the Bitvector that is set.\n\n If no such "
            "position exists (because all bits are `false`), Bitvector::npos\n is "
            "returned.\n\nC++: genesis::util::bit::find_last_set(const class "
            "genesis::util::bit::Bitvector &) --> unsigned long",
            pybind11::arg( "bv" )
        );

    // genesis::util::bit::find_next_set(const class genesis::util::bit::Bitvector &, unsigned long)
    // file:genesis/util/bit/bitvector/function.hpp line:196 function-signature: unsigned long
    // genesis::util::bit::find_next_set(const class genesis::util::bit::Bitvector &, unsigned
    // long)(const class genesis::util::bit::Bitvector &, unsigned long)
    // file:genesis/util/bit/bitvector/function.hpp line:196
    M( "genesis::util::bit" )
        .def(
            "find_next_set",
            ( unsigned long ( * )( const class genesis::util::bit::Bitvector&, unsigned long ) ) &
                genesis::util::bit::find_next_set,
            "Return the index of the next position in the Bitvector that is set.\n\n This returns "
            "the first position starting at  including  itself, that is set.\n If no such position "
            "exists (because all following bits are `false`), or if  is beyond\n the length of the "
            "vector, Bitvector::npos is returned instead.\n\nC++: "
            "genesis::util::bit::find_next_set(const class genesis::util::bit::Bitvector &, "
            "unsigned long) --> unsigned long",
            pybind11::arg( "bv" ),
            pybind11::arg( "start" )
        );

    // genesis::util::bit::for_each_set_bit(const class genesis::util::bit::Bitvector &, const class
    // std::function<void (unsigned long)> &) file:genesis/util/bit/bitvector/function.hpp line:203
    // function-signature: void genesis::util::bit::for_each_set_bit(const class
    // genesis::util::bit::Bitvector &, const class std::function<void (unsigned long)> &)(const
    // class genesis::util::bit::Bitvector &, const class std::function<void (unsigned long)> &)
    // file:genesis/util/bit/bitvector/function.hpp line:203
    M( "genesis::util::bit" )
        .def(
            "for_each_set_bit",
            ( void ( * )( const class genesis::util::bit::Bitvector&, const class std::function<void( unsigned long )>& )
            ) & genesis::util::bit::for_each_set_bit,
            "Call a function for every bit position that is set in the \n\n The callback receives "
            "the overall bit position.\n\nC++: genesis::util::bit::for_each_set_bit(const class "
            "genesis::util::bit::Bitvector &, const class std::function<void (unsigned long)> &) "
            "--> void",
            pybind11::arg( "bitvector" ),
            pybind11::arg( "callback" )
        );

    // genesis::util::bit::set_minus(const class genesis::util::bit::Bitvector &, const class
    // genesis::util::bit::Bitvector &) file:genesis/util/bit/bitvector/function.hpp line:212
    // function-signature: class genesis::util::bit::Bitvector genesis::util::bit::set_minus(const
    // class genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &)(const
    // class genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &)
    // file:genesis/util/bit/bitvector/function.hpp line:212
    M( "genesis::util::bit" )
        .def(
            "set_minus",
            ( class genesis::util::bit::
                  Bitvector( * )( const class genesis::util::bit::Bitvector&, const class genesis::util::bit::Bitvector& )
            ) & genesis::util::bit::set_minus,
            "Compute the set minus `lhs & (~rhs)` between two Bitvector%s.\n\nC++: "
            "genesis::util::bit::set_minus(const class genesis::util::bit::Bitvector &, const "
            "class genesis::util::bit::Bitvector &) --> class genesis::util::bit::Bitvector",
            pybind11::arg( "lhs" ),
            pybind11::arg( "rhs" )
        );

    // genesis::util::bit::symmetric_difference(const class genesis::util::bit::Bitvector &, const
    // class genesis::util::bit::Bitvector &) file:genesis/util/bit/bitvector/function.hpp line:217
    // function-signature: class genesis::util::bit::Bitvector
    // genesis::util::bit::symmetric_difference(const class genesis::util::bit::Bitvector &, const
    // class genesis::util::bit::Bitvector &)(const class genesis::util::bit::Bitvector &, const
    // class genesis::util::bit::Bitvector &) file:genesis/util/bit/bitvector/function.hpp line:217
    M( "genesis::util::bit" )
        .def(
            "symmetric_difference",
            ( class genesis::util::bit::
                  Bitvector( * )( const class genesis::util::bit::Bitvector&, const class genesis::util::bit::Bitvector& )
            ) & genesis::util::bit::symmetric_difference,
            "Compute the symmetric differeence `(lhs | rhs) & ~(lhs & rhs)` between two "
            "Bitvector%s.\n\nC++: genesis::util::bit::symmetric_difference(const class "
            "genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &) --> "
            "class genesis::util::bit::Bitvector",
            pybind11::arg( "lhs" ),
            pybind11::arg( "rhs" )
        );
}

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

void bind_genesis_util_bit_bitvector_function_1(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    // genesis::util::bit::is_subset(const class genesis::util::bit::Bitvector &, const class
    // genesis::util::bit::Bitvector &) file:genesis/util/bit/bitvector/function.hpp line:222
    // function-signature: bool genesis::util::bit::is_subset(const class
    // genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &)(const class
    // genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &)
    // file:genesis/util/bit/bitvector/function.hpp line:222
    M( "genesis::util::bit" )
        .def(
            "is_subset",
            ( bool ( * )( const class genesis::util::bit::Bitvector&, const class genesis::util::bit::Bitvector& )
            ) & genesis::util::bit::is_subset,
            "Subset or equal.\n\nC++: genesis::util::bit::is_subset(const class "
            "genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &) --> "
            "bool",
            pybind11::arg( "sub" ),
            pybind11::arg( "super" )
        );

    // genesis::util::bit::is_superset(const class genesis::util::bit::Bitvector &, const class
    // genesis::util::bit::Bitvector &) file:genesis/util/bit/bitvector/function.hpp line:227
    // function-signature: bool genesis::util::bit::is_superset(const class
    // genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &)(const class
    // genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &)
    // file:genesis/util/bit/bitvector/function.hpp line:227
    M( "genesis::util::bit" )
        .def(
            "is_superset",
            ( bool ( * )( const class genesis::util::bit::Bitvector&, const class genesis::util::bit::Bitvector& )
            ) & genesis::util::bit::is_superset,
            "Superset or equal.\n\nC++: genesis::util::bit::is_superset(const class "
            "genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &) --> "
            "bool",
            pybind11::arg( "super" ),
            pybind11::arg( "sub" )
        );

    // genesis::util::bit::is_strict_subset(const class genesis::util::bit::Bitvector &, const class
    // genesis::util::bit::Bitvector &) file:genesis/util/bit/bitvector/function.hpp line:232
    // function-signature: bool genesis::util::bit::is_strict_subset(const class
    // genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &)(const class
    // genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &)
    // file:genesis/util/bit/bitvector/function.hpp line:232
    M( "genesis::util::bit" )
        .def(
            "is_strict_subset",
            ( bool ( * )( const class genesis::util::bit::Bitvector&, const class genesis::util::bit::Bitvector& )
            ) & genesis::util::bit::is_strict_subset,
            "Strict subset.\n\nC++: genesis::util::bit::is_strict_subset(const class "
            "genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &) --> "
            "bool",
            pybind11::arg( "sub" ),
            pybind11::arg( "super" )
        );

    // genesis::util::bit::is_strict_superset(const class genesis::util::bit::Bitvector &, const
    // class genesis::util::bit::Bitvector &) file:genesis/util/bit/bitvector/function.hpp line:237
    // function-signature: bool genesis::util::bit::is_strict_superset(const class
    // genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &)(const class
    // genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &)
    // file:genesis/util/bit/bitvector/function.hpp line:237
    M( "genesis::util::bit" )
        .def(
            "is_strict_superset",
            ( bool ( * )( const class genesis::util::bit::Bitvector&, const class genesis::util::bit::Bitvector& )
            ) & genesis::util::bit::is_strict_superset,
            "Strict superset.\n\nC++: genesis::util::bit::is_strict_superset(const class "
            "genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &) --> "
            "bool",
            pybind11::arg( "super" ),
            pybind11::arg( "sub" )
        );

    // genesis::util::bit::jaccard_similarity(const class genesis::util::bit::Bitvector &, const
    // class genesis::util::bit::Bitvector &) file:genesis/util/bit/bitvector/function.hpp line:250
    // function-signature: double genesis::util::bit::jaccard_similarity(const class
    // genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &)(const class
    // genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &)
    // file:genesis/util/bit/bitvector/function.hpp line:250
    M( "genesis::util::bit" )
        .def(
            "jaccard_similarity",
            ( double ( * )( const class genesis::util::bit::Bitvector&, const class genesis::util::bit::Bitvector& )
            ) & genesis::util::bit::jaccard_similarity,
            "Compute the Jaccard index (Jaccard similarity coefficient) for two Bitvector%s\n of "
            "the same size.\n\n This is simply the count of bits in the intersection divided by "
            "the count of bits in the union\n of the Bitvectors.\n\nC++: "
            "genesis::util::bit::jaccard_similarity(const class genesis::util::bit::Bitvector &, "
            "const class genesis::util::bit::Bitvector &) --> double",
            pybind11::arg( "lhs" ),
            pybind11::arg( "rhs" )
        );

    // genesis::util::bit::jaccard_distance(const class genesis::util::bit::Bitvector &, const class
    // genesis::util::bit::Bitvector &) file:genesis/util/bit/bitvector/function.hpp line:257
    // function-signature: double genesis::util::bit::jaccard_distance(const class
    // genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &)(const class
    // genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &)
    // file:genesis/util/bit/bitvector/function.hpp line:257
    M( "genesis::util::bit" )
        .def(
            "jaccard_distance",
            ( double ( * )( const class genesis::util::bit::Bitvector&, const class genesis::util::bit::Bitvector& )
            ) & genesis::util::bit::jaccard_distance,
            "Compute the Jaccard distance for two Bitvector%s of the same size.\n\n This "
            "dissimilarity is simply 1 - jaccard_similarity().\n\nC++: "
            "genesis::util::bit::jaccard_distance(const class genesis::util::bit::Bitvector &, "
            "const class genesis::util::bit::Bitvector &) --> double",
            pybind11::arg( "lhs" ),
            pybind11::arg( "rhs" )
        );

    // genesis::util::bit::hamming_distance(const class genesis::util::bit::Bitvector &, const class
    // genesis::util::bit::Bitvector &) file:genesis/util/bit/bitvector/function.hpp line:263
    // function-signature: unsigned long genesis::util::bit::hamming_distance(const class
    // genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &)(const class
    // genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &)
    // file:genesis/util/bit/bitvector/function.hpp line:263
    M( "genesis::util::bit" )
        .def(
            "hamming_distance",
            ( unsigned long ( * )( const class genesis::util::bit::Bitvector&, const class genesis::util::bit::Bitvector& )
            ) & genesis::util::bit::hamming_distance,
            "Compute the Hamming distance between two Bitvector%s,\n i.e., the Hamming weight (pop "
            "count) of the `xor` of the inputs.\n\nC++: genesis::util::bit::hamming_distance(const "
            "class genesis::util::bit::Bitvector &, const class genesis::util::bit::Bitvector &) "
            "--> unsigned long",
            pybind11::arg( "lhs" ),
            pybind11::arg( "rhs" )
        );
}

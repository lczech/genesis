/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_math_bitvector_operators_export, ::genesis::utils, scope )
{

    scope.def(
        "bitwise_and",
        ( Bitvector ( * )( Bitvector const &, Bitvector const & ))( &::genesis::utils::bitwise_and ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs"),
        get_docstring("Bitvector ::genesis::utils::bitwise_and (Bitvector const & lhs, Bitvector const & rhs)")
    );

    scope.def(
        "bitwise_or",
        ( Bitvector ( * )( Bitvector const &, Bitvector const & ))( &::genesis::utils::bitwise_or ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs"),
        get_docstring("Bitvector ::genesis::utils::bitwise_or (Bitvector const & lhs, Bitvector const & rhs)")
    );

    scope.def(
        "bitwise_xor",
        ( Bitvector ( * )( Bitvector const &, Bitvector const & ))( &::genesis::utils::bitwise_xor ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs"),
        get_docstring("Bitvector ::genesis::utils::bitwise_xor (Bitvector const & lhs, Bitvector const & rhs)")
    );

    scope.def(
        "operator&",
        ( Bitvector ( * )( Bitvector const &, Bitvector const & ))( &::genesis::utils::operator& ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );

    scope.def(
        "operator^",
        ( Bitvector ( * )( Bitvector const &, Bitvector const & ))( &::genesis::utils::operator^ ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );

    scope.def(
        "operator|",
        ( Bitvector ( * )( Bitvector const &, Bitvector const & ))( &::genesis::utils::operator| ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );

    scope.def(
        "set_minus",
        ( Bitvector ( * )( Bitvector const &, Bitvector const & ))( &::genesis::utils::set_minus ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );

    scope.def(
        "symmetric_difference",
        ( Bitvector ( * )( Bitvector const &, Bitvector const & ))( &::genesis::utils::symmetric_difference ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );

    scope.def(
        "is_strict_subset",
        ( bool ( * )( Bitvector const &, Bitvector const & ))( &::genesis::utils::is_strict_subset ),
            pybind11::arg("sub"),
            pybind11::arg("super"),
        get_docstring("bool ::genesis::utils::is_strict_subset (Bitvector const & sub, Bitvector const & super)")
    );

    scope.def(
        "is_strict_superset",
        ( bool ( * )( Bitvector const &, Bitvector const & ))( &::genesis::utils::is_strict_superset ),
            pybind11::arg("super"),
            pybind11::arg("sub"),
        get_docstring("bool ::genesis::utils::is_strict_superset (Bitvector const & super, Bitvector const & sub)")
    );

    scope.def(
        "is_subset",
        ( bool ( * )( Bitvector const &, Bitvector const & ))( &::genesis::utils::is_subset ),
            pybind11::arg("sub"),
            pybind11::arg("super"),
        get_docstring("bool ::genesis::utils::is_subset (Bitvector const & sub, Bitvector const & super)")
    );

    scope.def(
        "is_superset",
        ( bool ( * )( Bitvector const &, Bitvector const & ))( &::genesis::utils::is_superset ),
            pybind11::arg("super"),
            pybind11::arg("sub"),
        get_docstring("bool ::genesis::utils::is_superset (Bitvector const & super, Bitvector const & sub)")
    );

    scope.def(
        "operator>>",
        ( std::istream & ( * )( std::istream &, Bitvector & ))( &::genesis::utils::operator>> ),
            pybind11::arg("in"),
            pybind11::arg("bv"),
        get_docstring("std::istream & ::genesis::utils::operator>> (std::istream & in, Bitvector & bv)")
    );
}

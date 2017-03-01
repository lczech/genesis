/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_math_twobit_vector_functions_export, ::genesis::utils, scope )
{

    scope.def(
        "from_nucleic_acids",
        ( TwobitVector ( * )( std::string const & ))( &::genesis::utils::from_nucleic_acids ),
            pybind11::arg("sequence")
    );

    scope.def(
        "translate_from_nucleic_acid",
        ( TwobitVector::ValueType ( * )( char ))( &::genesis::utils::translate_from_nucleic_acid ),
            pybind11::arg("site")
    );

    scope.def(
        "translate_to_nucleic_acid",
        ( char ( * )( TwobitVector::ValueType ))( &::genesis::utils::translate_to_nucleic_acid ),
            pybind11::arg("value")
    );

    scope.def(
        "bitstring",
        ( std::string ( * )( TwobitVector const & ))( &::genesis::utils::bitstring ),
            pybind11::arg("vec")
    );

    scope.def(
        "bitstring",
        ( std::string ( * )( TwobitVector::WordType const & ))( &::genesis::utils::bitstring ),
            pybind11::arg("vec")
    );

    scope.def(
        "to_nucleic_acids",
        ( std::string ( * )( TwobitVector const & ))( &::genesis::utils::to_nucleic_acids ),
            pybind11::arg("vec")
    );
}

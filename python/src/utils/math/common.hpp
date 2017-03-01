/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

template <typename T>
void PythonExportClass_::genesis::utils::MinMaxPair(std::string name)
{

    // -------------------------------------------------------------------
    //     Class MinMaxPair
    // -------------------------------------------------------------------

    using namespace ::genesis::utils;

    using MinMaxPairType = MinMaxPair<typename T>;

    pybind11::class_< MinMaxPairType, std::shared_ptr<MinMaxPairType> > ( scope, name.c_str() )
    ;
}

template<typename T>
void python_export_function_utils_math_common_T ()
{
    scope.def(
        "abs_diff",
        ( constexpr T ( * )( T const &, T const & ))( &::genesis::utils::abs_diff ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs"),
        get_docstring("constexpr T ::genesis::utils::abs_diff (T const & lhs, T const & rhs)")
    );

    scope.def(
        "signum",
        ( constexpr int ( * )( T ))( &::genesis::utils::signum ),
            pybind11::arg("x"),
        get_docstring("constexpr int ::genesis::utils::signum (T x)")
    );

    scope.def(
        "signum",
        ( constexpr int ( * )( T, std::false_type ))( &::genesis::utils::signum ),
            pybind11::arg("x"),
            pybind11::arg(""),
        get_docstring("constexpr int ::genesis::utils::signum (T x, std::false_type )")
    );

    scope.def(
        "signum",
        ( constexpr int ( * )( T, std::true_type ))( &::genesis::utils::signum ),
            pybind11::arg("x"),
            pybind11::arg(""),
        get_docstring("constexpr int ::genesis::utils::signum (T x, std::true_type )")
    );
}

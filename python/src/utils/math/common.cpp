/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::MeanStddevPair, scope )
{

    // -------------------------------------------------------------------
    //     Class MeanStddevPair
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::MeanStddevPair, std::shared_ptr<::genesis::utils::MeanStddevPair> > ( scope, "MeanStddevPair" )
    ;
}

PYTHON_EXPORT_FUNCTIONS( utils_math_common_export, ::genesis::utils, scope )
{

    scope.def(
        "almost_equal_relative",
        ( bool ( * )( double, double, double ))( &::genesis::utils::almost_equal_relative ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs"),
            pybind11::arg("max_rel_diff")=(double)(std::numeric_limits< double >::epsilon()),
        get_docstring("bool ::genesis::utils::almost_equal_relative (double lhs, double rhs, double max_rel_diff=std::numeric_limits< double >::epsilon())")
    );

    scope.def(
        "round_to",
        ( double ( * )( double, size_t ))( &::genesis::utils::round_to ),
            pybind11::arg("x"),
            pybind11::arg("accuracy_order"),
        get_docstring("double ::genesis::utils::round_to (double x, size_t accuracy_order)")
    );
}

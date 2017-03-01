/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::TridiagonalDecompositionData, scope )
{

    // -------------------------------------------------------------------
    //     Class TridiagonalDecompositionData
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::TridiagonalDecompositionData, std::shared_ptr<::genesis::utils::TridiagonalDecompositionData> > ( scope, "TridiagonalDecompositionData" )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::utils::PcaData, scope )
{

    // -------------------------------------------------------------------
    //     Class PcaData
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::PcaData, std::shared_ptr<::genesis::utils::PcaData> > ( scope, "PcaData" )
    ;
}

PYTHON_EXPORT_FUNCTIONS( utils_math_matrix_pca_export, ::genesis::utils, scope )
{

    scope.def(
        "principal_component_analysis",
        ( PcaData ( * )( Matrix< double > const &, size_t, PcaStandardization ))( &::genesis::utils::principal_component_analysis ),
            pybind11::arg("data"),
            pybind11::arg("components")=(size_t)(0),
            pybind11::arg("standardization"),
        get_docstring("PcaData ::genesis::utils::principal_component_analysis (Matrix< double > const & data, size_t components=0, PcaStandardization standardization)")
    );

    scope.def(
        "reduce_to_tridiagonal_matrix",
        ( TridiagonalDecompositionData ( * )( Matrix< double > & ))( &::genesis::utils::reduce_to_tridiagonal_matrix ),
            pybind11::arg("data"),
        get_docstring("TridiagonalDecompositionData ::genesis::utils::reduce_to_tridiagonal_matrix (Matrix< double > & data)")
    );

    scope.def(
        "tridiagonal_ql_algorithm",
        ( void ( * )( Matrix< double > &, TridiagonalDecompositionData &, size_t ))( &::genesis::utils::tridiagonal_ql_algorithm ),
            pybind11::arg("data"),
            pybind11::arg("tri"),
            pybind11::arg("max_iterations")=(size_t)(1000),
        get_docstring("void ::genesis::utils::tridiagonal_ql_algorithm (Matrix< double > & data, TridiagonalDecompositionData & tri, size_t max_iterations=1000)")
    );
}

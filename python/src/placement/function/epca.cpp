/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_FUNCTIONS( placement_function_epca_export, ::genesis::placement, scope )
{

    scope.def(
        "epca_imbalance_vector",
        ( std::vector< double > ( * )( Sample const & ))( &::genesis::placement::epca_imbalance_vector ),
            pybind11::arg("smp")
    );

    scope.def(
        "epca_imbalance_matrix",
        ( utils::Matrix< double > ( * )( SampleSet const & ))( &::genesis::placement::epca_imbalance_matrix ),
            pybind11::arg("samples")
    );

    scope.def(
        "epca",
        ( utils::PcaData ( * )( SampleSet const &, double, double, size_t ))( &::genesis::placement::epca ),
            pybind11::arg("samples"),
            pybind11::arg("kappa")=(double)(1.0),
            pybind11::arg("epsilon")=(double)(1e-5),
            pybind11::arg("components")=(size_t)(0)
    );

    scope.def(
        "epca_filter_constant_columns",
        ( void ( * )( utils::Matrix< double > &, double ))( &::genesis::placement::epca_filter_constant_columns ),
            pybind11::arg("imbalance_matrix"),
            pybind11::arg("epsilon")=(double)(1e-5)
    );

    scope.def(
        "epca_splitify_transform",
        ( void ( * )( utils::Matrix< double > &, double ))( &::genesis::placement::epca_splitify_transform ),
            pybind11::arg("imbalance_matrix"),
            pybind11::arg("kappa")=(double)(1.0),
        get_docstring("void ::genesis::placement::epca_splitify_transform (utils::Matrix< double > & imbalance_matrix, double kappa=1.0)")
    );
}

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_CLASS( ::genesis::placement::EpcaData, scope )
{

    // -------------------------------------------------------------------
    //     Class EpcaData
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::placement::EpcaData, std::shared_ptr<::genesis::placement::EpcaData> > ( scope, "EpcaData" )
    ;
}

PYTHON_EXPORT_FUNCTIONS( placement_function_epca_export, ::genesis::placement, scope )
{

    scope.def(
        "epca",
        ( EpcaData ( * )( SampleSet const &, double, double, size_t ))( &::genesis::placement::epca ),
            pybind11::arg("samples"),
            pybind11::arg("kappa")=(double)(1.0),
            pybind11::arg("epsilon")=(double)(1e-5),
            pybind11::arg("components")=(size_t)(0),
        get_docstring("EpcaData ::genesis::placement::epca (SampleSet const & samples, double kappa=1.0, double epsilon=1e-5, size_t components=0)")
    );

    scope.def(
        "epca_imbalance_vector",
        ( std::vector< double > ( * )( Sample const &, bool ))( &::genesis::placement::epca_imbalance_vector ),
            pybind11::arg("sample"),
            pybind11::arg("normalize")=(bool)(true),
        get_docstring("std::vector< double > ::genesis::placement::epca_imbalance_vector (Sample const & sample, bool normalize=true)")
    );

    scope.def(
        "epca_filter_constant_columns",
        ( std::vector< size_t > ( * )( utils::Matrix< double > &, double ))( &::genesis::placement::epca_filter_constant_columns ),
            pybind11::arg("imbalance_matrix"),
            pybind11::arg("epsilon")=(double)(1e-5),
        get_docstring("std::vector< size_t > ::genesis::placement::epca_filter_constant_columns (utils::Matrix< double > & imbalance_matrix, double epsilon=1e-5)")
    );

    scope.def(
        "epca_imbalance_matrix",
        ( utils::Matrix< double > ( * )( SampleSet const &, bool, bool ))( &::genesis::placement::epca_imbalance_matrix ),
            pybind11::arg("samples"),
            pybind11::arg("include_leaves")=(bool)(false),
            pybind11::arg("normalize")=(bool)(true),
        get_docstring("utils::Matrix< double > ::genesis::placement::epca_imbalance_matrix (SampleSet const & samples, bool include_leaves=false, bool normalize=true)")
    );

    scope.def(
        "epca_splitify_transform",
        ( void ( * )( utils::Matrix< double > &, double ))( &::genesis::placement::epca_splitify_transform ),
            pybind11::arg("imbalance_matrix"),
            pybind11::arg("kappa")=(double)(1.0),
        get_docstring("void ::genesis::placement::epca_splitify_transform (utils::Matrix< double > & imbalance_matrix, double kappa=1.0)")
    );
}

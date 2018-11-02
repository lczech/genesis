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

PYTHON_EXPORT_CLASS( ::genesis::utils::Quartiles, scope )
{

    // -------------------------------------------------------------------
    //     Class Quartiles
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::Quartiles, std::shared_ptr<::genesis::utils::Quartiles> > ( scope, "Quartiles" )
    ;
}

PYTHON_EXPORT_FUNCTIONS( utils_math_statistics_export, ::genesis::utils, scope )
{

    scope.def(
        "mean_stddev",
        ( MeanStddevPair ( * )( std::vector< double > const &, double ))( &::genesis::utils::mean_stddev ),
            pybind11::arg("vec"),
            pybind11::arg("epsilon")=(double)(-1.0),
        get_docstring("MeanStddevPair ::genesis::utils::mean_stddev (std::vector< double > const & vec, double epsilon=-1.0)")
    );

    scope.def(
        "quartiles",
        ( Quartiles ( * )( std::vector< double > const & ))( &::genesis::utils::quartiles ),
            pybind11::arg("vec"),
        get_docstring("Quartiles ::genesis::utils::quartiles (std::vector< double > const & vec)")
    );

    scope.def(
        "coefficient_of_variation",
        ( double ( * )( MeanStddevPair const & ))( &::genesis::utils::coefficient_of_variation ),
            pybind11::arg("ms"),
        get_docstring("double ::genesis::utils::coefficient_of_variation (MeanStddevPair const & ms)")
    );

    scope.def(
        "fisher_transformation",
        ( double ( * )( double ))( &::genesis::utils::fisher_transformation ),
            pybind11::arg("correlation_coefficient"),
        get_docstring("double ::genesis::utils::fisher_transformation (double correlation_coefficient)")
    );

    scope.def(
        "index_of_dispersion",
        ( double ( * )( MeanStddevPair const & ))( &::genesis::utils::index_of_dispersion ),
            pybind11::arg("ms"),
        get_docstring("double ::genesis::utils::index_of_dispersion (MeanStddevPair const & ms)")
    );

    scope.def(
        "median",
        ( double ( * )( std::vector< double > const & ))( &::genesis::utils::median ),
            pybind11::arg("vec"),
        get_docstring("double ::genesis::utils::median (std::vector< double > const & vec)")
    );

    scope.def(
        "pearson_correlation_coefficient",
        ( double ( * )( std::vector< double > const &, std::vector< double > const & ))( &::genesis::utils::pearson_correlation_coefficient ),
            pybind11::arg("vec_a"),
            pybind11::arg("vec_b"),
        get_docstring("double ::genesis::utils::pearson_correlation_coefficient (std::vector< double > const & vec_a, std::vector< double > const & vec_b)")
    );

    scope.def(
        "quartile_coefficient_of_dispersion",
        ( double ( * )( Quartiles const & ))( &::genesis::utils::quartile_coefficient_of_dispersion ),
            pybind11::arg("q"),
        get_docstring("double ::genesis::utils::quartile_coefficient_of_dispersion (Quartiles const & q)")
    );

    scope.def(
        "spearmans_rank_correlation_coefficient",
        ( double ( * )( std::vector< double > const &, std::vector< double > const & ))( &::genesis::utils::spearmans_rank_correlation_coefficient ),
            pybind11::arg("vec_a"),
            pybind11::arg("vec_b"),
        get_docstring("double ::genesis::utils::spearmans_rank_correlation_coefficient (std::vector< double > const & vec_a, std::vector< double > const & vec_b)")
    );

    scope.def(
        "coefficient_of_variation",
        ( std::vector< double > ( * )( std::vector< MeanStddevPair > const & ))( &::genesis::utils::coefficient_of_variation ),
            pybind11::arg("ms"),
        get_docstring("std::vector< double > ::genesis::utils::coefficient_of_variation (std::vector< MeanStddevPair > const & ms)")
    );

    scope.def(
        "fisher_transformation",
        ( std::vector< double > ( * )( std::vector< double > const & ))( &::genesis::utils::fisher_transformation ),
            pybind11::arg("correlation_coefficients"),
        get_docstring("std::vector< double > ::genesis::utils::fisher_transformation (std::vector< double > const & correlation_coefficients)")
    );

    scope.def(
        "index_of_dispersion",
        ( std::vector< double > ( * )( std::vector< MeanStddevPair > const & ))( &::genesis::utils::index_of_dispersion ),
            pybind11::arg("ms"),
        get_docstring("std::vector< double > ::genesis::utils::index_of_dispersion (std::vector< MeanStddevPair > const & ms)")
    );

    scope.def(
        "quartile_coefficient_of_dispersion",
        ( std::vector< double > ( * )( std::vector< Quartiles > const & ))( &::genesis::utils::quartile_coefficient_of_dispersion ),
            pybind11::arg("q"),
        get_docstring("std::vector< double > ::genesis::utils::quartile_coefficient_of_dispersion (std::vector< Quartiles > const & q)")
    );

    scope.def(
        "ranking_fractional",
        ( std::vector< double > ( * )( std::vector< double > const & ))( &::genesis::utils::ranking_fractional ),
            pybind11::arg("vec"),
        get_docstring("std::vector< double > ::genesis::utils::ranking_fractional (std::vector< double > const & vec)")
    );

    scope.def(
        "ranking_dense",
        ( std::vector< size_t > ( * )( std::vector< double > const & ))( &::genesis::utils::ranking_dense ),
            pybind11::arg("vec"),
        get_docstring("std::vector< size_t > ::genesis::utils::ranking_dense (std::vector< double > const & vec)")
    );

    scope.def(
        "ranking_modified",
        ( std::vector< size_t > ( * )( std::vector< double > const & ))( &::genesis::utils::ranking_modified ),
            pybind11::arg("vec"),
        get_docstring("std::vector< size_t > ::genesis::utils::ranking_modified (std::vector< double > const & vec)")
    );

    scope.def(
        "ranking_ordinal",
        ( std::vector< size_t > ( * )( std::vector< double > const & ))( &::genesis::utils::ranking_ordinal ),
            pybind11::arg("vec"),
        get_docstring("std::vector< size_t > ::genesis::utils::ranking_ordinal (std::vector< double > const & vec)")
    );

    scope.def(
        "ranking_standard",
        ( std::vector< size_t > ( * )( std::vector< double > const & ))( &::genesis::utils::ranking_standard ),
            pybind11::arg("vec"),
        get_docstring("std::vector< size_t > ::genesis::utils::ranking_standard (std::vector< double > const & vec)")
    );
}

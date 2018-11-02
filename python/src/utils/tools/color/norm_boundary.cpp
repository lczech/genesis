/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::ColorNormalizationBoundary, scope )
{

    // -------------------------------------------------------------------
    //     Class ColorNormalizationBoundary
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::ColorNormalizationBoundary, std::shared_ptr<::genesis::utils::ColorNormalizationBoundary> > ( scope, "ColorNormalizationBoundary" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< std::vector< double > const & >(),
            pybind11::arg("boundaries")
        )
        .def(
            pybind11::init< double, double, size_t >(),
            pybind11::arg("min"),
            pybind11::arg("max"),
            pybind11::arg("intervals")
        )
        .def(
            pybind11::init< ColorNormalizationBoundary const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        // .def(
        //     "autoscale",
        //     ( ColorNormalizationBoundary & ( ::genesis::utils::ColorNormalizationBoundary::* )( ForwardIterator, ForwardIterator, size_t ))( &::genesis::utils::ColorNormalizationBoundary::autoscale ),
        //     pybind11::arg("first"),
        //     pybind11::arg("last"),
        //     pybind11::arg("intervals"),
        //     get_docstring("ColorNormalizationBoundary & ::genesis::utils::ColorNormalizationBoundary::autoscale (ForwardIterator first, ForwardIterator last, size_t intervals)")
        // )
        .def(
            "autoscale",
            ( ColorNormalizationBoundary & ( ::genesis::utils::ColorNormalizationBoundary::* )( std::vector< double > const &, size_t ))( &::genesis::utils::ColorNormalizationBoundary::autoscale ),
            pybind11::arg("values"),
            pybind11::arg("intervals"),
            get_docstring("ColorNormalizationBoundary & ::genesis::utils::ColorNormalizationBoundary::autoscale (std::vector< double > const & values, size_t intervals)")
        )
        .def(
            "boundaries",
            ( ColorNormalizationBoundary & ( ::genesis::utils::ColorNormalizationBoundary::* )( std::vector< double > const & ))( &::genesis::utils::ColorNormalizationBoundary::boundaries ),
            pybind11::arg("values"),
            get_docstring("ColorNormalizationBoundary & ::genesis::utils::ColorNormalizationBoundary::boundaries (std::vector< double > const & values)")
        )
        .def(
            "boundaries",
            ( std::vector< double > const & ( ::genesis::utils::ColorNormalizationBoundary::* )(  ) const )( &::genesis::utils::ColorNormalizationBoundary::boundaries ),
            get_docstring("std::vector< double > const & ::genesis::utils::ColorNormalizationBoundary::boundaries () const")
        )
        .def(
            "interval",
            ( int ( ::genesis::utils::ColorNormalizationBoundary::* )( double ) const )( &::genesis::utils::ColorNormalizationBoundary::interval ),
            pybind11::arg("value"),
            get_docstring("int ::genesis::utils::ColorNormalizationBoundary::interval (double value) const")
        )
        .def(
            "is_valid_",
            ( bool ( ::genesis::utils::ColorNormalizationBoundary::* )(  ) const )( &::genesis::utils::ColorNormalizationBoundary::is_valid_ ),
            get_docstring("bool ::genesis::utils::ColorNormalizationBoundary::is_valid_ () const")
        )
        .def(
            "scale",
            ( ColorNormalizationBoundary & ( ::genesis::utils::ColorNormalizationBoundary::* )( double, double, size_t ))( &::genesis::utils::ColorNormalizationBoundary::scale ),
            pybind11::arg("min"),
            pybind11::arg("max"),
            pybind11::arg("intervals"),
            get_docstring("ColorNormalizationBoundary & ::genesis::utils::ColorNormalizationBoundary::scale (double min, double max, size_t intervals)")
        )
    ;
}

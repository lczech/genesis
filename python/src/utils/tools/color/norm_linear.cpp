/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::ColorNormalizationLinear, scope )
{

    // -------------------------------------------------------------------
    //     Class ColorNormalizationLinear
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::ColorNormalizationLinear, std::shared_ptr<::genesis::utils::ColorNormalizationLinear> > ( scope, "ColorNormalizationLinear" )
        .def(
            pybind11::init<  >(),
            get_docstring("::genesis::utils::ColorNormalizationLinear::ColorNormalizationLinear ()")
        )
        .def(
            pybind11::init< double, double >(),
            pybind11::arg("min"),
            pybind11::arg("max"),
            get_docstring("::genesis::utils::ColorNormalizationLinear::ColorNormalizationLinear (double min, double max)")
        )
        .def(
            pybind11::init< std::vector< double > const & >(),
            pybind11::arg("values"),
            get_docstring("::genesis::utils::ColorNormalizationLinear::ColorNormalizationLinear (std::vector< double > const & values)")
        )
        // .def(
        //     pybind11::init< ForwardIterator, ForwardIterator >(),
        //     pybind11::arg("first"),
        //     pybind11::arg("last"),
        //     get_docstring("::genesis::utils::ColorNormalizationLinear::ColorNormalizationLinear (ForwardIterator first, ForwardIterator last)")
        // )
        .def(
            pybind11::init< ColorNormalizationLinear const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        // .def(
        //     "autoscale",
        //     ( ColorNormalizationLinear & ( ::genesis::utils::ColorNormalizationLinear::* )( ForwardIterator, ForwardIterator ))( &::genesis::utils::ColorNormalizationLinear::autoscale ),
        //     pybind11::arg("first"),
        //     pybind11::arg("last"),
        //     get_docstring("ColorNormalizationLinear & ::genesis::utils::ColorNormalizationLinear::autoscale (ForwardIterator first, ForwardIterator last)")
        // )
        .def(
            "autoscale",
            ( ColorNormalizationLinear & ( ::genesis::utils::ColorNormalizationLinear::* )( std::vector< double > const & ))( &::genesis::utils::ColorNormalizationLinear::autoscale ),
            pybind11::arg("values")
        )
        // .def(
        //     "autoscale_max",
        //     ( ColorNormalizationLinear & ( ::genesis::utils::ColorNormalizationLinear::* )( ForwardIterator, ForwardIterator ))( &::genesis::utils::ColorNormalizationLinear::autoscale_max ),
        //     pybind11::arg("first"),
        //     pybind11::arg("last"),
        //     get_docstring("ColorNormalizationLinear & ::genesis::utils::ColorNormalizationLinear::autoscale_max (ForwardIterator first, ForwardIterator last)")
        // )
        .def(
            "autoscale_max",
            ( ColorNormalizationLinear & ( ::genesis::utils::ColorNormalizationLinear::* )( std::vector< double > const & ))( &::genesis::utils::ColorNormalizationLinear::autoscale_max ),
            pybind11::arg("values")
        )
        // .def(
        //     "autoscale_min",
        //     ( ColorNormalizationLinear & ( ::genesis::utils::ColorNormalizationLinear::* )( ForwardIterator, ForwardIterator ))( &::genesis::utils::ColorNormalizationLinear::autoscale_min ),
        //     pybind11::arg("first"),
        //     pybind11::arg("last"),
        //     get_docstring("ColorNormalizationLinear & ::genesis::utils::ColorNormalizationLinear::autoscale_min (ForwardIterator first, ForwardIterator last)")
        // )
        .def(
            "autoscale_min",
            ( ColorNormalizationLinear & ( ::genesis::utils::ColorNormalizationLinear::* )( std::vector< double > const & ))( &::genesis::utils::ColorNormalizationLinear::autoscale_min ),
            pybind11::arg("values")
        )
        .def(
            "is_valid_",
            ( bool ( ::genesis::utils::ColorNormalizationLinear::* )(  ) const )( &::genesis::utils::ColorNormalizationLinear::is_valid_ ),
            get_docstring("bool ::genesis::utils::ColorNormalizationLinear::is_valid_ () const")
        )
        .def(
            "max_value",
            ( ColorNormalizationLinear & ( ::genesis::utils::ColorNormalizationLinear::* )( double ))( &::genesis::utils::ColorNormalizationLinear::max_value ),
            pybind11::arg("value")
        )
        .def(
            "max_value",
            ( double ( ::genesis::utils::ColorNormalizationLinear::* )(  ) const )( &::genesis::utils::ColorNormalizationLinear::max_value ),
            get_docstring("double ::genesis::utils::ColorNormalizationLinear::max_value () const")
        )
        .def(
            "min_value",
            ( ColorNormalizationLinear & ( ::genesis::utils::ColorNormalizationLinear::* )( double ))( &::genesis::utils::ColorNormalizationLinear::min_value ),
            pybind11::arg("value")
        )
        .def(
            "min_value",
            ( double ( ::genesis::utils::ColorNormalizationLinear::* )(  ) const )( &::genesis::utils::ColorNormalizationLinear::min_value ),
            get_docstring("double ::genesis::utils::ColorNormalizationLinear::min_value () const")
        )
        .def(
            "scale",
            ( ColorNormalizationLinear & ( ::genesis::utils::ColorNormalizationLinear::* )( double, double ))( &::genesis::utils::ColorNormalizationLinear::scale ),
            pybind11::arg("min"),
            pybind11::arg("max")
        )
    ;
}

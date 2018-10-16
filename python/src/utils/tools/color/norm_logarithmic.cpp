/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::ColorNormalizationLogarithmic, scope )
{

    // -------------------------------------------------------------------
    //     Class ColorNormalizationLogarithmic
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::ColorNormalizationLogarithmic, std::shared_ptr<::genesis::utils::ColorNormalizationLogarithmic> > ( scope, "ColorNormalizationLogarithmic" )
        .def(
            pybind11::init<  >(),
            get_docstring("::genesis::utils::ColorNormalizationLogarithmic::ColorNormalizationLogarithmic ()")
        )
        .def(
            pybind11::init< double, double >(),
            pybind11::arg("min"),
            pybind11::arg("max"),
            get_docstring("::genesis::utils::ColorNormalizationLogarithmic::ColorNormalizationLogarithmic (double min, double max)")
        )
        .def(
            pybind11::init< std::vector< double > const & >(),
            pybind11::arg("values"),
            get_docstring("::genesis::utils::ColorNormalizationLogarithmic::ColorNormalizationLogarithmic (std::vector< double > const & values)")
        )
        // .def(
        //     pybind11::init< ForwardIterator, ForwardIterator >(),
        //     pybind11::arg("first"),
        //     pybind11::arg("last"),
        //     get_docstring("::genesis::utils::ColorNormalizationLogarithmic::ColorNormalizationLogarithmic (ForwardIterator first, ForwardIterator last)")
        // )
        .def(
            pybind11::init< ColorNormalizationLogarithmic const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "base",
            ( ColorNormalizationLogarithmic & ( ::genesis::utils::ColorNormalizationLogarithmic::* )( double ))( &::genesis::utils::ColorNormalizationLogarithmic::base ),
            pybind11::arg("value")
        )
        .def(
            "base",
            ( double ( ::genesis::utils::ColorNormalizationLogarithmic::* )(  ) const )( &::genesis::utils::ColorNormalizationLogarithmic::base )
        )
        .def(
            "exponential_labels",
            ( ColorNormalizationLogarithmic & ( ::genesis::utils::ColorNormalizationLogarithmic::* )( bool ))( &::genesis::utils::ColorNormalizationLogarithmic::exponential_labels ),
            pybind11::arg("value")
        )
        .def(
            "exponential_labels",
            ( bool ( ::genesis::utils::ColorNormalizationLogarithmic::* )(  ) const )( &::genesis::utils::ColorNormalizationLogarithmic::exponential_labels )
        )
        .def(
            "is_valid_",
            ( bool ( ::genesis::utils::ColorNormalizationLogarithmic::* )(  ) const )( &::genesis::utils::ColorNormalizationLogarithmic::is_valid_ ),
            get_docstring("bool ::genesis::utils::ColorNormalizationLogarithmic::is_valid_ () const")
        )
    ;
}

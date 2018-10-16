/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::ColorNormalizationDiverging, scope )
{

    // -------------------------------------------------------------------
    //     Class ColorNormalizationDiverging
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::ColorNormalizationDiverging, std::shared_ptr<::genesis::utils::ColorNormalizationDiverging> > ( scope, "ColorNormalizationDiverging" )
        .def(
            pybind11::init<  >(),
            get_docstring("::genesis::utils::ColorNormalizationDiverging::ColorNormalizationDiverging ()")
        )
        .def(
            pybind11::init< double, double >(),
            pybind11::arg("min"),
            pybind11::arg("max"),
            get_docstring("::genesis::utils::ColorNormalizationDiverging::ColorNormalizationDiverging (double min, double max)")
        )
        .def(
            pybind11::init< double, double, double >(),
            pybind11::arg("min"),
            pybind11::arg("mid"),
            pybind11::arg("max"),
            get_docstring("::genesis::utils::ColorNormalizationDiverging::ColorNormalizationDiverging (double min, double mid, double max)")
        )
        .def(
            pybind11::init< std::vector< double > const & >(),
            pybind11::arg("values"),
            get_docstring("::genesis::utils::ColorNormalizationDiverging::ColorNormalizationDiverging (std::vector< double > const & values)")
        )
        // .def(
        //     pybind11::init< ForwardIterator, ForwardIterator >(),
        //     pybind11::arg("first"),
        //     pybind11::arg("last"),
        //     get_docstring("::genesis::utils::ColorNormalizationDiverging::ColorNormalizationDiverging (ForwardIterator first, ForwardIterator last)")
        // )
        .def(
            pybind11::init< ColorNormalizationDiverging const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "is_valid_",
            ( bool ( ::genesis::utils::ColorNormalizationDiverging::* )(  ) const )( &::genesis::utils::ColorNormalizationDiverging::is_valid_ ),
            get_docstring("bool ::genesis::utils::ColorNormalizationDiverging::is_valid_ () const")
        )
        .def(
            "make_centric",
            ( ColorNormalizationDiverging & ( ::genesis::utils::ColorNormalizationDiverging::* )( double ))( &::genesis::utils::ColorNormalizationDiverging::make_centric ),
            pybind11::arg("center")=(double)(0.0),
            get_docstring("ColorNormalizationDiverging & ::genesis::utils::ColorNormalizationDiverging::make_centric (double center=0.0)")
        )
        .def(
            "mid_value",
            ( ColorNormalizationDiverging & ( ::genesis::utils::ColorNormalizationDiverging::* )( double ))( &::genesis::utils::ColorNormalizationDiverging::mid_value ),
            pybind11::arg("value")
        )
        .def(
            "mid_value",
            ( double ( ::genesis::utils::ColorNormalizationDiverging::* )(  ) const )( &::genesis::utils::ColorNormalizationDiverging::mid_value ),
            get_docstring("double ::genesis::utils::ColorNormalizationDiverging::mid_value () const")
        )
    ;
}

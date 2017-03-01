/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;
/*
PYTHON_EXPORT_CLASS( ::genesis::utils::Tickmarks, scope )
{

    // -------------------------------------------------------------------
    //     Class Tickmarks
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::Tickmarks, std::shared_ptr<::genesis::utils::Tickmarks> > ( scope, "Tickmarks" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< Tickmarks const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "linear_labels",
            ( std::vector< LabeledTick > ( ::genesis::utils::Tickmarks::* )( double, double, size_t ))( &::genesis::utils::Tickmarks::linear_labels ),
            pybind11::arg("min"),
            pybind11::arg("max"),
            pybind11::arg("target_steps"),
            get_docstring("std::vector< LabeledTick > ::genesis::utils::Tickmarks::linear_labels (double min, double max, size_t target_steps)")
        )
        .def(
            "linear_ticks",
            ( std::vector< double > ( ::genesis::utils::Tickmarks::* )( double, double, size_t ))( &::genesis::utils::Tickmarks::linear_ticks ),
            pybind11::arg("min"),
            pybind11::arg("max"),
            pybind11::arg("target_steps"),
            get_docstring("std::vector< double > ::genesis::utils::Tickmarks::linear_ticks (double min, double max, size_t target_steps)")
        )
        .def(
            "logarithmic_labels",
            ( std::vector< LabeledTick > ( ::genesis::utils::Tickmarks::* )( double ))( &::genesis::utils::Tickmarks::logarithmic_labels ),
            pybind11::arg("max"),
            get_docstring("std::vector< LabeledTick > ::genesis::utils::Tickmarks::logarithmic_labels (double max)")
        )
        .def_static(
            "step_size",
            ( double ( * )( double, size_t ))( &::genesis::utils::Tickmarks::step_size ),
            pybind11::arg("interval_size"),
            pybind11::arg("target_steps"),
            get_docstring("static double ::genesis::utils::Tickmarks::step_size (double interval_size, size_t target_steps)")
        )
    ;
}
*/

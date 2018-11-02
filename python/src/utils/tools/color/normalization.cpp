/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::ColorNormalization, scope )
{

    // -------------------------------------------------------------------
    //     Class ColorNormalization
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::ColorNormalization, std::shared_ptr<::genesis::utils::ColorNormalization> > ( scope, "ColorNormalization" )
        // .def(
        //     pybind11::init<  >()
        // )
        // .def(
        //     pybind11::init< ColorNormalization const & >(),
        //     pybind11::arg("arg")
        // )

        // Public Member Functions

        .def(
            "is_valid",
            ( bool ( ::genesis::utils::ColorNormalization::* )(  ) const )( &::genesis::utils::ColorNormalization::is_valid ),
            get_docstring("bool ::genesis::utils::ColorNormalization::is_valid () const")
        )
        .def(
            "mask_value",
            ( ColorNormalization & ( ::genesis::utils::ColorNormalization::* )( double ))( &::genesis::utils::ColorNormalization::mask_value ),
            pybind11::arg("value"),
            get_docstring("ColorNormalization & ::genesis::utils::ColorNormalization::mask_value (double value)")
        )
        .def(
            "mask_value",
            ( double ( ::genesis::utils::ColorNormalization::* )(  ) const )( &::genesis::utils::ColorNormalization::mask_value ),
            get_docstring("double ::genesis::utils::ColorNormalization::mask_value () const")
        )
    ;
}

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::ColorMap, scope )
{

    // -------------------------------------------------------------------
    //     Class ColorMap
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::ColorMap, std::shared_ptr<::genesis::utils::ColorMap> > ( scope, "ColorMap" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< std::vector< Color > const & >(),
            pybind11::arg("colors")
        )
        .def(
            pybind11::init< ColorMap const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "clip",
            ( ColorMap & ( ::genesis::utils::ColorMap::* )( bool ))( &::genesis::utils::ColorMap::clip ),
            pybind11::arg("value")
        )
        .def(
            "clip_over",
            ( ColorMap & ( ::genesis::utils::ColorMap::* )( bool ))( &::genesis::utils::ColorMap::clip_over ),
            pybind11::arg("value"),
            get_docstring("ColorMap & ::genesis::utils::ColorMap::clip_over (bool value)")
        )
        .def(
            "clip_over",
            ( bool ( ::genesis::utils::ColorMap::* )(  ) const )( &::genesis::utils::ColorMap::clip_over ),
            get_docstring("bool ::genesis::utils::ColorMap::clip_over () const")
        )
        .def(
            "clip_under",
            ( ColorMap & ( ::genesis::utils::ColorMap::* )( bool ))( &::genesis::utils::ColorMap::clip_under ),
            pybind11::arg("value"),
            get_docstring("ColorMap & ::genesis::utils::ColorMap::clip_under (bool value)")
        )
        .def(
            "clip_under",
            ( bool ( ::genesis::utils::ColorMap::* )(  ) const )( &::genesis::utils::ColorMap::clip_under ),
            get_docstring("bool ::genesis::utils::ColorMap::clip_under () const")
        )
        .def(
            "color",
            ( Color ( ::genesis::utils::ColorMap::* )( size_t ) const )( &::genesis::utils::ColorMap::color ),
            pybind11::arg("index"),
            get_docstring("Color ::genesis::utils::ColorMap::color (size_t index) const")
        )
        .def(
            "color_list",
            ( std::vector< Color > ( ::genesis::utils::ColorMap::* )( size_t ) const )( &::genesis::utils::ColorMap::color_list ),
            pybind11::arg("n")=(size_t)(256),
            get_docstring("std::vector< Color > ::genesis::utils::ColorMap::color_list (size_t n=256) const")
        )
        .def(
            "empty",
            ( bool ( ::genesis::utils::ColorMap::* )(  ) const )( &::genesis::utils::ColorMap::empty ),
            get_docstring("bool ::genesis::utils::ColorMap::empty () const")
        )
        .def(
            "mask_color",
            ( Color ( ::genesis::utils::ColorMap::* )(  ) const )( &::genesis::utils::ColorMap::mask_color ),
            get_docstring("Color ::genesis::utils::ColorMap::mask_color () const")
        )
        .def(
            "mask_color",
            ( ColorMap & ( ::genesis::utils::ColorMap::* )( Color ))( &::genesis::utils::ColorMap::mask_color ),
            pybind11::arg("value"),
            get_docstring("ColorMap & ::genesis::utils::ColorMap::mask_color (Color value)")
        )
        .def(
            "over_color",
            ( Color ( ::genesis::utils::ColorMap::* )(  ) const )( &::genesis::utils::ColorMap::over_color ),
            get_docstring("Color ::genesis::utils::ColorMap::over_color () const")
        )
        .def(
            "over_color",
            ( ColorMap & ( ::genesis::utils::ColorMap::* )( Color ))( &::genesis::utils::ColorMap::over_color ),
            pybind11::arg("value"),
            get_docstring("ColorMap & ::genesis::utils::ColorMap::over_color (Color value)")
        )
        .def(
            "palette",
            ( ColorMap & ( ::genesis::utils::ColorMap::* )( std::vector< Color > const & ))( &::genesis::utils::ColorMap::palette ),
            pybind11::arg("value")
        )
        .def(
            "palette",
            ( std::vector< Color > const & ( ::genesis::utils::ColorMap::* )(  ) const )( &::genesis::utils::ColorMap::palette ),
            get_docstring("std::vector< Color > const & ::genesis::utils::ColorMap::palette () const")
        )
        .def(
            "reverse",
            ( ColorMap & ( ::genesis::utils::ColorMap::* )( bool ))( &::genesis::utils::ColorMap::reverse ),
            pybind11::arg("value")
        )
        .def(
            "reverse",
            ( bool ( ::genesis::utils::ColorMap::* )(  ) const )( &::genesis::utils::ColorMap::reverse ),
            get_docstring("bool ::genesis::utils::ColorMap::reverse () const")
        )
        .def(
            "size",
            ( size_t ( ::genesis::utils::ColorMap::* )(  ) const )( &::genesis::utils::ColorMap::size ),
            get_docstring("size_t ::genesis::utils::ColorMap::size () const")
        )
        .def(
            "under_color",
            ( Color ( ::genesis::utils::ColorMap::* )(  ) const )( &::genesis::utils::ColorMap::under_color ),
            get_docstring("Color ::genesis::utils::ColorMap::under_color () const")
        )
        .def(
            "under_color",
            ( ColorMap & ( ::genesis::utils::ColorMap::* )( Color ))( &::genesis::utils::ColorMap::under_color ),
            pybind11::arg("value"),
            get_docstring("ColorMap & ::genesis::utils::ColorMap::under_color (Color value)")
        )
    ;
}

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::SvgText, scope )
{

    // -------------------------------------------------------------------
    //     Class SvgText
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::SvgText, std::shared_ptr<::genesis::utils::SvgText> > ( scope, "SvgText" )
        .def(
            pybind11::init< std::string const &, SvgPoint const &, SvgFont const &, SvgFill const &, SvgStroke const & >(),
            pybind11::arg("text"),
            pybind11::arg("position"),
            pybind11::arg("font"),
            pybind11::arg("fill"),
            pybind11::arg("stroke")
        )
        .def(
            pybind11::init< SvgText const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "bounding_box",
            ( SvgBox ( ::genesis::utils::SvgText::* )(  ) const )( &::genesis::utils::SvgText::bounding_box )
        )
        .def(
            "write",
            ( void ( ::genesis::utils::SvgText::* )( std::ostream &, size_t, SvgDrawingOptions const & ) const )( &::genesis::utils::SvgText::write ),
            pybind11::arg("out"),
            pybind11::arg("indent")=(size_t)(0),
            pybind11::arg("options")
        )
    ;
}

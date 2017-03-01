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
PYTHON_EXPORT_CLASS( ::genesis::utils::SvgFont, scope )
{

    // -------------------------------------------------------------------
    //     Class SvgFont
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::SvgFont, std::shared_ptr<::genesis::utils::SvgFont> > ( scope, "SvgFont" )
        .def(
            pybind11::init< double, std::string const & >(),
            pybind11::arg("size")=(double)(10),
            pybind11::arg("family")=(std::string const &)("Verdana")
        )
        .def(
            pybind11::init< SvgFont const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "write",
            ( void ( ::genesis::utils::SvgFont::* )( std::ostream & ) const )( &::genesis::utils::SvgFont::write ),
            pybind11::arg("out")
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::utils::SvgTransform, scope )
{

    // -------------------------------------------------------------------
    //     Class SvgTransform
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::SvgTransform, std::shared_ptr<::genesis::utils::SvgTransform> > ( scope, "SvgTransform" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< SvgTransform const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "append",
            ( void ( ::genesis::utils::SvgTransform::* )( Transformation && ))( &::genesis::utils::SvgTransform::append ),
            pybind11::arg("t")
        )
        .def(
            "append",
            ( void ( ::genesis::utils::SvgTransform::* )( Transformation const & ))( &::genesis::utils::SvgTransform::append ),
            pybind11::arg("t")
        )
        .def(
            "write",
            ( void ( ::genesis::utils::SvgTransform::* )( std::ostream & ) const )( &::genesis::utils::SvgTransform::write ),
            pybind11::arg("out")
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::utils::SvgFill, scope )
{

    // -------------------------------------------------------------------
    //     Class SvgFill
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::SvgFill, std::shared_ptr<::genesis::utils::SvgFill> > ( scope, "SvgFill" )
        .def(
            pybind11::init< Type >(),
            pybind11::arg("type")
        )
        .def(
            pybind11::init< Color, double >(),
            pybind11::arg("color"),
            pybind11::arg("opacity")=(double)(1.0)
        )
        .def(
            pybind11::init< std::string >(),
            pybind11::arg("gradient_id")
        )
        .def(
            pybind11::init< SvgFill const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "write",
            ( void ( ::genesis::utils::SvgFill::* )( std::ostream & ) const )( &::genesis::utils::SvgFill::write ),
            pybind11::arg("out")
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::utils::SvgStroke, scope )
{

    // -------------------------------------------------------------------
    //     Class SvgStroke
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::SvgStroke, std::shared_ptr<::genesis::utils::SvgStroke> > ( scope, "SvgStroke" )
        .def(
            pybind11::init< Type >(),
            pybind11::arg("type")
        )
        .def(
            pybind11::init< Color, double >(),
            pybind11::arg("color"),
            pybind11::arg("width")=(double)(1.0)
        )
        .def(
            pybind11::init< std::string >(),
            pybind11::arg("gradient_id")
        )
        .def(
            pybind11::init< SvgStroke const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "write",
            ( void ( ::genesis::utils::SvgStroke::* )( std::ostream & ) const )( &::genesis::utils::SvgStroke::write ),
            pybind11::arg("out")
        )
    ;
}
*/

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::SvgPoint, scope )
{

    // -------------------------------------------------------------------
    //     Class SvgPoint
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::SvgPoint, std::shared_ptr<::genesis::utils::SvgPoint> > ( scope, "SvgPoint" )
        .def(
            pybind11::init< double, double >(),
            pybind11::arg("x")=(double)(0.0),
            pybind11::arg("y")=(double)(0.0)
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::utils::SvgMargin, scope )
{

    // -------------------------------------------------------------------
    //     Class SvgMargin
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::SvgMargin, std::shared_ptr<::genesis::utils::SvgMargin> > ( scope, "SvgMargin" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< double >(),
            pybind11::arg("all")
        )
        .def(
            pybind11::init< double, double >(),
            pybind11::arg("top_bottom"),
            pybind11::arg("right_left")
        )
        .def(
            pybind11::init< double, double, double >(),
            pybind11::arg("top"),
            pybind11::arg("right_left"),
            pybind11::arg("bottom")
        )
        .def(
            pybind11::init< double, double, double, double >(),
            pybind11::arg("top"),
            pybind11::arg("right"),
            pybind11::arg("bottom"),
            pybind11::arg("left")
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::utils::SvgSize, scope )
{

    // -------------------------------------------------------------------
    //     Class SvgSize
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::SvgSize, std::shared_ptr<::genesis::utils::SvgSize> > ( scope, "SvgSize" )
        .def(
            pybind11::init< double, double >(),
            pybind11::arg("width")=(double)(0.0),
            pybind11::arg("height")=(double)(0.0)
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::utils::SvgDrawingOptions, scope )
{

    // -------------------------------------------------------------------
    //     Class SvgDrawingOptions
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::SvgDrawingOptions, std::shared_ptr<::genesis::utils::SvgDrawingOptions> > ( scope, "SvgDrawingOptions" )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::utils::SvgBox, scope )
{

    // -------------------------------------------------------------------
    //     Class SvgBox
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::SvgBox, std::shared_ptr<::genesis::utils::SvgBox> > ( scope, "SvgBox" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< SvgPoint, SvgPoint >(),
            pybind11::arg("top_left"),
            pybind11::arg("bottom_right")
        )
        .def(
            pybind11::init< SvgPoint, double, double >(),
            pybind11::arg("top_left_v"),
            pybind11::arg("width"),
            pybind11::arg("height")
        )

        // Public Member Functions

        .def(
            "empty",
            ( bool ( ::genesis::utils::SvgBox::* )(  ) const )( &::genesis::utils::SvgBox::empty )
        )
        .def(
            "height",
            ( double ( ::genesis::utils::SvgBox::* )(  ) const )( &::genesis::utils::SvgBox::height )
        )
        .def(
            "size",
            ( SvgSize ( ::genesis::utils::SvgBox::* )(  ) const )( &::genesis::utils::SvgBox::size )
        )
        .def(
            "width",
            ( double ( ::genesis::utils::SvgBox::* )(  ) const )( &::genesis::utils::SvgBox::width )
        )
        .def_static(
            "combine",
            ( SvgBox ( * )( SvgBox const &, SvgBox const & ))( &::genesis::utils::SvgBox::combine ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS( utils_formats_svg_helper_export, ::genesis::utils, scope )
{

    scope.def(
        "svg_comment",
        ( std::string ( * )( std::string const & ))( &::genesis::utils::svg_comment ),
            pybind11::arg("content")
    );
}

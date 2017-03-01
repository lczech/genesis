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
PYTHON_EXPORT_CLASS( ::genesis::utils::SvgLine, scope )
{

    // -------------------------------------------------------------------
    //     Class SvgLine
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::SvgLine, std::shared_ptr<::genesis::utils::SvgLine> > ( scope, "SvgLine" )
        .def(
            pybind11::init< SvgPoint const &, SvgPoint const &, SvgStroke const & >(),
            pybind11::arg("point_1"),
            pybind11::arg("point_2"),
            pybind11::arg("stroke")
        )
        .def(
            pybind11::init< double, double, double, double, SvgStroke const & >(),
            pybind11::arg("x1"),
            pybind11::arg("y1"),
            pybind11::arg("x2"),
            pybind11::arg("y2"),
            pybind11::arg("stroke")
        )
        .def(
            pybind11::init< SvgLine const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "bounding_box",
            ( SvgBox ( ::genesis::utils::SvgLine::* )(  ) const )( &::genesis::utils::SvgLine::bounding_box )
        )
        .def(
            "write",
            ( void ( ::genesis::utils::SvgLine::* )( std::ostream &, size_t, SvgDrawingOptions const & ) const )( &::genesis::utils::SvgLine::write ),
            pybind11::arg("out"),
            pybind11::arg("indent")=(size_t)(0),
            pybind11::arg("options")
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::utils::SvgPolyline, scope )
{

    // -------------------------------------------------------------------
    //     Class SvgPolyline
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::SvgPolyline, std::shared_ptr<::genesis::utils::SvgPolyline> > ( scope, "SvgPolyline" )
        .def(
            pybind11::init< SvgStroke const &, SvgFill const & >(),
            pybind11::arg("stroke"),
            pybind11::arg("fill")
        )
        .def(
            pybind11::init< std::vector< SvgPoint >, SvgStroke const &, SvgFill const & >(),
            pybind11::arg("points"),
            pybind11::arg("stroke"),
            pybind11::arg("fill")
        )
        .def(
            pybind11::init< SvgPolyline const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "add",
            ( self_type & ( ::genesis::utils::SvgPolyline::* )( SvgPoint ))( &::genesis::utils::SvgPolyline::add ),
            pybind11::arg("p")
        )
        .def(
            "add",
            ( self_type & ( ::genesis::utils::SvgPolyline::* )( double, double ))( &::genesis::utils::SvgPolyline::add ),
            pybind11::arg("x"),
            pybind11::arg("y")
        )
        .def(
            "bounding_box",
            ( SvgBox ( ::genesis::utils::SvgPolyline::* )(  ) const )( &::genesis::utils::SvgPolyline::bounding_box )
        )
        .def(
            "write",
            ( void ( ::genesis::utils::SvgPolyline::* )( std::ostream &, size_t, SvgDrawingOptions const & ) const )( &::genesis::utils::SvgPolyline::write ),
            pybind11::arg("out"),
            pybind11::arg("indent")=(size_t)(0),
            pybind11::arg("options")
        )

        // Operators

        .def(
            "__str__",
            []( ::genesis::utils::SvgPolyline const& obj ) -> std::string {
                std::ostringstream s;
                s << obj;
                return s.str();
            }
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::utils::SvgPath, scope )
{

    // -------------------------------------------------------------------
    //     Class SvgPath
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::SvgPath, std::shared_ptr<::genesis::utils::SvgPath> > ( scope, "SvgPath" )
        .def(
            pybind11::init< SvgStroke const &, SvgFill const & >(),
            pybind11::arg("stroke"),
            pybind11::arg("fill")
        )
        .def(
            pybind11::init< std::vector< std::string > const &, SvgStroke const &, SvgFill const & >(),
            pybind11::arg("elements"),
            pybind11::arg("stroke"),
            pybind11::arg("fill")
        )
        .def(
            pybind11::init< SvgPath const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "add",
            ( self_type & ( ::genesis::utils::SvgPath::* )( std::string ))( &::genesis::utils::SvgPath::add ),
            pybind11::arg("elem")
        )
        .def(
            "bounding_box",
            ( SvgBox ( ::genesis::utils::SvgPath::* )(  ) const )( &::genesis::utils::SvgPath::bounding_box )
        )
        .def(
            "write",
            ( void ( ::genesis::utils::SvgPath::* )( std::ostream &, size_t, SvgDrawingOptions const & ) const )( &::genesis::utils::SvgPath::write ),
            pybind11::arg("out"),
            pybind11::arg("indent")=(size_t)(0),
            pybind11::arg("options")
        )

        // Operators

        .def(
            "__str__",
            []( ::genesis::utils::SvgPath const& obj ) -> std::string {
                std::ostringstream s;
                s << obj;
                return s.str();
            }
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::utils::SvgRect, scope )
{

    // -------------------------------------------------------------------
    //     Class SvgRect
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::SvgRect, std::shared_ptr<::genesis::utils::SvgRect> > ( scope, "SvgRect" )
        .def(
            pybind11::init< SvgPoint const &, SvgSize const &, SvgStroke const &, SvgFill const & >(),
            pybind11::arg("position"),
            pybind11::arg("size"),
            pybind11::arg("stroke"),
            pybind11::arg("fill")
        )
        .def(
            pybind11::init< double, double, double, double, SvgStroke const &, SvgFill const & >(),
            pybind11::arg("x"),
            pybind11::arg("y"),
            pybind11::arg("w"),
            pybind11::arg("h"),
            pybind11::arg("stroke"),
            pybind11::arg("fill")
        )
        .def(
            pybind11::init< SvgRect const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "bounding_box",
            ( SvgBox ( ::genesis::utils::SvgRect::* )(  ) const )( &::genesis::utils::SvgRect::bounding_box )
        )
        .def(
            "write",
            ( void ( ::genesis::utils::SvgRect::* )( std::ostream &, size_t, SvgDrawingOptions const & ) const )( &::genesis::utils::SvgRect::write ),
            pybind11::arg("out"),
            pybind11::arg("indent")=(size_t)(0),
            pybind11::arg("options")
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::utils::SvgUse, scope )
{

    // -------------------------------------------------------------------
    //     Class SvgUse
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::SvgUse, std::shared_ptr<::genesis::utils::SvgUse> > ( scope, "SvgUse" )
        .def(
            pybind11::init< std::string const &, SvgPoint const & >(),
            pybind11::arg("id"),
            pybind11::arg("offset")
        )
        .def(
            pybind11::init< std::string const &, double, double >(),
            pybind11::arg("id"),
            pybind11::arg("offset_x"),
            pybind11::arg("offset_y")
        )
        .def(
            pybind11::init< SvgUse const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "bounding_box",
            ( SvgBox ( ::genesis::utils::SvgUse::* )(  ) const )( &::genesis::utils::SvgUse::bounding_box )
        )
        .def(
            "write",
            ( void ( ::genesis::utils::SvgUse::* )( std::ostream &, size_t, SvgDrawingOptions const & ) const )( &::genesis::utils::SvgUse::write ),
            pybind11::arg("out"),
            pybind11::arg("indent")=(size_t)(0),
            pybind11::arg("options")
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::utils::SvgPolygon, scope )
{

    // -------------------------------------------------------------------
    //     Class SvgPolygon
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::SvgPolygon, std::shared_ptr<::genesis::utils::SvgPolygon> > ( scope, "SvgPolygon" )
        .def(
            pybind11::init< SvgStroke const &, SvgFill const & >(),
            pybind11::arg("stroke"),
            pybind11::arg("fill")
        )
        .def(
            pybind11::init< std::vector< SvgPoint > const &, SvgStroke const &, SvgFill const & >(),
            pybind11::arg("points"),
            pybind11::arg("stroke"),
            pybind11::arg("fill")
        )
        .def(
            pybind11::init< SvgPolygon const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "add",
            ( self_type & ( ::genesis::utils::SvgPolygon::* )( SvgPoint ))( &::genesis::utils::SvgPolygon::add ),
            pybind11::arg("p")
        )
        .def(
            "add",
            ( self_type & ( ::genesis::utils::SvgPolygon::* )( double, double ))( &::genesis::utils::SvgPolygon::add ),
            pybind11::arg("x"),
            pybind11::arg("y")
        )
        .def(
            "bounding_box",
            ( SvgBox ( ::genesis::utils::SvgPolygon::* )(  ) const )( &::genesis::utils::SvgPolygon::bounding_box )
        )
        .def(
            "write",
            ( void ( ::genesis::utils::SvgPolygon::* )( std::ostream &, size_t, SvgDrawingOptions const & ) const )( &::genesis::utils::SvgPolygon::write ),
            pybind11::arg("out"),
            pybind11::arg("indent")=(size_t)(0),
            pybind11::arg("options")
        )

        // Operators

        .def(
            "__str__",
            []( ::genesis::utils::SvgPolygon const& obj ) -> std::string {
                std::ostringstream s;
                s << obj;
                return s.str();
            }
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::utils::SvgCircle, scope )
{

    // -------------------------------------------------------------------
    //     Class SvgCircle
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::SvgCircle, std::shared_ptr<::genesis::utils::SvgCircle> > ( scope, "SvgCircle" )
        .def(
            pybind11::init< SvgPoint const &, double, SvgStroke const &, SvgFill const & >(),
            pybind11::arg("center"),
            pybind11::arg("radius"),
            pybind11::arg("stroke"),
            pybind11::arg("fill")
        )
        .def(
            pybind11::init< double, double, double, SvgStroke const &, SvgFill const & >(),
            pybind11::arg("cx"),
            pybind11::arg("cy"),
            pybind11::arg("radius"),
            pybind11::arg("stroke"),
            pybind11::arg("fill")
        )
        .def(
            pybind11::init< SvgCircle const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "bounding_box",
            ( SvgBox ( ::genesis::utils::SvgCircle::* )(  ) const )( &::genesis::utils::SvgCircle::bounding_box )
        )
        .def(
            "write",
            ( void ( ::genesis::utils::SvgCircle::* )( std::ostream &, size_t, SvgDrawingOptions const & ) const )( &::genesis::utils::SvgCircle::write ),
            pybind11::arg("out"),
            pybind11::arg("indent")=(size_t)(0),
            pybind11::arg("options")
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::utils::SvgEllipse, scope )
{

    // -------------------------------------------------------------------
    //     Class SvgEllipse
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::SvgEllipse, std::shared_ptr<::genesis::utils::SvgEllipse> > ( scope, "SvgEllipse" )
        .def(
            pybind11::init< SvgPoint const &, double, double, SvgStroke const &, SvgFill const & >(),
            pybind11::arg("center"),
            pybind11::arg("rx"),
            pybind11::arg("ry"),
            pybind11::arg("stroke"),
            pybind11::arg("fill")
        )
        .def(
            pybind11::init< double, double, double, double, SvgStroke const &, SvgFill const & >(),
            pybind11::arg("cx"),
            pybind11::arg("cy"),
            pybind11::arg("rx"),
            pybind11::arg("ry"),
            pybind11::arg("stroke"),
            pybind11::arg("fill")
        )
        .def(
            pybind11::init< SvgEllipse const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "bounding_box",
            ( SvgBox ( ::genesis::utils::SvgEllipse::* )(  ) const )( &::genesis::utils::SvgEllipse::bounding_box )
        )
        .def(
            "write",
            ( void ( ::genesis::utils::SvgEllipse::* )( std::ostream &, size_t, SvgDrawingOptions const & ) const )( &::genesis::utils::SvgEllipse::write ),
            pybind11::arg("out"),
            pybind11::arg("indent")=(size_t)(0),
            pybind11::arg("options")
        )
    ;
}
*/

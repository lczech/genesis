/**
 * @brief
 *
 * @file
 * @ingroup python
 */

/*

#include <python/src/common.hpp>

#include "lib/placement/measures.hpp"
#include "lib/placement/sample.hpp"

PYTHON_EXPORT_CLASS (PlacementMeasures, "placement")
{
    boost::python::class_< ::genesis::PlacementMeasures > ( "PlacementMeasures", boost::python::init<  >(  ) )

        // Public Member Functions

        .def(
            "center_of_gravity",
            ( std::pair< PlacementTreeEdge *, double > ( * )( const Sample & ))( &::genesis::PlacementMeasures::center_of_gravity ),
            ( boost::python::arg("map") ),
            get_docstring("static std::pair< PlacementTreeEdge *, double > ::genesis::PlacementMeasures::center_of_gravity (const Sample & map)")
        )
        .staticmethod("center_of_gravity")
        .def(
            "center_of_gravity_distance",
            ( double ( * )( const Sample &, const Sample & ))( &::genesis::PlacementMeasures::center_of_gravity_distance ),
            ( boost::python::arg("map_a"), boost::python::arg("map_b") )
        )
        .staticmethod("center_of_gravity_distance")
        .def(
            "center_of_gravity_variance",
            ( double ( * )( const Sample & ))( &::genesis::PlacementMeasures::center_of_gravity_variance ),
            ( boost::python::arg("map") ),
            get_docstring("static double ::genesis::PlacementMeasures::center_of_gravity_variance (const Sample & map)")
        )
        .staticmethod("center_of_gravity_variance")
        .def(
            "earth_movers_distance",
            ( double ( * )( const Sample &, const Sample & ))( &::genesis::PlacementMeasures::earth_movers_distance ),
            ( boost::python::arg("map_a"), boost::python::arg("map_b") ),
            get_docstring("static double ::genesis::PlacementMeasures::earth_movers_distance (const Sample & map_a, const Sample & map_b)")
        )
        .staticmethod("earth_movers_distance")
        .def(
            "pairwise_distance",
            ( double ( * )( const Sample &, const Sample & ))( &::genesis::PlacementMeasures::pairwise_distance ),
            ( boost::python::arg("map_a"), boost::python::arg("map_b") ),
            get_docstring("static double ::genesis::PlacementMeasures::pairwise_distance (const Sample & map_a, const Sample & map_b)")
        )
        .staticmethod("pairwise_distance")
        .def(
            "variance",
            ( double ( * )( const Sample & ))( &::genesis::PlacementMeasures::variance ),
            ( boost::python::arg("map") ),
            get_docstring("static double ::genesis::PlacementMeasures::variance (const Sample & map)")
        )
        .staticmethod("variance")

        // Public Member Variables

        .def_readwrite("with_pendant_length", &::genesis::PlacementMeasures::with_pendant_length)
    ;
}

*/

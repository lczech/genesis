/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/placement/measures.hpp"
#include "lib/placement/placement_map.hpp"

PYTHON_EXPORT_CLASS ("placement", PlacementMeasures)
{
    boost::python::class_< ::genesis::PlacementMeasures > ( "PlacementMeasures", boost::python::init<  >(  ) )

        // Public Member Functions

        .def(
            "center_of_gravity",
            ( std::pair< PlacementTreeEdge *, double > ( * )( const PlacementMap & ))( &::genesis::PlacementMeasures::center_of_gravity ),
            ( boost::python::arg("map") ),
            get_docstring("static std::pair< PlacementTreeEdge *, double > ::genesis::PlacementMeasures::center_of_gravity (const PlacementMap & map)")
        )
        .staticmethod("center_of_gravity")
        .def(
            "center_of_gravity_distance",
            ( double ( * )( const PlacementMap &, const PlacementMap & ))( &::genesis::PlacementMeasures::center_of_gravity_distance ),
            ( boost::python::arg("map_a"), boost::python::arg("map_b") )
        )
        .staticmethod("center_of_gravity_distance")
        .def(
            "center_of_gravity_variance",
            ( double ( * )( const PlacementMap & ))( &::genesis::PlacementMeasures::center_of_gravity_variance ),
            ( boost::python::arg("map") ),
            get_docstring("static double ::genesis::PlacementMeasures::center_of_gravity_variance (const PlacementMap & map)")
        )
        .staticmethod("center_of_gravity_variance")
        .def(
            "earth_movers_distance",
            ( double ( * )( const PlacementMap &, const PlacementMap & ))( &::genesis::PlacementMeasures::earth_movers_distance ),
            ( boost::python::arg("map_a"), boost::python::arg("map_b") ),
            get_docstring("static double ::genesis::PlacementMeasures::earth_movers_distance (const PlacementMap & map_a, const PlacementMap & map_b)")
        )
        .staticmethod("earth_movers_distance")
        .def(
            "pairwise_distance",
            ( double ( * )( const PlacementMap &, const PlacementMap & ))( &::genesis::PlacementMeasures::pairwise_distance ),
            ( boost::python::arg("map_a"), boost::python::arg("map_b") ),
            get_docstring("static double ::genesis::PlacementMeasures::pairwise_distance (const PlacementMap & map_a, const PlacementMap & map_b)")
        )
        .staticmethod("pairwise_distance")
        .def(
            "variance",
            ( double ( * )( const PlacementMap & ))( &::genesis::PlacementMeasures::variance ),
            ( boost::python::arg("map") ),
            get_docstring("static double ::genesis::PlacementMeasures::variance (const PlacementMap & map)")
        )
        .staticmethod("variance")

        // Public Member Variables

        .def_readwrite("with_pendant_length", &::genesis::PlacementMeasures::with_pendant_length)
    ;
}

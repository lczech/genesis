/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <boost/python.hpp>

#include "placement/placement_map.hpp"

void BoostPythonExport_PlacementMap()
{

    boost::python::class_< ::genesis::PlacementMap > ( "PlacementMap" )

        // Public Member Functions

        .def(
            "clear",
            ( void ( ::genesis::PlacementMap::* )(  ))( &::genesis::PlacementMap::clear ),
            "Clears all data of this object."
        )
        //~ .def(
            //~ "EdgeNumMap",
            //~ ( EdgeNumMapType * ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::EdgeNumMap ),
            //~ "Returns a mapping of edge_num integers to the corresponding Edge object."
        //~ )
        .def(
            "Merge",
            ( bool ( ::genesis::PlacementMap::* )( const ::genesis::PlacementMap & ))( &::genesis::PlacementMap::merge ),
            ( boost::python::arg("other") ),
            "Adds the pqueries from another ::genesis::PlacementMap objects to this one."
        )
        .def(
            "NormalizeWeightRatios",
            ( void ( ::genesis::PlacementMap::* )(  ))( &::genesis::PlacementMap::normalize_weight_ratios ),
            "Recalculates the like_weight_ratio of the placements of each Pquery so that their sum is 1.0, while maintaining their ratio to each other."
        )
        .def(
            "RestrainToMaxWeightPlacements",
            ( void ( ::genesis::PlacementMap::* )(  ))( &::genesis::PlacementMap::restrain_to_max_weight_placements ),
            "Removes all placements but the most likely one from all pqueries."
        )
        .def(
            "PlacementCount",
            ( size_t ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::placement_count ),
            "Get the total number of placements in all pqueries."
        )
        .def(
            "PlacementMass",
            ( double ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::placement_mass ),
            "Get the summed mass of all placements on the tree, given by their like_weight_ratio."
        )
        .def(
            "ClosestLeafDepthHistogram",
            ( std::vector< int > ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::closest_leaf_depth_histogram ),
            "Returns a histogram representing how many placements have which depth with respect to their closest leaf node."
        )
        .def(
            "ClosestLeafDistanceHistogram",
            ( std::vector< int > ( ::genesis::PlacementMap::* )( const double, const double, const int ) const )( &::genesis::PlacementMap::closest_leaf_distance_histogram ),
            ( boost::python::arg("min"), boost::python::arg("max"), boost::python::arg("bins")=(const int)(10) ),
            "Returns a histogram counting the number of placements that have a certain distance to their closest leaf node, divided into equally large intervals between a min and a max distance."
        )
        .def(
            "ClosestLeafDistanceHistogramAuto",
            ( std::vector< int > ( ::genesis::PlacementMap::* )( double &, double &, const int ) const )( &::genesis::PlacementMap::closest_leaf_distance_histogram_auto ),
            ( boost::python::arg("min"), boost::python::arg("max"), boost::python::arg("bins")=(const int)(10) ),
            "Returns the same type of histogram as ClosestLeafDistanceHistogram(), but automatically determines the needed boundaries."
        )
        .def(
            "EMD",
            ( double ( ::genesis::PlacementMap::* )( const ::genesis::PlacementMap &, const bool ) const )( &::genesis::PlacementMap::earth_movers_distance ),
            ( boost::python::arg("other"), boost::python::arg("with_pendant_length")=(const bool)(true) ),
            "Calculates the Earth Movers Distance to another sets of placements on a fixed reference tree."
        )
        .def(
            "COG",
            ( void ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::center_of_gravity ),
            "Calculate the Center of Gravity of the placements on a tree."
        )
        .def(
            "Variance",
            ( double ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::variance ),
            "Calculate the Variance of the placements on a tree."
        )
        .def(
            "Dump",
            ( std::string ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::dump ),
            "Returns a list of all Pqueries with their Placements and Names."
        )
        .def(
            "Validate",
            ( bool ( ::genesis::PlacementMap::* )( bool, bool ) const )( &::genesis::PlacementMap::validate ),
            ( boost::python::arg("check_values")=(bool)(false), boost::python::arg("break_on_values")=(bool)(false) ),
            "Validates the integrity of the pointers, references and data in this Placement object."
        )
        //~ .def(
            //~ "earth_movers_distance",
            //~ ( double ( ::genesis::PlacementMap::* )( const ::genesis::PlacementMap &, const ::genesis::PlacementMap & ))( &::genesis::PlacementMap::earth_movers_distance ),
            //~ ( boost::python::arg("left"), boost::python::arg("right") ),
            //~ "Calculates the Earth Movers Distance between two sets of placements on a fixed reference tree."
        //~ )
        //~ .staticmethod("earth_movers_distance")

        // Public Member Variables

        .def_readonly("tree", &::genesis::PlacementMap::tree)
    ;

}

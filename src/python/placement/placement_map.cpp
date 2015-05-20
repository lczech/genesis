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
            ( bool ( ::genesis::PlacementMap::* )( const ::genesis::PlacementMap & ))( &::genesis::PlacementMap::Merge ),
            ( boost::python::arg("other") ),
            "Adds the pqueries from another ::genesis::PlacementMap objects to this one."
        )
        .def(
            "NormalizeWeightRatios",
            ( void ( ::genesis::PlacementMap::* )(  ))( &::genesis::PlacementMap::NormalizeWeightRatios ),
            "Recalculates the like_weight_ratio of the placements of each Pquery so that their sum is 1.0, while maintaining their ratio to each other."
        )
        .def(
            "RestrainToMaxWeightPlacements",
            ( void ( ::genesis::PlacementMap::* )(  ))( &::genesis::PlacementMap::RestrainToMaxWeightPlacements ),
            "Removes all placements but the most likely one from all pqueries."
        )
        .def(
            "PlacementCount",
            ( size_t ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::PlacementCount ),
            "Get the total number of placements in all pqueries."
        )
        .def(
            "PlacementMass",
            ( double ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::PlacementMass ),
            "Get the summed mass of all placements on the tree, given by their like_weight_ratio."
        )
        .def(
            "ClosestLeafDepthHistogram",
            ( std::vector< int > ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::ClosestLeafDepthHistogram ),
            "Returns a histogram representing how many placements have which depth with respect to their closest leaf node."
        )
        .def(
            "ClosestLeafDistanceHistogram",
            ( std::vector< int > ( ::genesis::PlacementMap::* )( const double, const double, const int ) const )( &::genesis::PlacementMap::ClosestLeafDistanceHistogram ),
            ( boost::python::arg("min"), boost::python::arg("max"), boost::python::arg("bins")=(const int)(10) ),
            "Returns a histogram counting the number of placements that have a certain distance to their closest leaf node, divided into equally large intervals between a min and a max distance."
        )
        .def(
            "ClosestLeafDistanceHistogramAuto",
            ( std::vector< int > ( ::genesis::PlacementMap::* )( double &, double &, const int ) const )( &::genesis::PlacementMap::ClosestLeafDistanceHistogramAuto ),
            ( boost::python::arg("min"), boost::python::arg("max"), boost::python::arg("bins")=(const int)(10) ),
            "Returns the same type of histogram as ClosestLeafDistanceHistogram(), but automatically determines the needed boundaries."
        )
        .def(
            "EMD",
            ( double ( ::genesis::PlacementMap::* )( const ::genesis::PlacementMap &, const bool ) const )( &::genesis::PlacementMap::EMD ),
            ( boost::python::arg("other"), boost::python::arg("with_pendant_length")=(const bool)(true) ),
            "Calculates the Earth Movers Distance to another sets of placements on a fixed reference tree."
        )
        .def(
            "COG",
            ( void ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::COG ),
            "Calculate the Center of Gravity of the placements on a tree."
        )
        .def(
            "Variance",
            ( double ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::Variance ),
            "Calculate the Variance of the placements on a tree."
        )
        .def(
            "Dump",
            ( std::string ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::Dump ),
            "Returns a list of all Pqueries with their Placements and Names."
        )
        .def(
            "Validate",
            ( bool ( ::genesis::PlacementMap::* )( bool, bool ) const )( &::genesis::PlacementMap::Validate ),
            ( boost::python::arg("check_values")=(bool)(false), boost::python::arg("break_on_values")=(bool)(false) ),
            "Validates the integrity of the pointers, references and data in this Placement object."
        )
        //~ .def(
            //~ "EMD",
            //~ ( double ( ::genesis::PlacementMap::* )( const ::genesis::PlacementMap &, const ::genesis::PlacementMap & ))( &::genesis::PlacementMap::EMD ),
            //~ ( boost::python::arg("left"), boost::python::arg("right") ),
            //~ "Calculates the Earth Movers Distance between two sets of placements on a fixed reference tree."
        //~ )
        //~ .staticmethod("EMD")

        // Public Member Variables

        .def_readonly("tree", &::genesis::PlacementMap::tree)
    ;

}

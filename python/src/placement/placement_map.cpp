/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/placement/placement_map.hpp"

void BoostPythonExport_PlacementMap()
{

    using namespace genesis;

    boost::python::class_< ::genesis::PlacementMap > ( "PlacementMap", boost::python::init<  >(  ) )
        .def( boost::python::init< std::shared_ptr<PlacementTree> >(( boost::python::arg("ptree") )) )
        .def( boost::python::init< const PlacementMap & >(( boost::python::arg("other") )) )

        // Public Member Functions

        .def(
            "center_of_gravity",
            ( void ( ::genesis::PlacementMap::* )( const bool ) const )( &::genesis::PlacementMap::center_of_gravity ),
            ( boost::python::arg("with_pendant_length")=(const bool)(true) ),
            get_docstring("void ::genesis::PlacementMap::center_of_gravity (const bool with_pendant_length=true) const")
        )
        .def(
            "center_of_gravity_distance",
            ( double ( ::genesis::PlacementMap::* )( const PlacementMap &, const bool ) const )( &::genesis::PlacementMap::center_of_gravity_distance ),
            ( boost::python::arg("other"), boost::python::arg("with_pendant_length")=(const bool)(true) )
        )
        .def(
            "center_of_gravity_variance",
            ( double ( ::genesis::PlacementMap::* )( bool ) const )( &::genesis::PlacementMap::center_of_gravity_variance ),
            ( boost::python::arg("with_pendant_length")=(bool)(true)),
            get_docstring("double ::genesis::PlacementMap::center_of_gravity_variance (const bool with_pendant_length=true) const")
        )
        .def(
            "clear",
            ( void ( ::genesis::PlacementMap::* )(  ))( &::genesis::PlacementMap::clear ),
            get_docstring("void ::genesis::PlacementMap::clear ()")
        )
        .def(
            "clear_placements",
            ( void ( ::genesis::PlacementMap::* )(  ))( &::genesis::PlacementMap::clear_placements ),
            get_docstring("void ::genesis::PlacementMap::clear_placements ()")
        )
        .def(
            "closest_leaf_depth_histogram",
            ( std::vector< int > ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::closest_leaf_depth_histogram ),
            get_docstring("std::vector< int > ::genesis::PlacementMap::closest_leaf_depth_histogram () const")
        )
        .def(
            "closest_leaf_distance_histogram",
            ( std::vector< int > ( ::genesis::PlacementMap::* )( const double, const double, const int ) const )( &::genesis::PlacementMap::closest_leaf_distance_histogram ),
            ( boost::python::arg("min"), boost::python::arg("max"), boost::python::arg("bins")=(const int)(10) ),
            get_docstring("std::vector< int > ::genesis::PlacementMap::closest_leaf_distance_histogram (const double min, const double max, const int bins=10) const")
        )
        .def(
            "closest_leaf_distance_histogram_auto",
            ( std::vector< int > ( ::genesis::PlacementMap::* )( double &, double &, const int ) const )( &::genesis::PlacementMap::closest_leaf_distance_histogram_auto ),
            ( boost::python::arg("min"), boost::python::arg("max"), boost::python::arg("bins")=(const int)(10) ),
            get_docstring("std::vector< int > ::genesis::PlacementMap::closest_leaf_distance_histogram_auto (double & min, double & max, const int bins=10) const")
        )
        .def(
            "dump",
            ( std::string ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::dump ),
            get_docstring("std::string ::genesis::PlacementMap::dump () const")
        )
        .def(
            "dump_tree",
            ( std::string ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::dump_tree ),
            get_docstring("std::string ::genesis::PlacementMap::dump_tree () const")
        )
        // .def(
        //     "earth_movers_distance",
        //     ( double ( * )( const PlacementMap &, const PlacementMap &, const bool ))( &::genesis::PlacementMap::earth_movers_distance ),
        //     ( boost::python::arg("left"), boost::python::arg("right"), boost::python::arg("with_pendant_length")=(const bool)(true) ),
        //     get_docstring("static double ::genesis::PlacementMap::earth_movers_distance (const PlacementMap & left, const PlacementMap & right, const bool with_pendant_length=true)")
        // )
        // .staticmethod("earth_movers_distance")
        .def(
            "earth_movers_distance",
            ( double ( ::genesis::PlacementMap::* )( const PlacementMap &, const bool ) const )( &::genesis::PlacementMap::earth_movers_distance ),
            ( boost::python::arg("other"), boost::python::arg("with_pendant_length")=(const bool)(true) ),
            get_docstring("double ::genesis::PlacementMap::earth_movers_distance (const PlacementMap & other, const bool with_pendant_length=true) const")
        )
        // .def(
        //     "edge_num_map",
        //     ( EdgeNumMapType * ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::edge_num_map ),
        //     get_docstring("EdgeNumMapType * ::genesis::PlacementMap::edge_num_map () const")
        // )
        .def(
            "merge",
            ( bool ( ::genesis::PlacementMap::* )( const PlacementMap & ))( &::genesis::PlacementMap::merge ),
            ( boost::python::arg("other") ),
            get_docstring("bool ::genesis::PlacementMap::merge (const PlacementMap & other)")
        )
        .def(
            "normalize_weight_ratios",
            ( void ( ::genesis::PlacementMap::* )(  ))( &::genesis::PlacementMap::normalize_weight_ratios ),
            get_docstring("void ::genesis::PlacementMap::normalize_weight_ratios ()")
        )
        .def(
            "pairwise_distance",
            ( double ( ::genesis::PlacementMap::* )( const PlacementMap &, const bool ) const )( &::genesis::PlacementMap::pairwise_distance ),
            ( boost::python::arg("other"), boost::python::arg("with_pendant_length")=(const bool)(true) ),
            get_docstring("double ::genesis::PlacementMap::pairwise_distance (const PlacementMap & other, const bool with_pendant_length=true) const")
        )
        .def(
            "placement_count",
            ( size_t ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::placement_count ),
            get_docstring("size_t ::genesis::PlacementMap::placement_count () const")
        )
        .def(
            "placement_mass",
            ( double ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::placement_mass ),
            get_docstring("double ::genesis::PlacementMap::placement_mass () const")
        )
        .def(
            "restrain_to_max_weight_placements",
            ( void ( ::genesis::PlacementMap::* )(  ))( &::genesis::PlacementMap::restrain_to_max_weight_placements ),
            get_docstring("void ::genesis::PlacementMap::restrain_to_max_weight_placements ()")
        )
        .def(
            "tree",
            ( PlacementTree & ( ::genesis::PlacementMap::* )(  ))( &::genesis::PlacementMap::tree ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "validate",
            ( bool ( ::genesis::PlacementMap::* )( bool, bool ) const )( &::genesis::PlacementMap::validate ),
            ( boost::python::arg("check_values")=(bool)(false), boost::python::arg("break_on_values")=(bool)(false) ),
            get_docstring("bool ::genesis::PlacementMap::validate (bool check_values=false, bool break_on_values=false) const")
        )
        .def(
            "variance",
            ( double ( ::genesis::PlacementMap::* )( bool ) const )( &::genesis::PlacementMap::variance ),
            ( boost::python::arg("with_pendant_length")=(bool)(true)),
            get_docstring("double ::genesis::PlacementMap::variance (const bool with_pendant_length=true) const")
        )

        // Public Member Variables

        // .def_readonly("tree", &::genesis::PlacementMap::tree)
    ;

}

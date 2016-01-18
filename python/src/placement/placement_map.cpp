/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/placement/placement_map.hpp"
#include "lib/placement/operators.hpp"
#include "lib/placement/functions.hpp"

PYTHON_EXPORT_CLASS (PlacementMap, "placement")
{

    using namespace genesis;

    // boost::python::class_< PlacementMap, std::shared_ptr<PlacementMap> > ( "PlacementMap", boost::python::init<  >(  ) )
    boost::python::class_< PlacementMap > ( "PlacementMap", boost::python::init<  >(  ) )
        .def( boost::python::init< std::shared_ptr<PlacementTree> >(( boost::python::arg("ptree") )) )
        .def( boost::python::init< const PlacementMap & >(( boost::python::arg("other") )) )

        // Public Member Functions

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
        // .def(
        //     "dump",
        //     ( std::string ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::dump ),
        //     get_docstring("std::string ::genesis::PlacementMap::dump () const")
        // )
        .def(
            "dump_tree",
            ( std::string ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::dump_tree ),
            get_docstring("std::string ::genesis::PlacementMap::dump_tree () const")
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
            "pquery",
            ( Pquery const & ( ::genesis::PlacementMap::* )( const size_t ))( &::genesis::PlacementMap::pquery ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            ( boost::python::arg("index") )
        )
        .def(
            "pquery_size",
            ( size_t ( ::genesis::PlacementMap::* )(  ) const )( &::genesis::PlacementMap::pquery_size )
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

        // Public Member Variables

        // .def_readonly("tree", &::genesis::PlacementMap::tree)

        // Operators

        .def(
            boost::python::self_ns::str( boost::python::self )
        )
    ;

    // -------------------------------------------------------------------
    //     operators.hpp
    // -------------------------------------------------------------------

    boost::python::def(
        "compatible_trees",
        ::genesis::compatible_trees
    );
    boost::python::def(
        "has_correct_edge_nums",
        ::genesis::has_correct_edge_nums
    );

    // -------------------------------------------------------------------
    //     functions.hpp
    // -------------------------------------------------------------------

    // def(
    //     "merge_duplicates",
    //     void ( ::genesis::PlacementMap & ) ( &::genesis::merge_duplicates )
    // );
    boost::python::def(
        "merge_duplicates",
        ::genesis::merge_duplicates
    );

    boost::python::def(
        "has_name",
        ( bool ( * )( ::genesis::Pquery const& pquery, std::string const& name ))( &::genesis::has_name )
    );
    boost::python::def(
        "has_name",
        ( bool ( * )( ::genesis::PlacementMap const& map, std::string const& name ))( &::genesis::has_name )
    );
    boost::python::def(
        "find_pquery",
        ( Pquery const* ( * )( ::genesis::PlacementMap const& map, std::string const& name ))( &::genesis::find_pquery ),
        boost::python::return_value_policy<boost::python::reference_existing_object>()
    );

    boost::python::def(
        "sort_placements_by_like_weight_ratio",
        ( void ( * )( ::genesis::PlacementMap& map ))( &::genesis::sort_placements_by_like_weight_ratio )
    );

}

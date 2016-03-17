/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/placement/sample.hpp"
#include "lib/placement/function/helper.hpp"
#include "lib/placement/function/operators.hpp"
#include "lib/placement/function/functions.hpp"

using namespace genesis::placement;

PYTHON_EXPORT_CLASS (Sample, "placement")
{
    // boost::python::class_< Sample, std::shared_ptr<Sample> > ( "Sample", boost::python::init<  >(  ) )
    boost::python::class_< Sample > ( "Sample", boost::python::init<  >(  ) )
        .def( boost::python::init< std::shared_ptr<PlacementTree> >(( boost::python::arg("ptree") )) )
        .def( boost::python::init< const Sample & >(( boost::python::arg("other") )) )

        // Public Member Functions

        .def(
            "clear",
            ( void ( ::genesis::placement::Sample::* )(  ))( &::genesis::placement::Sample::clear ),
            get_docstring("void ::genesis::placement::Sample::clear ()")
        )
        .def(
            "clear_pqueries",
            ( void ( ::genesis::placement::Sample::* )(  ))( &::genesis::placement::Sample::clear_pqueries ),
            get_docstring("void ::genesis::placement::Sample::clear_pqueries ()")
        )
        // .def(
        //     "closest_leaf_depth_histogram",
        //     ( std::vector< int > ( ::genesis::placement::Sample::* )(  ) const )( &::genesis::placement::Sample::closest_leaf_depth_histogram ),
        //     get_docstring("std::vector< int > ::genesis::placement::Sample::closest_leaf_depth_histogram () const")
        // )
        // .def(
        //     "closest_leaf_distance_histogram",
        //     ( std::vector< int > ( ::genesis::placement::Sample::* )( const double, const double, const int ) const )( &::genesis::placement::Sample::closest_leaf_distance_histogram ),
        //     ( boost::python::arg("min"), boost::python::arg("max"), boost::python::arg("bins")=(const int)(10) ),
        //     get_docstring("std::vector< int > ::genesis::placement::Sample::closest_leaf_distance_histogram (const double min, const double max, const int bins=10) const")
        // )
        // .def(
        //     "closest_leaf_distance_histogram_auto",
        //     ( std::vector< int > ( ::genesis::placement::Sample::* )( double &, double &, const int ) const )( &::genesis::placement::Sample::closest_leaf_distance_histogram_auto ),
        //     ( boost::python::arg("min"), boost::python::arg("max"), boost::python::arg("bins")=(const int)(10) ),
        //     get_docstring("std::vector< int > ::genesis::placement::Sample::closest_leaf_distance_histogram_auto (double & min, double & max, const int bins=10) const")
        // )
        // .def(
        //     "dump",
        //     ( std::string ( ::genesis::placement::Sample::* )(  ) const )( &::genesis::placement::Sample::dump ),
        //     get_docstring("std::string ::genesis::placement::Sample::dump () const")
        // )
        // .def(
        //     "dump_tree",
        //     ( std::string ( ::genesis::placement::Sample::* )(  ) const )( &::genesis::placement::Sample::dump_tree ),
        //     get_docstring("std::string ::genesis::placement::Sample::dump_tree () const")
        // )
        // .def(
        //     "edge_num_map",
        //     ( EdgeNumMapType * ( ::genesis::placement::Sample::* )(  ) const )( &::genesis::placement::Sample::edge_num_map ),
        //     get_docstring("EdgeNumMapType * ::genesis::placement::Sample::edge_num_map () const")
        // )
        // .def(
        //     "merge",
        //     ( bool ( ::genesis::placement::Sample::* )( const Sample & ))( &::genesis::placement::Sample::merge ),
        //     ( boost::python::arg("other") ),
        //     get_docstring("bool ::genesis::placement::Sample::merge (const Sample & other)")
        // )
        // .def(
        //     "normalize_weight_ratios",
        //     ( void ( ::genesis::placement::Sample::* )(  ))( &::genesis::placement::Sample::normalize_weight_ratios ),
        //     get_docstring("void ::genesis::placement::Sample::normalize_weight_ratios ()")
        // )
        // .def(
        //     "placement_count",
        //     ( size_t ( ::genesis::placement::Sample::* )(  ) const )( &::genesis::placement::Sample::placement_count ),
        //     get_docstring("size_t ::genesis::placement::Sample::placement_count () const")
        // )
        // .def(
        //     "placement_mass",
        //     ( double ( ::genesis::placement::Sample::* )(  ) const )( &::genesis::placement::Sample::placement_mass ),
        //     get_docstring("double ::genesis::placement::Sample::placement_mass () const")
        // )
        // .def(
        //     "pquery_at",
        //     ( Pquery const & ( ::genesis::placement::Sample::* )( const size_t ))( &::genesis::placement::Sample::pquery_at ),
        //     boost::python::return_value_policy<boost::python::reference_existing_object>(),
        //     ( boost::python::arg("index") )
        // )
        .def(
            "pquery_size",
            ( size_t ( ::genesis::placement::Sample::* )(  ) const )( &::genesis::placement::Sample::pquery_size )
        )
        // .def(
        //     "restrain_to_max_weight_placements",
        //     ( void ( ::genesis::placement::Sample::* )(  ))( &::genesis::placement::Sample::restrain_to_max_weight_placements ),
        //     get_docstring("void ::genesis::placement::Sample::restrain_to_max_weight_placements ()")
        // )
        .def(
            "tree",
            ( PlacementTree & ( ::genesis::placement::Sample::* )(  ))( &::genesis::placement::Sample::tree ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        // .def(
        //     "validate",
        //     ( bool ( ::genesis::placement::Sample::* )( bool, bool ) const )( &::genesis::placement::Sample::validate ),
        //     ( boost::python::arg("check_values")=(bool)(false), boost::python::arg("break_on_values")=(bool)(false) ),
        //     get_docstring("bool ::genesis::placement::Sample::validate (bool check_values=false, bool break_on_values=false) const")
        // )

        // Public Member Variables

        // .def_readonly("tree", &::genesis::placement::Sample::tree)

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
        ::genesis::placement::compatible_trees
    );
    boost::python::def(
        "has_correct_edge_nums",
        ::genesis::placement::has_correct_edge_nums
    );

    // -------------------------------------------------------------------
    //     functions.hpp
    // -------------------------------------------------------------------

    // def(
    //     "merge_duplicates",
    //     void ( ::genesis::placement::Sample & ) ( &::genesis::placement::merge_duplicates )
    // );
    boost::python::def(
        "merge_duplicates",
        ::genesis::placement::merge_duplicates
    );

    boost::python::def(
        "has_name",
        ( bool ( * )( ::genesis::placement::Pquery const& pquery, std::string const& name ))( &::genesis::placement::has_name )
    );
    boost::python::def(
        "has_name",
        ( bool ( * )( ::genesis::placement::Sample const& map, std::string const& name ))( &::genesis::placement::has_name )
    );
    boost::python::def(
        "find_pquery",
        ( Pquery const* ( * )( ::genesis::placement::Sample const& map, std::string const& name ))( &::genesis::placement::find_pquery ),
        boost::python::return_value_policy<boost::python::reference_existing_object>()
    );

    boost::python::def(
        "sort_placements_by_like_weight_ratio",
        ( void ( * )( ::genesis::placement::Sample& map ))( &::genesis::placement::sort_placements_by_like_weight_ratio )
    );

}

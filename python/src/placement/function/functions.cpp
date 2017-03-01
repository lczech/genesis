/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_FUNCTIONS( placement_function_functions_export, ::genesis::placement, scope )
{

    scope.def(
        "find_pquery",
        ( Pquery * ( * )( Sample &, std::string const & ))( &::genesis::placement::find_pquery ),
            pybind11::arg("smp"),
            pybind11::arg("name"),
        get_docstring("Pquery * ::genesis::placement::find_pquery (Sample & smp, std::string const & name)")
    );

    scope.def(
        "find_pquery",
        ( Pquery const * ( * )( Sample const &, std::string const & ))( &::genesis::placement::find_pquery ),
            pybind11::arg("smp"),
            pybind11::arg("name"),
        get_docstring("Pquery const * ::genesis::placement::find_pquery (Sample const & smp, std::string const & name)")
    );

    scope.def(
        "has_name",
        ( bool ( * )( Pquery const &, std::string const & ))( &::genesis::placement::has_name ),
            pybind11::arg("pquery"),
            pybind11::arg("name"),
        get_docstring("bool ::genesis::placement::has_name (Pquery const & pquery, std::string const & name)")
    );

    scope.def(
        "has_name",
        ( bool ( * )( Sample const &, std::string const & ))( &::genesis::placement::has_name ),
            pybind11::arg("smp"),
            pybind11::arg("name"),
        get_docstring("bool ::genesis::placement::has_name (Sample const & smp, std::string const & name)")
    );

    scope.def(
        "total_multiplicity",
        ( double ( * )( Pquery const & ))( &::genesis::placement::total_multiplicity ),
            pybind11::arg("pqry"),
        get_docstring("double ::genesis::placement::total_multiplicity (Pquery const & pqry)")
    );

    scope.def(
        "total_multiplicity",
        ( double ( * )( Sample const & ))( &::genesis::placement::total_multiplicity ),
            pybind11::arg("sample"),
        get_docstring("double ::genesis::placement::total_multiplicity (Sample const & sample)")
    );

    scope.def(
        "total_placement_mass",
        ( double ( * )( Sample const & ))( &::genesis::placement::total_placement_mass ),
            pybind11::arg("smp"),
        get_docstring("double ::genesis::placement::total_placement_mass (Sample const & smp)")
    );

    scope.def(
        "total_placement_mass_with_multiplicities",
        ( double ( * )( Sample const & ))( &::genesis::placement::total_placement_mass_with_multiplicities ),
            pybind11::arg("smp"),
        get_docstring("double ::genesis::placement::total_placement_mass_with_multiplicities (Sample const & smp)")
    );

    scope.def(
        "total_placement_count",
        ( size_t ( * )( Sample const & ))( &::genesis::placement::total_placement_count ),
            pybind11::arg("smp"),
        get_docstring("size_t ::genesis::placement::total_placement_count (Sample const & smp)")
    );

    scope.def(
        "placement_mass_max_edge",
        ( std::pair< PlacementTreeEdge const *, double > ( * )( Sample const & ))( &::genesis::placement::placement_mass_max_edge ),
            pybind11::arg("smp"),
        get_docstring("std::pair< PlacementTreeEdge const *, double > ::genesis::placement::placement_mass_max_edge (Sample const & smp)")
    );

    scope.def(
        "placement_count_max_edge",
        ( std::pair< PlacementTreeEdge const *, size_t > ( * )( Sample const & ))( &::genesis::placement::placement_count_max_edge ),
            pybind11::arg("smp"),
        get_docstring("std::pair< PlacementTreeEdge const *, size_t > ::genesis::placement::placement_count_max_edge (Sample const & smp)")
    );

    scope.def(
        "all_pquery_names",
        ( std::unordered_set< std::string > ( * )( Sample const & ))( &::genesis::placement::all_pquery_names ),
            pybind11::arg("sample"),
        get_docstring("std::unordered_set< std::string > ::genesis::placement::all_pquery_names (Sample const & sample)")
    );

    scope.def(
        "closest_leaf_weight_distribution",
        ( std::vector< double > ( * )( Sample const & ))( &::genesis::placement::closest_leaf_weight_distribution ),
            pybind11::arg("sample")
    );

    scope.def(
        "closest_leaf_depth_histogram",
        ( std::vector< int > ( * )( Sample const & ))( &::genesis::placement::closest_leaf_depth_histogram ),
            pybind11::arg("smp"),
        get_docstring("std::vector< int > ::genesis::placement::closest_leaf_depth_histogram (Sample const & smp)")
    );

    scope.def(
        "closest_leaf_distance_histogram",
        ( std::vector< int > ( * )( Sample const &, const double, const double, const int ))( &::genesis::placement::closest_leaf_distance_histogram ),
            pybind11::arg("smp"),
            pybind11::arg("min"),
            pybind11::arg("max"),
            pybind11::arg("bins")=(const int)(10),
        get_docstring("std::vector< int > ::genesis::placement::closest_leaf_distance_histogram (Sample const & smp, const double min, const double max, const int bins=10)")
    );

    scope.def(
        "closest_leaf_distance_histogram_auto",
        ( std::vector< int > ( * )( Sample const &, double &, double &, const int ))( &::genesis::placement::closest_leaf_distance_histogram_auto ),
            pybind11::arg("smp"),
            pybind11::arg("min"),
            pybind11::arg("max"),
            pybind11::arg("bins")=(const int)(10),
        get_docstring("std::vector< int > ::genesis::placement::closest_leaf_distance_histogram_auto (Sample const & smp, double & min, double & max, const int bins=10)")
    );

    scope.def(
        "collect_duplicate_pqueries",
        ( void ( * )( Sample & ))( &::genesis::placement::collect_duplicate_pqueries ),
            pybind11::arg("smp"),
        get_docstring("void ::genesis::placement::collect_duplicate_pqueries (Sample & smp)")
    );

    scope.def(
        "copy_pqueries",
        ( void ( * )( Sample const &, Sample & ))( &::genesis::placement::copy_pqueries ),
            pybind11::arg("source"),
            pybind11::arg("target"),
        get_docstring("void ::genesis::placement::copy_pqueries (Sample const & source, Sample & target)")
    );

    scope.def(
        "filter_min_accumulated_weight",
        ( void ( * )( Pquery &, double ))( &::genesis::placement::filter_min_accumulated_weight ),
            pybind11::arg("pquery"),
            pybind11::arg("threshold")=(double)(0.99),
        get_docstring("void ::genesis::placement::filter_min_accumulated_weight (Pquery & pquery, double threshold=0.99)")
    );

    scope.def(
        "filter_min_accumulated_weight",
        ( void ( * )( Sample &, double ))( &::genesis::placement::filter_min_accumulated_weight ),
            pybind11::arg("smp"),
            pybind11::arg("threshold")=(double)(0.99),
        get_docstring("void ::genesis::placement::filter_min_accumulated_weight (Sample & smp, double threshold=0.99)")
    );

    scope.def(
        "filter_min_weight_threshold",
        ( void ( * )( Pquery &, double ))( &::genesis::placement::filter_min_weight_threshold ),
            pybind11::arg("pquery"),
            pybind11::arg("threshold")=(double)(0.01),
        get_docstring("void ::genesis::placement::filter_min_weight_threshold (Pquery & pquery, double threshold=0.01)")
    );

    scope.def(
        "filter_min_weight_threshold",
        ( void ( * )( Sample &, double ))( &::genesis::placement::filter_min_weight_threshold ),
            pybind11::arg("smp"),
            pybind11::arg("threshold")=(double)(0.01),
        get_docstring("void ::genesis::placement::filter_min_weight_threshold (Sample & smp, double threshold=0.01)")
    );

    scope.def(
        "filter_n_max_weight_placements",
        ( void ( * )( Pquery &, size_t ))( &::genesis::placement::filter_n_max_weight_placements ),
            pybind11::arg("pquery"),
            pybind11::arg("n")=(size_t)(1),
        get_docstring("void ::genesis::placement::filter_n_max_weight_placements (Pquery & pquery, size_t n=1)")
    );

    scope.def(
        "filter_n_max_weight_placements",
        ( void ( * )( Sample &, size_t ))( &::genesis::placement::filter_n_max_weight_placements ),
            pybind11::arg("smp"),
            pybind11::arg("n")=(size_t)(1),
        get_docstring("void ::genesis::placement::filter_n_max_weight_placements (Sample & smp, size_t n=1)")
    );

    scope.def(
        "filter_pqueries_differing_names",
        ( void ( * )( Sample &, Sample & ))( &::genesis::placement::filter_pqueries_differing_names ),
            pybind11::arg("sample_1"),
            pybind11::arg("sample_2"),
        get_docstring("void ::genesis::placement::filter_pqueries_differing_names (Sample & sample_1, Sample & sample_2)")
    );

    scope.def(
        "filter_pqueries_intersecting_names",
        ( void ( * )( Sample &, Sample & ))( &::genesis::placement::filter_pqueries_intersecting_names ),
            pybind11::arg("sample_1"),
            pybind11::arg("sample_2"),
        get_docstring("void ::genesis::placement::filter_pqueries_intersecting_names (Sample & sample_1, Sample & sample_2)")
    );

    scope.def(
        "filter_pqueries_keeping_names",
        ( void ( * )( Sample &, std::string const & ))( &::genesis::placement::filter_pqueries_keeping_names ),
            pybind11::arg("smp"),
            pybind11::arg("regex"),
        get_docstring("void ::genesis::placement::filter_pqueries_keeping_names (Sample & smp, std::string const & regex)")
    );

    scope.def(
        "filter_pqueries_keeping_names",
        ( void ( * )( Sample &, std::unordered_set< std::string > ))( &::genesis::placement::filter_pqueries_keeping_names ),
            pybind11::arg("smp"),
            pybind11::arg("keep_list"),
        get_docstring("void ::genesis::placement::filter_pqueries_keeping_names (Sample & smp, std::unordered_set< std::string > keep_list)")
    );

    scope.def(
        "filter_pqueries_removing_names",
        ( void ( * )( Sample &, std::string const & ))( &::genesis::placement::filter_pqueries_removing_names ),
            pybind11::arg("smp"),
            pybind11::arg("regex"),
        get_docstring("void ::genesis::placement::filter_pqueries_removing_names (Sample & smp, std::string const & regex)")
    );

    scope.def(
        "filter_pqueries_removing_names",
        ( void ( * )( Sample &, std::unordered_set< std::string > ))( &::genesis::placement::filter_pqueries_removing_names ),
            pybind11::arg("smp"),
            pybind11::arg("remove_list"),
        get_docstring("void ::genesis::placement::filter_pqueries_removing_names (Sample & smp, std::unordered_set< std::string > remove_list)")
    );

    scope.def(
        "merge_duplicate_names",
        ( void ( * )( Pquery & ))( &::genesis::placement::merge_duplicate_names ),
            pybind11::arg("pquery"),
        get_docstring("void ::genesis::placement::merge_duplicate_names (Pquery & pquery)")
    );

    scope.def(
        "merge_duplicate_names",
        ( void ( * )( Sample & ))( &::genesis::placement::merge_duplicate_names ),
            pybind11::arg("smp"),
        get_docstring("void ::genesis::placement::merge_duplicate_names (Sample & smp)")
    );

    scope.def(
        "merge_duplicate_placements",
        ( void ( * )( Pquery & ))( &::genesis::placement::merge_duplicate_placements ),
            pybind11::arg("pquery"),
        get_docstring("void ::genesis::placement::merge_duplicate_placements (Pquery & pquery)")
    );

    scope.def(
        "merge_duplicate_placements",
        ( void ( * )( Sample & ))( &::genesis::placement::merge_duplicate_placements ),
            pybind11::arg("smp"),
        get_docstring("void ::genesis::placement::merge_duplicate_placements (Sample & smp)")
    );

    scope.def(
        "merge_duplicates",
        ( void ( * )( Sample & ))( &::genesis::placement::merge_duplicates ),
            pybind11::arg("smp"),
        get_docstring("void ::genesis::placement::merge_duplicates (Sample & smp)")
    );

    scope.def(
        "normalize_weight_ratios",
        ( void ( * )( Pquery & ))( &::genesis::placement::normalize_weight_ratios ),
            pybind11::arg("pquery"),
        get_docstring("void ::genesis::placement::normalize_weight_ratios (Pquery & pquery)")
    );

    scope.def(
        "normalize_weight_ratios",
        ( void ( * )( Sample & ))( &::genesis::placement::normalize_weight_ratios ),
            pybind11::arg("smp"),
        get_docstring("void ::genesis::placement::normalize_weight_ratios (Sample & smp)")
    );

    scope.def(
        "scale_all_branch_lengths",
        ( void ( * )( Sample &, double ))( &::genesis::placement::scale_all_branch_lengths ),
            pybind11::arg("smp"),
            pybind11::arg("factor")=(double)(1.0),
        get_docstring("void ::genesis::placement::scale_all_branch_lengths (Sample & smp, double factor=1.0)")
    );

    scope.def(
        "sort_placements_by_weight",
        ( void ( * )( Pquery & ))( &::genesis::placement::sort_placements_by_weight ),
            pybind11::arg("pquery"),
        get_docstring("void ::genesis::placement::sort_placements_by_weight (Pquery & pquery)")
    );

    scope.def(
        "sort_placements_by_weight",
        ( void ( * )( Sample & ))( &::genesis::placement::sort_placements_by_weight ),
            pybind11::arg("smp"),
        get_docstring("void ::genesis::placement::sort_placements_by_weight (Sample & smp)")
    );
}

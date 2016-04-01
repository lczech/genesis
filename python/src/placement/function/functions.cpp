/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_FUNCTIONS(placement_function_functions_export, "placement")
{

    boost::python::def(
        "find_pquery",
        ( Pquery const * ( * )( Sample const &, std::string const & ))( &::genesis::placement::find_pquery ),
        ( boost::python::arg("smp"), boost::python::arg("name") ),
        boost::python::return_value_policy<boost::python::reference_existing_object>(),
        get_docstring("Pquery const * ::genesis::placement::find_pquery (Sample const & smp, std::string const & name)")
    );

    boost::python::def(
        "has_name",
        ( bool ( * )( Pquery const &, std::string const & ))( &::genesis::placement::has_name ),
        ( boost::python::arg("pquery"), boost::python::arg("name") ),
        get_docstring("bool ::genesis::placement::has_name (Pquery const & pquery, std::string const & name)")
    );

    boost::python::def(
        "has_name",
        ( bool ( * )( Sample const &, std::string const & ))( &::genesis::placement::has_name ),
        ( boost::python::arg("smp"), boost::python::arg("name") ),
        get_docstring("bool ::genesis::placement::has_name (Sample const & smp, std::string const & name)")
    );

    boost::python::def(
        "total_multiplicity",
        ( double ( * )( Pquery const & ))( &::genesis::placement::total_multiplicity ),
        ( boost::python::arg("pqry") ),
        get_docstring("double ::genesis::placement::total_multiplicity (Pquery const & pqry)")
    );

    boost::python::def(
        "total_placement_mass",
        ( double ( * )( Sample const & ))( &::genesis::placement::total_placement_mass ),
        ( boost::python::arg("smp") ),
        get_docstring("double ::genesis::placement::total_placement_mass (Sample const & smp)")
    );

    boost::python::def(
        "total_placement_mass_with_multiplicities",
        ( double ( * )( Sample const & ))( &::genesis::placement::total_placement_mass_with_multiplicities ),
        ( boost::python::arg("smp") ),
        get_docstring("double ::genesis::placement::total_placement_mass_with_multiplicities (Sample const & smp)")
    );

    boost::python::def(
        "total_placement_count",
        ( size_t ( * )( Sample const & ))( &::genesis::placement::total_placement_count ),
        ( boost::python::arg("smp") ),
        get_docstring("size_t ::genesis::placement::total_placement_count (Sample const & smp)")
    );

    boost::python::def(
        "placement_mass_max_edge",
        ( std::pair< PlacementTreeEdge const *, double > ( * )( Sample const & ))( &::genesis::placement::placement_mass_max_edge ),
        ( boost::python::arg("smp") ),
        get_docstring("std::pair< PlacementTreeEdge const *, double > ::genesis::placement::placement_mass_max_edge (Sample const & smp)")
    );

    boost::python::def(
        "placement_count_max_edge",
        ( std::pair< PlacementTreeEdge const *, size_t > ( * )( Sample const & ))( &::genesis::placement::placement_count_max_edge ),
        ( boost::python::arg("smp") ),
        get_docstring("std::pair< PlacementTreeEdge const *, size_t > ::genesis::placement::placement_count_max_edge (Sample const & smp)")
    );

    boost::python::def(
        "all_pquery_names",
        ( std::unordered_set< std::string > ( * )( Sample const & ))( &::genesis::placement::all_pquery_names ),
        ( boost::python::arg("sample") ),
        get_docstring("std::unordered_set< std::string > ::genesis::placement::all_pquery_names (Sample const & sample)")
    );

    boost::python::def(
        "closest_leaf_depth_histogram",
        ( std::vector< int > ( * )( Sample const & ))( &::genesis::placement::closest_leaf_depth_histogram ),
        ( boost::python::arg("smp") ),
        get_docstring("std::vector< int > ::genesis::placement::closest_leaf_depth_histogram (Sample const & smp)")
    );

    boost::python::def(
        "closest_leaf_distance_histogram",
        ( std::vector< int > ( * )( Sample const &, const double, const double, const int ))( &::genesis::placement::closest_leaf_distance_histogram ),
        ( boost::python::arg("smp"), boost::python::arg("min"), boost::python::arg("max"), boost::python::arg("bins") ),
        get_docstring("std::vector< int > ::genesis::placement::closest_leaf_distance_histogram (Sample const & smp, const double min, const double max, const int bins)")
    );

    boost::python::def(
        "closest_leaf_distance_histogram_auto",
        ( std::vector< int > ( * )( Sample const &, double &, double &, const int ))( &::genesis::placement::closest_leaf_distance_histogram_auto ),
        ( boost::python::arg("smp"), boost::python::arg("min"), boost::python::arg("max"), boost::python::arg("bins") ),
        get_docstring("std::vector< int > ::genesis::placement::closest_leaf_distance_histogram_auto (Sample const & smp, double & min, double & max, const int bins)")
    );

    boost::python::def(
        "collect_duplicate_pqueries",
        ( void ( * )( Sample & ))( &::genesis::placement::collect_duplicate_pqueries ),
        ( boost::python::arg("smp") ),
        get_docstring("void ::genesis::placement::collect_duplicate_pqueries (Sample & smp)")
    );

    boost::python::def(
        "copy_pqueries",
        ( void ( * )( Sample const &, Sample & ))( &::genesis::placement::copy_pqueries ),
        ( boost::python::arg("source"), boost::python::arg("target") ),
        get_docstring("void ::genesis::placement::copy_pqueries (Sample const & source, Sample & target)")
    );

    boost::python::def(
        "filter_min_accumulated_weight",
        ( void ( * )( Pquery &, double ))( &::genesis::placement::filter_min_accumulated_weight ),
        ( boost::python::arg("pquery"), boost::python::arg("threshold") ),
        get_docstring("void ::genesis::placement::filter_min_accumulated_weight (Pquery & pquery, double threshold)")
    );

    boost::python::def(
        "filter_min_accumulated_weight",
        ( void ( * )( Sample &, double ))( &::genesis::placement::filter_min_accumulated_weight ),
        ( boost::python::arg("smp"), boost::python::arg("threshold") ),
        get_docstring("void ::genesis::placement::filter_min_accumulated_weight (Sample & smp, double threshold)")
    );

    boost::python::def(
        "filter_min_weight_threshold",
        ( void ( * )( Pquery &, double ))( &::genesis::placement::filter_min_weight_threshold ),
        ( boost::python::arg("pquery"), boost::python::arg("threshold") ),
        get_docstring("void ::genesis::placement::filter_min_weight_threshold (Pquery & pquery, double threshold)")
    );

    boost::python::def(
        "filter_min_weight_threshold",
        ( void ( * )( Sample &, double ))( &::genesis::placement::filter_min_weight_threshold ),
        ( boost::python::arg("smp"), boost::python::arg("threshold") ),
        get_docstring("void ::genesis::placement::filter_min_weight_threshold (Sample & smp, double threshold)")
    );

    boost::python::def(
        "filter_n_max_weight_placements",
        ( void ( * )( Pquery &, size_t ))( &::genesis::placement::filter_n_max_weight_placements ),
        ( boost::python::arg("pquery"), boost::python::arg("n") ),
        get_docstring("void ::genesis::placement::filter_n_max_weight_placements (Pquery & pquery, size_t n)")
    );

    boost::python::def(
        "filter_n_max_weight_placements",
        ( void ( * )( Sample &, size_t ))( &::genesis::placement::filter_n_max_weight_placements ),
        ( boost::python::arg("smp"), boost::python::arg("n") ),
        get_docstring("void ::genesis::placement::filter_n_max_weight_placements (Sample & smp, size_t n)")
    );

    boost::python::def(
        "merge_duplicate_names",
        ( void ( * )( Pquery & ))( &::genesis::placement::merge_duplicate_names ),
        ( boost::python::arg("pquery") ),
        get_docstring("void ::genesis::placement::merge_duplicate_names (Pquery & pquery)")
    );

    boost::python::def(
        "merge_duplicate_names",
        ( void ( * )( Sample & ))( &::genesis::placement::merge_duplicate_names ),
        ( boost::python::arg("smp") ),
        get_docstring("void ::genesis::placement::merge_duplicate_names (Sample & smp)")
    );

    boost::python::def(
        "merge_duplicate_placements",
        ( void ( * )( Pquery & ))( &::genesis::placement::merge_duplicate_placements ),
        ( boost::python::arg("pquery") ),
        get_docstring("void ::genesis::placement::merge_duplicate_placements (Pquery & pquery)")
    );

    boost::python::def(
        "merge_duplicate_placements",
        ( void ( * )( Sample & ))( &::genesis::placement::merge_duplicate_placements ),
        ( boost::python::arg("smp") ),
        get_docstring("void ::genesis::placement::merge_duplicate_placements (Sample & smp)")
    );

    boost::python::def(
        "merge_duplicates",
        ( void ( * )( Sample & ))( &::genesis::placement::merge_duplicates ),
        ( boost::python::arg("smp") ),
        get_docstring("void ::genesis::placement::merge_duplicates (Sample & smp)")
    );

    boost::python::def(
        "normalize_weight_ratios",
        ( void ( * )( Pquery & ))( &::genesis::placement::normalize_weight_ratios ),
        ( boost::python::arg("pquery") ),
        get_docstring("void ::genesis::placement::normalize_weight_ratios (Pquery & pquery)")
    );

    boost::python::def(
        "normalize_weight_ratios",
        ( void ( * )( Sample & ))( &::genesis::placement::normalize_weight_ratios ),
        ( boost::python::arg("smp") ),
        get_docstring("void ::genesis::placement::normalize_weight_ratios (Sample & smp)")
    );

    boost::python::def(
        "sort_placements_by_weight",
        ( void ( * )( Pquery & ))( &::genesis::placement::sort_placements_by_weight ),
        ( boost::python::arg("pquery") ),
        get_docstring("void ::genesis::placement::sort_placements_by_weight (Pquery & pquery)")
    );

    boost::python::def(
        "sort_placements_by_weight",
        ( void ( * )( Sample & ))( &::genesis::placement::sort_placements_by_weight ),
        ( boost::python::arg("smp") ),
        get_docstring("void ::genesis::placement::sort_placements_by_weight (Sample & smp)")
    );
}

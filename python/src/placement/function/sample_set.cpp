/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_FUNCTIONS( placement_function_sample_set_export, ::genesis::placement, scope )
{

    scope.def(
        "find_sample",
        ( Sample * ( * )( SampleSet &, std::string const & ))( &::genesis::placement::find_sample ),
            pybind11::arg("sample_set"),
            pybind11::arg("name"),
        get_docstring("Sample * ::genesis::placement::find_sample (SampleSet & sample_set, std::string const & name)")
    );

    scope.def(
        "merge_all",
        ( Sample ( * )( SampleSet const & ))( &::genesis::placement::merge_all ),
            pybind11::arg("sample_set"),
        get_docstring("Sample ::genesis::placement::merge_all (SampleSet const & sample_set)")
    );

    scope.def(
        "find_sample",
        ( Sample const * ( * )( SampleSet const &, std::string const & ))( &::genesis::placement::find_sample ),
            pybind11::arg("sample_set"),
            pybind11::arg("name"),
        get_docstring("Sample const * ::genesis::placement::find_sample (SampleSet const & sample_set, std::string const & name)")
    );

    scope.def(
        "all_identical_trees",
        ( bool ( * )( SampleSet const & ))( &::genesis::placement::all_identical_trees ),
            pybind11::arg("sample_set"),
        get_docstring("bool ::genesis::placement::all_identical_trees (SampleSet const & sample_set)")
    );

    scope.def(
        "total_pquery_count",
        ( size_t ( * )( SampleSet const & ))( &::genesis::placement::total_pquery_count ),
            pybind11::arg("sample_set"),
        get_docstring("size_t ::genesis::placement::total_pquery_count (SampleSet const & sample_set)")
    );

    scope.def(
        "average_branch_length_tree",
        ( tree::Tree ( * )( SampleSet const & ))( &::genesis::placement::average_branch_length_tree ),
            pybind11::arg("sample_set"),
        get_docstring("tree::Tree ::genesis::placement::average_branch_length_tree (SampleSet const & sample_set)")
    );

    scope.def(
        "tree_set",
        ( tree::TreeSet ( * )( SampleSet const & ))( &::genesis::placement::tree_set ),
            pybind11::arg("sample_set"),
        get_docstring("tree::TreeSet ::genesis::placement::tree_set (SampleSet const & sample_set)")
    );

    scope.def(
        "adjust_branch_lengths",
        ( void ( * )( SampleSet &, tree::Tree const & ))( &::genesis::placement::adjust_branch_lengths ),
            pybind11::arg("sample_set"),
            pybind11::arg("source"),
        get_docstring("void ::genesis::placement::adjust_branch_lengths (SampleSet & sample_set, tree::Tree const & source)")
    );

    scope.def(
        "adjust_to_average_branch_lengths",
        ( void ( * )( SampleSet & ))( &::genesis::placement::adjust_to_average_branch_lengths ),
            pybind11::arg("sample_set"),
        get_docstring("void ::genesis::placement::adjust_to_average_branch_lengths (SampleSet & sample_set)")
    );
}

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
            pybind11::arg("sset"),
            pybind11::arg("name")
    );

    scope.def(
        "merge_all",
        ( Sample ( * )( SampleSet const & ))( &::genesis::placement::merge_all ),
            pybind11::arg("sset")
    );

    scope.def(
        "find_sample",
        ( Sample const * ( * )( SampleSet const &, std::string const & ))( &::genesis::placement::find_sample ),
            pybind11::arg("sset"),
            pybind11::arg("name")
    );

    scope.def(
        "all_identical_trees",
        ( bool ( * )( SampleSet const & ))( &::genesis::placement::all_identical_trees ),
            pybind11::arg("sset")
    );

    scope.def(
        "tree_set",
        ( tree::TreeSet ( * )( SampleSet const & ))( &::genesis::placement::tree_set ),
            pybind11::arg("sset")
    );
}

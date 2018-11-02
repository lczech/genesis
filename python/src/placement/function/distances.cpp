/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_FUNCTIONS( placement_function_distances_export, ::genesis::placement, scope )
{

    scope.def(
        "placement_distance",
        ( double ( * )( PqueryPlacement const &, PqueryPlacement const &, utils::Matrix< double > const & ))( &::genesis::placement::placement_distance ),
            pybind11::arg("place_a"),
            pybind11::arg("place_b"),
            pybind11::arg("node_distances"),
        get_docstring("double ::genesis::placement::placement_distance (PqueryPlacement const & place_a, PqueryPlacement const & place_b, utils::Matrix< double > const & node_distances)")
    );

    scope.def(
        "placement_distance",
        ( double ( * )( PqueryPlacement const &, tree::TreeNode const &, utils::Matrix< double > const & ))( &::genesis::placement::placement_distance ),
            pybind11::arg("placement"),
            pybind11::arg("node"),
            pybind11::arg("node_distances"),
        get_docstring("double ::genesis::placement::placement_distance (PqueryPlacement const & placement, tree::TreeNode const & node, utils::Matrix< double > const & node_distances)")
    );

    scope.def(
        "pquery_distance",
        ( double ( * )( Pquery const &, tree::TreeNode const &, utils::Matrix< double > const & ))( &::genesis::placement::pquery_distance ),
            pybind11::arg("pquery"),
            pybind11::arg("node"),
            pybind11::arg("node_distances"),
        get_docstring("double ::genesis::placement::pquery_distance (Pquery const & pquery, tree::TreeNode const & node, utils::Matrix< double > const & node_distances)")
    );

    scope.def(
        "pquery_distance",
        ( double ( * )( Pquery const &, Pquery const &, utils::Matrix< double > const &, bool ))( &::genesis::placement::pquery_distance ),
            pybind11::arg("pquery_a"),
            pybind11::arg("pquery_b"),
            pybind11::arg("node_distances"),
            pybind11::arg("with_pendant_length")=(bool)(false),
        get_docstring("double ::genesis::placement::pquery_distance (Pquery const & pquery_a, Pquery const & pquery_b, utils::Matrix< double > const & node_distances, bool with_pendant_length=false)")
    );

    scope.def(
        "pquery_distance",
        ( double ( * )( PqueryPlain const &, PqueryPlain const &, utils::Matrix< double > const &, bool ))( &::genesis::placement::pquery_distance ),
            pybind11::arg("pquery_a"),
            pybind11::arg("pquery_b"),
            pybind11::arg("node_distances"),
            pybind11::arg("with_pendant_length")=(bool)(false),
        get_docstring("double ::genesis::placement::pquery_distance (PqueryPlain const & pquery_a, PqueryPlain const & pquery_b, utils::Matrix< double > const & node_distances, bool with_pendant_length=false)")
    );

    scope.def(
        "pquery_path_length_distance",
        ( double ( * )( Pquery const &, tree::TreeEdge const &, utils::Matrix< size_t > const & ))( &::genesis::placement::pquery_path_length_distance ),
            pybind11::arg("pquery"),
            pybind11::arg("edge"),
            pybind11::arg("edge_path_lengths"),
        get_docstring("double ::genesis::placement::pquery_path_length_distance (Pquery const & pquery, tree::TreeEdge const & edge, utils::Matrix< size_t > const & edge_path_lengths)")
    );

    scope.def(
        "pquery_path_length_distance",
        ( double ( * )( Pquery const &, Pquery const &, utils::Matrix< size_t > const & ))( &::genesis::placement::pquery_path_length_distance ),
            pybind11::arg("pquery_a"),
            pybind11::arg("pquery_b"),
            pybind11::arg("node_path_lengths"),
        get_docstring("double ::genesis::placement::pquery_path_length_distance (Pquery const & pquery_a, Pquery const & pquery_b, utils::Matrix< size_t > const & node_path_lengths)")
    );

    scope.def(
        "placement_path_length_distance",
        ( size_t ( * )( PqueryPlacement const &, PqueryPlacement const &, utils::Matrix< size_t > const & ))( &::genesis::placement::placement_path_length_distance ),
            pybind11::arg("place_a"),
            pybind11::arg("place_b"),
            pybind11::arg("node_path_lengths")
    );

    scope.def(
        "placement_path_length_distance",
        ( size_t ( * )( PqueryPlacement const &, tree::TreeEdge const &, utils::Matrix< size_t > const & ))( &::genesis::placement::placement_path_length_distance ),
            pybind11::arg("placement"),
            pybind11::arg("edge"),
            pybind11::arg("edge_path_lengths"),
        get_docstring("size_t ::genesis::placement::placement_path_length_distance (PqueryPlacement const & placement, tree::TreeEdge const & edge, utils::Matrix< size_t > const & edge_path_lengths)")
    );
}

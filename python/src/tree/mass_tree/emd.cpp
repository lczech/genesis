/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_FUNCTIONS( tree_mass_tree_emd_export, ::genesis::tree, scope )
{

    scope.def(
        "earth_movers_distance",
        ( double ( * )( MassTree const &, MassTree const &, double ))( &::genesis::tree::earth_movers_distance ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs"),
            pybind11::arg("p")=(double)(1.0),
        get_docstring("double ::genesis::tree::earth_movers_distance (MassTree const & lhs, MassTree const & rhs, double p=1.0)")
    );

    scope.def(
        "earth_movers_distance",
        ( std::pair< double, double > ( * )( MassTree const &, double ))( &::genesis::tree::earth_movers_distance ),
            pybind11::arg("tree"),
            pybind11::arg("p")=(double)(1.0),
        get_docstring("std::pair< double, double > ::genesis::tree::earth_movers_distance (MassTree const & tree, double p=1.0)")
    );

    scope.def(
        "earth_movers_distance",
        ( utils::Matrix< double > ( * )( std::vector< MassTree > const &, double ))( &::genesis::tree::earth_movers_distance ),
            pybind11::arg("trees"),
            pybind11::arg("p")=(double)(1.0),
        get_docstring("utils::Matrix< double > ::genesis::tree::earth_movers_distance (std::vector< MassTree > const & trees, double p=1.0)")
    );
}

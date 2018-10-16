/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::Bipartition, scope )
{

    // -------------------------------------------------------------------
    //     Class Bipartition
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::Bipartition, std::shared_ptr<::genesis::tree::Bipartition> > ( scope, "Bipartition" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< TreeLink const &, utils::Bitvector const & >(),
            pybind11::arg("link"),
            pybind11::arg("leaf_nodes")
        )
        .def(
            pybind11::init< Bipartition const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "bitvector",
            ( utils::Bitvector & ( ::genesis::tree::Bipartition::* )(  ))( &::genesis::tree::Bipartition::bitvector )
        )
        .def(
            "empty",
            ( bool ( ::genesis::tree::Bipartition::* )(  ) const )( &::genesis::tree::Bipartition::empty )
        )
        .def(
            "invert",
            ( void ( ::genesis::tree::Bipartition::* )(  ))( &::genesis::tree::Bipartition::invert )
        )
        .def(
            "leaf_nodes",
            ( utils::Bitvector const & ( ::genesis::tree::Bipartition::* )(  ) const )( &::genesis::tree::Bipartition::leaf_nodes )
        )
        .def(
            "link",
            ( TreeLink const & ( ::genesis::tree::Bipartition::* )(  ) const )( &::genesis::tree::Bipartition::link )
        )
    ;
}

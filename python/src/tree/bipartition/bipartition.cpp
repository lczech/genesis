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
            pybind11::init< size_t >(),
            pybind11::arg("num_leaves")
        )

        // Public Member Functions

        .def(
            "invert",
            ( void ( ::genesis::tree::Bipartition::* )(  ))( &::genesis::tree::Bipartition::invert )
        )
        .def(
            "link",
            ( TreeLink * ( ::genesis::tree::Bipartition::* )(  ))( &::genesis::tree::Bipartition::link )
        )
    ;
}

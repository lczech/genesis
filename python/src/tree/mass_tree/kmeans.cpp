/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::MassTreeKmeans, scope )
{

    // -------------------------------------------------------------------
    //     Class MassTreeKmeans
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::MassTreeKmeans, std::shared_ptr<::genesis::tree::MassTreeKmeans> > ( scope, "MassTreeKmeans" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< MassTreeKmeans const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "accumulate_centroid_masses",
            ( size_t ( ::genesis::tree::MassTreeKmeans::* )(  ) const )( &::genesis::tree::MassTreeKmeans::accumulate_centroid_masses )
        )
        .def(
            "accumulate_centroid_masses",
            ( void ( ::genesis::tree::MassTreeKmeans::* )( size_t ))( &::genesis::tree::MassTreeKmeans::accumulate_centroid_masses ),
            pybind11::arg("value")
        )
    ;
}

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::EuclideanKmeans, scope )
{

    // -------------------------------------------------------------------
    //     Class EuclideanKmeans
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::EuclideanKmeans, std::shared_ptr<::genesis::utils::EuclideanKmeans> > ( scope, "EuclideanKmeans" )
        .def(
            pybind11::init< size_t >(),
            pybind11::arg("dimensions")
        )
        .def(
            pybind11::init< EuclideanKmeans const & >(),
            pybind11::arg("arg")
        )
    ;
}

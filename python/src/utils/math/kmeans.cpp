/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::KmeansClusteringInfo, scope )
{

    // -------------------------------------------------------------------
    //     Class KmeansClusteringInfo
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::KmeansClusteringInfo, std::shared_ptr<::genesis::utils::KmeansClusteringInfo> > ( scope, "KmeansClusteringInfo" )
    ;
}

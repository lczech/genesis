/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_CLASS( ::genesis::placement::PqueryPlain, scope )
{

    // -------------------------------------------------------------------
    //     Class PqueryPlain
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::placement::PqueryPlain, std::shared_ptr<::genesis::placement::PqueryPlain> > ( scope, "PqueryPlain" )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::placement::PqueryPlacementPlain, scope )
{

    // -------------------------------------------------------------------
    //     Class PqueryPlacementPlain
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::placement::PqueryPlacementPlain, std::shared_ptr<::genesis::placement::PqueryPlacementPlain> > ( scope, "PqueryPlacementPlain" )
    ;
}

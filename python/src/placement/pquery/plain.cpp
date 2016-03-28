/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_CLASS (PqueryPlain, "placement")
{

    // -------------------------------------------------------------------
    //     Class PqueryPlain
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::placement::PqueryPlain > ( "PqueryPlain" )
    ;
}

PYTHON_EXPORT_CLASS (PqueryPlacementPlain, "placement")
{

    // -------------------------------------------------------------------
    //     Class PqueryPlacementPlain
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::placement::PqueryPlacementPlain > ( "PqueryPlacementPlain" )
    ;
}

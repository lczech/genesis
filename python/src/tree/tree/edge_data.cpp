/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

/*
PYTHON_EXPORT_CLASS( ::genesis::tree::BaseEdgeData, scope )
{

    // -------------------------------------------------------------------
    //     Class BaseEdgeData
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::BaseEdgeData, std::shared_ptr<::genesis::tree::BaseEdgeData> > ( scope, "BaseEdgeData" )

        // Public Member Functions

        .def(
            "clone",
            ( std::unique_ptr< BaseEdgeData > ( ::genesis::tree::BaseEdgeData::* )(  ) const )( &::genesis::tree::BaseEdgeData::clone ),
            get_docstring("std::unique_ptr< BaseEdgeData > ::genesis::tree::BaseEdgeData::clone () const")
        )
        .def_static(
            "create",
            ( std::unique_ptr< BaseEdgeData > ( * )(  ))( &::genesis::tree::BaseEdgeData::create ),
            get_docstring("static std::unique_ptr< BaseEdgeData > ::genesis::tree::BaseEdgeData::create ()")
        )
    ;
}
*/

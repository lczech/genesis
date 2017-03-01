/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::RectangularEdgeData, scope )
{

    // -------------------------------------------------------------------
    //     Class RectangularEdgeData
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::RectangularEdgeData, std::shared_ptr<::genesis::tree::RectangularEdgeData> > ( scope, "RectangularEdgeData" )

        // Public Member Functions

        .def(
            "clone",
            ( std::unique_ptr< BaseEdgeData > ( ::genesis::tree::RectangularEdgeData::* )(  ) const )( &::genesis::tree::RectangularEdgeData::clone ),
            get_docstring("std::unique_ptr< BaseEdgeData > ::genesis::tree::RectangularEdgeData::clone () const")
        )
        .def_static(
            "create",
            ( std::unique_ptr< RectangularEdgeData > ( * )(  ))( &::genesis::tree::RectangularEdgeData::create )
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::tree::RectangularNodeData, scope )
{

    // -------------------------------------------------------------------
    //     Class RectangularNodeData
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::RectangularNodeData, std::shared_ptr<::genesis::tree::RectangularNodeData> > ( scope, "RectangularNodeData" )

        // Public Member Functions

        .def(
            "clone",
            ( std::unique_ptr< BaseNodeData > ( ::genesis::tree::RectangularNodeData::* )(  ) const )( &::genesis::tree::RectangularNodeData::clone ),
            get_docstring("std::unique_ptr< BaseNodeData > ::genesis::tree::RectangularNodeData::clone () const")
        )
        .def_static(
            "create",
            ( std::unique_ptr< RectangularNodeData > ( * )(  ))( &::genesis::tree::RectangularNodeData::create )
        )
    ;
}

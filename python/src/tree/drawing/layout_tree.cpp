/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::LayoutNodeData, scope )
{

    // -------------------------------------------------------------------
    //     Class LayoutNodeData
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::LayoutNodeData, std::shared_ptr<::genesis::tree::LayoutNodeData> > ( scope, "LayoutNodeData" )

        // Public Member Functions

        .def(
            "clone",
            ( std::unique_ptr< BaseNodeData > ( ::genesis::tree::LayoutNodeData::* )(  ) const )( &::genesis::tree::LayoutNodeData::clone ),
            get_docstring("std::unique_ptr< BaseNodeData > ::genesis::tree::LayoutNodeData::clone () const")
        )
        .def_static(
            "create",
            ( std::unique_ptr< LayoutNodeData > ( * )(  ))( &::genesis::tree::LayoutNodeData::create )
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::tree::LayoutEdgeData, scope )
{

    // -------------------------------------------------------------------
    //     Class LayoutEdgeData
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::LayoutEdgeData, std::shared_ptr<::genesis::tree::LayoutEdgeData> > ( scope, "LayoutEdgeData" )

        // Public Member Functions

        .def(
            "clone",
            ( std::unique_ptr< BaseEdgeData > ( ::genesis::tree::LayoutEdgeData::* )(  ) const )( &::genesis::tree::LayoutEdgeData::clone ),
            get_docstring("std::unique_ptr< BaseEdgeData > ::genesis::tree::LayoutEdgeData::clone () const")
        )
        .def_static(
            "create",
            ( std::unique_ptr< LayoutEdgeData > ( * )(  ))( &::genesis::tree::LayoutEdgeData::create )
        )
    ;
}

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;
/*
PYTHON_EXPORT_CLASS( ::genesis::placement::PlacementNodeData, scope )
{

    // -------------------------------------------------------------------
    //     Class PlacementNodeData
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::placement::PlacementNodeData, std::shared_ptr<::genesis::placement::PlacementNodeData> > ( scope, "PlacementNodeData" )

        // Public Member Functions

        .def(
            "clone",
            ( std::unique_ptr< BaseNodeData > ( ::genesis::placement::PlacementNodeData::* )(  ) const )( &::genesis::placement::PlacementNodeData::clone ),
            get_docstring("std::unique_ptr< BaseNodeData > ::genesis::placement::PlacementNodeData::clone () const")
        )
        .def_static(
            "create",
            ( std::unique_ptr< PlacementNodeData > ( * )(  ))( &::genesis::placement::PlacementNodeData::create )
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::placement::PlacementEdgeData, scope )
{

    // -------------------------------------------------------------------
    //     Class PlacementEdgeData
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::placement::PlacementEdgeData, std::shared_ptr<::genesis::placement::PlacementEdgeData> > ( scope, "PlacementEdgeData" )

        // Public Member Functions

        .def(
            "clone",
            ( std::unique_ptr< BaseEdgeData > ( ::genesis::placement::PlacementEdgeData::* )(  ) const )( &::genesis::placement::PlacementEdgeData::clone ),
            get_docstring("std::unique_ptr< BaseEdgeData > ::genesis::placement::PlacementEdgeData::clone () const")
        )
        .def(
            "edge_num",
            ( int ( ::genesis::placement::PlacementEdgeData::* )(  ) const )( &::genesis::placement::PlacementEdgeData::edge_num )
        )
        .def(
            "reset_edge_num",
            ( void ( ::genesis::placement::PlacementEdgeData::* )( int ))( &::genesis::placement::PlacementEdgeData::reset_edge_num ),
            pybind11::arg("val")
        )
        .def_static(
            "create",
            ( std::unique_ptr< PlacementEdgeData > ( * )(  ))( &::genesis::placement::PlacementEdgeData::create )
        )
    ;
}
*/

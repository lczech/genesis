/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::CommonNodeData, scope )
{

    // -------------------------------------------------------------------
    //     Class CommonNodeData
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::CommonNodeData, std::shared_ptr<::genesis::tree::CommonNodeData> > ( scope, "CommonNodeData" )

        // Public Member Functions

        .def(
            "clone",
            ( std::unique_ptr< BaseNodeData > ( ::genesis::tree::CommonNodeData::* )(  ) const )( &::genesis::tree::CommonNodeData::clone ),
            get_docstring("std::unique_ptr< BaseNodeData > ::genesis::tree::CommonNodeData::clone () const")
        )
        .def(
            "recreate",
            ( std::unique_ptr< BaseNodeData > ( ::genesis::tree::CommonNodeData::* )(  ) const )( &::genesis::tree::CommonNodeData::recreate ),
            get_docstring("std::unique_ptr< BaseNodeData > ::genesis::tree::CommonNodeData::recreate () const")
        )
        .def_static(
            "create",
            ( std::unique_ptr< CommonNodeData > ( * )(  ))( &::genesis::tree::CommonNodeData::create )
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::tree::CommonEdgeData, scope )
{

    // -------------------------------------------------------------------
    //     Class CommonEdgeData
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::CommonEdgeData, std::shared_ptr<::genesis::tree::CommonEdgeData> > ( scope, "CommonEdgeData" )

        // Public Member Functions

        .def(
            "clone",
            ( std::unique_ptr< BaseEdgeData > ( ::genesis::tree::CommonEdgeData::* )(  ) const )( &::genesis::tree::CommonEdgeData::clone ),
            get_docstring("std::unique_ptr< BaseEdgeData > ::genesis::tree::CommonEdgeData::clone () const")
        )
        .def(
            "recreate",
            ( std::unique_ptr< BaseEdgeData > ( ::genesis::tree::CommonEdgeData::* )(  ) const )( &::genesis::tree::CommonEdgeData::recreate ),
            get_docstring("std::unique_ptr< BaseEdgeData > ::genesis::tree::CommonEdgeData::recreate () const")
        )
        .def_static(
            "create",
            ( std::unique_ptr< CommonEdgeData > ( * )(  ))( &::genesis::tree::CommonEdgeData::create )
        )
    ;
}

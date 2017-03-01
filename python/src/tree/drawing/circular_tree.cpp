/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::CircularEdgeData, scope )
{

    // -------------------------------------------------------------------
    //     Class CircularEdgeData
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::CircularEdgeData, std::shared_ptr<::genesis::tree::CircularEdgeData> > ( scope, "CircularEdgeData" )

        // Public Member Functions

        .def(
            "clone",
            ( std::unique_ptr< BaseEdgeData > ( ::genesis::tree::CircularEdgeData::* )(  ) const )( &::genesis::tree::CircularEdgeData::clone ),
            get_docstring("std::unique_ptr< BaseEdgeData > ::genesis::tree::CircularEdgeData::clone () const")
        )
        .def_static(
            "create",
            ( std::unique_ptr< CircularEdgeData > ( * )(  ))( &::genesis::tree::CircularEdgeData::create )
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::tree::CircularNodeData, scope )
{

    // -------------------------------------------------------------------
    //     Class CircularNodeData
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::CircularNodeData, std::shared_ptr<::genesis::tree::CircularNodeData> > ( scope, "CircularNodeData" )

        // Public Member Functions

        .def(
            "clone",
            ( std::unique_ptr< BaseNodeData > ( ::genesis::tree::CircularNodeData::* )(  ) const )( &::genesis::tree::CircularNodeData::clone ),
            get_docstring("std::unique_ptr< BaseNodeData > ::genesis::tree::CircularNodeData::clone () const")
        )
        .def_static(
            "create",
            ( std::unique_ptr< CircularNodeData > ( * )(  ))( &::genesis::tree::CircularNodeData::create )
        )
    ;
}

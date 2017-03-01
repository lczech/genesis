/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::DefaultNodeData, scope )
{

    // -------------------------------------------------------------------
    //     Class DefaultNodeData
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::DefaultNodeData, std::shared_ptr<::genesis::tree::DefaultNodeData> > ( scope, "DefaultNodeData" )

        // Public Member Functions

        .def(
            "clone",
            ( std::unique_ptr< BaseNodeData > ( ::genesis::tree::DefaultNodeData::* )(  ) const )( &::genesis::tree::DefaultNodeData::clone ),
            get_docstring("std::unique_ptr< BaseNodeData > ::genesis::tree::DefaultNodeData::clone () const")
        )
        .def_static(
            "create",
            ( std::unique_ptr< DefaultNodeData > ( * )(  ))( &::genesis::tree::DefaultNodeData::create )
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::tree::DefaultEdgeData, scope )
{

    // -------------------------------------------------------------------
    //     Class DefaultEdgeData
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::DefaultEdgeData, std::shared_ptr<::genesis::tree::DefaultEdgeData> > ( scope, "DefaultEdgeData" )

        // Public Member Functions

        .def(
            "clone",
            ( std::unique_ptr< BaseEdgeData > ( ::genesis::tree::DefaultEdgeData::* )(  ) const )( &::genesis::tree::DefaultEdgeData::clone ),
            get_docstring("std::unique_ptr< BaseEdgeData > ::genesis::tree::DefaultEdgeData::clone () const")
        )
        .def_static(
            "create",
            ( std::unique_ptr< DefaultEdgeData > ( * )(  ))( &::genesis::tree::DefaultEdgeData::create )
        )
    ;
}

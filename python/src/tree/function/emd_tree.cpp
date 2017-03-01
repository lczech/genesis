/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::EmdNodeData, scope )
{

    // -------------------------------------------------------------------
    //     Class EmdNodeData
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::EmdNodeData, std::shared_ptr<::genesis::tree::EmdNodeData> > ( scope, "EmdNodeData" )

        // Public Member Functions

        .def(
            "clone",
            ( std::unique_ptr< BaseNodeData > ( ::genesis::tree::EmdNodeData::* )(  ) const )( &::genesis::tree::EmdNodeData::clone ),
            get_docstring("std::unique_ptr< BaseNodeData > ::genesis::tree::EmdNodeData::clone () const")
        )
        .def_static(
            "create",
            ( std::unique_ptr< EmdNodeData > ( * )(  ))( &::genesis::tree::EmdNodeData::create )
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::tree::EmdEdgeData, scope )
{

    // -------------------------------------------------------------------
    //     Class EmdEdgeData
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::EmdEdgeData, std::shared_ptr<::genesis::tree::EmdEdgeData> > ( scope, "EmdEdgeData" )

        // Public Member Functions

        .def(
            "clone",
            ( std::unique_ptr< BaseEdgeData > ( ::genesis::tree::EmdEdgeData::* )(  ) const )( &::genesis::tree::EmdEdgeData::clone ),
            get_docstring("std::unique_ptr< BaseEdgeData > ::genesis::tree::EmdEdgeData::clone () const")
        )
        .def_static(
            "create",
            ( std::unique_ptr< EmdEdgeData > ( * )(  ))( &::genesis::tree::EmdEdgeData::create )
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS( tree_function_emd_tree_export, ::genesis::tree, scope )
{

    scope.def(
        "convert_default_tree_to_emd_tree",
        ( EmdTree ( * )( DefaultTree const & ))( &::genesis::tree::convert_default_tree_to_emd_tree ),
            pybind11::arg("source"),
        get_docstring("EmdTree ::genesis::tree::convert_default_tree_to_emd_tree (DefaultTree const & source)")
    );
}

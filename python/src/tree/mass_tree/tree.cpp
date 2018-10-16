/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::MassTreeNodeData, scope )
{

    // -------------------------------------------------------------------
    //     Class MassTreeNodeData
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::MassTreeNodeData, std::shared_ptr<::genesis::tree::MassTreeNodeData> > ( scope, "MassTreeNodeData" )

        // Public Member Functions

        .def(
            "clone",
            ( std::unique_ptr< BaseNodeData > ( ::genesis::tree::MassTreeNodeData::* )(  ) const )( &::genesis::tree::MassTreeNodeData::clone ),
            get_docstring("std::unique_ptr< BaseNodeData > ::genesis::tree::MassTreeNodeData::clone () const")
        )
        .def(
            "recreate",
            ( std::unique_ptr< BaseNodeData > ( ::genesis::tree::MassTreeNodeData::* )(  ) const )( &::genesis::tree::MassTreeNodeData::recreate ),
            get_docstring("std::unique_ptr< BaseNodeData > ::genesis::tree::MassTreeNodeData::recreate () const")
        )
        .def_static(
            "create",
            ( std::unique_ptr< MassTreeNodeData > ( * )(  ))( &::genesis::tree::MassTreeNodeData::create )
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::tree::MassTreeEdgeData, scope )
{

    // -------------------------------------------------------------------
    //     Class MassTreeEdgeData
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::MassTreeEdgeData, std::shared_ptr<::genesis::tree::MassTreeEdgeData> > ( scope, "MassTreeEdgeData" )

        // Public Member Functions

        .def(
            "clone",
            ( std::unique_ptr< BaseEdgeData > ( ::genesis::tree::MassTreeEdgeData::* )(  ) const )( &::genesis::tree::MassTreeEdgeData::clone ),
            get_docstring("std::unique_ptr< BaseEdgeData > ::genesis::tree::MassTreeEdgeData::clone () const")
        )
        .def(
            "recreate",
            ( std::unique_ptr< BaseEdgeData > ( ::genesis::tree::MassTreeEdgeData::* )(  ) const )( &::genesis::tree::MassTreeEdgeData::recreate ),
            get_docstring("std::unique_ptr< BaseEdgeData > ::genesis::tree::MassTreeEdgeData::recreate () const")
        )
        .def_static(
            "create",
            ( std::unique_ptr< MassTreeEdgeData > ( * )(  ))( &::genesis::tree::MassTreeEdgeData::create )
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS( tree_mass_tree_tree_export, ::genesis::tree, scope )
{

    scope.def(
        "convert_common_tree_to_mass_tree",
        ( MassTree ( * )( CommonTree const & ))( &::genesis::tree::convert_common_tree_to_mass_tree ),
            pybind11::arg("source"),
        get_docstring("MassTree ::genesis::tree::convert_common_tree_to_mass_tree (CommonTree const & source)")
    );
}

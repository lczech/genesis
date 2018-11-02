/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::AttributeTreeNodeData, scope )
{

    // -------------------------------------------------------------------
    //     Class AttributeTreeNodeData
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::AttributeTreeNodeData, std::shared_ptr<::genesis::tree::AttributeTreeNodeData> > ( scope, "AttributeTreeNodeData" )

        // Public Member Functions

        .def(
            "clone",
            ( std::unique_ptr< BaseNodeData > ( ::genesis::tree::AttributeTreeNodeData::* )(  ) const )( &::genesis::tree::AttributeTreeNodeData::clone ),
            get_docstring("std::unique_ptr< BaseNodeData > ::genesis::tree::AttributeTreeNodeData::clone () const")
        )
        .def(
            "recreate",
            ( std::unique_ptr< BaseNodeData > ( ::genesis::tree::AttributeTreeNodeData::* )(  ) const )( &::genesis::tree::AttributeTreeNodeData::recreate ),
            get_docstring("std::unique_ptr< BaseNodeData > ::genesis::tree::AttributeTreeNodeData::recreate () const")
        )
        .def_static(
            "create",
            ( std::unique_ptr< AttributeTreeNodeData > ( * )(  ))( &::genesis::tree::AttributeTreeNodeData::create )
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::tree::AttributeTreeEdgeData, scope )
{

    // -------------------------------------------------------------------
    //     Class AttributeTreeEdgeData
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::AttributeTreeEdgeData, std::shared_ptr<::genesis::tree::AttributeTreeEdgeData> > ( scope, "AttributeTreeEdgeData" )

        // Public Member Functions

        .def(
            "clone",
            ( std::unique_ptr< BaseEdgeData > ( ::genesis::tree::AttributeTreeEdgeData::* )(  ) const )( &::genesis::tree::AttributeTreeEdgeData::clone ),
            get_docstring("std::unique_ptr< BaseEdgeData > ::genesis::tree::AttributeTreeEdgeData::clone () const")
        )
        .def(
            "recreate",
            ( std::unique_ptr< BaseEdgeData > ( ::genesis::tree::AttributeTreeEdgeData::* )(  ) const )( &::genesis::tree::AttributeTreeEdgeData::recreate ),
            get_docstring("std::unique_ptr< BaseEdgeData > ::genesis::tree::AttributeTreeEdgeData::recreate () const")
        )
        .def_static(
            "create",
            ( std::unique_ptr< AttributeTreeEdgeData > ( * )(  ))( &::genesis::tree::AttributeTreeEdgeData::create )
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS( tree_attribute_tree_tree_export, ::genesis::tree, scope )
{

    scope.def(
        "convert_common_tree_to_attribute_tree",
        ( AttributeTree ( * )( CommonTree const & ))( &::genesis::tree::convert_common_tree_to_attribute_tree ),
            pybind11::arg("source"),
        get_docstring("AttributeTree ::genesis::tree::convert_common_tree_to_attribute_tree (CommonTree const & source)")
    );
}

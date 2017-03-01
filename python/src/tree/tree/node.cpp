/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::TreeNode, scope )
{

    // -------------------------------------------------------------------
    //     Class TreeNode
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::TreeNode, std::shared_ptr<::genesis::tree::TreeNode> > ( scope, "TreeNode" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< size_t, TreeLink * >(),
            pybind11::arg("index"),
            pybind11::arg("primary_link")
        )
        // .def(
        //     pybind11::init< TreeNode const & >(),
        //     pybind11::arg("arg")
        // )

        // Public Member Functions

        // .def(
        //     "data",
        //     ( NodeDataType & ( ::genesis::tree::TreeNode::* )(  ))( &::genesis::tree::TreeNode::data )
        // )
        // .def(
        //     "data",
        //     ( NodeDataType const & ( ::genesis::tree::TreeNode::* )(  ) const )( &::genesis::tree::TreeNode::data )
        // )
        // .def(
        //     "data_cast",
        //     ( NodeDataType * ( ::genesis::tree::TreeNode::* )(  ))( &::genesis::tree::TreeNode::data_cast )
        // )
        // .def(
        //     "data_cast",
        //     ( NodeDataType const * ( ::genesis::tree::TreeNode::* )(  ) const )( &::genesis::tree::TreeNode::data_cast )
        // )
        // .def(
        //     "data_ptr",
        //     ( BaseNodeData * ( ::genesis::tree::TreeNode::* )(  ))( &::genesis::tree::TreeNode::data_ptr )
        // )
        // .def(
        //     "data_ptr",
        //     ( BaseNodeData const * ( ::genesis::tree::TreeNode::* )(  ) const )( &::genesis::tree::TreeNode::data_ptr )
        // )
        .def(
            "dump",
            ( std::string ( ::genesis::tree::TreeNode::* )(  ) const )( &::genesis::tree::TreeNode::dump )
        )
        .def(
            "has_data",
            ( bool ( ::genesis::tree::TreeNode::* )(  ) const )( &::genesis::tree::TreeNode::has_data )
        )
        .def(
            "index",
            ( size_t ( ::genesis::tree::TreeNode::* )(  ) const )( &::genesis::tree::TreeNode::index )
        )
        .def(
            "is_inner",
            ( bool ( ::genesis::tree::TreeNode::* )(  ) const )( &::genesis::tree::TreeNode::is_inner )
        )
        .def(
            "is_leaf",
            ( bool ( ::genesis::tree::TreeNode::* )(  ) const )( &::genesis::tree::TreeNode::is_leaf )
        )
        .def(
            "is_root",
            ( bool ( ::genesis::tree::TreeNode::* )(  ) const )( &::genesis::tree::TreeNode::is_root )
        )
        .def(
            "link",
            ( TreeLink & ( ::genesis::tree::TreeNode::* )(  ))( &::genesis::tree::TreeNode::link )
        )
        .def(
            "link",
            ( TreeLink const & ( ::genesis::tree::TreeNode::* )(  ) const )( &::genesis::tree::TreeNode::link )
        )
        .def(
            "primary_link",
            ( TreeLink & ( ::genesis::tree::TreeNode::* )(  ))( &::genesis::tree::TreeNode::primary_link )
        )
        .def(
            "primary_link",
            ( TreeLink const & ( ::genesis::tree::TreeNode::* )(  ) const )( &::genesis::tree::TreeNode::primary_link )
        )
        .def(
            "rank",
            ( size_t ( ::genesis::tree::TreeNode::* )(  ) const )( &::genesis::tree::TreeNode::rank )
        )
        // .def(
        //     "reset_data",
        //     ( TreeNode & ( ::genesis::tree::TreeNode::* )( std::unique_ptr< BaseNodeData > ))( &::genesis::tree::TreeNode::reset_data ),
        //     pybind11::arg("data")
        // )
        .def(
            "reset_index",
            ( TreeNode & ( ::genesis::tree::TreeNode::* )( size_t ))( &::genesis::tree::TreeNode::reset_index ),
            pybind11::arg("val")
        )
        .def(
            "reset_primary_link",
            ( TreeNode & ( ::genesis::tree::TreeNode::* )( TreeLink * ))( &::genesis::tree::TreeNode::reset_primary_link ),
            pybind11::arg("val")
        )
    ;
}

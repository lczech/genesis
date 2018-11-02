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
        .def(
            "data_ptr",
            ( BaseNodeData * ( ::genesis::tree::TreeNode::* )(  ))( &::genesis::tree::TreeNode::data_ptr ),
            get_docstring("BaseNodeData * ::genesis::tree::TreeNode::data_ptr ()")
        )
        .def(
            "data_ptr",
            ( BaseNodeData const * ( ::genesis::tree::TreeNode::* )(  ) const )( &::genesis::tree::TreeNode::data_ptr ),
            get_docstring("BaseNodeData const * ::genesis::tree::TreeNode::data_ptr () const")
        )
        .def(
            "has_data",
            ( bool ( ::genesis::tree::TreeNode::* )(  ) const )( &::genesis::tree::TreeNode::has_data ),
            get_docstring("bool ::genesis::tree::TreeNode::has_data () const")
        )
        .def(
            "index",
            ( size_t ( ::genesis::tree::TreeNode::* )(  ) const )( &::genesis::tree::TreeNode::index ),
            get_docstring("size_t ::genesis::tree::TreeNode::index () const")
        )
        .def(
            "link",
            ( TreeLink & ( ::genesis::tree::TreeNode::* )(  ))( &::genesis::tree::TreeNode::link ),
            get_docstring("TreeLink & ::genesis::tree::TreeNode::link ()")
        )
        .def(
            "link",
            ( TreeLink const & ( ::genesis::tree::TreeNode::* )(  ) const )( &::genesis::tree::TreeNode::link ),
            get_docstring("TreeLink const & ::genesis::tree::TreeNode::link () const")
        )
        .def(
            "primary_link",
            ( TreeLink & ( ::genesis::tree::TreeNode::* )(  ))( &::genesis::tree::TreeNode::primary_link ),
            get_docstring("TreeLink & ::genesis::tree::TreeNode::primary_link ()")
        )
        .def(
            "primary_link",
            ( TreeLink const & ( ::genesis::tree::TreeNode::* )(  ) const )( &::genesis::tree::TreeNode::primary_link ),
            get_docstring("TreeLink const & ::genesis::tree::TreeNode::primary_link () const")
        )
        // .def(
        //     "reset_data",
        //     ( TreeNode & ( ::genesis::tree::TreeNode::* )( std::unique_ptr< BaseNodeData > ))( &::genesis::tree::TreeNode::reset_data ),
        //     pybind11::arg("data"),
        //     get_docstring("TreeNode & ::genesis::tree::TreeNode::reset_data (std::unique_ptr< BaseNodeData > data)")
        // )
        .def(
            "reset_index",
            ( TreeNode & ( ::genesis::tree::TreeNode::* )( size_t ))( &::genesis::tree::TreeNode::reset_index ),
            pybind11::arg("val"),
            get_docstring("TreeNode & ::genesis::tree::TreeNode::reset_index (size_t val)")
        )
        .def(
            "reset_primary_link",
            ( TreeNode & ( ::genesis::tree::TreeNode::* )( TreeLink * ))( &::genesis::tree::TreeNode::reset_primary_link ),
            pybind11::arg("val"),
            get_docstring("TreeNode & ::genesis::tree::TreeNode::reset_primary_link (TreeLink * val)")
        )

        // Operators

        .def(
            "__str__",
            []( ::genesis::tree::TreeNode const& obj ) -> std::string {
                std::ostringstream s;
                s << obj;
                return s.str();
            }
        )
    ;
}

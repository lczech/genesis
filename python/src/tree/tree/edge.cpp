/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::TreeEdge, scope )
{

    // -------------------------------------------------------------------
    //     Class TreeEdge
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::TreeEdge, std::shared_ptr<::genesis::tree::TreeEdge> > ( scope, "TreeEdge" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< size_t, TreeLink *, TreeLink * >(),
            pybind11::arg("index"),
            pybind11::arg("primary_link"),
            pybind11::arg("secondary_link")
        )
        // .def(
        //     pybind11::init< TreeEdge const & >(),
        //     pybind11::arg("arg")
        // )

        // Public Member Functions

        // .def(
        //     "data",
        //     ( EdgeDataType & ( ::genesis::tree::TreeEdge::* )(  ))( &::genesis::tree::TreeEdge::data )
        // )
        // .def(
        //     "data",
        //     ( EdgeDataType const & ( ::genesis::tree::TreeEdge::* )(  ) const )( &::genesis::tree::TreeEdge::data )
        // )
        // .def(
        //     "data_cast",
        //     ( EdgeDataType * ( ::genesis::tree::TreeEdge::* )(  ))( &::genesis::tree::TreeEdge::data_cast )
        // )
        // .def(
        //     "data_cast",
        //     ( EdgeDataType const * ( ::genesis::tree::TreeEdge::* )(  ) const )( &::genesis::tree::TreeEdge::data_cast )
        // )
        // .def(
        //     "data_ptr",
        //     ( BaseEdgeData * ( ::genesis::tree::TreeEdge::* )(  ))( &::genesis::tree::TreeEdge::data_ptr )
        // )
        // .def(
        //     "data_ptr",
        //     ( BaseEdgeData const * ( ::genesis::tree::TreeEdge::* )(  ) const )( &::genesis::tree::TreeEdge::data_ptr )
        // )
        .def(
            "dump",
            ( std::string ( ::genesis::tree::TreeEdge::* )(  ) const )( &::genesis::tree::TreeEdge::dump )
        )
        .def(
            "has_data",
            ( bool ( ::genesis::tree::TreeEdge::* )(  ) const )( &::genesis::tree::TreeEdge::has_data )
        )
        .def(
            "index",
            ( size_t ( ::genesis::tree::TreeEdge::* )(  ) const )( &::genesis::tree::TreeEdge::index )
        )
        .def(
            "primary_link",
            ( TreeLink & ( ::genesis::tree::TreeEdge::* )(  ))( &::genesis::tree::TreeEdge::primary_link )
        )
        .def(
            "primary_link",
            ( TreeLink const & ( ::genesis::tree::TreeEdge::* )(  ) const )( &::genesis::tree::TreeEdge::primary_link )
        )
        .def(
            "primary_node",
            ( TreeNode & ( ::genesis::tree::TreeEdge::* )(  ))( &::genesis::tree::TreeEdge::primary_node )
        )
        .def(
            "primary_node",
            ( TreeNode const & ( ::genesis::tree::TreeEdge::* )(  ) const )( &::genesis::tree::TreeEdge::primary_node )
        )
        // .def(
        //     "reset_data",
        //     ( TreeEdge & ( ::genesis::tree::TreeEdge::* )( std::unique_ptr< BaseEdgeData > ))( &::genesis::tree::TreeEdge::reset_data ),
        //     pybind11::arg("data")
        // )
        .def(
            "reset_index",
            ( TreeEdge & ( ::genesis::tree::TreeEdge::* )( size_t ))( &::genesis::tree::TreeEdge::reset_index ),
            pybind11::arg("val")
        )
        .def(
            "reset_primary_link",
            ( TreeEdge & ( ::genesis::tree::TreeEdge::* )( TreeLink * ))( &::genesis::tree::TreeEdge::reset_primary_link ),
            pybind11::arg("val")
        )
        .def(
            "reset_secondary_link",
            ( TreeEdge & ( ::genesis::tree::TreeEdge::* )( TreeLink * ))( &::genesis::tree::TreeEdge::reset_secondary_link ),
            pybind11::arg("val")
        )
        .def(
            "secondary_link",
            ( TreeLink & ( ::genesis::tree::TreeEdge::* )(  ))( &::genesis::tree::TreeEdge::secondary_link )
        )
        .def(
            "secondary_link",
            ( TreeLink const & ( ::genesis::tree::TreeEdge::* )(  ) const )( &::genesis::tree::TreeEdge::secondary_link )
        )
        .def(
            "secondary_node",
            ( TreeNode & ( ::genesis::tree::TreeEdge::* )(  ))( &::genesis::tree::TreeEdge::secondary_node )
        )
        .def(
            "secondary_node",
            ( TreeNode const & ( ::genesis::tree::TreeEdge::* )(  ) const )( &::genesis::tree::TreeEdge::secondary_node )
        )
    ;
}

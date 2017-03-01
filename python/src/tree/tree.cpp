/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::Tree, scope )
{

    // -------------------------------------------------------------------
    //     Class Tree
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::Tree, std::shared_ptr<::genesis::tree::Tree> > ( scope, "Tree" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< Tree const & >(),
            pybind11::arg("other")
        )

        // Public Member Functions

        .def(
            "clear",
            ( void ( ::genesis::tree::Tree::* )(  ))( &::genesis::tree::Tree::clear )
        )
        .def(
            "clone_topology",
            ( Tree ( ::genesis::tree::Tree::* )(  ) const )( &::genesis::tree::Tree::clone_topology )
        )
        .def(
            "edge_at",
            ( TreeEdge & ( ::genesis::tree::Tree::* )( size_t ))( &::genesis::tree::Tree::edge_at ),
            pybind11::arg("index")
        )
        .def(
            "edge_at",
            ( TreeEdge const & ( ::genesis::tree::Tree::* )( size_t ) const )( &::genesis::tree::Tree::edge_at ),
            pybind11::arg("index")
        )
        .def(
            "edge_count",
            ( size_t ( ::genesis::tree::Tree::* )(  ) const )( &::genesis::tree::Tree::edge_count )
        )
        // .def(
        //     "edges",
        //     ( utils::Range< ConstIteratorEdges > ( ::genesis::tree::Tree::* )(  ) const )( &::genesis::tree::Tree::edges )
        // )
        // .def(
        //     "edges",
        //     ( utils::Range< IteratorEdges > ( ::genesis::tree::Tree::* )(  ))( &::genesis::tree::Tree::edges )
        // )
        .def(
            "empty",
            ( bool ( ::genesis::tree::Tree::* )(  ) const )( &::genesis::tree::Tree::empty )
        )
        // .def(
        //     "expose_edge_container",
        //     ( EdgeContainerType & ( ::genesis::tree::Tree::* )(  ))( &::genesis::tree::Tree::expose_edge_container )
        // )
        // .def(
        //     "expose_link_container",
        //     ( LinkContainerType & ( ::genesis::tree::Tree::* )(  ))( &::genesis::tree::Tree::expose_link_container )
        // )
        // .def(
        //     "expose_node_container",
        //     ( NodeContainerType & ( ::genesis::tree::Tree::* )(  ))( &::genesis::tree::Tree::expose_node_container )
        // )
        .def(
            "link_at",
            ( TreeLink & ( ::genesis::tree::Tree::* )( size_t ))( &::genesis::tree::Tree::link_at ),
            pybind11::arg("index")
        )
        .def(
            "link_at",
            ( TreeLink const & ( ::genesis::tree::Tree::* )( size_t ) const )( &::genesis::tree::Tree::link_at ),
            pybind11::arg("index")
        )
        .def(
            "link_count",
            ( size_t ( ::genesis::tree::Tree::* )(  ) const )( &::genesis::tree::Tree::link_count )
        )
        // .def(
        //     "links",
        //     ( utils::Range< ConstIteratorLinks > ( ::genesis::tree::Tree::* )(  ) const )( &::genesis::tree::Tree::links )
        // )
        // .def(
        //     "links",
        //     ( utils::Range< IteratorLinks > ( ::genesis::tree::Tree::* )(  ))( &::genesis::tree::Tree::links )
        // )
        .def(
            "node_at",
            ( TreeNode & ( ::genesis::tree::Tree::* )( size_t ))( &::genesis::tree::Tree::node_at ),
            pybind11::arg("index")
        )
        .def(
            "node_at",
            ( TreeNode const & ( ::genesis::tree::Tree::* )( size_t ) const )( &::genesis::tree::Tree::node_at ),
            pybind11::arg("index")
        )
        .def(
            "node_count",
            ( size_t ( ::genesis::tree::Tree::* )(  ) const )( &::genesis::tree::Tree::node_count )
        )
        // .def(
        //     "nodes",
        //     ( utils::Range< ConstIteratorNodes > ( ::genesis::tree::Tree::* )(  ) const )( &::genesis::tree::Tree::nodes )
        // )
        // .def(
        //     "nodes",
        //     ( utils::Range< IteratorNodes > ( ::genesis::tree::Tree::* )(  ))( &::genesis::tree::Tree::nodes )
        // )
        .def(
            "reset_root_link_index",
            ( Tree & ( ::genesis::tree::Tree::* )( size_t ))( &::genesis::tree::Tree::reset_root_link_index ),
            pybind11::arg("val")
        )
        .def(
            "root_link",
            ( TreeLink & ( ::genesis::tree::Tree::* )(  ))( &::genesis::tree::Tree::root_link )
        )
        .def(
            "root_link",
            ( TreeLink const & ( ::genesis::tree::Tree::* )(  ) const )( &::genesis::tree::Tree::root_link )
        )
        .def(
            "root_node",
            ( TreeNode & ( ::genesis::tree::Tree::* )(  ))( &::genesis::tree::Tree::root_node )
        )
        .def(
            "root_node",
            ( TreeNode const & ( ::genesis::tree::Tree::* )(  ) const )( &::genesis::tree::Tree::root_node )
        )
        .def(
            "swap",
            ( void ( ::genesis::tree::Tree::* )( Tree & ))( &::genesis::tree::Tree::swap ),
            pybind11::arg("other")
        )

        // Operators

        .def(
            "__str__",
            []( ::genesis::tree::Tree const& obj ) -> std::string {
                std::ostringstream s;
                s << obj;
                return s.str();
            }
        )

        // Iterators

        // .def(
        //     "edges",
        //     []( ::genesis::tree::Tree& obj ){
        //         return pybind11::make_iterator( obj.begin_edges(), obj.end_edges() );            }
        // )
        // .def(
        //     "links",
        //     []( ::genesis::tree::Tree& obj ){
        //         return pybind11::make_iterator( obj.begin_links(), obj.end_links() );            }
        // )
        // .def(
        //     "nodes",
        //     []( ::genesis::tree::Tree& obj ){
        //         return pybind11::make_iterator( obj.begin_nodes(), obj.end_nodes() );            }
        // )
    ;
}

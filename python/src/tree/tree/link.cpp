/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::TreeLink, scope )
{

    // -------------------------------------------------------------------
    //     Class TreeLink
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::TreeLink, std::shared_ptr<::genesis::tree::TreeLink> > ( scope, "TreeLink" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< size_t, TreeLink *, TreeLink *, TreeNode *, TreeEdge * >(),
            pybind11::arg("index"),
            pybind11::arg("next"),
            pybind11::arg("outer"),
            pybind11::arg("node"),
            pybind11::arg("edge")
        )
        // .def(
        //     pybind11::init< TreeLink const & >(),
        //     pybind11::arg("arg")
        // )

        // Public Member Functions

        .def(
            "edge",
            ( TreeEdge & ( ::genesis::tree::TreeLink::* )(  ))( &::genesis::tree::TreeLink::edge ),
            get_docstring("TreeEdge & ::genesis::tree::TreeLink::edge ()")
        )
        .def(
            "edge",
            ( TreeEdge const & ( ::genesis::tree::TreeLink::* )(  ) const )( &::genesis::tree::TreeLink::edge ),
            get_docstring("TreeEdge const & ::genesis::tree::TreeLink::edge () const")
        )
        .def(
            "index",
            ( size_t ( ::genesis::tree::TreeLink::* )(  ) const )( &::genesis::tree::TreeLink::index ),
            get_docstring("size_t ::genesis::tree::TreeLink::index () const")
        )
        .def(
            "next",
            ( TreeLink & ( ::genesis::tree::TreeLink::* )(  ))( &::genesis::tree::TreeLink::next ),
            get_docstring("TreeLink & ::genesis::tree::TreeLink::next ()")
        )
        .def(
            "next",
            ( TreeLink const & ( ::genesis::tree::TreeLink::* )(  ) const )( &::genesis::tree::TreeLink::next ),
            get_docstring("TreeLink const & ::genesis::tree::TreeLink::next () const")
        )
        .def(
            "node",
            ( TreeNode & ( ::genesis::tree::TreeLink::* )(  ))( &::genesis::tree::TreeLink::node ),
            get_docstring("TreeNode & ::genesis::tree::TreeLink::node ()")
        )
        .def(
            "node",
            ( TreeNode const & ( ::genesis::tree::TreeLink::* )(  ) const )( &::genesis::tree::TreeLink::node ),
            get_docstring("TreeNode const & ::genesis::tree::TreeLink::node () const")
        )
        .def(
            "outer",
            ( TreeLink & ( ::genesis::tree::TreeLink::* )(  ))( &::genesis::tree::TreeLink::outer ),
            get_docstring("TreeLink & ::genesis::tree::TreeLink::outer ()")
        )
        .def(
            "outer",
            ( TreeLink const & ( ::genesis::tree::TreeLink::* )(  ) const )( &::genesis::tree::TreeLink::outer ),
            get_docstring("TreeLink const & ::genesis::tree::TreeLink::outer () const")
        )
        .def(
            "prev",
            ( TreeLink & ( ::genesis::tree::TreeLink::* )(  ))( &::genesis::tree::TreeLink::prev ),
            get_docstring("TreeLink & ::genesis::tree::TreeLink::prev ()")
        )
        .def(
            "prev",
            ( TreeLink const & ( ::genesis::tree::TreeLink::* )(  ) const )( &::genesis::tree::TreeLink::prev ),
            get_docstring("TreeLink const & ::genesis::tree::TreeLink::prev () const")
        )
        .def(
            "reset_edge",
            ( TreeLink & ( ::genesis::tree::TreeLink::* )( TreeEdge * ))( &::genesis::tree::TreeLink::reset_edge ),
            pybind11::arg("val"),
            get_docstring("TreeLink & ::genesis::tree::TreeLink::reset_edge (TreeEdge * val)")
        )
        .def(
            "reset_index",
            ( TreeLink & ( ::genesis::tree::TreeLink::* )( size_t ))( &::genesis::tree::TreeLink::reset_index ),
            pybind11::arg("val"),
            get_docstring("TreeLink & ::genesis::tree::TreeLink::reset_index (size_t val)")
        )
        .def(
            "reset_next",
            ( TreeLink & ( ::genesis::tree::TreeLink::* )( TreeLink * ))( &::genesis::tree::TreeLink::reset_next ),
            pybind11::arg("val"),
            get_docstring("TreeLink & ::genesis::tree::TreeLink::reset_next (TreeLink * val)")
        )
        .def(
            "reset_node",
            ( TreeLink & ( ::genesis::tree::TreeLink::* )( TreeNode * ))( &::genesis::tree::TreeLink::reset_node ),
            pybind11::arg("val"),
            get_docstring("TreeLink & ::genesis::tree::TreeLink::reset_node (TreeNode * val)")
        )
        .def(
            "reset_outer",
            ( TreeLink & ( ::genesis::tree::TreeLink::* )( TreeLink * ))( &::genesis::tree::TreeLink::reset_outer ),
            pybind11::arg("val"),
            get_docstring("TreeLink & ::genesis::tree::TreeLink::reset_outer (TreeLink * val)")
        )

        // Operators

        .def(
            "__str__",
            []( ::genesis::tree::TreeLink const& obj ) -> std::string {
                std::ostringstream s;
                s << obj;
                return s.str();
            }
        )
    ;
}

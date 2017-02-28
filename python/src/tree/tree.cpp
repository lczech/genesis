#include <utility>
#include <genesis/tree/function/operators.hpp>
#include <genesis/tree/tree.hpp>
#include <genesis/tree/tree/edge.hpp>
#include <genesis/tree/tree/edge_data.hpp>
#include <genesis/tree/tree/link.hpp>
#include <genesis/tree/tree/node.hpp>
#include <genesis/tree/tree/node_data.hpp>
#include <genesis/utils/core/range.hpp>
#include <initializer_list>
#include <ios>
#include <iterator>
#include <locale>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
#include <string>
#include <vector>

#include <src/common.hpp>

// #include <pybind11/pybind11.h>
//
// #ifndef BINDER_PYBIND11_TYPE_CASTER
// 	#define BINDER_PYBIND11_TYPE_CASTER
// 	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
// 	PYBIND11_DECLARE_HOLDER_TYPE(T, T*);
// #endif

void bind_genesis_tree_tree(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::tree::Tree file:genesis/tree/tree.hpp line:88
        using namespace ::genesis::tree;
        using namespace ::genesis;

		pybind11::class_<genesis::tree::Tree, std::shared_ptr<genesis::tree::Tree>> cl(M("genesis::tree"), "Tree", "Class for representing phylogenetic trees.");

        cl
        // .def( pybind11::init< Tree const & >(( pybind11::arg("other") )) )
        .def(pybind11::init<>())

        // Public Member Functions

        .def(
            "clear",
            ( void ( ::genesis::tree::Tree::* )(  ))( &::genesis::tree::Tree::clear ),
            get_docstring("void ::genesis::tree::Tree::clear ()")
        )
        .def(
            "clone_topology",
            ( Tree ( ::genesis::tree::Tree::* )(  ) const )( &::genesis::tree::Tree::clone_topology ),
            get_docstring("Tree ::genesis::tree::Tree::clone_topology () const")
        )
        .def(
            "edge_at",
            ( TreeEdge & ( ::genesis::tree::Tree::* )( size_t ))( &::genesis::tree::Tree::edge_at ),
            ( pybind11::arg("index") ),
            get_docstring("TreeEdge & ::genesis::tree::Tree::edge_at (size_t index)")
        )
        .def(
            "edge_at",
            ( TreeEdge const & ( ::genesis::tree::Tree::* )( size_t ) const )( &::genesis::tree::Tree::edge_at ),
            ( pybind11::arg("index") ),
            get_docstring("TreeEdge const & ::genesis::tree::Tree::edge_at (size_t index) const")
        )
        .def(
            "edge_count",
            ( size_t ( ::genesis::tree::Tree::* )(  ) const )( &::genesis::tree::Tree::edge_count ),
            get_docstring("size_t ::genesis::tree::Tree::edge_count () const")
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
            ( bool ( ::genesis::tree::Tree::* )(  ) const )( &::genesis::tree::Tree::empty ),
            get_docstring("bool ::genesis::tree::Tree::empty () const")
        );

		cl.def("__str__", [](genesis::tree::Tree const &o) -> std::string { std::ostringstream s; s << o; return s.str(); } );
	}
}

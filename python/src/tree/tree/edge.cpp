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
        .def(
            "data_ptr",
            ( BaseEdgeData * ( ::genesis::tree::TreeEdge::* )(  ))( &::genesis::tree::TreeEdge::data_ptr ),
            get_docstring("BaseEdgeData * ::genesis::tree::TreeEdge::data_ptr ()")
        )
        .def(
            "data_ptr",
            ( BaseEdgeData const * ( ::genesis::tree::TreeEdge::* )(  ) const )( &::genesis::tree::TreeEdge::data_ptr ),
            get_docstring("BaseEdgeData const * ::genesis::tree::TreeEdge::data_ptr () const")
        )
        .def(
            "has_data",
            ( bool ( ::genesis::tree::TreeEdge::* )(  ) const )( &::genesis::tree::TreeEdge::has_data ),
            get_docstring("bool ::genesis::tree::TreeEdge::has_data () const")
        )
        .def(
            "index",
            ( size_t ( ::genesis::tree::TreeEdge::* )(  ) const )( &::genesis::tree::TreeEdge::index ),
            get_docstring("size_t ::genesis::tree::TreeEdge::index () const")
        )
        .def(
            "primary_link",
            ( TreeLink & ( ::genesis::tree::TreeEdge::* )(  ))( &::genesis::tree::TreeEdge::primary_link ),
            get_docstring("TreeLink & ::genesis::tree::TreeEdge::primary_link ()")
        )
        .def(
            "primary_link",
            ( TreeLink const & ( ::genesis::tree::TreeEdge::* )(  ) const )( &::genesis::tree::TreeEdge::primary_link ),
            get_docstring("TreeLink const & ::genesis::tree::TreeEdge::primary_link () const")
        )
        .def(
            "primary_node",
            ( TreeNode & ( ::genesis::tree::TreeEdge::* )(  ))( &::genesis::tree::TreeEdge::primary_node ),
            get_docstring("TreeNode & ::genesis::tree::TreeEdge::primary_node ()")
        )
        .def(
            "primary_node",
            ( TreeNode const & ( ::genesis::tree::TreeEdge::* )(  ) const )( &::genesis::tree::TreeEdge::primary_node ),
            get_docstring("TreeNode const & ::genesis::tree::TreeEdge::primary_node () const")
        )
        // .def(
        //     "reset_data",
        //     ( TreeEdge & ( ::genesis::tree::TreeEdge::* )( std::unique_ptr< BaseEdgeData > ))( &::genesis::tree::TreeEdge::reset_data ),
        //     pybind11::arg("data"),
        //     get_docstring("TreeEdge & ::genesis::tree::TreeEdge::reset_data (std::unique_ptr< BaseEdgeData > data)")
        // )
        .def(
            "reset_index",
            ( TreeEdge & ( ::genesis::tree::TreeEdge::* )( size_t ))( &::genesis::tree::TreeEdge::reset_index ),
            pybind11::arg("val"),
            get_docstring("TreeEdge & ::genesis::tree::TreeEdge::reset_index (size_t val)")
        )
        .def(
            "reset_primary_link",
            ( TreeEdge & ( ::genesis::tree::TreeEdge::* )( TreeLink * ))( &::genesis::tree::TreeEdge::reset_primary_link ),
            pybind11::arg("val"),
            get_docstring("TreeEdge & ::genesis::tree::TreeEdge::reset_primary_link (TreeLink * val)")
        )
        .def(
            "reset_secondary_link",
            ( TreeEdge & ( ::genesis::tree::TreeEdge::* )( TreeLink * ))( &::genesis::tree::TreeEdge::reset_secondary_link ),
            pybind11::arg("val"),
            get_docstring("TreeEdge & ::genesis::tree::TreeEdge::reset_secondary_link (TreeLink * val)")
        )
        .def(
            "secondary_link",
            ( TreeLink & ( ::genesis::tree::TreeEdge::* )(  ))( &::genesis::tree::TreeEdge::secondary_link ),
            get_docstring("TreeLink & ::genesis::tree::TreeEdge::secondary_link ()")
        )
        .def(
            "secondary_link",
            ( TreeLink const & ( ::genesis::tree::TreeEdge::* )(  ) const )( &::genesis::tree::TreeEdge::secondary_link ),
            get_docstring("TreeLink const & ::genesis::tree::TreeEdge::secondary_link () const")
        )
        .def(
            "secondary_node",
            ( TreeNode & ( ::genesis::tree::TreeEdge::* )(  ))( &::genesis::tree::TreeEdge::secondary_node ),
            get_docstring("TreeNode & ::genesis::tree::TreeEdge::secondary_node ()")
        )
        .def(
            "secondary_node",
            ( TreeNode const & ( ::genesis::tree::TreeEdge::* )(  ) const )( &::genesis::tree::TreeEdge::secondary_node ),
            get_docstring("TreeNode const & ::genesis::tree::TreeEdge::secondary_node () const")
        )

        // Operators

        .def(
            "__str__",
            []( ::genesis::tree::TreeEdge const& obj ) -> std::string {
                std::ostringstream s;
                s << obj;
                return s.str();
            }
        )
    ;
}

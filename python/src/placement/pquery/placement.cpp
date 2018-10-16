/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_CLASS( ::genesis::placement::PqueryPlacement, scope )
{

    // -------------------------------------------------------------------
    //     Class PqueryPlacement
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::placement::PqueryPlacement, std::shared_ptr<::genesis::placement::PqueryPlacement> > ( scope, "PqueryPlacement" )
        .def(
            pybind11::init<  >(),
            get_docstring("::genesis::placement::PqueryPlacement::PqueryPlacement ()")
        )
        .def(
            pybind11::init< PlacementTreeEdge & >(),
            pybind11::arg("edge"),
            get_docstring("::genesis::placement::PqueryPlacement::PqueryPlacement (PlacementTreeEdge & edge)")
        )
        .def(
            pybind11::init< PqueryPlacement const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "edge",
            ( PlacementTreeEdge & ( ::genesis::placement::PqueryPlacement::* )(  ))( &::genesis::placement::PqueryPlacement::edge ),
            get_docstring("PlacementTreeEdge & ::genesis::placement::PqueryPlacement::edge ()")
        )
        .def(
            "edge",
            ( const PlacementTreeEdge & ( ::genesis::placement::PqueryPlacement::* )(  ) const )( &::genesis::placement::PqueryPlacement::edge ),
            get_docstring("const PlacementTreeEdge & ::genesis::placement::PqueryPlacement::edge () const")
        )
        .def(
            "edge_num",
            ( int ( ::genesis::placement::PqueryPlacement::* )(  ) const )( &::genesis::placement::PqueryPlacement::edge_num ),
            get_docstring("int ::genesis::placement::PqueryPlacement::edge_num () const")
        )
        .def(
            "reset_edge",
            ( void ( ::genesis::placement::PqueryPlacement::* )( PlacementTreeEdge & ))( &::genesis::placement::PqueryPlacement::reset_edge ),
            pybind11::arg("edge"),
            get_docstring("void ::genesis::placement::PqueryPlacement::reset_edge (PlacementTreeEdge & edge)")
        )
    ;
}

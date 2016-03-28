/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_CLASS (PqueryPlacement, "placement")
{

    // -------------------------------------------------------------------
    //     Class PqueryPlacement
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::placement::PqueryPlacement > ( "PqueryPlacement", boost::python::init<  >(  ) )
        .def( boost::python::init< PlacementTreeEdge & >(( boost::python::arg("edge") )) )
        .def( boost::python::init< PqueryPlacement const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "edge",
            ( PlacementTreeEdge & ( ::genesis::placement::PqueryPlacement::* )(  ))( &::genesis::placement::PqueryPlacement::edge ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("PlacementTreeEdge & ::genesis::placement::PqueryPlacement::edge ()")
        )
        .def(
            "edge",
            ( const PlacementTreeEdge & ( ::genesis::placement::PqueryPlacement::* )(  ) const )( &::genesis::placement::PqueryPlacement::edge ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
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
            ( boost::python::arg("edge") ),
            get_docstring("void ::genesis::placement::PqueryPlacement::reset_edge (PlacementTreeEdge & edge)")
        )
    ;
}

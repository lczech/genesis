/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_CLASS (PlacementTreeNodeData, "placement")
{

    // -------------------------------------------------------------------
    //     Class PlacementTreeNodeData
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::placement::PlacementTreeNodeData > ( "PlacementTreeNodeData" )
    ;
}

PYTHON_EXPORT_CLASS (PlacementTreeEdgeData, "placement")
{

    // -------------------------------------------------------------------
    //     Class PlacementTreeEdgeData
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::placement::PlacementTreeEdgeData > ( "PlacementTreeEdgeData" )

        // Public Member Functions

        .def(
            "dump",
            ( std::string ( ::genesis::placement::PlacementTreeEdgeData::* )(  ) const )( &::genesis::placement::PlacementTreeEdgeData::dump )
        )
        .def(
            "edge_num",
            ( int ( ::genesis::placement::PlacementTreeEdgeData::* )(  ) const )( &::genesis::placement::PlacementTreeEdgeData::edge_num )
        )
        .def(
            "reset_edge_num",
            ( void ( ::genesis::placement::PlacementTreeEdgeData::* )( int ))( &::genesis::placement::PlacementTreeEdgeData::reset_edge_num ),
            ( boost::python::arg("val") ),
            get_docstring("void ::genesis::placement::PlacementTreeEdgeData::reset_edge_num (int val)")
        )

        // Operators

        .def( boost::python::self != boost::python::self )
        .def( boost::python::self == boost::python::self )
    ;
}

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <boost/python.hpp>

#include "placement/placement_tree.hpp"

#include "../src/python/tree/newick_processor.hpp"
#include "../src/python/tree/tree.hpp"

// -------------------------------------------------------------------
//     Class PlacementTreeNodeData
// -------------------------------------------------------------------

void BoostPythonExport_PlacementTreeNodeData()
{

    boost::python::class_< ::genesis::PlacementTreeNodeData, boost::python::bases< ::genesis::DefaultTreeNodeData > > ( "PlacementTreeNodeData" )
    ;
}

// -------------------------------------------------------------------
//     Class PlacementTreeEdgeData
// -------------------------------------------------------------------

void BoostPythonExport_PlacementTreeEdgeData()
{

    boost::python::class_< ::genesis::PlacementTreeEdgeData, boost::python::bases< ::genesis::DefaultTreeEdgeData > > ( "PlacementTreeEdgeData" )

        // Public Member Functions

        // .def(
        //     "from_newick_broker_element",
        //     ( void ( ::genesis::PlacementTreeEdgeData::* )( NewickBrokerElement * ))( &::genesis::PlacementTreeEdgeData::from_newick_broker_element ),
        //     ( boost::python::arg("nbe") )
        // )
        // .def(
        //     "to_newick_broker_element",
        //     ( void ( ::genesis::PlacementTreeEdgeData::* )( NewickBrokerElement * ) const )( &::genesis::PlacementTreeEdgeData::to_newick_broker_element ),
        //     ( boost::python::arg("nbe") )
        // )
        .def(
            "dump",
            ( std::string ( ::genesis::PlacementTreeEdgeData::* )(  ) const )( &::genesis::PlacementTreeEdgeData::dump )
        )
        .def(
            "placement_count",
            ( size_t ( ::genesis::PlacementTreeEdgeData::* )(  ) const )( &::genesis::PlacementTreeEdgeData::placement_count ),
            "Returns the number of placements on this edge."
        )
        .def(
            "placement_mass",
            ( double ( ::genesis::PlacementTreeEdgeData::* )(  ) const )( &::genesis::PlacementTreeEdgeData::placement_mass ),
            "Returns the mass of the placements on this edge, as given by their like_weight_ratio."
        )
        .def(
            "sort_placements",
            ( void ( ::genesis::PlacementTreeEdgeData::* )(  ))( &::genesis::PlacementTreeEdgeData::sort_placements ),
            "Sorts the placements on this edge by their distance from the root, ascending."
        )

        // Operators

        .def( boost::python::self == boost::python::self )
        .def( boost::python::self != boost::python::self )
    ;
}

// -------------------------------------------------------------------
//     Class PlacementTree
// -------------------------------------------------------------------

void BoostPythonExport_PlacementTree()
{
    using namespace genesis;

    BoostPythonExport_PlacementTreeNodeData();
    BoostPythonExport_PlacementTreeEdgeData();

    BoostPythonExport_Tree<PlacementTreeNodeData, PlacementTreeEdgeData>("PlacementTree");
    BoostPythonExport_Overload_NewickProcessor<PlacementTree>();
}

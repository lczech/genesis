/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/placement/phyloxml_adapter.hpp"
#include "lib/placement/placement_tree.hpp"

#include "python/src/tree/phyloxml_processor.hpp"
#include "python/src/tree/tree.hpp"

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
    using namespace genesis;

    boost::python::class_< ::genesis::PlacementTreeEdgeData, boost::python::bases< ::genesis::DefaultTreeEdgeData > > ( "PlacementTreeEdgeData" )

        // Public Member Functions

        .def(
            "dump",
            ( std::string ( ::genesis::PlacementTreeEdgeData::* )(  ) const )( &::genesis::PlacementTreeEdgeData::dump )
        )
        // .def(
        //     "from_newick_broker_element",
        //     ( void ( ::genesis::PlacementTreeEdgeData::* )( NewickBrokerElement * ))( &::genesis::PlacementTreeEdgeData::from_newick_broker_element ),
        //     ( boost::python::arg("nbe") )
        // )
        .def(
            "placement_count",
            ( size_t ( ::genesis::PlacementTreeEdgeData::* )(  ) const )( &::genesis::PlacementTreeEdgeData::placement_count ),
            get_docstring("size_t ::genesis::PlacementTreeEdgeData::placement_count () const")
        )
        .def(
            "placement_mass",
            ( double ( ::genesis::PlacementTreeEdgeData::* )(  ) const )( &::genesis::PlacementTreeEdgeData::placement_mass ),
            get_docstring("double ::genesis::PlacementTreeEdgeData::placement_mass () const")
        )
        .def(
            "sort_placements",
            ( void ( ::genesis::PlacementTreeEdgeData::* )(  ))( &::genesis::PlacementTreeEdgeData::sort_placements ),
            get_docstring("void ::genesis::PlacementTreeEdgeData::sort_placements ()")
        )
        // .def(
        //     "to_newick_broker_element",
        //     ( void ( ::genesis::PlacementTreeEdgeData::* )( NewickBrokerElement * ) const )( &::genesis::PlacementTreeEdgeData::to_newick_broker_element ),
        //     ( boost::python::arg("nbe") )
        // )

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
    BoostPythonExport_PhyloxmlProcessor<DefaultTreePhyloxmlAdapter<PlacementTree>>("PlacementTreePhyloxmlProcessorBase");
    BoostPythonExport_PhyloxmlProcessor<PlacementTreePhyloxmlAdapter>("PlacementTreePhyloxmlProcessor");
}

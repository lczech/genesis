/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/placement/io/newick_processor.hpp"
// #include "lib/placement/io/phyloxml_adapter.hpp"
#include "lib/placement/placement_tree.hpp"

#include "python/src/tree/newick_processor.hpp"
// #include "python/src/tree/phyloxml_processor.hpp"
#include "python/src/tree/tree.hpp"

using namespace genesis::placement;

// -------------------------------------------------------------------
//     Class PlacementTreeNodeData
// -------------------------------------------------------------------

void BoostPythonExport_PlacementTreeNodeData()
{

    boost::python::class_<
        ::genesis::placement::PlacementTreeNodeData,
        boost::python::bases< ::genesis::tree::DefaultTreeNodeData >
    > ( "PlacementTreeNodeData" );
}

// -------------------------------------------------------------------
//     Class PlacementTreeEdgeData
// -------------------------------------------------------------------

void BoostPythonExport_PlacementTreeEdgeData()
{
    boost::python::class_<
        ::genesis::placement::PlacementTreeEdgeData,
        boost::python::bases< ::genesis::tree::DefaultTreeEdgeData >
    > ( "PlacementTreeEdgeData" )

        // Public Member Functions

        .def(
            "dump",
            ( std::string ( ::genesis::placement::PlacementTreeEdgeData::* )(  ) const )( &::genesis::placement::PlacementTreeEdgeData::dump )
        )
        // .def(
        //     "placement_count",
        //     ( size_t ( ::genesis::placement::PlacementTreeEdgeData::* )(  ) const )( &::genesis::placement::PlacementTreeEdgeData::placement_count ),
        //     get_docstring("size_t ::genesis::PlacementTreeEdgeData::placement_count () const")
        // )
        // .def(
        //     "placement_mass",
        //     ( double ( ::genesis::placement::PlacementTreeEdgeData::* )(  ) const )( &::genesis::placement::PlacementTreeEdgeData::placement_mass ),
        //     get_docstring("double ::genesis::PlacementTreeEdgeData::placement_mass () const")
        // )
        // .def(
        //     "sort_placements",
        //     ( void ( ::genesis::placement::PlacementTreeEdgeData::* )(  ))( &::genesis::placement::PlacementTreeEdgeData::sort_placements ),
        //     get_docstring("void ::genesis::PlacementTreeEdgeData::sort_placements ()")
        // )

        // Operators

        .def( boost::python::self == boost::python::self )
        .def( boost::python::self != boost::python::self )

        // Public Member Variables

        .def_readonly("edge_num", &::genesis::placement::PlacementTreeEdgeData::edge_num)
    ;
}

// -------------------------------------------------------------------
//     Class PlacementTree
// -------------------------------------------------------------------

PYTHON_EXPORT_CLASS (PlacementTree, "placement.tree")
{
    using namespace genesis;

    PYTHON_REQUIRES_CLASS( tree::DefaultTree )

    BoostPythonExport_PlacementTreeNodeData();
    BoostPythonExport_PlacementTreeEdgeData();

    BoostPythonExport_Tree<PlacementTreeNodeData, PlacementTreeEdgeData>("PlacementTree");

    // BoostPythonExport_PhyloxmlProcessor<DefaultTreePhyloxmlAdapter<PlacementTree>>("PlacementTreePhyloxmlProcessorBase");
    // BoostPythonExport_PhyloxmlProcessor<PlacementTreePhyloxmlAdapter>("PlacementTreePhyloxmlProcessor");

    // BoostPythonExport_NewickProcessor<DefaultTreeNewickAdapter<PlacementTree>>("PlacementTreeNewickProcessorBase");
    // BoostPythonExport_NewickProcessor<PlacementTreeNewickAdapter>("PlacementTreeNewickProcessor");

}

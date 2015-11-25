/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/tree/default_tree.hpp"
#include "lib/tree/default_tree_newick_adapter.hpp"
// #include "lib/tree/default_tree_phyloxml_adapter.hpp"

#include "python/src/tree/newick_processor.hpp"
// #include "python/src/tree/phyloxml_processor.hpp"
#include "python/src/tree/tree.hpp"

// -------------------------------------------------------------------
//     Class DefaultTreeEdgeData
// -------------------------------------------------------------------

void BoostPythonExport_DefaultTreeEdgeData()
{
    boost::python::class_< ::genesis::DefaultTreeEdgeData > ( "DefaultTreeEdgeData" )

        // Public Member Functions

        // .def(
        //     "from_newick_broker_element",
        //     ( void ( ::genesis::DefaultTreeEdgeData::* )( NewickBrokerElement * ))( &::genesis::DefaultTreeEdgeData::from_newick_broker_element ),
        //     ( boost::python::arg("node") ),
        //     "Fills the edge with data from a NewickBrokerElement."
        // )
        // .def(
        //     "to_newick_broker_element",
        //     ( void ( ::genesis::DefaultTreeEdgeData::* )( NewickBrokerElement * ) const )( &::genesis::DefaultTreeEdgeData::to_newick_broker_element ),
        //     ( boost::python::arg("node") )
        // )
        .def(
            "dump",
            ( std::string ( ::genesis::DefaultTreeEdgeData::* )(  ) const )( &::genesis::DefaultTreeEdgeData::dump )
        )

        // Operators

        .def( boost::python::self == boost::python::self )
        .def( boost::python::self != boost::python::self )
    ;
}

// -------------------------------------------------------------------
//     Class DefaultTreeNodeData
// -------------------------------------------------------------------

void BoostPythonExport_DefaultTreeNodeData()
{
    boost::python::class_< ::genesis::DefaultTreeNodeData > ( "DefaultTreeNodeData" )

        // Public Member Functions

        // .def(
        //     "from_newick_broker_element",
        //     ( void ( ::genesis::DefaultTreeNodeData::* )( NewickBrokerElement * ))( &::genesis::DefaultTreeNodeData::from_newick_broker_element ),
        //     ( boost::python::arg("node") ),
        //     "Fills the node with data from a NewickBrokerElement."
        // )
        // .def(
        //     "to_newick_broker_element",
        //     ( void ( ::genesis::DefaultTreeNodeData::* )( NewickBrokerElement * ) const )( &::genesis::DefaultTreeNodeData::to_newick_broker_element ),
        //     ( boost::python::arg("node") )
        // )
        .def(
            "dump",
            ( std::string ( ::genesis::DefaultTreeNodeData::* )(  ) const )( &::genesis::DefaultTreeNodeData::dump )
        )

        // Operators

        .def( boost::python::self == boost::python::self )
        .def( boost::python::self != boost::python::self )
    ;
}

// -------------------------------------------------------------------
//     Class DefaultTree
// -------------------------------------------------------------------

PYTHON_EXPORT_CLASS (DefaultTree, "tree")
{
    using namespace genesis;

    BoostPythonExport_DefaultTreeEdgeData();
    BoostPythonExport_DefaultTreeNodeData();

    BoostPythonExport_Tree<DefaultTreeNodeData, DefaultTreeEdgeData>("DefaultTree");
    BoostPythonExport_NewickProcessor<DefaultTreeNewickAdapter<DefaultTree>>("DefaultTreeNewickProcessor");
    // BoostPythonExport_PhyloxmlProcessor<DefaultTreePhyloxmlAdapter<DefaultTree>>("DefaultTreePhyloxmlProcessor");
}

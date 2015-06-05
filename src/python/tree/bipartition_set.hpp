#ifndef GENESIS_BOOST_PYTHON_EXPORT_TREE_BIPARTITION_SET_H_
#define GENESIS_BOOST_PYTHON_EXPORT_TREE_BIPARTITION_SET_H_

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <boost/python.hpp>
#include "tree/bipartition_set.hpp"

const char* get_docstring (const std::string& signature);

template <class NodeDataType, class EdgeDataType>
void BoostPythonExport_BipartitionSet(std::string name)
{
    typedef ::genesis::Tree          <NodeDataType, EdgeDataType> TreeType;
    typedef ::genesis::TreeEdge      <NodeDataType, EdgeDataType> EdgeType;
    typedef ::genesis::TreeLink      <NodeDataType, EdgeDataType> LinkType;
    typedef ::genesis::TreeNode      <NodeDataType, EdgeDataType> NodeType;

    typedef ::genesis::Bipartition   <NodeDataType, EdgeDataType> BipartitionType;
    typedef ::genesis::BipartitionSet<NodeDataType, EdgeDataType> BipartitionSetType;

    // -------------------------------------------------------------------
    //     Class BipartitionSet
    // -------------------------------------------------------------------

    boost::python::class_< BipartitionSetType > ( name.c_str() , boost::python::init< const TreeType * >(( boost::python::arg("tree") )) )

        // Public Member Functions
        .def(
            "dump",
            ( std::string ( BipartitionSetType::* )(  ))( &BipartitionSetType::dump )
        )
        .def(
            "find_smallest_subtree",
            ( BipartitionType * ( BipartitionSetType::* )( std::vector< NodeType * > ))( &BipartitionSetType::find_smallest_subtree ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            ( boost::python::arg("nodes") ),
            get_docstring("BipartitionType * ::genesis::BipartitionSet::find_smallest_subtree (std::vector< NodeType * > nodes)")
        )
        .def(
            "get_subtree_edges",
            ( std::vector< const EdgeType * > ( BipartitionSetType::* )( const LinkType * ))( &BipartitionSetType::get_subtree_edges ),
            ( boost::python::arg("subtree") )
        )
        .def(
            "make",
            ( void ( BipartitionSetType::* )(  ))( &BipartitionSetType::make )
        )
        .def(
            "make_index",
            ( void ( BipartitionSetType::* )(  ))( &BipartitionSetType::make_index )
        )
        .def(
            "validate",
            ( bool ( BipartitionSetType::* )(  ))( &BipartitionSetType::validate )
        )
    ;
}

#endif // include guard

#ifndef GENESIS_BOOST_PYTHON_EXPORT_TREE_BIPARTITION_SET_H_
#define GENESIS_BOOST_PYTHON_EXPORT_TREE_BIPARTITION_SET_H_

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>
#include "lib/tree/bipartition/bipartition_set.hpp"

template <typename Tree>
void BoostPythonExport_BipartitionSet(std::string name)
{
    typedef Tree                    TreeType;
    typedef typename Tree::NodeType NodeType;
    typedef typename Tree::LinkType LinkType;
    typedef typename Tree::EdgeType EdgeType;

    typedef ::genesis::Bipartition   <TreeType> BipartitionType;
    typedef ::genesis::BipartitionSet<TreeType> BipartitionSetType;

    // -------------------------------------------------------------------
    //     Class BipartitionSet
    // -------------------------------------------------------------------

    boost::python::class_< BipartitionSetType > ( name.c_str() , boost::python::init< TreeType& >(( boost::python::arg("tree") )) )

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
            ( std::vector< EdgeType * > ( BipartitionSetType::* )( LinkType * ))( &BipartitionSetType::get_subtree_edges ),
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

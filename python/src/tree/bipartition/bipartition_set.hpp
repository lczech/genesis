/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

template <typename Tree>
void PythonExportClass_BipartitionSet(std::string name)
{

    // -------------------------------------------------------------------
    //     Class BipartitionSet
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using BipartitionSetType = BipartitionSet<typename Tree>;

    boost::python::class_< BipartitionSetType > ( name.c_str(), boost::python::init< TreeType & >(( boost::python::arg("tree") )) )

        // Public Member Functions

        .def(
            "dump",
            ( std::string ( BipartitionSetType::* )(  ))( &BipartitionSetType::dump )
        )
        .def(
            "find_smallest_subtree",
            ( BipartitionType * ( BipartitionSetType::* )( std::vector< NodeType * > ))( &BipartitionSetType::find_smallest_subtree ),
            ( boost::python::arg("nodes") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("BipartitionType * ::genesis::tree::BipartitionSet::find_smallest_subtree (std::vector< NodeType * > nodes)")
        )
        .def(
            "get_subtree_edges",
            ( std::unordered_set< EdgeType * > ( BipartitionSetType::* )( LinkType * ))( &BipartitionSetType::get_subtree_edges ),
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

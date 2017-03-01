/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::BipartitionSet, scope )
{

    // -------------------------------------------------------------------
    //     Class BipartitionSet
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::BipartitionSet, std::shared_ptr<::genesis::tree::BipartitionSet> > ( scope, "BipartitionSet" )
        .def(
            pybind11::init< Tree & >(),
            pybind11::arg("tree")
        )

        // Public Member Functions

        .def(
            "dump",
            ( std::string ( ::genesis::tree::BipartitionSet::* )(  ))( &::genesis::tree::BipartitionSet::dump )
        )
        .def(
            "find_smallest_subtree",
            ( Bipartition * ( ::genesis::tree::BipartitionSet::* )( std::vector< TreeNode * > ))( &::genesis::tree::BipartitionSet::find_smallest_subtree ),
            pybind11::arg("nodes")
        )
        .def(
            "get_subtree_edges",
            ( std::unordered_set< size_t > ( ::genesis::tree::BipartitionSet::* )( TreeLink * ))( &::genesis::tree::BipartitionSet::get_subtree_edges ),
            pybind11::arg("subtree")
        )
        .def(
            "make",
            ( void ( ::genesis::tree::BipartitionSet::* )(  ))( &::genesis::tree::BipartitionSet::make )
        )
        .def(
            "make_index",
            ( void ( ::genesis::tree::BipartitionSet::* )(  ))( &::genesis::tree::BipartitionSet::make_index )
        )
        .def(
            "validate",
            ( bool ( ::genesis::tree::BipartitionSet::* )(  ))( &::genesis::tree::BipartitionSet::validate )
        )
    ;
}

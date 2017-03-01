/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::taxonomy;

PYTHON_EXPORT_FUNCTIONS( taxonomy_functions_taxopath_export, ::genesis::taxonomy, scope )
{

    scope.def(
        "add_from_taxopath",
        ( Taxon & ( * )( Taxonomy &, Taxopath const &, bool ))( &::genesis::taxonomy::add_from_taxopath ),
            pybind11::arg("taxonomy"),
            pybind11::arg("taxopath"),
            pybind11::arg("expect_parents")=(bool)(false)
    );

    scope.def(
        "find_taxon_by_taxopath",
        ( Taxon * ( * )( Taxonomy &, Taxopath const & ))( &::genesis::taxonomy::find_taxon_by_taxopath ),
            pybind11::arg("tax"),
            pybind11::arg("taxopath")
    );

    scope.def(
        "find_taxon_by_taxopath",
        ( Taxon const * ( * )( Taxonomy const &, Taxopath const & ))( &::genesis::taxonomy::find_taxon_by_taxopath ),
            pybind11::arg("tax"),
            pybind11::arg("taxopath")
    );
}

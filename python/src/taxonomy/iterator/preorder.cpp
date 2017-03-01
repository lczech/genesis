/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::taxonomy;

PYTHON_EXPORT_FUNCTIONS( taxonomy_iterator_preorder_export, ::genesis::taxonomy, scope )
{

    scope.def(
        "preorder_for_each",
        ( void ( * )( Taxonomy &, std::function< void(Taxon &)>, bool ))( &::genesis::taxonomy::preorder_for_each ),
            pybind11::arg("tax"),
            pybind11::arg("fn"),
            pybind11::arg("include_inner_taxa")=(bool)(true),
        get_docstring("void ::genesis::taxonomy::preorder_for_each (Taxonomy & tax, std::function< void(Taxon &)> fn, bool include_inner_taxa=true)")
    );

    scope.def(
        "preorder_for_each",
        ( void ( * )( Taxonomy const &, std::function< void(Taxon const &)>, bool ))( &::genesis::taxonomy::preorder_for_each ),
            pybind11::arg("tax"),
            pybind11::arg("fn"),
            pybind11::arg("include_inner_taxa")=(bool)(true),
        get_docstring("void ::genesis::taxonomy::preorder_for_each (Taxonomy const & tax, std::function< void(Taxon const &)> fn, bool include_inner_taxa=true)")
    );
}

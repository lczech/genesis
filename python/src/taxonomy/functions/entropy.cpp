/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::taxonomy;

PYTHON_EXPORT_CLASS( ::genesis::taxonomy::PruneByEntropySettings, scope )
{

    // -------------------------------------------------------------------
    //     Class PruneByEntropySettings
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::taxonomy::PruneByEntropySettings, std::shared_ptr<::genesis::taxonomy::PruneByEntropySettings> > ( scope, "PruneByEntropySettings" )
    ;
}

PYTHON_EXPORT_FUNCTIONS( taxonomy_functions_entropy_export, ::genesis::taxonomy, scope )
{

    scope.def(
        "validate_pruned_taxonomy",
        ( bool ( * )( Taxonomy const & ))( &::genesis::taxonomy::validate_pruned_taxonomy ),
            pybind11::arg("taxonomy"),
        get_docstring("bool ::genesis::taxonomy::validate_pruned_taxonomy (Taxonomy const & taxonomy)")
    );

    scope.def(
        "count_taxa_with_prune_status",
        ( size_t ( * )( Taxonomy const &, EntropyTaxonData::PruneStatus ))( &::genesis::taxonomy::count_taxa_with_prune_status ),
            pybind11::arg("taxonomy"),
            pybind11::arg("status"),
        get_docstring("size_t ::genesis::taxonomy::count_taxa_with_prune_status (Taxonomy const & taxonomy, EntropyTaxonData::PruneStatus status)")
    );

    scope.def(
        "print_pruned_taxonomy",
        ( std::string ( * )( Taxonomy const & ))( &::genesis::taxonomy::print_pruned_taxonomy ),
            pybind11::arg("taxonomy"),
        get_docstring("std::string ::genesis::taxonomy::print_pruned_taxonomy (Taxonomy const & taxonomy)")
    );

    scope.def(
        "expand_small_subtaxonomies",
        ( void ( * )( Taxonomy &, size_t ))( &::genesis::taxonomy::expand_small_subtaxonomies ),
            pybind11::arg("taxonomy"),
            pybind11::arg("min_subtaxonomy_size"),
        get_docstring("void ::genesis::taxonomy::expand_small_subtaxonomies (Taxonomy & taxonomy, size_t min_subtaxonomy_size)")
    );

    scope.def(
        "prune_by_entropy",
        ( void ( * )( Taxonomy &, size_t, PruneByEntropySettings ))( &::genesis::taxonomy::prune_by_entropy ),
            pybind11::arg("taxonomy"),
            pybind11::arg("target_taxonomy_size"),
            pybind11::arg("settings")=(PruneByEntropySettings()),
        get_docstring("void ::genesis::taxonomy::prune_by_entropy (Taxonomy & taxonomy, size_t target_taxonomy_size, PruneByEntropySettings settings={})")
    );

    scope.def(
        "remove_pruned_taxonomy_children",
        ( void ( * )( Taxonomy & ))( &::genesis::taxonomy::remove_pruned_taxonomy_children ),
            pybind11::arg("taxonomy"),
        get_docstring("void ::genesis::taxonomy::remove_pruned_taxonomy_children (Taxonomy & taxonomy)")
    );
}

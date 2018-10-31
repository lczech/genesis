/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::taxonomy;

PYTHON_EXPORT_CLASS( ::genesis::taxonomy::BreadthFirstSearch, scope )
{

    // -------------------------------------------------------------------
    //     Class BreadthFirstSearch
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::taxonomy::BreadthFirstSearch, std::shared_ptr<::genesis::taxonomy::BreadthFirstSearch> > ( scope, "BreadthFirstSearch" )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::taxonomy::DepthFirstSearch, scope )
{

    // -------------------------------------------------------------------
    //     Class DepthFirstSearch
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::taxonomy::DepthFirstSearch, std::shared_ptr<::genesis::taxonomy::DepthFirstSearch> > ( scope, "DepthFirstSearch" )
    ;
}

PYTHON_EXPORT_FUNCTIONS( taxonomy_functions_taxonomy_export, ::genesis::taxonomy, scope )
{

    scope.def(
        "find_taxon_by_id",
        ( Taxon * ( * )( Taxonomy &, std::string const & ))( &::genesis::taxonomy::find_taxon_by_id ),
            pybind11::arg("tax"),
            pybind11::arg("id"),
        get_docstring("Taxon * ::genesis::taxonomy::find_taxon_by_id (Taxonomy & tax, std::string const & id)")
    );

    scope.def(
        "find_taxon_by_name",
        ( Taxon * ( * )( Taxonomy &, std::string const & ))( &::genesis::taxonomy::find_taxon_by_name ),
            pybind11::arg("tax"),
            pybind11::arg("name"),
        get_docstring("Taxon * ::genesis::taxonomy::find_taxon_by_name (Taxonomy & tax, std::string const & name)")
    );

    scope.def(
        "find_taxon_by_id",
        ( Taxon const * ( * )( Taxonomy const &, std::string const & ))( &::genesis::taxonomy::find_taxon_by_id ),
            pybind11::arg("tax"),
            pybind11::arg("id"),
        get_docstring("Taxon const * ::genesis::taxonomy::find_taxon_by_id (Taxonomy const & tax, std::string const & id)")
    );

    scope.def(
        "find_taxon_by_name",
        ( Taxon const * ( * )( Taxonomy const &, std::string const & ))( &::genesis::taxonomy::find_taxon_by_name ),
            pybind11::arg("tax"),
            pybind11::arg("name"),
        get_docstring("Taxon const * ::genesis::taxonomy::find_taxon_by_name (Taxonomy const & tax, std::string const & name)")
    );

    scope.def(
        "has_unique_ids",
        ( bool ( * )( Taxonomy const & ))( &::genesis::taxonomy::has_unique_ids ),
            pybind11::arg("tax"),
        get_docstring("bool ::genesis::taxonomy::has_unique_ids (Taxonomy const & tax)")
    );

    scope.def(
        "validate",
        ( bool ( * )( Taxonomy const &, bool ))( &::genesis::taxonomy::validate ),
            pybind11::arg("taxonomy"),
            pybind11::arg("stop_at_first_error")=(bool)(false),
        get_docstring("bool ::genesis::taxonomy::validate (Taxonomy const & taxonomy, bool stop_at_first_error=false)")
    );

    scope.def(
        "taxa_count_at_level",
        ( size_t ( * )( Taxonomy const &, size_t ))( &::genesis::taxonomy::taxa_count_at_level ),
            pybind11::arg("tax"),
            pybind11::arg("level"),
        get_docstring("size_t ::genesis::taxonomy::taxa_count_at_level (Taxonomy const & tax, size_t level)")
    );

    scope.def(
        "taxa_count_lowest_levels",
        ( size_t ( * )( Taxonomy const & ))( &::genesis::taxonomy::taxa_count_lowest_levels ),
            pybind11::arg("tax"),
        get_docstring("size_t ::genesis::taxonomy::taxa_count_lowest_levels (Taxonomy const & tax)")
    );

    scope.def(
        "taxa_count_with_rank",
        ( size_t ( * )( Taxonomy const &, std::string const &, bool ))( &::genesis::taxonomy::taxa_count_with_rank ),
            pybind11::arg("tax"),
            pybind11::arg("rank"),
            pybind11::arg("case_sensitive")=(bool)(false),
        get_docstring("size_t ::genesis::taxonomy::taxa_count_with_rank (Taxonomy const & tax, std::string const & rank, bool case_sensitive=false)")
    );

    scope.def(
        "taxon_level",
        ( size_t ( * )( Taxon const & ))( &::genesis::taxonomy::taxon_level ),
            pybind11::arg("taxon"),
        get_docstring("size_t ::genesis::taxonomy::taxon_level (Taxon const & taxon)")
    );

    scope.def(
        "total_taxa_count",
        ( size_t ( * )( Taxonomy const & ))( &::genesis::taxonomy::total_taxa_count ),
            pybind11::arg("tax"),
        get_docstring("size_t ::genesis::taxonomy::total_taxa_count (Taxonomy const & tax)")
    );

    scope.def(
        "taxa_count_ranks",
        ( std::unordered_map< std::string, size_t > ( * )( Taxonomy const &, bool ))( &::genesis::taxonomy::taxa_count_ranks ),
            pybind11::arg("tax"),
            pybind11::arg("case_sensitive")=(bool)(false),
        get_docstring("std::unordered_map< std::string, size_t > ::genesis::taxonomy::taxa_count_ranks (Taxonomy const & tax, bool case_sensitive=false)")
    );

    scope.def(
        "taxa_count_levels",
        ( std::vector< size_t > ( * )( Taxonomy const & ))( &::genesis::taxonomy::taxa_count_levels ),
            pybind11::arg("tax"),
        get_docstring("std::vector< size_t > ::genesis::taxonomy::taxa_count_levels (Taxonomy const & tax)")
    );

    scope.def(
        "remove_taxa_at_level",
        ( void ( * )( Taxonomy &, size_t ))( &::genesis::taxonomy::remove_taxa_at_level ),
            pybind11::arg("tax"),
            pybind11::arg("level"),
        get_docstring("void ::genesis::taxonomy::remove_taxa_at_level (Taxonomy & tax, size_t level)")
    );

    scope.def(
        "sort_by_name",
        ( void ( * )( Taxonomy &, bool, bool ))( &::genesis::taxonomy::sort_by_name ),
            pybind11::arg("tax"),
            pybind11::arg("recursive")=(bool)(true),
            pybind11::arg("case_sensitive")=(bool)(false),
        get_docstring("void ::genesis::taxonomy::sort_by_name (Taxonomy & tax, bool recursive=true, bool case_sensitive=false)")
    );
}

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::taxonomy;

PYTHON_EXPORT_FUNCTIONS( taxonomy_functions_taxonomy_export, ::genesis::taxonomy, scope )
{

    scope.def(
        "find_taxon_by_name",
        ( Taxon * ( * )( Taxonomy &, std::string const & ))( &::genesis::taxonomy::find_taxon_by_name ),
            pybind11::arg("tax"),
            pybind11::arg("name")
    );

    scope.def(
        "find_taxon_by_name",
        ( Taxon const * ( * )( Taxonomy const &, std::string const & ))( &::genesis::taxonomy::find_taxon_by_name ),
            pybind11::arg("tax"),
            pybind11::arg("name")
    );

    scope.def(
        "validate",
        ( bool ( * )( Taxonomy const &, bool ))( &::genesis::taxonomy::validate ),
            pybind11::arg("taxonomy"),
            pybind11::arg("stop_at_first_error")=(bool)(false)
    );

    scope.def(
        "taxa_count_at_level",
        ( size_t ( * )( Taxonomy const &, size_t ))( &::genesis::taxonomy::taxa_count_at_level ),
            pybind11::arg("tax"),
            pybind11::arg("level")
    );

    scope.def(
        "taxa_count_lowest_levels",
        ( size_t ( * )( Taxonomy const & ))( &::genesis::taxonomy::taxa_count_lowest_levels ),
            pybind11::arg("tax")
    );

    scope.def(
        "taxa_count_with_rank",
        ( size_t ( * )( Taxonomy const &, std::string const &, bool ))( &::genesis::taxonomy::taxa_count_with_rank ),
            pybind11::arg("tax"),
            pybind11::arg("rank"),
            pybind11::arg("case_sensitive")=(bool)(false)
    );

    scope.def(
        "taxon_level",
        ( size_t ( * )( Taxon const & ))( &::genesis::taxonomy::taxon_level ),
            pybind11::arg("taxon")
    );

    scope.def(
        "total_taxa_count",
        ( size_t ( * )( Taxonomy const & ))( &::genesis::taxonomy::total_taxa_count ),
            pybind11::arg("tax")
    );

    scope.def(
        "taxa_count_ranks",
        ( std::unordered_map< std::string, size_t > ( * )( Taxonomy const &, bool ))( &::genesis::taxonomy::taxa_count_ranks ),
            pybind11::arg("tax"),
            pybind11::arg("case_sensitive")=(bool)(false)
    );

    scope.def(
        "taxa_count_levels",
        ( std::vector< size_t > ( * )( Taxonomy const & ))( &::genesis::taxonomy::taxa_count_levels ),
            pybind11::arg("tax")
    );

    scope.def(
        "remove_taxa_at_level",
        ( void ( * )( Taxonomy &, size_t ))( &::genesis::taxonomy::remove_taxa_at_level ),
            pybind11::arg("tax"),
            pybind11::arg("level")
    );

    scope.def(
        "sort_by_name",
        ( void ( * )( Taxonomy &, bool, bool ))( &::genesis::taxonomy::sort_by_name ),
            pybind11::arg("tax"),
            pybind11::arg("recursive")=(bool)(true),
            pybind11::arg("case_sensitive")=(bool)(false)
    );
}

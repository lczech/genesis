/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::taxonomy;

template<class UnaryPredicate>
void python_export_function_taxonomy_functions_taxonomy_UnaryPredicate ()
{
    scope.def(
        "find_taxon",
        ( Taxon * ( * )( Taxonomy &, UnaryPredicate ))( &::genesis::taxonomy::find_taxon ),
            pybind11::arg("tax"),
            pybind11::arg("p"),
        get_docstring("Taxon * ::genesis::taxonomy::find_taxon (Taxonomy & tax, UnaryPredicate p)")
    );

    scope.def(
        "find_taxon",
        ( Taxon const * ( * )( Taxonomy const &, UnaryPredicate ))( &::genesis::taxonomy::find_taxon ),
            pybind11::arg("tax"),
            pybind11::arg("p"),
        get_docstring("Taxon const * ::genesis::taxonomy::find_taxon (Taxonomy const & tax, UnaryPredicate p)")
    );

    scope.def(
        "find_taxon",
        ( Taxon const * ( * )( Taxonomy const &, UnaryPredicate, BreadthFirstSearch ))( &::genesis::taxonomy::find_taxon ),
            pybind11::arg("tax"),
            pybind11::arg("p"),
            pybind11::arg(""),
        get_docstring("Taxon const * ::genesis::taxonomy::find_taxon (Taxonomy const & tax, UnaryPredicate p, BreadthFirstSearch )")
    );

    scope.def(
        "find_taxon",
        ( Taxon const * ( * )( Taxonomy const &, UnaryPredicate, DepthFirstSearch ))( &::genesis::taxonomy::find_taxon ),
            pybind11::arg("tax"),
            pybind11::arg("p"),
            pybind11::arg(""),
        get_docstring("Taxon const * ::genesis::taxonomy::find_taxon (Taxonomy const & tax, UnaryPredicate p, DepthFirstSearch )")
    );
}

template<class SearchStrategy, class UnaryPredicate>
void python_export_function_taxonomy_functions_taxonomy_SearchStrategy_UnaryPredicate ()
{
    scope.def(
        "find_taxon",
        ( Taxon * ( * )( Taxonomy &, UnaryPredicate, SearchStrategy ))( &::genesis::taxonomy::find_taxon ),
            pybind11::arg("tax"),
            pybind11::arg("p"),
            pybind11::arg("strat"),
        get_docstring("Taxon * ::genesis::taxonomy::find_taxon (Taxonomy & tax, UnaryPredicate p, SearchStrategy strat)")
    );
}

template<class SearchStrategy>
void python_export_function_taxonomy_functions_taxonomy_SearchStrategy ()
{
    scope.def(
        "find_taxon_by_id",
        ( Taxon * ( * )( Taxonomy &, std::string const &, SearchStrategy ))( &::genesis::taxonomy::find_taxon_by_id ),
            pybind11::arg("tax"),
            pybind11::arg("id"),
            pybind11::arg("strat"),
        get_docstring("Taxon * ::genesis::taxonomy::find_taxon_by_id (Taxonomy & tax, std::string const & id, SearchStrategy strat)")
    );

    scope.def(
        "find_taxon_by_name",
        ( Taxon * ( * )( Taxonomy &, std::string const &, SearchStrategy ))( &::genesis::taxonomy::find_taxon_by_name ),
            pybind11::arg("tax"),
            pybind11::arg("name"),
            pybind11::arg("strat"),
        get_docstring("Taxon * ::genesis::taxonomy::find_taxon_by_name (Taxonomy & tax, std::string const & name, SearchStrategy strat)")
    );

    scope.def(
        "find_taxon_by_id",
        ( Taxon const * ( * )( Taxonomy const &, std::string const &, SearchStrategy ))( &::genesis::taxonomy::find_taxon_by_id ),
            pybind11::arg("tax"),
            pybind11::arg("id"),
            pybind11::arg("strat"),
        get_docstring("Taxon const * ::genesis::taxonomy::find_taxon_by_id (Taxonomy const & tax, std::string const & id, SearchStrategy strat)")
    );

    scope.def(
        "find_taxon_by_name",
        ( Taxon const * ( * )( Taxonomy const &, std::string const &, SearchStrategy ))( &::genesis::taxonomy::find_taxon_by_name ),
            pybind11::arg("tax"),
            pybind11::arg("name"),
            pybind11::arg("strat"),
        get_docstring("Taxon const * ::genesis::taxonomy::find_taxon_by_name (Taxonomy const & tax, std::string const & name, SearchStrategy strat)")
    );
}

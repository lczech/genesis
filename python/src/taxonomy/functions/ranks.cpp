/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::taxonomy;

PYTHON_EXPORT_FUNCTIONS( taxonomy_functions_ranks_export, ::genesis::taxonomy, scope )
{

    scope.def(
        "resolve_rank_abbreviation",
        ( std::pair< std::string, std::string > ( * )( std::string const & ))( &::genesis::taxonomy::resolve_rank_abbreviation ),
            pybind11::arg("entry")
    );

    scope.def(
        "rank_from_abbreviation",
        ( std::string ( * )( char ))( &::genesis::taxonomy::rank_from_abbreviation ),
            pybind11::arg("r")
    );

    scope.def(
        "rank_to_abbreviation",
        ( std::string ( * )( std::string const & ))( &::genesis::taxonomy::rank_to_abbreviation ),
            pybind11::arg("rank")
    );
}

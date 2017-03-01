/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::taxonomy;

template<class TaxonDataType>
void python_export_function_taxonomy_functions_operators_TaxonDataType ()
{
    scope.def(
        "taxonomy_data_is",
        ( bool ( * )( Taxonomy const & ))( &::genesis::taxonomy::taxonomy_data_is ),
            pybind11::arg("taxonomy"),
        get_docstring("bool ::genesis::taxonomy::taxonomy_data_is (Taxonomy const & taxonomy)")
    );

    scope.def(
        "taxonomy_data_is_derived_from",
        ( bool ( * )( Taxonomy const & ))( &::genesis::taxonomy::taxonomy_data_is_derived_from ),
            pybind11::arg("taxonomy"),
        get_docstring("bool ::genesis::taxonomy::taxonomy_data_is_derived_from (Taxonomy const & taxonomy)")
    );

    scope.def(
        "reset_taxonomy_data",
        ( void ( * )( Taxonomy &, bool ))( &::genesis::taxonomy::reset_taxonomy_data ),
            pybind11::arg("taxonomy"),
            pybind11::arg("allow_overwrite")=(bool)(true),
        get_docstring("void ::genesis::taxonomy::reset_taxonomy_data (Taxonomy & taxonomy, bool allow_overwrite=true)")
    );
}

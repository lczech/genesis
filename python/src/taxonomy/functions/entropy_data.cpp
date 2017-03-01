/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::taxonomy;
/*
PYTHON_EXPORT_CLASS( ::genesis::taxonomy::EntropyTaxonData, scope )
{

    // -------------------------------------------------------------------
    //     Class EntropyTaxonData
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::taxonomy::EntropyTaxonData, std::shared_ptr<::genesis::taxonomy::EntropyTaxonData> > ( scope, "EntropyTaxonData" )

        // Public Member Functions

        .def(
            "clone",
            ( std::unique_ptr< BaseTaxonData > ( ::genesis::taxonomy::EntropyTaxonData::* )(  ) const )( &::genesis::taxonomy::EntropyTaxonData::clone ),
            get_docstring("std::unique_ptr< BaseTaxonData > ::genesis::taxonomy::EntropyTaxonData::clone () const")
        )
        .def_static(
            "create",
            ( std::unique_ptr< EntropyTaxonData > ( * )(  ))( &::genesis::taxonomy::EntropyTaxonData::create )
        )
        .def_static(
            "status_abbreviation",
            ( std::string ( * )( PruneStatus ))( &::genesis::taxonomy::EntropyTaxonData::status_abbreviation ),
            pybind11::arg("stat")
        )
        .def_static(
            "status_text",
            ( std::string ( * )( PruneStatus ))( &::genesis::taxonomy::EntropyTaxonData::status_text ),
            pybind11::arg("stat")
        )
    ;
}
*/

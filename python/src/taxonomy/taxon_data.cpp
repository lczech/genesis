/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::taxonomy;

PYTHON_EXPORT_CLASS( ::genesis::taxonomy::BaseTaxonData, scope )
{

    // -------------------------------------------------------------------
    //     Class BaseTaxonData
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::taxonomy::BaseTaxonData, std::shared_ptr<::genesis::taxonomy::BaseTaxonData> > ( scope, "BaseTaxonData" )

        // Public Member Functions

        .def(
            "clone",
            ( std::unique_ptr< BaseTaxonData > ( ::genesis::taxonomy::BaseTaxonData::* )(  ) const )( &::genesis::taxonomy::BaseTaxonData::clone ),
            get_docstring("std::unique_ptr< BaseTaxonData > ::genesis::taxonomy::BaseTaxonData::clone () const")
        )
        .def_static(
            "create",
            ( std::unique_ptr< BaseTaxonData > ( * )(  ))( &::genesis::taxonomy::BaseTaxonData::create ),
            get_docstring("static std::unique_ptr< BaseTaxonData > ::genesis::taxonomy::BaseTaxonData::create ()")
        )
    ;
}

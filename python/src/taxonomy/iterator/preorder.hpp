/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::taxonomy;

template <typename TaxonomyType, typename TaxonType>
void PythonExportClass_::genesis::taxonomy::IteratorPreorder(std::string name)
{

    // -------------------------------------------------------------------
    //     Class IteratorPreorder
    // -------------------------------------------------------------------

    using namespace ::genesis::taxonomy;

    using IteratorPreorderType = IteratorPreorder<typename TaxonomyType, typename TaxonType>;

    pybind11::class_< IteratorPreorderType, std::shared_ptr<IteratorPreorderType> > ( scope, name.c_str() )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< TaxonomyType & >(),
            pybind11::arg("taxonomy")
        )
        .def(
            pybind11::init< IteratorPreorder const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "taxon",
            ( TaxonType & ( IteratorPreorderType::* )(  ) const )( &IteratorPreorderType::taxon )
        )

        // Operators

        .def( pybind11::self != pybind11::self )
        .def( pybind11::self == pybind11::self )
    ;
}

template<typename TaxonomyType>
void python_export_function_taxonomy_iterator_preorder_TaxonomyType ()
{
    scope.def(
        "preorder",
        ( utils::Range< IteratorPreorder< Taxonomy const, Taxon const  > > ( * )( TaxonomyType const & ))( &::genesis::taxonomy::preorder ),
            pybind11::arg("taxonomy")
    );

    scope.def(
        "preorder",
        ( utils::Range< IteratorPreorder< Taxonomy, Taxon > > ( * )( TaxonomyType & ))( &::genesis::taxonomy::preorder ),
            pybind11::arg("taxonomy")
    );
}

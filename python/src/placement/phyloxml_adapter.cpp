/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "placement/phyloxml_adapter.hpp"

PYTHON_EXPORT_CLASS (PlacementTreePhyloxmlAdapter, "placement.tree")
{
    // -------------------------------------------------------------------
    //     Class PlacementTreePhyloxmlAdapter
    // -------------------------------------------------------------------

    /*

    boost::python::class_< ::genesis::DefaultTreePhyloxmlAdapter<::genesis::PlacementTree> > ( "PlacementTreePhyloxmlAdapterBase" )

        // Public Member Functions

        // .def(
        //     "populate_clade",
        //     ( void ( ::genesis::PlacementTreePhyloxmlAdapter::* )( XmlElement *, typename DefaultTree::ConstIteratorPreorder & ))( &::genesis::PlacementTreePhyloxmlAdapter::populate_clade ),
        //     ( boost::python::arg("clade"), boost::python::arg("it") )
        // )
    ;

    boost::python::class_< ::genesis::PlacementTreePhyloxmlAdapter, boost::python::bases<::genesis::DefaultTreePhyloxmlAdapter<genesis::PlacementTree>> > ( "PlacementTreePhyloxmlAdapter" )
        .def( boost::python::init< ::genesis::PlacementMap & >(( boost::python::arg("map") )) )

        // Public Member Functions

        // .def(
        //     "populate_clade",
        //     ( void ( ::genesis::PlacementTreePhyloxmlAdapter::* )( XmlElement *, typename DefaultTree::ConstIteratorPreorder & ))( &::genesis::PlacementTreePhyloxmlAdapter::populate_clade ),
        //     ( boost::python::arg("clade"), boost::python::arg("it") )
        // )
    ;

    */

    boost::python::class_< ::genesis::PlacementTreePhyloxmlAdapter > ( "PlacementTreePhyloxmlAdapter" )
        .def( boost::python::init< ::genesis::PlacementMap & >(( boost::python::arg("map") )) )

    ;

}

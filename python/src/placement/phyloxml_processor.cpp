/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/placement/io/phyloxml_processor.hpp"

using namespace genesis::placement;

PYTHON_EXPORT_CLASS (PlacementTreePhyloxmlProcessor, "placement.tree")
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
        .def( boost::python::init< ::genesis::Sample & >(( boost::python::arg("map") )) )

        // Public Member Functions

        // .def(
        //     "populate_clade",
        //     ( void ( ::genesis::PlacementTreePhyloxmlAdapter::* )( XmlElement *, typename DefaultTree::ConstIteratorPreorder & ))( &::genesis::PlacementTreePhyloxmlAdapter::populate_clade ),
        //     ( boost::python::arg("clade"), boost::python::arg("it") )
        // )
    ;

    */

    boost::python::class_< ::genesis::placement::PlacementTreePhyloxmlProcessor > ( "PhyloxmlProcessor" )

    ;

}

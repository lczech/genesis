/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS (DefaultTreeNodeData, "tree")
{

    // -------------------------------------------------------------------
    //     Class DefaultTreeNodeData
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::tree::DefaultTreeNodeData > ( "DefaultTreeNodeData" )

        // Public Member Functions

        .def(
            "dump",
            ( std::string ( ::genesis::tree::DefaultTreeNodeData::* )(  ) const )( &::genesis::tree::DefaultTreeNodeData::dump )
        )

        // Operators

        .def( boost::python::self != boost::python::self )
        .def( boost::python::self == boost::python::self )
    ;
}

PYTHON_EXPORT_CLASS (DefaultTreeEdgeData, "tree")
{

    // -------------------------------------------------------------------
    //     Class DefaultTreeEdgeData
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::tree::DefaultTreeEdgeData > ( "DefaultTreeEdgeData" )

        // Public Member Functions

        .def(
            "dump",
            ( std::string ( ::genesis::tree::DefaultTreeEdgeData::* )(  ) const )( &::genesis::tree::DefaultTreeEdgeData::dump )
        )

        // Operators

        .def( boost::python::self != boost::python::self )
        .def( boost::python::self == boost::python::self )
    ;
}

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS (NewickBrokerElement, "tree")
{

    // -------------------------------------------------------------------
    //     Class NewickBrokerElement
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::tree::NewickBrokerElement > ( "NewickBrokerElement", boost::python::init<  >(  ) )
        .def( boost::python::init< NewickBrokerElement const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "rank",
            ( int ( ::genesis::tree::NewickBrokerElement::* )(  ) const )( &::genesis::tree::NewickBrokerElement::rank ),
            get_docstring("int ::genesis::tree::NewickBrokerElement::rank () const")
        )
    ;
}

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::NewickBrokerElement, scope )
{

    // -------------------------------------------------------------------
    //     Class NewickBrokerElement
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::NewickBrokerElement, std::shared_ptr<::genesis::tree::NewickBrokerElement> > ( scope, "NewickBrokerElement" )
        .def(
            pybind11::init<  >(),
            get_docstring("::genesis::tree::NewickBrokerElement::NewickBrokerElement ()")
        )
        .def(
            pybind11::init< NewickBrokerElement const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "rank",
            ( int ( ::genesis::tree::NewickBrokerElement::* )(  ) const )( &::genesis::tree::NewickBrokerElement::rank ),
            get_docstring("int ::genesis::tree::NewickBrokerElement::rank () const")
        )
    ;
}

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::LcaLookup, scope )
{

    // -------------------------------------------------------------------
    //     Class LcaLookup
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::LcaLookup, std::shared_ptr<::genesis::tree::LcaLookup> > ( scope, "LcaLookup" )
        // .def(
        //     pybind11::init<  >()
        // )
        .def(
            pybind11::init< Tree const & >(),
            pybind11::arg("tree")
        )
        .def(
            pybind11::init< LcaLookup const & >(),
            pybind11::arg("other")
        )
    ;
}

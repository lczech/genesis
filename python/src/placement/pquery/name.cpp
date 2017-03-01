/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_CLASS( ::genesis::placement::PqueryName, scope )
{

    // -------------------------------------------------------------------
    //     Class PqueryName
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::placement::PqueryName, std::shared_ptr<::genesis::placement::PqueryName> > ( scope, "PqueryName" )
        .def(
            pybind11::init<  >(),
            get_docstring("::genesis::placement::PqueryName::PqueryName ()")
        )
        .def(
            pybind11::init< std::string, double >(),
            pybind11::arg("name"),
            pybind11::arg("multiplicity")=(double)(1.0),
            get_docstring("::genesis::placement::PqueryName::PqueryName (std::string name, double multiplicity=1.0)")
        )
        .def(
            pybind11::init< PqueryName const & >(),
            pybind11::arg("arg")
        )
    ;
}

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_CLASS (PqueryName, "placement")
{

    // -------------------------------------------------------------------
    //     Class PqueryName
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::placement::PqueryName > ( "PqueryName", boost::python::init<  >(  ) )
        .def( boost::python::init< std::string, boost::python::optional< double > >(( boost::python::arg("name"), boost::python::arg("multiplicity")=(double)(0.0) )) )
        .def( boost::python::init< PqueryName const & >(( boost::python::arg("") )) )
    ;
}

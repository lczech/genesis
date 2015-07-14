/**
 * @brief Python binding of ###CLASSNAME### class.
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/###MODULNAME###/###FILENAME###.hpp"

void BoostPythonExport_###CLASSNAME###()
{
    using namespace genesis;

    boost::python::class_< ::genesis::###CLASSNAME### > ( "###CLASSNAME###", boost::python::init<  >(  ) )

    ;

}

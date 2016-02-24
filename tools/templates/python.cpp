/**
 * @brief Python binding of ###CLASSNAME### class.
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/###MODULNAME###/###FILENAME###.hpp"

PYTHON_EXPORT_CLASS( ###CLASSNAME###, "###MODULNAME###" )
{
    using namespace genesis;

    boost::python::class_< ::genesis::###CLASSNAME### > ( "###CLASSNAME###", boost::python::init<  >(  ) )

    ;

}

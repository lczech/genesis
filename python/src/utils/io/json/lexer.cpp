/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS (JsonLexer, "utils")
{

    // -------------------------------------------------------------------
    //     Class JsonLexer
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::JsonLexer > ( "JsonLexer", boost::python::init<  >(  ) )
    ;
}

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS (NewickLexer, "tree")
{

    // -------------------------------------------------------------------
    //     Class NewickLexer
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::tree::NewickLexer > ( "NewickLexer", boost::python::init<  >(  ) )
    ;
}

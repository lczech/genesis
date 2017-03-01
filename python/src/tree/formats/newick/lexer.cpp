/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::NewickLexer, scope )
{

    // -------------------------------------------------------------------
    //     Class NewickLexer
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::NewickLexer, std::shared_ptr<::genesis::tree::NewickLexer> > ( scope, "NewickLexer" )
        .def(
            pybind11::init<  >()
        )
    ;
}

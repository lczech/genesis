/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

template <typename Base>
void PythonExportClass_DefaultTreePhyloxmlWriterMixin(std::string name)
{

    // -------------------------------------------------------------------
    //     Class DefaultTreePhyloxmlWriterMixin
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using DefaultTreePhyloxmlWriterMixinType = DefaultTreePhyloxmlWriterMixin<typename Base>;

    boost::python::class_< DefaultTreePhyloxmlWriterMixinType > ( name.c_str() )
    ;
}

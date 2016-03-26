/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

template <typename Base>
void PythonExportClass_PhyloxmlColorWriterMixin(std::string name)
{

    // -------------------------------------------------------------------
    //     Class PhyloxmlColorWriterMixin
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using PhyloxmlColorWriterMixinType = PhyloxmlColorWriterMixin<typename Base>;

    boost::python::class_< PhyloxmlColorWriterMixinType > ( name.c_str() )
    ;
}

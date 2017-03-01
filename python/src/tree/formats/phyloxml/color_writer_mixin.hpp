/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

template <typename Base>
void PythonExportClass_::genesis::tree::PhyloxmlColorWriterMixin(std::string name)
{

    // -------------------------------------------------------------------
    //     Class PhyloxmlColorWriterMixin
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using PhyloxmlColorWriterMixinType = PhyloxmlColorWriterMixin<typename Base>;

    pybind11::class_< PhyloxmlColorWriterMixinType, std::shared_ptr<PhyloxmlColorWriterMixinType> > ( scope, name.c_str() )
    ;
}

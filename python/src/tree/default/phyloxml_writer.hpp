/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

template <typename Base>
void PythonExportClass_::genesis::tree::DefaultTreePhyloxmlWriterMixin(std::string name)
{

    // -------------------------------------------------------------------
    //     Class DefaultTreePhyloxmlWriterMixin
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using DefaultTreePhyloxmlWriterMixinType = DefaultTreePhyloxmlWriterMixin<typename Base>;

    pybind11::class_< DefaultTreePhyloxmlWriterMixinType, std::shared_ptr<DefaultTreePhyloxmlWriterMixinType> > ( scope, name.c_str() )
    ;
}

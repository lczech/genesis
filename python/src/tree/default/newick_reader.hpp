/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

template <typename Base>
void PythonExportClass_::genesis::tree::DefaultTreeNewickReaderMixin(std::string name)
{

    // -------------------------------------------------------------------
    //     Class DefaultTreeNewickReaderMixin
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using DefaultTreeNewickReaderMixinType = DefaultTreeNewickReaderMixin<typename Base>;

    pybind11::class_< DefaultTreeNewickReaderMixinType, std::shared_ptr<DefaultTreeNewickReaderMixinType> > ( scope, name.c_str() )
    ;
}

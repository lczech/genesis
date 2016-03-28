/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

template <typename Base>
void PythonExportClass_DefaultTreeNewickReaderMixin(std::string name)
{

    // -------------------------------------------------------------------
    //     Class DefaultTreeNewickReaderMixin
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using DefaultTreeNewickReaderMixinType = DefaultTreeNewickReaderMixin<typename Base>;

    boost::python::class_< DefaultTreeNewickReaderMixinType > ( name.c_str() )
    ;
}

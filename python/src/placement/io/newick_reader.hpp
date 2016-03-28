/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

template <typename Base>
void PythonExportClass_PlacementTreeNewickReaderMixin(std::string name)
{

    // -------------------------------------------------------------------
    //     Class PlacementTreeNewickReaderMixin
    // -------------------------------------------------------------------

    using namespace ::genesis::placement;

    using PlacementTreeNewickReaderMixinType = PlacementTreeNewickReaderMixin<typename Base>;

    boost::python::class_< PlacementTreeNewickReaderMixinType > ( name.c_str() )
    ;
}

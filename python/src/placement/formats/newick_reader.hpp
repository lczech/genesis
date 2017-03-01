/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

template <typename Base>
void PythonExportClass_::genesis::placement::PlacementTreeNewickReaderMixin(std::string name)
{

    // -------------------------------------------------------------------
    //     Class PlacementTreeNewickReaderMixin
    // -------------------------------------------------------------------

    using namespace ::genesis::placement;

    using PlacementTreeNewickReaderMixinType = PlacementTreeNewickReaderMixin<typename Base>;

    pybind11::class_< PlacementTreeNewickReaderMixinType, std::shared_ptr<PlacementTreeNewickReaderMixinType> > ( scope, name.c_str() )
    ;
}

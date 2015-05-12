/**
 * @brief Definition of the genesis Python module.
 *
 * @file
 * @ingroup python
 */

#include <boost/python.hpp>

// =============================================================================
//     Forward declarations of all exported classes
// =============================================================================

    // -------------------------------------------
    //     Placement
    // -------------------------------------------

void BoostPythonExport_JplaceProcessor();
void BoostPythonExport_PlacementMap();
void BoostPythonExport_PlacementTree();
void BoostPythonExport_PlacementTree_NewickProcessor();

    // -------------------------------------------
    //     Utils
    // -------------------------------------------

void BoostPythonExport_Bitvector();
void BoostPythonExport_Logging();

// =============================================================================
//     Boost Python Module
// =============================================================================

BOOST_PYTHON_MODULE(genesis)
{

    // -------------------------------------------
    //     Placement
    // -------------------------------------------

    BoostPythonExport_JplaceProcessor();
    BoostPythonExport_PlacementMap();
    BoostPythonExport_PlacementTree();
    BoostPythonExport_PlacementTree_NewickProcessor();

    // -------------------------------------------
    //     Utils
    // -------------------------------------------

    BoostPythonExport_Bitvector();
    BoostPythonExport_Logging();

}

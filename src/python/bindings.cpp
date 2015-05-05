/**
 * @brief Definition of the genesis Python module.
 *
 * @file
 * @ingroup python
 */

#include <boost/python.hpp>

//~ #include "genesis.hpp"

// =============================================================================
//     Forward declarations of all exported classes
// =============================================================================

void BoostPythonExport_JplaceProcessor();
void BoostPythonExport_PlacementMap();
void BoostPythonExport_PlacementTree();
void BoostPythonExport_Bitvector();

// =============================================================================
//     Boost Python Module
// =============================================================================

BOOST_PYTHON_MODULE(genesis)
{
    //~ using namespace boost::python;
    //~ using namespace genesis;

    BoostPythonExport_JplaceProcessor();
    BoostPythonExport_PlacementMap();
    BoostPythonExport_PlacementTree();
    BoostPythonExport_Bitvector();

}

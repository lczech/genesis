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

void BoostPythonExport_Bitvector();

// =============================================================================
//     Boost Python Module
// =============================================================================

BOOST_PYTHON_MODULE(genesis)
{
    //~ using namespace boost::python;
    //~ using namespace genesis;

    BoostPythonExport_Bitvector();
}

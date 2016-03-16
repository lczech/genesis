#ifndef GENESIS_PLACEMENT_SIMULATOR_SIMULATOR_H_
#define GENESIS_PLACEMENT_SIMULATOR_SIMULATOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include "placement/sample.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Placement Simulator
// =================================================================================================

/**
 * @brief Simulate Sample%s on a Tree.
 */
class Simulator
{
public:

    // -----------------------------------------------------
    //     Constructor
    // -----------------------------------------------------

    Simulator( Sample& smp )
        : sample_( smp )
    {}

    // -----------------------------------------------------
    //     Member Functions
    // -----------------------------------------------------

    virtual void generate( size_t n ) = 0;

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

protected:

    Sample& sample_;
};

} // namespace placement
} // namespace genesis

#endif // include guard

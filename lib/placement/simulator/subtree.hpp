#ifndef GENESIS_PLACEMENT_SIMULATOR_SUBTREE_H_
#define GENESIS_PLACEMENT_SIMULATOR_SUBTREE_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include "placement/simulator/simulator.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Placement Simulator Subtree
// =================================================================================================

/**
 * @brief
 */
class SimulatorSubtree : public Simulator
{
public:

    SimulatorSubtree( Sample& smp )
        : Simulator( smp )
    {}

    void generate( size_t n ) override;
};

} // namespace placement
} // namespace genesis

#endif // include guard

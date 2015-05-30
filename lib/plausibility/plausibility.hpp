#ifndef GENESIS_PLAUSIBILITY_PLAUSIBILITY_H_
#define GENESIS_PLAUSIBILITY_PLAUSIBILITY_H_

/**
 * @brief
 *
 * @file
 * @ingroup plausibility
 */

#include <map>
#include <string>
#include <vector>

#include "plausibility/plausibility_tree.hpp"
#include "utils/bitvector.hpp"

namespace genesis {

// =============================================================================
//     Plausibility
// =============================================================================

class Plausibility
{
public:

    // ---------------------------------------------------------
    //     Plausibility Measurement
    // ---------------------------------------------------------

    void spiderpig_function (const std::string& reference_tree_file, const std::string& small_tree_file);

    // ---------------------------------------------------------
    //     Data Members
    // ---------------------------------------------------------

private:

    PlausibilityTree                   reference_tree;
    std::map<unsigned long, Bitvector> bipartition_trees;

};

} // namespace genesis

#endif // include guard

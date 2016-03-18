#ifndef GENESIS_PLACEMENT_FUNCTION_SAMPLE_SET_H_
#define GENESIS_PLACEMENT_FUNCTION_SAMPLE_SET_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include "placement/sample.hpp"
#include "placement/sample_set.hpp"
#include "tree/tree_set.hpp"

#include <iosfwd>
#include <string>

namespace genesis {
namespace placement {

// =================================================================================================
//     Sample Set Functions
// =================================================================================================

Sample      * find_sample( SampleSet      & sset, std::string const& name );
Sample const* find_sample( SampleSet const& sset, std::string const& name );

bool all_identical_trees( SampleSet const& sset );

tree::TreeSet<PlacementTree> tree_set( SampleSet const& sset );

Sample merge_all( SampleSet const& sset );

// =================================================================================================
//     Output
// =================================================================================================

std::ostream& operator << ( std::ostream& out, SampleSet const& sset );

} // namespace placement
} // namespace genesis

#endif // include guard

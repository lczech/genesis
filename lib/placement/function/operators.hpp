#ifndef GENESIS_PLACEMENT_FUNCTION_OPERATORS_H_
#define GENESIS_PLACEMENT_FUNCTION_OPERATORS_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include <iosfwd>
#include <string>

namespace genesis {
namespace placement {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Sample;

// =================================================================================================
//     Comparision and Equality
// =================================================================================================

bool compatible_trees (const Sample& lhs, const Sample& rhs);

// =================================================================================================
//     Verification
// =================================================================================================

bool has_correct_edge_nums( Sample const& map );

// =================================================================================================
//     Output
// =================================================================================================

std::ostream& operator << ( std::ostream& out, Sample const& map );

} // namespace placement
} // namespace genesis

#endif // include guard

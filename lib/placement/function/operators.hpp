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
//     Output
// =================================================================================================

std::ostream& operator << ( std::ostream& out, Sample const& smp );

std::string print_tree( Sample const& smp );

} // namespace placement
} // namespace genesis

#endif // include guard

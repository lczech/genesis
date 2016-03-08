#ifndef GENESIS_UTILS_MATH_MATRIX_OPERATORS_H_
#define GENESIS_UTILS_MATH_MATRIX_OPERATORS_H_

/**
 * @brief Matrix operators.
 *
 * @file
 * @ingroup utils
 */

#include <ostream>

#include "utils/math/matrix.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Matrix Operators
// =================================================================================================

template <typename T>
std::ostream& operator<< (std::ostream& os, const Matrix<T>& matrix)
{
    for (size_t i = 0; i < matrix.rows(); ++i) {
        for (size_t j = 0; j < matrix.cols(); ++j) {
            os << matrix(i, j);
            if (j < matrix.cols() - 1) {
                os << " ";
            }
        }
        os << "\n";
    }
    return os;
}

} // namespace utils
} // namespace genesis

#endif // include guard

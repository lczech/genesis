#ifndef GNS_UTILS_MATH_H_
#define GNS_UTILS_MATH_H_

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include <sstream>

namespace genesis {

// =============================================================================
//     Matrix
// =============================================================================

template <typename value_type>
struct Matrix
{
public:
    Matrix (size_t rows, size_t cols) : rows_(rows), cols_(cols)
    {
        data_ = new double [rows_ * cols_];
    }

    ~Matrix ()
    {
        delete [] data_;
    }

    inline size_t Rows() const
    {
        return rows_;
    }

    inline size_t Cols() const
    {
        return cols_;
    }

    inline size_t size() const
    {
        return rows_ * cols_;
    }

    inline value_type& operator () (const size_t row, const size_t col)
    {
        return data_[row * cols_ + col];
    }

    inline const value_type operator () (const size_t row, const size_t col) const
    {
        return data_[row * cols_ + col];
    }

    inline std::string Dump()
    {
        std::ostringstream out;
        for (size_t i = 0; i < rows_; ++i) {
            for (size_t j = 0; j < cols_; ++j) {
                out << data_[i * cols_ + j] << " ";
            }
            out << "\n";
        }
        return out.str();
    }

protected:

    value_type* data_;
    size_t      rows_;
    size_t      cols_;
};

} // namespace genesis

#endif // include guard

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
        data_ = new double* [rows_];
        for (size_t i = 0; i < rows_; ++i) {
            data_[i] = new double [cols_];
        }
    }

    ~Matrix ()
    {
        for (size_t i = 0; i < rows_; ++i) {
            delete [] data_[i];
        }
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
        return data_[row][col];
    }

    inline const value_type operator () (const size_t row, const size_t col) const
    {
        return data_[row][col];
    }

    inline std::string Dump()
    {
        std::ostringstream out;
        for (size_t i = 0; i < rows_; ++i) {
            for (size_t j = 0; j < cols_; ++j) {
                out << data_[i][j] << " ";
            }
            out << "\n";
        }
        return out.str();
    }

protected:

    value_type** data_;
    size_t       rows_;
    size_t       cols_;
};

} // namespace genesis

#endif // include guard

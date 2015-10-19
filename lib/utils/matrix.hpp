#ifndef GENESIS_UTILS_MATRIX_H_
#define GENESIS_UTILS_MATRIX_H_

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include <algorithm>
#include <sstream>
#include <vector>

namespace genesis {

// =================================================================================================
//     Matrix
// =================================================================================================

template <typename T>
class Matrix
{
public:

    typedef T                                       value_type;
    typedef typename std::vector<T>::iterator       iterator;
    typedef typename std::vector<T>::const_iterator const_iterator;

    Matrix (size_t rows, size_t cols) :
        rows_(rows),
        cols_(cols),
        data_(rows * cols)
    {}

    Matrix (size_t rows, size_t cols, T init) :
        rows_(rows),
        cols_(cols),
        data_(rows * cols, init)
    {}

    Matrix (size_t rows, size_t cols, std::initializer_list<T> init_list) :
        rows_(rows),
        cols_(cols),
        data_(rows * cols)
    {
        size_t i = 0;
        for (T v : init_list) {
            if (i >= data_.size()) {
                break;
            }

            data_[i] = v;
            ++i;
        }
    }

    inline size_t rows() const
    {
        return rows_;
    }

    inline size_t cols() const
    {
        return cols_;
    }

    inline size_t size() const
    {
        return rows_ * cols_;
    }

    inline iterator begin()
    {
        return data_.begin();
    }

    inline iterator end()
    {
        return data_.end();
    }

    inline const_iterator begin() const
    {
        return data_.begin();
    }

    inline const_iterator end() const
    {
        return data_.end();
    }

    inline T& operator () (const size_t row, const size_t col)
    {
        return data_[row * cols_ + col];
    }

    inline const T operator () (const size_t row, const size_t col) const
    {
        return data_[row * cols_ + col];
    }

    inline std::string dump()
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

    size_t         rows_;
    size_t         cols_;
    std::vector<T> data_;
};

// =================================================================================================
//     Operators
// =================================================================================================

template <typename value_type>
bool operator == (const Matrix<value_type>& lhs, const Matrix<value_type>& rhs)
{
    if (lhs.rows() != rhs.rows() || lhs.cols() != rhs.cols()) {
        return false;
    }

    for (size_t i = 0; i < lhs.rows(); ++i) {
        for (size_t j = 0; j < lhs.cols(); ++j) {
            if (lhs(i,j) != rhs(i,j)) {
                return false;
            }
        }
    }

    return true;
}

template <typename value_type>
bool operator != (const Matrix<value_type>& lhs, const Matrix<value_type>& rhs)
{
    return !(lhs == rhs);
}

} // namespace genesis

#endif // include guard

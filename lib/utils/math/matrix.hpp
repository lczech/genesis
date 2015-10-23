#ifndef GENESIS_UTILS_MATH_MATRIX_H_
#define GENESIS_UTILS_MATH_MATRIX_H_

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

    // -------------------------------------------------------------
    //     Typedefs
    // -------------------------------------------------------------

    typedef T                                       value_type;
    typedef typename std::vector<T>::iterator       iterator;
    typedef typename std::vector<T>::const_iterator const_iterator;

    // -------------------------------------------------------------
    //     Constructors
    // -------------------------------------------------------------

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

    // -------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------

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

    inline T& operator () (const size_t row, const size_t col)
    {
        return data_[row * cols_ + col];
    }

    inline const T operator () (const size_t row, const size_t col) const
    {
        return data_[row * cols_ + col];
    }

    // -------------------------------------------------------------
    //     Iterators
    // -------------------------------------------------------------

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

    // -------------------------------------------------------------
    //     Dump and Debug
    // -------------------------------------------------------------

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

    // -------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------

protected:

    size_t         rows_;
    size_t         cols_;
    std::vector<T> data_;
};

// =================================================================================================
//     Operators
// =================================================================================================

template <typename T>
bool operator == (const Matrix<T>& lhs, const Matrix<T>& rhs)
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

template <typename T>
bool operator != (const Matrix<T>& lhs, const Matrix<T>& rhs)
{
    return !(lhs == rhs);
}

} // namespace genesis

#endif // include guard

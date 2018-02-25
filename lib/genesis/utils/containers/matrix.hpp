#ifndef GENESIS_UTILS_CONTAINERS_MATRIX_H_
#define GENESIS_UTILS_CONTAINERS_MATRIX_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/containers/matrix/col.hpp"
#include "genesis/utils/containers/matrix/row.hpp"

#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

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

    using self_type      = Matrix<T>;
    using value_type     = T;

    using container_type = std::vector<T>;
    using iterator       = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    Matrix()
        : rows_(0)
        , cols_(0)
        , data_()
    {}

    Matrix (size_t rows, size_t cols)
        : rows_( rows )
        , cols_( cols )
        , data_( rows * cols )
    {}

    Matrix (size_t rows, size_t cols, T init)
        : rows_( rows )
        , cols_( cols )
        , data_( rows * cols, init )
    {}

    Matrix( size_t rows, size_t cols, std::vector<T> const& data )
        : rows_( rows )
        , cols_( cols )
        , data_( data )
    {
        if( rows * cols != data_.size() ) {
            throw std::length_error(
                std::string(__PRETTY_FUNCTION__) + ": length_error. Expecting " +
                std::to_string( rows ) + " * " + std::to_string( cols ) + " = " +
                std::to_string( rows * cols ) + " elements, but provided data contains " +
                std::to_string( data_.size() ) + " elements."
            );
        }
    }

    Matrix( size_t rows, size_t cols, std::vector<T>&& data )
        : rows_( rows )
        , cols_( cols )
        , data_( std::move( data ))
    {
        if( rows * cols != data_.size() ) {
            throw std::length_error(
                std::string(__PRETTY_FUNCTION__) + ": length_error. Expecting " +
                std::to_string( rows ) + " * " + std::to_string( cols ) + " = " +
                std::to_string( rows * cols ) + " elements, but provided data contains " +
                std::to_string( data_.size() ) + " elements."
            );
        }
    }

    Matrix (size_t rows, size_t cols, std::initializer_list<T> const& init_list)
        : rows_(rows)
        , cols_(cols)
        , data_(rows * cols)
    {
        if (init_list.size() != size()) {
            throw std::length_error(
                std::string(__PRETTY_FUNCTION__) + ": length_error. Expecting " +
                std::to_string( rows ) + " * " + std::to_string( cols ) + " = " +
                std::to_string( rows * cols ) + " elements, but provided data contains " +
                std::to_string( init_list.size() ) + " elements."
            );
        }

        size_t i = 0;
        for (T const& v : init_list) {
            data_[i] = v;
            ++i;
        }
    }

    ~Matrix() = default;

    Matrix(Matrix const&) = default;
    Matrix(Matrix&&)      = default;

    Matrix& operator= (Matrix const&) = default;
    Matrix& operator= (Matrix&&)      = default;

    void swap (Matrix& other)
    {
        using std::swap;
        swap(rows_, other.rows_);
        swap(cols_, other.cols_);
        swap(data_, other.data_);
    }

    friend void swap( Matrix& lhs, Matrix& rhs )
    {
        lhs.swap( rhs );
    }

    // -------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------

    size_t rows() const
    {
        return rows_;
    }

    size_t cols() const
    {
        return cols_;
    }

    size_t size() const
    {
        return rows_ * cols_;
    }

    bool empty() const
    {
        return size() == 0;
    }

    container_type const& data() const
    {
        return data_;
    }

    // -------------------------------------------------------------
    //     Element Access
    // -------------------------------------------------------------

    T& at (const size_t row, const size_t col)
    {
        if (row >= rows_ || col >= cols_) {
            throw std::out_of_range( "Matrix index out of range." );
        }
        return data_[row * cols_ + col];
    }

    T const& at (const size_t row, const size_t col) const
    {
        if (row >= rows_ || col >= cols_) {
            throw std::out_of_range( "Matrix index out of range." );
        }
        return data_[row * cols_ + col];
    }

    T& operator () (const size_t row, const size_t col)
    {
        return data_[row * cols_ + col];
    }

    T const& operator () (const size_t row, const size_t col) const
    {
        return data_[row * cols_ + col];
    }

    // -------------------------------------------------------------
    //     Slicing
    // -------------------------------------------------------------

    MatrixRow<self_type, value_type> row( size_t row )
    {
        if( row >= rows_ ) {
            throw std::out_of_range( "Matrix row index out of range." );
        }

        return MatrixRow<self_type, value_type>( *this, row );
    }

    MatrixRow<const self_type, const value_type> row( size_t row ) const
    {
        if( row >= rows_ ) {
            throw std::out_of_range( "Matrix row index out of range." );
        }

        return MatrixRow<const self_type, const value_type>( *this, row );
    }

    MatrixCol<self_type, value_type> col( size_t col )
    {
        if( col >= cols_ ) {
            throw std::out_of_range( "Matrix column index out of range." );
        }

        return MatrixCol<self_type, value_type>( *this, col );
    }

    MatrixCol<const self_type, const value_type> col( size_t col ) const
    {
        if( col >= cols_ ) {
            throw std::out_of_range( "Matrix column index out of range." );
        }

        return MatrixCol<const self_type, const value_type>( *this, col );
    }

    // -------------------------------------------------------------
    //     Iterators
    // -------------------------------------------------------------

    iterator begin()
    {
        return data_.begin();
    }

    iterator end()
    {
        return data_.end();
    }

    const_iterator begin() const
    {
        return data_.begin();
    }

    const_iterator end() const
    {
        return data_.end();
    }

    const_iterator cbegin() const
    {
        return data_.cbegin();
    }

    const_iterator cend() const
    {
        return data_.cend();
    }

    // -------------------------------------------------------------
    //     Operators
    // -------------------------------------------------------------

    bool operator == ( Matrix<T> const& other ) const
    {
        return rows_ == other.rows_
            && cols_ == other.cols_
            && data_ == other.data_;
    }

    bool operator != ( Matrix<T> const& other ) const
    {
        return !(*this == other);
    }

    // -------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------

private:

    size_t         rows_;
    size_t         cols_;
    container_type data_;
};

} // namespace utils
} // namespace genesis

#endif // include guard

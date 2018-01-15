#ifndef GENESIS_UTILS_CONTAINERS_MATRIX_COL_H_
#define GENESIS_UTILS_CONTAINERS_MATRIX_COL_H_

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

#include "genesis/utils/core/range.hpp"

#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

template <typename T>
class Matrix;

// =================================================================================================
//     Matrix Col
// =================================================================================================

/**
 * @brief View into a Matrix column.
 */
template <typename MT, typename T>
class MatrixCol
{
public:

    // -------------------------------------------------------------
    //     Typedefs
    // -------------------------------------------------------------

    using self_type   = MatrixCol<MT, T>;
    using matrix_type = MT;
    using value_type  = T;

    using non_const_value_type = typename std::remove_const<T>::type;

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    MatrixCol( matrix_type& mat, size_t col )
        : mat_( &mat )
        , col_( col )
    {}

    ~MatrixCol() = default;

    MatrixCol(MatrixCol const&) = default;
    MatrixCol(MatrixCol&&)      = default;

    MatrixCol& operator= (MatrixCol const&) = default;
    MatrixCol& operator= (MatrixCol&&)      = default;

    // -------------------------------------------------------------
    //     Iterator
    // -------------------------------------------------------------

    /**
     * @brief Random access iterator into a Matrix column.
     */
    class Iterator
    {
    public:

        // -----------------------------------------------------
        //     Typedefs
        // -----------------------------------------------------

        using iterator_category = std::random_access_iterator_tag;
        using self_type         = MatrixCol<MT, T>::Iterator;

        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;
        using difference_type   = ptrdiff_t;

        // -----------------------------------------------------
        //     Constructors and Rule of Five
        // -----------------------------------------------------

    private:

        friend class MatrixCol;

        Iterator( matrix_type* mat, size_t row, size_t col )
            : mat_( mat )
            , row_( row )
            , col_( col )
        {}

    public:

        ~Iterator() = default;

        Iterator(Iterator const&) = default;
        Iterator(Iterator&&)      = default;

        Iterator& operator= (Iterator const&) = default;
        Iterator& operator= (Iterator&&)      = default;

        // -----------------------------------------------------
        //     Operators
        // -----------------------------------------------------

        value_type& operator[] ( difference_type n ) const
        {
            return (*mat_)( row_ + n, col_ );
        }

        value_type& operator * () const
        {
            return (*mat_)( row_, col_ );
        }

        value_type* operator -> () const
        {
            return &((*mat_)( row_, col_ ));
        }

        self_type operator ++ ()
        {
            ++row_;
            return *this;
        }

        self_type operator ++ (int)
        {
            self_type tmp = *this;
            ++(*this);
            return tmp;
        }

        self_type operator -- ()
        {
            --row_;
            return *this;
        }

        self_type operator -- (int)
        {
            self_type tmp = *this;
            --(*this);
            return tmp;
        }

        self_type operator += ( difference_type n )
        {
            row_ += n;
            return *this;
        }

        self_type operator -= ( difference_type n )
        {
            row_ -= n;
            return *this;
        }

        friend self_type operator + ( difference_type n, self_type const& it )
        {
            return self_type( it.mat_, it.row_ + n, it.col_ );
        }

        friend self_type operator + ( self_type const& it, difference_type n )
        {
            return self_type( it.mat_, it.row_ + n, it.col_ );
        }

        friend self_type operator - ( self_type const& it, difference_type n )
        {
            return self_type( it.mat_, it.row_ - n, it.col_ );
        }

        difference_type operator - ( self_type const& other ) const
        {
            return row_ - other.row_;
        }

        bool operator == ( self_type const& other ) const
        {
            return other.mat_ == mat_ && other.row_ == row_ && other.col_ == col_;
        }

        bool operator != ( self_type const& other ) const
        {
            return !(other == *this);
        }

        bool operator < ( self_type const& other ) const
        {
            return row_ < other.row_;
        }

        bool operator <= ( self_type const& other ) const
        {
            return row_ <= other.row_;
        }

        bool operator > ( self_type const& other ) const
        {
            return row_ > other.row_;
        }

        bool operator >= ( self_type const& other ) const
        {
            return row_ >= other.row_;
        }

        void swap( self_type& other )
        {
            using std::swap;
            swap( mat_, other.mat_ );
            swap( row_, other.row_ );
            swap( col_, other.col_ );
        }

        friend void swap( self_type& lhs, self_type& rhs )
        {
            lhs.swap( rhs );
        }

        // -----------------------------------------------------
        //     Data Members
        // -----------------------------------------------------

    private:

        matrix_type* mat_ = nullptr;
        size_t       row_ = 0;
        size_t       col_ = 0;

    };

    // -------------------------------------------------------------
    //     Members
    // -------------------------------------------------------------

    /**
     * @brief Return an iterator to the beginning of the row.
     */
    Iterator begin() const
    {
        return Iterator( mat_, 0, col_ );
    }

    /**
     * @brief Return an iterator to past-the-end of the row.
     */
    Iterator end() const
    {
        return Iterator( mat_, mat_->rows(), col_ );
    }

    /**
     * @brief Return the element at a given @p row of the Matrix column.
     */
    value_type& at( size_t row ) const
    {
        return mat_->at( row, col_ );
    }

    /**
     * @brief Return the element at a given @p row of the Matrix column.
     */
    value_type& operator[]( size_t row ) const
    {
        return (*mat_)( row, col_ );
    }

    /**
     * @brief Get the underlying Matrix.
     */
    matrix_type& matrix() const
    {
        return *mat_;
    }

    /**
     * @brief Get the column index that this object represents.
     */
    size_t col() const
    {
        return col_;
    }

    /**
     * @brief Get the size of the column, that is, the number of rows of the Matrix.
     */
    size_t size() const
    {
        return mat_->rows();
    }

    // -------------------------------------------------------------
    //     Interaction Operators
    // -------------------------------------------------------------

    /**
     * @brief Get a copy of the column in form of a `std::vector`.
     */
    operator std::vector<non_const_value_type>() const
    {
        return to_vector();
    }

    /**
     * @brief Explicit conversion to vector.
     */
    std::vector<non_const_value_type> to_vector() const
    {
        auto result = std::vector<non_const_value_type>( mat_->rows() );
        for( size_t i = 0; i < mat_->rows(); ++i ) {
            result[i] = (*mat_)( i, col_ );
        }
        return result;
    }

    /**
     * @brief Overwrite a column by the elements of a `std::vector`.
     *
     * The size of the vector needs to match the number of rows of the Matrix.
     */
    self_type& operator = ( std::vector<T> const& vec )
    {
        if( vec.size() != mat_->rows() ) {
            throw std::runtime_error( "Cannot assign vector with different size to Matrix column." );
        }

        for( size_t i = 0; i < vec.size(); ++i ) {
            (*mat_)( i, col_ ) = vec[i];
        }

        return *this;
    }

    /**
     * @brief Overwrite a column by the elements of another column.
     *
     * The sizes of both columns need to match each other. The given column can also be a column
     * of the same Matrix.
     */
    self_type& assign( self_type const& other )
    {
        if( other.size() != mat_->rows() ) {
            throw std::runtime_error( "Cannot assign column with different size to Matrix column." );
        }

        for( size_t i = 0; i < other.size(); ++i ) {
            (*mat_)( i, col_ ) = other[i];
        }

        return *this;
    }

    // -------------------------------------------------------------
    //     Comparison Operators
    // -------------------------------------------------------------

    /**
     * @brief Return whether the elements of two columns are identical,
     * using `==` for comparing elements.
     */
    bool operator == ( self_type const& other ) const
    {
        // Need to have the same length
        if( mat_->rows() != other.mat_->rows()  ) {
            return false;
        }

        // Element-wise comparison
        for( size_t i = 0; i < mat_->rows(); ++i ) {
            if( !( (*mat_)( i, col_ ) == other[ i ] )) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Return whether the elements of two columns are not identical,
     * using `==` for comparing elements.
     */
    bool operator != ( self_type const& other ) const
    {
        return !(*this == other);
    }

    // -------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------

private:

    matrix_type* mat_ = nullptr;
    size_t       col_ = 0;

};

} // namespace utils
} // namespace genesis

#endif // include guard

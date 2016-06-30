#ifndef GENESIS_UTILS_MATH_TWOBIT_VECTOR_ITERATOR_INSERTIONS_H_
#define GENESIS_UTILS_MATH_TWOBIT_VECTOR_ITERATOR_INSERTIONS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

#include "utils/core/range.hpp"
#include "utils/math/twobit_vector.hpp"
#include "utils/math/twobit_vector/functions.hpp"

#include "utils/core/logging.hpp"

#include <assert.h>
#include <deque>
#include <iterator>

namespace genesis {
namespace utils {

// =================================================================================================
//     Iterator Insertions
// =================================================================================================

class IteratorInsertions
{

public:

    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    using iterator_category = std::forward_iterator_tag;
    using self_type         = IteratorInsertions;
    using value_type        = TwobitVector;

    // -----------------------------------------------------
    //     Constructors and Rule of Five
    // -----------------------------------------------------

    IteratorInsertions ()
        : origin_( nullptr )
        , vec_()
        , pos_( 0 )
        , cnt_( 0 )
        , hash_( 0 )
    {}

    explicit IteratorInsertions( TwobitVector const& vector )
        : origin_( &vector )
        , vec_( vector )
        , pos_( 0 )
        , cnt_( 0 )
    {
        vec_.insert_at( 0, 0 );
        hash_ = vec_.hash();
    }

    ~IteratorInsertions() = default;

    IteratorInsertions( IteratorInsertions const& ) = default;
    IteratorInsertions( IteratorInsertions&& )      = default;

    IteratorInsertions& operator= ( IteratorInsertions const& ) = default;
    IteratorInsertions& operator= ( IteratorInsertions&& )      = default;

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    self_type operator * ()
    {
        return *this;
    }

    value_type const& operator -> ()
    {
        return vec_;
    }

    self_type operator ++ ()
    {
        size_t shift = ( 2 * ( pos_ % TwobitVector::CharsPerWord ));

        if( cnt_ < 3 ) {
            auto const one_shift = static_cast< TwobitVector::WordType >( 1 ) << shift;
            vec_.data_at( pos_ / TwobitVector::CharsPerWord ) += one_shift;

            auto hash_xor   = static_cast< TwobitVector::WordType >( cnt_ ^ ( cnt_ + 1 ) );
            auto hash_shift = hash_xor << shift;
            hash_ ^= hash_shift;

            ++cnt_;

        } else if( pos_ < vec_.size() - 1 ) {
            auto next = vec_.get( pos_ + 1 );
            vec_.set( pos_, next );

            auto hash_xor   = static_cast< TwobitVector::WordType >( 3 ^ next );
            auto hash_shift = hash_xor << shift;
            hash_ ^= hash_shift;

            shift = ( 2 * (( pos_ + 1 ) % TwobitVector::CharsPerWord ));
            hash_xor   = static_cast< TwobitVector::WordType >( next );
            hash_shift = hash_xor << shift;
            hash_ ^= hash_shift;

            ++pos_;
            vec_.set( pos_, 0 );
            cnt_ = 0;

        } else {
            origin_ = nullptr;
            vec_.clear();
            pos_  = 0;
            cnt_  = 0;
            hash_ = 0;
        }

        return *this;
    }

    self_type operator ++ (int)
    {
        self_type tmp = *this;
        ++(*this);
        return tmp;
    }

    bool operator == ( self_type const& other ) const
    {
        return ( origin_ == other.origin_ ) && ( pos_ == other.pos_ ) && ( cnt_ == other.cnt_ );
    }

    bool operator != ( self_type const& other ) const
    {
        return !( other == *this );
    }

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    size_t position() const
    {
        return pos_;
    }

    TwobitVector::WordType hash() const
    {
        return hash_;
    }

    TwobitVector const& vector() const
    {
        return vec_;
    }

private:

    TwobitVector const* origin_;

    TwobitVector vec_;

    size_t pos_;
    size_t cnt_;

    TwobitVector::WordType hash_;

};

// =================================================================================================
//     Range Wrapper
// =================================================================================================

utils::Range< IteratorInsertions > iterate_insertions( TwobitVector const& vector )
{
    return {
        IteratorInsertions( vector ),
        IteratorInsertions()
    };
}

} // namespace utils
} // namespace genesis

#endif // include guard

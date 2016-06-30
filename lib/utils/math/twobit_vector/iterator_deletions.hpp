#ifndef GENESIS_UTILS_MATH_TWOBIT_VECTOR_ITERATOR_DELETIONS_H_
#define GENESIS_UTILS_MATH_TWOBIT_VECTOR_ITERATOR_DELETIONS_H_

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
//     Iterator Deletions
// =================================================================================================

class IteratorDeletions
{

public:

    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    using iterator_category = std::forward_iterator_tag;
    using self_type         = IteratorDeletions;
    using value_type        = TwobitVector;

    // -----------------------------------------------------
    //     Constructors and Rule of Five
    // -----------------------------------------------------

    IteratorDeletions ()
        : origin_( nullptr )
        , vec_()
        , pos_( 0 )
        , cur_( 0u )
        , hash_( 0 )
    {}

    explicit IteratorDeletions( TwobitVector const& vector )
        : origin_( &vector )
        , vec_( vector )
        , pos_( 0 )
        , cur_( vector[ 0 ] )
    {
        vec_.remove_at( 0 );
        hash_ = vec_.hash();
    }

    ~IteratorDeletions() = default;

    IteratorDeletions( IteratorDeletions const& ) = default;
    IteratorDeletions( IteratorDeletions&& )      = default;

    IteratorDeletions& operator= ( IteratorDeletions const& ) = default;
    IteratorDeletions& operator= ( IteratorDeletions&& )      = default;

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
        // Original: ACGT
        // CGT --> pos = 0, cur = A
        // AGT --> pos = 1, cur = C
        // ACT --> pos = 2, cur = G
        // ACG --> pos = 3, cur = T

        if( pos_ < vec_.size() ) {
            auto tmp = vec_[ pos_ ];

            hash_ ^= (
                static_cast<TwobitVector::WordType>( tmp ^ cur_ )
                << ( 2 * ( pos_ % TwobitVector::CharsPerWord ))
            );

            vec_.set( pos_, cur_ );
            ++pos_;
            cur_ = tmp;

        } else {
            origin_ = nullptr;
            vec_.clear();
            pos_  = 0;
            cur_  = 0;
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
        return ( origin_ == other.origin_ ) && ( pos_ == other.pos_ );
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

    size_t                 pos_;
    TwobitVector::CharType cur_;

    TwobitVector::WordType hash_;

};

// =================================================================================================
//     Range Wrapper
// =================================================================================================

utils::Range< IteratorDeletions > iterate_deletions( TwobitVector const& vector )
{
    return {
        IteratorDeletions( vector ),
        IteratorDeletions()
    };
}

} // namespace utils
} // namespace genesis

#endif // include guard

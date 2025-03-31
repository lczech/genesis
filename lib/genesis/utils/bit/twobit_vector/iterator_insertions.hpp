#ifndef GENESIS_UTILS_BIT_TWOBIT_VECTOR_ITERATOR_INSERTIONS_H_
#define GENESIS_UTILS_BIT_TWOBIT_VECTOR_ITERATOR_INSERTIONS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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

#include "genesis/utils/containers/range.hpp"
#include "genesis/utils/bit/twobit_vector.hpp"
#include "genesis/utils/bit/twobit_vector/functions.hpp"

#include <cassert>
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
        // Insert a 0 (=A) value at the first position, and do a first hash calculation.
        // Later iterations will just update all of this.
        vec_.insert_at( 0, TwobitVector::ValueType::A );
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

    value_type const& operator * ()
    {
        return vec_;
    }

    value_type const* operator -> ()
    {
        return &vec_;
    }

    self_type& operator ++ ()
    {
        // Example:
        // Original: CAT
        // ACAT, CCAT, GCAT, TCAT,
        // CAAT, CCAT, CGAT, CTAT,
        // CAAT, CACT, CAGT, CATT,
        // CATA, CATC, CATG, CATT
        //
        // There are duplicates in there. Currently, they are not skipped - this is left as a
        // future optimization.

        // Shorthand.
        size_t shift = ( 2 * ( pos_ % TwobitVector::kValuesPerWord ));

        // If there are still possible insertion values at the current position,
        // use the next value.
        if( cnt_ < 3 ) {

            // If we are not at the last value (11), we can simply move to the next one by
            // adding one to the current position (00 -> 01, 01 -> 10, 10 -> 11).

            // Move a 1 to the position in the word and add it.
            auto const one_shift = static_cast< TwobitVector::WordType >( 0x1 ) << shift;
            vec_.data_at( pos_ / TwobitVector::kValuesPerWord ) += one_shift;

            // Update the hash: Remove the current count value, store the next one.
            auto hash_xor = static_cast< TwobitVector::WordType >( cnt_ ^ ( cnt_ + 1 ) );
            hash_ ^= ( hash_xor << shift );

            ++cnt_;

        // If we used all four possible insertion values at the current position,
        // but if this is not the last possible position, move to the next one.
        } else if( pos_ < vec_.size() - 1 ) {

            // Move the value at the next position one to the left.
            // We can then fill is previous position with the new insertion value.
            auto next = vec_.get( pos_ + 1 );
            vec_.set( pos_, next );

            // Update the hash at the old position: Remove the last value of the insertion
            // (which is a 11 = 3), and store the value that we just moved to that position.
            auto hash_xor   = static_cast< TwobitVector::WordType >( next ) ^ 0x3;
            hash_ ^= ( hash_xor << shift );

            // Move to the next position and recalculate the shift value accordingly.
            ++pos_;
            shift = ( 2 * (( pos_ ) % TwobitVector::kValuesPerWord ));

            // Update the hash at the new position: Remove the value that was there before.
            // We do not need to store a new value here, as it will be a 0 (=A) anyway.
            hash_xor   = static_cast< TwobitVector::WordType >( next );
            hash_ ^= ( hash_xor << shift );

            // Set the value at the new position to 0 (=A) and restart the counter.
            vec_.set( pos_, TwobitVector::ValueType::A );
            cnt_ = 0;

        // If we are done, set everything to zero, so that the iterator
        // is equal to the default-constructed end-iterator.
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

    /**
     * @brief Get the position that is currently deleted.
     */
    size_t position() const
    {
        return pos_;
    }

    /**
     * @brief Get the hash value of the current vector.
     */
    TwobitVector::WordType hash() const
    {
        return hash_;
    }

    /**
     * @brief Get the current vector.
     */
    TwobitVector const& vector() const
    {
        return vec_;
    }

private:

    // Store the location of the original vector. This is mainly used for quickly checking
    // whether two iterators refer to the same underlying vector.
    // (We do not want to do a full vector equality check at each iteration.)
    TwobitVector const* origin_;

    // The current vector, which always has an additional value (compared to the original vector).
    TwobitVector vec_;

    // The position where currently a value is inserted.
    size_t pos_;

    // A counter for the possible insertion values (0-3).
    size_t cnt_;

    // The hash value of the current vector.
    TwobitVector::WordType hash_;

};

// =================================================================================================
//     Range Wrapper
// =================================================================================================

inline utils::Range< IteratorInsertions > iterate_insertions( TwobitVector const& vector )
{
    return {
        IteratorInsertions( vector ),
        IteratorInsertions()
    };
}

} // namespace utils
} // namespace genesis

#endif // include guard

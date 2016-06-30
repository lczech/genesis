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

#include "utils/math/twobit_vector.hpp"

#include "utils/core/logging.hpp"

#include <assert.h>
#include <stdexcept>

namespace genesis {
namespace utils {

// ================================================================================================
//     Typedefs and Constants
// ================================================================================================

const TwobitVector::WordType TwobitVector::all_0_ = 0ul;
const TwobitVector::WordType TwobitVector::all_1_ = (((1ul << 32) - 1) << 32)  +  ((1ul << 32) - 1);

const TwobitVector::WordType TwobitVector::bit_mask_[ TwobitVector::CharsPerWord ] =
{
    3ul << 0,   3ul << 2,   3ul << 4,   3ul << 6,   3ul << 8,   3ul << 10,  3ul << 12,  3ul << 14,
    3ul << 16,  3ul << 18,  3ul << 20,  3ul << 22,  3ul << 24,  3ul << 26,  3ul << 28,  3ul << 30,
    3ul << 32,  3ul << 34,  3ul << 36,  3ul << 38,  3ul << 40,  3ul << 42,  3ul << 44,  3ul << 46,
    3ul << 48,  3ul << 50,  3ul << 52,  3ul << 54,  3ul << 56,  3ul << 58,  3ul << 60,  3ul << 62
};

const TwobitVector::WordType TwobitVector::ones_mask_[ TwobitVector::CharsPerWord ] =
{
    TwobitVector::all_0_,       TwobitVector::all_1_ >> 62,
    TwobitVector::all_1_ >> 60, TwobitVector::all_1_ >> 58,
    TwobitVector::all_1_ >> 56, TwobitVector::all_1_ >> 54,
    TwobitVector::all_1_ >> 52, TwobitVector::all_1_ >> 50,
    TwobitVector::all_1_ >> 48, TwobitVector::all_1_ >> 46,
    TwobitVector::all_1_ >> 44, TwobitVector::all_1_ >> 42,
    TwobitVector::all_1_ >> 40, TwobitVector::all_1_ >> 38,
    TwobitVector::all_1_ >> 36, TwobitVector::all_1_ >> 34,
    TwobitVector::all_1_ >> 32, TwobitVector::all_1_ >> 30,
    TwobitVector::all_1_ >> 28, TwobitVector::all_1_ >> 26,
    TwobitVector::all_1_ >> 24, TwobitVector::all_1_ >> 22,
    TwobitVector::all_1_ >> 20, TwobitVector::all_1_ >> 18,
    TwobitVector::all_1_ >> 16, TwobitVector::all_1_ >> 14,
    TwobitVector::all_1_ >> 12, TwobitVector::all_1_ >> 10,
    TwobitVector::all_1_ >> 8,  TwobitVector::all_1_ >> 6,
    TwobitVector::all_1_ >> 4,  TwobitVector::all_1_ >> 2
};

// ================================================================================================
//     Constructors and Rule of Five
// ================================================================================================

TwobitVector::TwobitVector( size_t size )
    : size_( size )
    , data_( ( size / CharsPerWord ) + ( size % CharsPerWord == 0 ? 0 : 1 ), 0 )
{}

// ================================================================================================
//     Accessors
// ================================================================================================

size_t TwobitVector::size() const
{
    return size_;
}

size_t TwobitVector::data_size() const
{
    assert( ( size_ / CharsPerWord ) + ( size_ % CharsPerWord == 0 ? 0 : 1 ) == data_.size() );
    return data_.size();
}

TwobitVector::CharType TwobitVector::get( size_t index ) const
{
    if (index >= size_) {
        throw std::out_of_range( "TwobitVector::get: Invalid index." );
    }

    auto segment = data_[ index / CharsPerWord ] & bit_mask_[ index % CharsPerWord ];
    auto value   = static_cast< CharType >( segment >> ( 2 * ( index % CharsPerWord )));
    assert( value <= 3 );
    return value;
}

TwobitVector::CharType TwobitVector::operator [] ( size_t index ) const
{
    return get( index );
}

TwobitVector::WordType const& TwobitVector::data_at( size_t index ) const
{
    return data_.at( index );
}

TwobitVector::WordType& TwobitVector::data_at( size_t index )
{
    return data_.at( index );
}

TwobitVector::WordType TwobitVector::hash() const
{
    WordType result = static_cast< WordType >( size() );
    for( auto s : data_ ) {
        result ^= s;
    }
    return result;
}

// ================================================================================================
//     Operators
// ================================================================================================

bool TwobitVector::operator == ( TwobitVector const& other ) const
{
    if( size_ != other.size_ ) {
        return false;
    }
    for( size_t i = 0; i < data_.size(); ++i ) {
        if( data_[i] != other.data_[i] ) {
            return false;
        }
    }
    return true;
}

bool TwobitVector::operator != ( TwobitVector const& other ) const
{
    return !(*this == other);
}

// ================================================================================================
//     Modifiers
// ================================================================================================

void TwobitVector::set( size_t index, TwobitVector::CharType value )
{
    if( index >= size_ ) {
        throw std::out_of_range( "TwobitVector::set: Invalid index." );
    }
    check_char( value );

    auto const tmp = static_cast< WordType >( value ) << ( 2 * ( index % CharsPerWord ));
    data_[ index / CharsPerWord ] &= ~ bit_mask_[ index % CharsPerWord ];
    data_[ index / CharsPerWord ] |= tmp;
}

void TwobitVector::insert_at( size_t index, TwobitVector::CharType value )
{
    if( index > size_ ) {
        throw std::out_of_range( "TwobitVector::insert_at: Invalid index." );
    }

    size_t const word_id = index / CharsPerWord;
    size_t const segm_id = index % CharsPerWord;

    if( size_ % CharsPerWord == 0 ) {
        data_.push_back( 0ul );
    }

    for( size_t i = data_.size() - 1; i > word_id; --i ) {
        data_[ i ] <<= 2;
        auto bleed = data_[ i - 1 ] >> ( sizeof( WordType ) * 8 - 2 );
        data_[ i ] |= bleed;
    }

    auto const remainder = data_[ word_id ] & ( ~ ones_mask_[ segm_id ] );
    data_[ word_id ] &= ones_mask_[ segm_id ];
    data_[ word_id ] |= ( remainder << 2 );

    auto const val_shifted = static_cast< WordType >( value ) << ( 2 * segm_id );
    data_[ word_id ] |= val_shifted;
    ++size_;
}

void TwobitVector::remove_at( size_t index )
{
    if( index >= size_ ) {
        throw std::out_of_range( "TwobitVector::remove_at: Invalid index." );
    }

    size_t const word_id = index / CharsPerWord;
    size_t const segm_id = index % CharsPerWord;

    if( segm_id < CharsPerWord - 1 ) {
        auto remainder = data_[ word_id ] & ( ~ ones_mask_[ segm_id + 1 ] );
        data_[ word_id ] &= ones_mask_[ segm_id ];
        data_[ word_id ] |= ( remainder >> 2 );
    } else {
        data_[ word_id ] &= ones_mask_[ segm_id ];
    }

    if( word_id < data_.size() - 1 ) {
        auto bleed = data_[ word_id + 1 ] << ( sizeof( WordType ) * 8 - 2 );
        data_[ word_id ] |= bleed;

        for( size_t i = word_id + 1; i < data_.size() - 1; ++i ) {
            bleed = data_[ i + 1 ] << ( sizeof( WordType ) * 8 - 2 );
            data_[ i ] >>= 2;
            data_[ i ] |= bleed;
        }
        data_.back() >>= 2;
    }

    --size_;
    if( size_ % CharsPerWord == 0 ) {
        data_.pop_back();
    }
    assert( ( size_ / CharsPerWord ) + ( size_ % CharsPerWord == 0 ? 0 : 1 ) == data_.size() );
}

void TwobitVector::clear()
{
    size_ = 0;
    data_.clear();
}

// ================================================================================================
//     Internal Members
// ================================================================================================

void TwobitVector::check_char( CharType value ) const
{
    // At maximum the first three bits can be set.
    if( value > 3 ) {
        throw std::runtime_error( "Invalid char." );
    }
}

bool TwobitVector::validate() const
{
    if( ( size_ / CharsPerWord ) + ( size_ % CharsPerWord == 0 ? 0 : 1 ) != data_.size() ) {
        LOG_INFO << "Sizes does not match.";
        return false;
    }
    if(
        ( size_ % CharsPerWord != 0 )
        && ( ( data_.back() & ~ ones_mask_[ size_ % CharsPerWord ] ) != 0ul )
    ) {
        LOG_INFO << "Invalid padding bits.";
        return false;
    }

    return true;
}

} // namespace utils
} // namespace genesis

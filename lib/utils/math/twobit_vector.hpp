#ifndef GENESIS_UTILS_MATH_TWOBIT_VECTOR_H_
#define GENESIS_UTILS_MATH_TWOBIT_VECTOR_H_

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

#include <stddef.h>
#include <stdint.h>
#include <vector>

namespace genesis {
namespace utils {

class TwobitVector
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Constants
    // -------------------------------------------------------------------------

    using WordType = uint64_t;
    using CharType = unsigned char;

    static const size_t CharsPerWord = sizeof( WordType ) * 8 / 2;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    TwobitVector() = default;
    TwobitVector( size_t size );

    ~TwobitVector() = default;

    TwobitVector( TwobitVector const& ) = default;
    TwobitVector( TwobitVector&& )      = default;

    TwobitVector& operator= ( TwobitVector const& ) = default;
    TwobitVector& operator= ( TwobitVector&& )      = default;

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    size_t size() const;
    size_t data_size() const;

    CharType get( size_t index ) const;

    CharType operator [] ( size_t index ) const;

    WordType const& data_at( size_t index ) const;
    WordType&       data_at( size_t index );

    WordType hash() const;

    // -------------------------------------------------------------------------
    //     Operators
    // -------------------------------------------------------------------------

    bool operator == ( TwobitVector const& other ) const;
    bool operator != ( TwobitVector const& other ) const;

    bool validate() const;

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    void set( size_t index, CharType value );

    void insert_at( size_t index, CharType value );
    void remove_at( size_t index );

    void clear();

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    void check_char( CharType value ) const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    static const WordType all_0_;
    static const WordType all_1_;

    static const WordType bit_mask_[ CharsPerWord ];
    static const WordType ones_mask_[ CharsPerWord ];

    size_t                size_ = 0;
    std::vector<WordType> data_;

};

} // namespace utils
} // namespace genesis

#endif // include guard

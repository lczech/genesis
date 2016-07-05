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

    /**
     * @brief Underlying word type for the bitvector.
     *
     * We use 64bit words to store the 2bit values (of type ValueType), so that we get best
     * speed on modern architectures.
     */
    using WordType = uint64_t;

    /**
     * @brief Value Type enumeration for the elements of a TwobitVector.
     *
     * The values 0-3 are named `A`, `C`, `G` and `T`, respectively, in order to ease the
     * use with nucleotide sequences.
     *
     * The underlying value of the enum is WordType, so that a cast does not need to
     * convert internally.
     */
    enum class ValueType : WordType {
        A = 0,
        C = 1,
        G = 2,
        T = 3
    };

    /**
     * @brief Constant that holds the number of values (of tyoe ValueType) that are stored
     * in a single word in the vector.
     *
     * As we use 64bit words, this value is 32.
     */
    static const size_t kValuesPerWord = sizeof( WordType ) * 8 / 2;

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

    ValueType get( size_t index ) const;

    ValueType operator [] ( size_t index ) const;

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

    void set( size_t index, ValueType value );

    void insert_at( size_t index, ValueType value );
    void remove_at( size_t index );

    void clear();

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    static const WordType all_0_;
    static const WordType all_1_;

    static const WordType bit_mask_[  kValuesPerWord ];
    static const WordType ones_mask_[ kValuesPerWord ];

    size_t                size_ = 0;
    std::vector<WordType> data_;

};

} // namespace utils
} // namespace genesis

#endif // include guard

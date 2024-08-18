#ifndef GENESIS_SEQUENCE_KMER_MICROVARIANT_SCANNER_H_
#define GENESIS_SEQUENCE_KMER_MICROVARIANT_SCANNER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "genesis/sequence/kmer/kmer.hpp"
#include "genesis/utils/containers/range.hpp"

#include <array>
#include <cassert>
#include <climits>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Kmer Microvariant Scanner
// =================================================================================================

/**
 * @brief Iterate all microvariants (adjacent kmers with a single mismatch) of a Kmer.
 *
 * The scanner takes a Kmer as input, and offers to iterate through all Kmers that have exactly
 * one mismatching base, which we call its microvariants. For instance, with Kmer "CAT`", which
 * has size 3, there are 3*3=9 microvariants, one for each position and alternanative base:
 *
 *     CAT:
 *     AAT GAT TAT (microvariants first position)
 *     CCT CGT CTT (second)
 *     CAA CAC CAG (third)
 *
 * By default, the first step of the iteration is the original Kmer, so that when using this class
 * to iterate a Kmer and all its microvariants, this can be done in one loop. Note that we leave the
 * order of the microvariants implementation-defined, as this might be changed for optimizations.
 */
template<typename Tag>
class MicrovariantScanner
{
public:

    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    using self_type         = MicrovariantScanner;
    using value_type        = Kmer<Tag>;
    using pointer           = value_type*;
    using reference         = value_type&;
    using iterator_category = std::forward_iterator_tag;

    // -----------------------------------------------------
    //     Constructors and Rule of Five
    // -----------------------------------------------------

    MicrovariantScanner()
        : kmer_( 0 )
    {}

    explicit MicrovariantScanner( Kmer<Tag> const& kmer, bool include_original = true )
        : kmer_( kmer )
        , pos_( 0 )
        , cnt_( 0 )
    {
        // If we do not want the original kmer, iterate to the first microvariant.
        if( ! include_original ) {
            increment_();
        }
    }

    ~MicrovariantScanner() = default;

    MicrovariantScanner( MicrovariantScanner const& ) = default;
    MicrovariantScanner( MicrovariantScanner&& )      = default;

    MicrovariantScanner& operator= ( MicrovariantScanner const& ) = default;
    MicrovariantScanner& operator= ( MicrovariantScanner&& )      = default;

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    value_type const& operator * ()
    {
        return kmer_;
    }

    value_type const* operator -> ()
    {
        return &kmer_;
    }

    self_type& operator ++ ()
    {
        increment_();
        return *this;
    }

    // self_type operator ++ (int)
    // {
    //     self_type tmp = *this;
    //     ++(*this);
    //     return tmp;
    // }

    bool operator == ( self_type const& other ) const
    {
        return ( pos_ == other.pos_ ) && ( cnt_ == other.cnt_ );
    }

    bool operator != ( self_type const& other ) const
    {
        return !( other == *this );
    }

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    /**
     * @brief Get the position that is substituted in the current microvariant.
     */
    uint8_t position() const
    {
        return pos_;
    }

    /**
     * @brief Get the number of the microvariant that is currently substituted at the position.
     */
    uint8_t cycle() const
    {
        return cnt_;
    }

    /**
     * @brief Get the current k-mer microvariant.
     */
    Kmer<Tag> const& kmer() const
    {
        return kmer_;
    }

    // -----------------------------------------------------
    //     Internal Member Functions
    // -----------------------------------------------------

private:

    void increment_()
    {
        // Check assumptions of this function.
        using Bitfield = typename Kmer<Tag>::Bitfield;
        using WordType = typename Kmer<Tag>::Bitfield::WordType;
        static_assert( Bitfield::BITS_PER_CHAR == 2, "Kmer<Tag>::BITS_PER_CHAR != 2" );

        // We use four xor steps at the current position to cycle through the variants:
        // The first thee are the substitutions, the last one then restores the original value.
        // For this, we use the xor order 01 11 01 11.
        //
        // The table shows that this works for all four possible values. Each row is one step
        // in the cycle, applying the xor order to the four possible starting values.
        // Note that this does not result in lexicographical order of the microvariants.
        //
        //     Start:    00 | 01 | 10 | 11
        //              ----|----|----|----
        //     0: 01     01 | 00 | 11 | 10
        //     1: 11     10 | 11 | 00 | 01
        //     2: 01     11 | 10 | 01 | 00
        //     3: 11     00 | 01 | 10 | 11
        //
        // Luckily, for two values that are the complement of each other (AT and GC), the order
        // above also keeps the microvariants applied to both of them as each others complements.
        // That is, starting with an A, whose rc is T, we iterte the A as ACGTA and the T as TGCAT.
        // In each of those steps, the characters are their complement in these two lists.
        // Thus, we can easily update the rc of the kmer using the values from the table as well.

        // Helper function that cycles the value at a position, using the above table.
        auto cycle_next_microvariant_ = [&]( uint8_t pos, uint8_t& cnt )
        {
            // Move the needed xor value to the position in the word and apply it.
            assert( pos < kmer_.k() );
            WordType const xor_val = ( cnt % 2 == 0 ? 0x1 : 0x3 );
            kmer_.value    ^= ( xor_val << ( Bitfield::BITS_PER_CHAR * pos ));
            kmer_.rev_comp ^= ( xor_val << ( Bitfield::BITS_PER_CHAR * ( kmer_.k() - pos - 1 )));
            ++cnt;
        };

        // Do at least one cycle at the current position.
        cycle_next_microvariant_( pos_, cnt_ );

        // Check if this was the last cycle, where we already did all three possible substitutions
        // at the current position, and are now back where we started.
        if( cnt_ == 4 ) {

            // If this is not the last possible position, move to the next one.
            // Otherwise, we are done, indicated by setting everything to the default.
            assert( kmer_.k() > 0 );
            if( pos_ < kmer_.k() - 1 ) {
                ++pos_;
                cnt_ = 0;
                cycle_next_microvariant_( pos_, cnt_ );
            } else {
                pos_ = std::numeric_limits<uint8_t>::max();
                cnt_ = std::numeric_limits<uint8_t>::max();
            }
        }
    }

    // -----------------------------------------------------
    //     Internal Members
    // -----------------------------------------------------

private:

    // Assumptions on the underlying representation. Needed for later
    // to avoid mistakes, if we introduce different kmer types.
    // Anything that fails here means that the class is used with
    // an incompatible bit representation or alphabet encoding.
    static_assert(
        Kmer<Tag>::Bitfield::BITS_PER_CHAR == 2,
        "Kmer<Tag>::Bitfield::BITS_PER_CHAR != 2"
    );
    static_assert(
        Kmer<Tag>::Alphabet::SIZE == 4,
        "Kmer<Tag>::Alphabet::SIZE != 4"
    );
    static_assert(
        Kmer<Tag>::Alphabet::NEGATE_IS_COMPLEMENT,
        "Kmer<Tag>::Alphabet::NEGATE_IS_COMPLEMENT != true"
    );

    // The current k-mer
    Kmer<Tag> kmer_;

    // The position where currently a value is substitutioned, and the counter for the possible
    // microvariant (substitution) possibilities at the position.
    uint8_t pos_ = std::numeric_limits<uint8_t>::max();
    uint8_t cnt_ = std::numeric_limits<uint8_t>::max();

};

// =================================================================================================
//     Range Wrapper
// =================================================================================================

template<typename Tag>
inline utils::Range<MicrovariantScanner<Tag>> iterate_microvariants(
    Kmer<Tag> const& kmer, bool include_original = true
) {
    return {
        MicrovariantScanner<Tag>( kmer, include_original ),
        MicrovariantScanner<Tag>()
    };
}

} // namespace sequence
} // namespace genesis

#endif // include guard

#ifndef GENESIS_SEQUENCE_KMER_EXTRACTOR_H_
#define GENESIS_SEQUENCE_KMER_EXTRACTOR_H_

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
#include "genesis/utils/core/std.hpp"

#include <array>
#include <cassert>
#include <climits>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>

#if GENESIS_CPP_STD >= GENESIS_CPP_STD_17
#    include <string_view>
#endif

namespace genesis {
namespace sequence {

// =================================================================================================
//     Kmer Extractor
// =================================================================================================

/**
 * @brief
 */
class KmerExtractor
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type         = KmerExtractor;
    using value_type        = Kmer;
    using pointer           = value_type*;
    using reference         = value_type&;
    using iterator_category = std::input_iterator_tag;

    using Alphabet = typename Kmer::Alphabet;
    using Bitfield = typename Kmer::Bitfield;

    // ======================================================================================
    //      Internal Iterator
    // ======================================================================================

public:

    class Iterator
    {
        // -------------------------------------------------------------------------
        //     Typedefs and Enums
        // -------------------------------------------------------------------------

    public:

        using self_type         = KmerExtractor::Iterator;
        using value_type        = Kmer;
        using pointer           = value_type const*;
        using reference         = value_type const&;
        using iterator_category = std::input_iterator_tag;

        using Alphabet = typename Kmer::Alphabet;
        using Bitfield = typename Kmer::Bitfield;
        using WordType = typename Bitfield::WordType;

        // -------------------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------------------

    private:

        Iterator() = default;

        Iterator( KmerExtractor const* parent )
            : parent_( parent )
        {
            // Safeguard
            if( ! parent_ ) {
                return;
            }

            // Correct setup
            auto const k = parent_->k_;
            assert( k > 0 && k <= Bitfield::MAX_CHARS_PER_KMER );
            kmer_ = Kmer( k, 0 );

            // Precomputed values
            ones_mask_ = Bitfield::ones_mask[ k ];
            rc_shift_ = (( k - 1 ) * Bitfield::BITS_PER_CHAR );

            // Start streaming the data
            assert( location_ == 0 );
            init_kmer_from_current_location_();
        }

    public:

        ~Iterator() = default;

        Iterator( self_type const& ) = default;
        Iterator( self_type&& )      = default;

        Iterator& operator= ( self_type const& ) = default;
        Iterator& operator= ( self_type&& )      = default;

        friend KmerExtractor;

        // -------------------------------------------------------------------------
        //     Iterator Accessors
        // -------------------------------------------------------------------------

        value_type const* operator->() const
        {
            return &kmer_;
        }

        value_type const& operator*() const
        {
            return kmer_;
        }

        // -------------------------------------------------------------------------
        //     Iteration
        // -------------------------------------------------------------------------

        self_type& operator ++ ()
        {
            increment_();
            return *this;
        }

        bool operator==( self_type const& other ) const
        {
            return parent_ == other.parent_;
        }

        bool operator!=( self_type const& other ) const
        {
            return !(*this == other);
        }

        // -------------------------------------------------------------------------
        //     Internal Members
        // -------------------------------------------------------------------------

    private:

        // ---------------------------------------------
        //     Increment and Processing Samples
        // ---------------------------------------------

        inline void increment_()
        {
            assert( parent_ );

            // location_ points to the index in the input sequence that we are processing next.
            // Test for end of iteration. Signal this by unsetting the parent,
            // which is used in the equality comparison to check for the end iterator.
            if( location_ >= parent_->input_.size() ) GENESIS_CPP_UNLIKELY {
                parent_ = nullptr;
                return;
            }

            // Now try to process the char at the current location.
            // If that works, we have moved to the next location in the input and are done.
            // If not, we found an invalid character, and will have to start a new kmer.
            if( process_current_char_() ) GENESIS_CPP_LIKELY {
                ++kmer_.location;
            } else GENESIS_CPP_UNLIKELY {
                init_kmer_from_current_location_();
            }
        }

        inline void init_kmer_from_current_location_()
        {
            assert( parent_ );

            // Shorthands.
            auto const k = parent_->k_;
            assert( k <= Bitfield::MAX_CHARS_PER_KMER );

            size_t start_location = location_;
            size_t length = 0;
            while( true ) {
                // Test for whether we are done populating this kmer. If so, we can stop.
                if( length == k ) {
                    break;
                }

                // Test for end of iteration. Signal this by unsetting the parent.
                // This will process the rest of the input for completeness, even though we might
                // already know that start_location + k > parent_->input_.size() (meaning, we will
                // reach the end of the sequence before the kmer is filled). We do this so that the
                // char counts are correct. This function here is called only initally, and if
                // there is an invalid char in the input, and that happening in the last k chars
                // of the input is probably rare enough that it does not matter in terms of speed.
                if( location_ >= parent_->input_.size() ) {
                    parent_ = nullptr;
                    return;
                }

                // Process one character from the input. This returns if that was valid.
                // If not, we start with a new kmer.
                if( !process_current_char_() ) {
                    start_location = location_;
                    length = 0;
                    continue;
                }

                ++length;
            }
            assert( length == k );
            assert( start_location + k == location_ );

            // Store the starting location of the kmer, for downstream algorithms
            // that can optimize if we only moved a single character.
            kmer_.location = start_location;
        }

        inline bool process_current_char_()
        {
            assert( parent_ );

            // Get the next bits to use. If it is invalid, we let the caller know.
            assert( location_ < parent_->input_.size() );
            auto const rank = Alphabet::char_to_rank( parent_->input_[ location_ ] );
            ++location_;
            if( rank > Alphabet::MAX_RANK ) GENESIS_CPP_UNLIKELY {
                ++(parent_->invalid_character_count_);
                return false;
            }

            // Move the new value into the kmer.
            auto const word = static_cast<WordType>( rank );
            kmer_.value <<= Bitfield::BITS_PER_CHAR;
            kmer_.value &=  ones_mask_;
            kmer_.value |=  word;

            // Also populate the reverse complement.
            WordType const rc_word = Alphabet::complement( rank );
            kmer_.rev_comp >>= Bitfield::BITS_PER_CHAR;
            kmer_.rev_comp |= ( rc_word << rc_shift_ );

            // Successfully processed the char.
            ++(parent_->valid_character_count_);
            return true;
        }

        // -------------------------------------------------------------------------
        //     Data Members
        // -------------------------------------------------------------------------

    private:

        // Parent. If null, this indicates the end of the input and that we are done iterating.
        KmerExtractor const* parent_ = nullptr;

        // The internal kmer we operate on, and the current location in the input sequence.
        // This corresponds to the char that is extracted from the sequence at the end of the kmer.
        Kmer kmer_;
        size_t location_ = 0;

        // Precomputed values for speed
        WordType ones_mask_;
        int rc_shift_ = 0;

    };

    // ======================================================================================
    //      Main Class
    // ======================================================================================

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    KmerExtractor() = default;

    // With C++17, we can optimize by offering a std::string_view interface.
    #if GENESIS_CPP_STD >= GENESIS_CPP_STD_17

    KmerExtractor( uint8_t k, std::string const& input )
        : buffer_( input )
        , input_( buffer_ )
    {
        set_k_( k );
    }

    KmerExtractor( uint8_t k, std::string&& input )
        : buffer_( std::move( input ))
        , input_( buffer_ )
    {
        set_k_( k );
    }

    KmerExtractor( uint8_t k, std::string_view input )
        : input_( input )
    {
        set_k_( k );
    }

    // For C++11, we instead just offer std::string
    #else // GENESIS_CPP_STD >= GENESIS_CPP_STD_17

    KmerExtractor( uint8_t k, std::string const& input )
        : input_( input )
    {
        set_k_( k );
    }

    KmerExtractor( uint8_t k, std::string&& input )
        : input_( std::move( input ))
    {
        set_k_( k );
    }

    #endif // GENESIS_CPP_STD >= GENESIS_CPP_STD_17

    ~KmerExtractor() = default;

    KmerExtractor( self_type const& ) = default;
    KmerExtractor( self_type&& )      = default;

    self_type& operator= ( self_type const& ) = default;
    self_type& operator= ( self_type&& )      = default;

    // -------------------------------------------------------------------------
    //     Iteration
    // -------------------------------------------------------------------------

    Iterator begin() const
    {
        return Iterator( this );
    }

    Iterator end() const
    {
        return Iterator();
    }

    // -------------------------------------------------------------------------
    //     Statistics
    // -------------------------------------------------------------------------

    size_t valid_character_count() const
    {
        return valid_character_count_;
    }

    size_t invalid_character_count() const
    {
        return invalid_character_count_;
    }

    void reset_character_counts()
    {
        valid_character_count_ = 0;
        invalid_character_count_ = 0;
    }

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    void set_k_( uint8_t k )
    {
        if( k == 0 || k > Bitfield::MAX_CHARS_PER_KMER ) {
            throw std::invalid_argument(
                "Cannot use k-mer with k==" + std::to_string( k )
            );
        }
        k_ = k;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    uint8_t k_;

    #if GENESIS_CPP_STD >= GENESIS_CPP_STD_17

    // We use a string as a buffer if the class was initialized with a std::string,
    // so that we can keep ownership of (a copy of) the input if needed. If not needed,
    // we instead can use a string_view, which is cheaper. Interally, either way, we are
    // only using the string_view, pointing to either the buffer, or the given input view.
    std::string buffer_;
    std::string_view input_;

    #else // GENESIS_CPP_STD >= GENESIS_CPP_STD_17

    // For C++11, we just use a normal string
    std::string input_;

    #endif // GENESIS_CPP_STD >= GENESIS_CPP_STD_17

    // Count data during iteration
    mutable size_t valid_character_count_ = 0;
    mutable size_t invalid_character_count_ = 0;

};

} // namespace sequence
} // namespace genesis

#endif // include guard

#ifndef GENESIS_SEQUENCE_KMER_KMER_SCANNER_H_
#define GENESIS_SEQUENCE_KMER_KMER_SCANNER_H_

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

#include <array>
#include <cassert>
#include <climits>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
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
template<typename Tag>
class KmerExtractor
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type         = KmerExtractor;
    using value_type        = Kmer<Tag>;
    using pointer           = value_type*;
    using reference         = value_type&;
    using iterator_category = std::input_iterator_tag;

    using Alphabet = typename Kmer<Tag>::Alphabet;
    using Bitfield = typename Kmer<Tag>::Bitfield;

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
        using value_type        = Kmer<Tag>;
        using pointer           = value_type const*;
        using reference         = value_type const&;
        using iterator_category = std::input_iterator_tag;

        using Alphabet = typename Kmer<Tag>::Alphabet;
        using Bitfield = typename Kmer<Tag>::Bitfield;

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
            if( Kmer<Tag>::k() == 0 || Kmer<Tag>::k() > Bitfield::MAX_CHARS_PER_KMER ) {
                throw std::invalid_argument(
                    "Cannot extract kmers with k=" + std::to_string( Kmer<Tag>::k() )
                );
            }

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

        void increment_()
        {
            assert( parent_ );

            // location_ points to the index in the input sequence that we are processing next.
            // Test for end of iteration. Signal this by unsetting the parent,
            // which is used in the equality comparison to check for the end iterator.
            if( location_ >= parent_->input_.size() ) {
                parent_ = nullptr;
                return;
            }

            // Now try to process the char at the current location.
            // If that works, we have moved to the next location in the input and are done.
            // If not, we found an invalid character, and will have to start a new kmer.
            if( process_current_char_() ) {
                ++kmer_.location;
            } else {
                init_kmer_from_current_location_();
            }
        }

        void init_kmer_from_current_location_()
        {
            assert( parent_ );

            // Shorthands.
            auto const k = Kmer<Tag>::k();
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

        bool process_current_char_()
        {
            // Shorthands and checks.
            auto const k = Kmer<Tag>::k();
            assert( k > 0 );
            assert( parent_ );

            // Get the next bits to use. If it is invalid, we let the caller know.
            assert( location_ < parent_->input_.size() );
            auto const rank = Alphabet::char_to_rank( parent_->input_[ location_ ] );
            ++location_;
            if( rank > Alphabet::MAX_RANK ) {
                ++(parent_->count_invalid_characters_);
                return false;
            }

            // Move the new value into the kmer.
            auto const word = static_cast<typename Bitfield::WordType>( rank );
            kmer_.value <<= Bitfield::BITS_PER_CHAR;
            kmer_.value &=  Bitfield::ones_mask[ k ];
            kmer_.value |=  word;

            // Also populate the reverse complement.
            typename Bitfield::WordType const rc_word = Alphabet::complement( rank );
            kmer_.rev_comp >>= Bitfield::BITS_PER_CHAR;
            kmer_.rev_comp |= ( rc_word << (( k - 1 ) * Bitfield::BITS_PER_CHAR ));

            // Successfully processed the char.
            ++(parent_->count_valid_characters_);
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
        Kmer<Tag> kmer_;
        size_t location_ = 0;
    };

    // ======================================================================================
    //      Main Class
    // ======================================================================================

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    // With C++17, we can optimize by offering a std::string_view interface.
    #if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)

    KmerExtractor( std::string const& input )
        : buffer_( input )
        , input_( buffer_ )
    {}

    KmerExtractor( std::string&& input )
        : buffer_( std::move( input ))
        , input_( buffer_ )
    {}

    KmerExtractor( std::string_view input )
        : input_( input )
    {}

    // For C++11, we instead just offer std::string
    #else // ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)

    KmerExtractor( std::string const& input )
        : input_( input )
    {}

    KmerExtractor( std::string&& input )
        : input_( std::move( input ))
    {}

    #endif // ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)

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

    size_t count_valid_characters() const
    {
        return count_valid_characters_;
    }

    size_t count_invalid_characters() const
    {
        return count_invalid_characters_;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    #if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)

    // We use a string as a buffer if the class was initialized with a std::string,
    // so that we can keep ownership of (a copy of) the input if needed. If not needed,
    // we instead can use a string_view, which is cheaper. Interally, either way, we are
    // only using the string_view, pointing to either the buffer, or the given input view.
    std::string buffer_;
    std::string_view input_;

    #else // ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)

    // For C++11, we just use a normal string
    std::string input_;

    #endif // ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)

    // Count data during iteration
    mutable size_t count_valid_characters_ = 0;
    mutable size_t count_invalid_characters_ = 0;

};

} // namespace sequence
} // namespace genesis

#endif // include guard

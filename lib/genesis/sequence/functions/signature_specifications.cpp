/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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
 * @ingroup sequence
 */

#include "genesis/sequence/functions/signature_specifications.hpp"

#include "genesis/sequence/functions/codes.hpp"
#include "genesis/utils/math/common.hpp"

#include <algorithm>
#include <cassert>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <unordered_map>

namespace genesis {
namespace sequence {

// ================================================================================================
//     Constructors and Rule of Five
// ================================================================================================

size_t const SignatureSpecifications::InvalidCharIndex = std::numeric_limits<size_t>::max();

SignatureSpecifications::SignatureSpecifications( std::string const& alphabet, size_t k )
{
    alphabet_ = normalize_code_alphabet( alphabet );
    k_ = k;
    is_nucleic_acids_ = ( alphabet_ == "ACGT" );

    // Size checks.
    if( alphabet_.size() == 0 ) {
        throw std::invalid_argument( "Invalid alphabet for kmer list." );
    }
    if( ! utils::is_valid_int_pow( alphabet_.size(), k ) ) {
        throw std::invalid_argument( "Cannot store kmers for such large k." );
    }
    if( k == 0 ) {
        throw std::invalid_argument( "Invalid k==0 for kmer list." );
    }

    // Create lookup from char to its index in the alphabet.
    index_lookup_ = utils::CharLookup<size_t>( InvalidCharIndex );
    for( size_t i = 0; i < alphabet_.size(); ++i ) {
        index_lookup_.set_char_upper_lower( alphabet_[i], i );
    }
}

// ================================================================================================
//     Derived Properties
// ================================================================================================

std::vector<std::string> const& SignatureSpecifications::kmer_list() const
{
    // Lazy loading.
    if( kmer_list_.size() > 0 ) {
        return kmer_list_;
    }

    // Get some settings.
    auto const& w = alphabet();
    size_t const p = kmer_list_size();

    // Result vector. List all kmers.
    kmer_list_ = std::vector<std::string>( p );
    for( size_t i = 0; i < p; ++i ) {

        // Start with a dummy kmer of the correct size and the current index.
        auto kmer = std::string( k_, '#' );
        assert( kmer.size() == k_ );
        size_t c = i;

        // Fill the kmer from right to left, using conversion of c to base w.size().
        for( size_t j = 0; j < k_; ++j ) {
            assert(( 1 + j <= k_ ) && ( k_ - 1 - j < kmer.size() ));

            kmer[ k_ - 1 - j ] = w[ c % w.size() ];
            c /= w.size();
        }

        // In the last loop iteration, we processed all bits of c, so there should be nothing left.
        assert( c == 0 );

        kmer_list_[ i ] = kmer;
    }

    return kmer_list_;
}

size_t SignatureSpecifications::kmer_list_size() const
{
    return utils::int_pow( alphabet_.size(), k_ );
}

std::vector<size_t> const& SignatureSpecifications::kmer_combined_reverse_complement_map() const
{
    if( ! is_nucleic_acids() ) {
        throw std::runtime_error( "Reverse complement kmers only valid for nucleic acid codes." );
    }

    // Lazy loading.
    if( rev_comp_map_.size() > 0 ) {
        return rev_comp_map_;
    }

    // Calculate a vector that maps from a kmer index according to kmer_list
    // to a position in [ 0 , s ), so that rev comp indices map to the same position.
    rev_comp_map_ = std::vector<size_t>( utils::int_pow( 4, k_ ), 0 );

    // Store a list of kmers and their indices. it only stores one direction - that is,
    // if we see that the rev comp of a kmer is already in this map, we do not add anything.
    std::unordered_map<std::string, size_t> done;

    // Get all kmers as strings.
    auto const& list = kmer_list();
    assert( rev_comp_map_.size() == list.size() );

    // This uses string maps and is terribly slow.
    // But I am really lazy today and the function will not be called often.
    for( size_t i = 0; i < list.size(); ++i ) {
        auto const& seq = list[i];

        // We always encounter the alphabetically first entry fist.
        // So, we cannot have added its complement before.
        assert( done.count( seq ) == 0 );

        // If we have seen the rev comp of the current kmer before, reuse its index.
        // If not, make a new index, which is one higher than the currently highest index
        // (by using done.size() to get the number of current entries).
        auto const rev = reverse_complement( seq );
        if( done.count( rev ) == 0 ) {
            auto const ni = done.size();
            rev_comp_map_[ i ] = ni;
            done[ seq ] = ni;
        } else {
            // We have seen the rev comp before, so we do not update the done list.
            rev_comp_map_[ i ] = done[ rev ];
        }
    }

    // In the end, we have as many entries in done as we have rev comp kmers.
    assert( done.size() == kmer_reverse_complement_list_size() );

    return rev_comp_map_;
}

std::vector<size_t> const& SignatureSpecifications::kmer_reverse_complement_indices() const
{
    if( ! is_nucleic_acids() ) {
        throw std::runtime_error( "Reverse complement kmers only valid for nucleic acid codes." );
    }

    // Lazy loading.
    if( rev_comp_indices_.size() > 0 ) {
        return rev_comp_indices_;
    }

    // Get all kmers as strings.
    auto const& list = kmer_list();

    // Init the result list with max as an indicator that this value has yet to be set.
    rev_comp_indices_ = std::vector<size_t>( list.size(), std::numeric_limits<size_t>::max() );

    // Fill the list.
    for( size_t i = 0; i < list.size(); ++i ) {

        // We always set two values at a time for speeup, so some can be skipped.
        if( rev_comp_indices_[i] != std::numeric_limits<size_t>::max() ) {
            continue;
        }

        // Get the current kmer and its rev comp.
        auto const& seq = list[i];
        auto const  rev = reverse_complement( seq );

        // If they are the same, the index is also the identity map.
        // Otherwise, find the rev comp in the list.
        if( seq == rev ) {
            rev_comp_indices_[i] = i;
        } else {

            // Find the index of the rev compt entry.
            // It must exist, and is located after the current position i,
            // because the kmer list is sorted.
            auto const it = std::find( list.begin() + i, list.end(), rev );
            assert( it != list.end() );
            size_t const rci = std::distance( list.begin(), it );
            assert( rci < rev_comp_indices_.size() );

            // Set both indicies so that they point to each other.
            rev_comp_indices_[   i ] = rci;
            rev_comp_indices_[ rci ] = i;
        }
    }

    return rev_comp_indices_;
}

std::vector<std::string> const& SignatureSpecifications::kmer_reverse_complement_list() const
{
    if( ! is_nucleic_acids() ) {
        throw std::runtime_error( "Reverse complement kmers only valid for nucleic acid codes." );
    }

    // Lazy loading.
    if( rev_comp_list_.size() > 0 ) {
        return rev_comp_list_;
    }

    // Some properties.
    auto const& kl  = kmer_list();
    auto const& rci = kmer_combined_reverse_complement_map();
    auto const rcls = kmer_reverse_complement_list_size();
    assert( kl.size() == rci.size() );
    assert( rcls <= kl.size() );

    // Init result with empty strings.
    rev_comp_list_ = std::vector<std::string>( rcls, "" );

    // Fill the list by iterating all normal kmers.
    for( size_t i = 0; i < kl.size(); ++i ) {

        // Get index in rev comp list for the current kmer.
        auto const idx = rci[i];
        assert( idx < rev_comp_list_.size() );

        // If there is not yet an entry, set it.
        if( rev_comp_list_[ idx ].empty() ) {
            rev_comp_list_[ idx ] = kl[ i ];
        }
    }

    return rev_comp_list_;
}

size_t SignatureSpecifications::kmer_reverse_complement_list_size( bool with_palindromes ) const
{
    if( ! is_nucleic_acids() ) {
        throw std::runtime_error( "Reverse complement kmers only valid for nucleic acid codes." );
    }

    // Calcualtions according to: https://stackoverflow.com/a/40953130

    // Number of palindromic k-mers. For odd k, there are none, for even k, there are 4^(k/2) = 2^k
    auto const p = ( k_ % 2 == 1 ? 0 : utils::int_pow( 2, k_ ));

    // Number of entries needed to store rev comp kmers.
    if( with_palindromes ) {
        return p + ( utils::int_pow( 4, k_ ) - p ) / 2;
    } else {
        return ( utils::int_pow( 4, k_ ) - p ) / 2;
    }
}

} // namespace sequence
} // namespace genesis

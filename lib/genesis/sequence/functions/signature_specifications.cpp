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
//     Accessors
// ================================================================================================

size_t SignatureSpecifications::kmer_list_size() const
{
    return utils::int_pow( alphabet_.size(), k_ );
}

} // namespace sequence
} // namespace genesis

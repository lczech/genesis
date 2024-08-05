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
 * @ingroup population
 */

#include "genesis/population/function/genome_locus_set.hpp"

#include "genesis/sequence/formats/fastx_input_stream.hpp"
#include "genesis/sequence/formats/fasta_reader.hpp"
#include "genesis/sequence/sequence.hpp"
#include "genesis/utils/math/bitvector.hpp"
#include "genesis/utils/text/char.hpp"

#include <cassert>
#include <cstring>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Conversion
// =================================================================================================

genesis::sequence::SequenceDict reference_locus_set_to_dict( GenomeLocusSet const& set )
{
    genesis::sequence::SequenceDict result;
    for( auto const& elem : set ) {
        // The elements are bitvectors that contain an extra entry for the 0th bit,
        // which we hence need to subtract from the length here.
        size_t len = elem.second.size() - 1;
        if( elem.second.size() == 0 ) {
            len = 0;
        }
        result.add( elem.first, len );
    }
    return result;
}

// =================================================================================================
//     Mask Fasta Reading
// =================================================================================================

GenomeLocusSet read_mask_fasta(
    std::shared_ptr< utils::BaseInputSource > source,
    size_t mask_min,
    bool   invert
) {
    GenomeLocusSet result;

    // Boundary checks.
    if( mask_min > 9 ) {
        throw std::invalid_argument(
            "Fasta mask min value is " + std::to_string( mask_min ) + ", but has to be in [0-9]."
        );
    }

    // Read using a fasta input iterator
    for( auto const& seq : sequence::FastaInputStream( source )) {
        if( result.has_chromosome( seq.label() )) {
            throw std::runtime_error(
                "Duplicate sequence name \"" + seq.label() + "\" in " + source->source_name()
            );
        }

        // Make a bitvector of the correct size and fill it.
        // We use 1-based positions in the GenomeLocusSet, so we have to adjust for that.
        auto bv = utils::Bitvector( seq.length() + 1 );
        for( size_t i = 0; i < seq.length(); ++i ) {
            if( seq[i] < '0' || seq[i] > '9' ) {
                throw std::runtime_error(
                    "Invalid mask code '" + std::string( 1, seq[i] ) + "' not in [0-9] "
                    "in sequence \"" +  seq.label() + "\" in " + source->source_name()
                );
            }

            // Get the numerical value, and compare it to our min.
            size_t const val = seq[i] - '0';
            if( val > mask_min ) {
                bv.set( i + 1 );
            }
        }

        // If we invert, we do that here at the end. We could also switch in the set function
        // above, but it's easier to do this in bulk. We need to unset the first bit then.
        if( invert ) {
            bv.negate();
            bv.unset(0);
        }

        result.add( seq.label(), bv );
    }

    return result;
}

} // namespace population
} // namespace genesis

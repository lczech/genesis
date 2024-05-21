#ifndef GENESIS_POPULATION_FUNCTION_GENOME_LOCUS_SET_H_
#define GENESIS_POPULATION_FUNCTION_GENOME_LOCUS_SET_H_

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

#include "genesis/population/genome_locus_set.hpp"
#include "genesis/sequence/sequence_dict.hpp"
#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/io/input_stream.hpp"

#include <string>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Conversion
// =================================================================================================

genesis::sequence::SequenceDict reference_locus_set_to_dict( GenomeLocusSet const& set );

// =================================================================================================
//     Mask Fasta Reading
// =================================================================================================

/**
 * @brief Read an input source as a mask fasta file, and return its content as a GenomeLocusSet.
 *
 * The input is expected to be a FASTA-like mask file, e.g., to filter positions with. This mask
 * file contains a sequence of integer digits (between 0 and 9) for each position on a chromosome
 * that specify if a site at that position should be filtered/masked or not.
 *
 * An example mask file would look like:
 *
 *     >1
 *     0000011111222...
 *     >2
 *     2222211111000...
 *
 * In this example, the first 5 sites of the start of chromosome 1 are not masked, whereas sites at
 * position 6 onwards would be filtered out. And sites after the 11th position on chromosome 2 are
 * kept as well. (NB: The vcftools documentation as of 2023-03-29 states though that these sites
 * "would be filtered out as well", which seems like an error.)
 *
 * The @p mask_min argument specifies a threshold mask value between 0 and 9 to filter positions by.
 * The default threshold is 0, meaning only sites with that value or lower will be kept.
 * The @p invert argument flips the interpretation of masked/unmasked.
 *
 * Our internal representation of this data is to set the masked/filtered positions to `true` in
 * the underlying Bitvector, and the non-masked/kept positions to `false` by default. The argument
 * @p invert flips this. The special position `0` of the GenomeLocusSet is always set to `false`.
 *
 * See https://vcftools.github.io/man_latest.html for details.
 */
GenomeLocusSet read_mask_fasta(
    std::shared_ptr< utils::BaseInputSource > source,
    size_t mask_min = 0,
    bool   invert   = false
);

} // namespace population
} // namespace genesis

#endif // include guard

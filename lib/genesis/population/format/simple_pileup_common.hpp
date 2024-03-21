#ifndef GENESIS_POPULATION_FORMAT_SIMPLE_PILEUP_COMMON_H_
#define GENESIS_POPULATION_FORMAT_SIMPLE_PILEUP_COMMON_H_

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

#include "genesis/population/format/simple_pileup_reader.hpp"
#include "genesis/sequence/functions/quality.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Simple (m)pileup Reader Helper Functions
// =================================================================================================

SampleCounts convert_to_sample_counts(
    SimplePileupReader::Sample const& sample,
    unsigned char min_phred_score = 0
);

Variant convert_to_variant(
    SimplePileupReader::Record const& record,
    unsigned char min_phred_score = 0
);

/**
 * @brief Guess the quality score encoding for an (m)pileup input, based on counts of how often each
 * char appeared in the quality string (of the input pileup file for example).
 *
 * The function reads and parses the input source as a pileup file, counts all quality score chars
 * as they appear in there, and then guesses the encoding that was used.
 * If @p max_lines is set to a value greater than 0, only that many lines are read.
 * If @p max_chars is set to a value greater than 0, only that many quality score charaters are read.
 *
 * @see guess_quality_encoding()
 */
genesis::sequence::QualityEncoding guess_pileup_quality_encoding(
    std::shared_ptr< utils::BaseInputSource > source,
    size_t max_lines = 0
);

} // namespace population
} // namespace genesis

#endif // include guard

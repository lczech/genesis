#ifndef GENESIS_SEQUENCE_FUNCTIONS_DICT_H_
#define GENESIS_SEQUENCE_FUNCTIONS_DICT_H_

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

#include "genesis/sequence/reference_genome.hpp"
#include "genesis/sequence/sequence_dict.hpp"
#include "genesis/sequence/sequence_set.hpp"
#include "genesis/utils/io/input_source.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Sequence Dict
// =================================================================================================

/**
 * @brief Read a `.dict` sequence dictionary file, describing, e.g., reference genome sequence
 * properties.
 *
 * The file format is for example produced by `GATK/Picard CreateSequenceDictionary`
 * or by `samtools dict`. More information on the format can be found at:
 *
 *    * http://www.htslib.org/doc/samtools-dict.html
 *    * https://gatk.broadinstitute.org/hc/en-us/articles/360036729911-CreateSequenceDictionary-Picard-
 *
 * See there for details.
 */
SequenceDict read_sequence_dict( std::shared_ptr<utils::BaseInputSource> source );

/**
 * @brief Read a `.fai` sequence index file, describing, e.g., reference genome sequence properties.
 *
 * The file format is for example produced by `samtools faidx`.
 * More information on the format can be found at:
 *
 *    * http://www.htslib.org/doc/samtools-faidx.html
 *
 * See there for details.
 */
SequenceDict read_sequence_fai( std::shared_ptr<utils::BaseInputSource> source );

/**
 * @brief Get the sequence dict/index information of a given set of Sequence%s.
 *
 * This is a helper function that can be used to obtain an overview of the names and lengths
 * of a set of sequences, for some downstream processing, or as information about a reference genome.
 *
 * @see reference_genome_to_dict()
 */
SequenceDict sequence_set_to_dict( SequenceSet const& set );

/**
 * @brief Get the sequence dict/index information of a given set of Sequence%s that are stored
 * in a ReferenceGenome.
 *
 * This is a helper function that can be used to obtain an overview of the names and lengths
 * of a set of sequences, for some downstream processing, or as information about a reference genome.
 *
 * @see sequence_set_to_dict()
 */
SequenceDict reference_genome_to_dict( ReferenceGenome const& rg );

// =================================================================================================
//     Reference
// =================================================================================================

/**
 * @brief Chose how to deal with sub-/super-sets when comparing references.
 *
 * When comparing different reference genomes (e.g, fasta files), dictionaries (e.g., dict or fai
 * files), mask files, etc, we have to make a decision on whether to be strict about the sequences
 * contained in each of them. For instance, a reference genome might contain chromosomes "1" to "5",
 * while a mask file might only be containing a mask for chromosome "1". This mode enum selects what
 * to do in these cases: Do we want to allow one of the two comparands to be a sub- or super-set of
 * the other?
 *
 * Typically, we then want to compare if the sequence lengths of all sequences in the two sets match.
 * If that is not the case for the sequences that are in both sets, they are definitely not
 * compatible. See compatible_references() for a function where this is used.
 */
enum class ReferenceComparisonMode
{
    /**
     * @brief Both compared reference sets have to contain the exact same sequence names.
     */
    kStrict,

    /**
     * @brief The left hand reference set is allowed to contain sequences that are not in the
     * right hand side. These sequences are ignored in the comparison.
     */
    kLeftSuperset,

    /**
     * @brief The right hand reference set is allowed to contain sequences that are not in the
     * lefthand side. These sequences are ignored in the comparison.
     */
    kRightSuperset,

    /**
     * @brief Either reference set can contain sequences that are not in the other. Only the shared
     * ones are used for the comparison.
     */
    kSharedOnly
};

/**
 * @brief Verify that a SequenceDict fits a SequenceSet.
 *
 * The function return `true` iff the @p lhs has sequences of the same name and length,
 * in the same order, as the @p rhs, and `false` otherwise. This takes a @p mode into consideration,
 * to select how sequences are handled that are not in both sets. See ReferenceComparisonMode
 * for details.

 * Note that this function is written for two SequenceDict instances for simplicity. It is however
 * meant to be use to compare for instance ReferenceGenome or SequenceSet instances as well.
 * As the comparison is solely based on sequence names and lengths though, using this as a shared
 * data structure to which all of these types can be converted to makes it easier to use.
 * See sequence_set_to_dict() or reference_genome_to_dict() for these conversions for instance.
 */
bool compatible_references(
    SequenceDict const& lhs,
    SequenceDict const& rhs,
    ReferenceComparisonMode mode = ReferenceComparisonMode::kStrict
);

/**
 * @brief Verify that a SequenceDict fits a SequenceSet.
 *
 * The function return `true` iff the @p dict has sequences of the same name and length,
 * in the same order, as the @p set, and `false` otherwise.
 *
 * If @p match_first_word is set, we cut off the sequence names after the first tab or space
 * character, in case there are any, as this is what typical fasta indexing tools also seem to do.
 * In that case, anything after the first tab or space is not used for the comparison.
 */
bool verify( SequenceDict const& dict, SequenceSet const& set, bool match_first_word = false );

} // namespace sequence
} // namespace genesis

#endif // include guard

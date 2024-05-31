#ifndef GENESIS_POPULATION_FORMAT_SAM_FLAGS_H_
#define GENESIS_POPULATION_FORMAT_SAM_FLAGS_H_

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

#ifdef GENESIS_HTSLIB

#include <cstdint>
#include <string>

namespace genesis {
namespace population {

// =================================================================================================
//     SAM/BAM/CRAM Flags
// =================================================================================================

// Enum not used at the moment, as we instead simply use the htslib int bit sets.
// Keeping it here, in case it comes in useful in the future.

// /**
//  * @brief Flags for read properties as used in SAM/BAM/CRAM files.
//  *
//  * Each read in these file formats can be annotated by certain flags that mark its status:
//  *
//  * | Value | Name          | Description                                                     |
//  * | ----- | ------------- | --------------------------------------------------------------- |
//  * | 0x1   | PAIRED        | paired-end (or multiple-segment) sequencing technology          |
//  * | 0x2   | PROPER_PAIR   | each segment properly aligned according to the aligner          |
//  * | 0x4   | UNMAP         | segment unmapped                                                |
//  * | 0x8   | MUNMAP        | next segment in the template unmapped                           |
//  * | 0x10  | REVERSE       | SEQ is reverse complemented                                     |
//  * | 0x20  | MREVERSE      | SEQ of the next segment in the template is reverse complemented |
//  * | 0x40  | READ1         | the first segment in the template                               |
//  * | 0x80  | READ2         | the last segment in the template                                |
//  * | 0x100 | SECONDARY     | secondary alignment                                             |
//  * | 0x200 | QCFAIL        | not passing quality controls                                    |
//  * | 0x400 | DUP           | PCR or optical duplicate                                        |
//  * | 0x800 | SUPPLEMENTARY | supplementary alignment                                         |
//  *
//  * We use the same flag values as htslib, so these can be translated at will.
//  * Using an enum here instead of their direct values ensures a bit of typesafety on our end.
//  *
//  * See http://www.htslib.org/doc/samtools-flags.html and
//  * https://broadinstitute.github.io/picard/explain-flags.html for details.
//  */
// enum class SamFlag : int
// {
//     /**
//      * @brief The read is paired in sequencing, no matter whether it is mapped in a pair.
//      */
//     kPaired = 1,
//
//     /**
//      * @brief The read is mapped in a proper pair.
//      */
//     kProperPair = 2,
//     /**
//
//      * @brief The read itself is unmapped; conflictive with BAM_FPROPER_PAIR.
//      */
//     kUnmapped = 4,
//
//     /**
//      * @brief The mate is unmapped.
//      */
//     kMateUnmapped = 8,
//
//     /**
//      * @brief The read is mapped to the reverse strand.
//      */
//     kReverse = 16,
//
//     /**
//      * @brief The mate is mapped to the reverse strand.
//      */
//     kMateReverse = 32,
//
//     /**
//      * @brief This is read1.
//      */
//     kRead1 = 64,
//
//     /**
//      * @brief This is read2.
//      */
//     kRead2 = 128,
//
//     /**
//      * @brief Not primary alignment.
//      */
//     kSecondary = 256,
//
//     /**
//      * @brief QC failure.
//      */
//     kQcFail = 512,
//
//     /**
//      * @brief Optical or PCR duplicate.
//      */
//     kDuplicate = 1024,
//
//     /**
//      * @brief Supplementary alignment.
//      */
//     kSupplementary = 2048
// };

// =================================================================================================
//     Flag Helper Functions
// =================================================================================================

/**
 * @brief Parse a string as a set of flags for sam/bam/cram reads.
 *
 * The given string can either be the numeric value as specified by the sam standard, or given
 * as a list of flag names or values, which can be separated by comma, space, vertical bar,
 * or plus sign, and where each flag name is treated case-insensitive and without regarding
 * non-alpha-numeric characters. This is a more lenient parsing than what htslib and samtools offer.
 *
 * For example, it accepts:
 *
 *     1
 *     0x12
 *     PROPER_PAIR,MREVERSE
 *     ProperPair + MateReverse
 *     PROPER_PAIR | 0x20
 *
 * See http://www.htslib.org/doc/samtools-flags.html and
 * https://broadinstitute.github.io/picard/explain-flags.html for details.
 */
int string_to_sam_flag( std::string const& value );

/**
 * @brief Turn a set of flags for sam/bam/cram reads into their textual representation.
 *
 * This is useful for user output. We here use the format of names as used by htslib and samtools,
 * were names are upper case and words in flag names separated by underscores. This ensures
 * compatibility of the output with existing tools.
 *
 * See http://www.htslib.org/doc/samtools-flags.html and
 * https://broadinstitute.github.io/picard/explain-flags.html for details.
 */
std::string sam_flag_to_string( int flags );

} // namespace population
} // namespace genesis

#endif // htslib guard
#endif // include guard

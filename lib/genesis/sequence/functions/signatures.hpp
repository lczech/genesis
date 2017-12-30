#ifndef GENESIS_SEQUENCE_FUNCTIONS_SIGNATURES_H_
#define GENESIS_SEQUENCE_FUNCTIONS_SIGNATURES_H_

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

#include <string>
#include <vector>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Forwad Declarations
// =================================================================================================

class SignatureSpecifications;
class Sequence;
class SequenceSet;

// =================================================================================================
//     Signature Counts and Frequencies
// =================================================================================================

/**
 * @brief Count the occurences of k-mers in the @p sequence according to the @p settings.
 *
 * The function returns a vector that contains the count for each k-mer that can be build from
 * the characters in the alphabet, in the order given by SignatureSpecifications::kmer_list().
 */
std::vector<size_t> signature_counts(
    Sequence const&                sequence,
    SignatureSpecifications const& settings
);

/**
 * @brief Calculate the frequencies of occurences of k-mers in the @p sequence according to the
 * @p settings.
 *
 * The function simply calculates the frequencies as the normalized values of signature_counts().
 */
std::vector<double> signature_frequencies(
    Sequence const& sequence,
    SignatureSpecifications const& settings
);

// =================================================================================================
//     Signature Symmetrized
// =================================================================================================

/**
 * @brief Calcuate the symmetrized counts of the @p sequence according to the @p settings.
 *
 * The function uses signature_counts(), and sums up the counts of k-mers that are reverse
 * complements of each other.
 */
std::vector<size_t> signature_symmetrized_counts(
    Sequence const&                sequence,
    SignatureSpecifications const& settings
);

/**
 * @brief Calcuate the symmetrized counts of the @p sequence according to the @p settings.
 *
 * The function uses signature_frequencies(), and sums up the counts of k-mers that are reverse
 * complements of each other.
 */
std::vector<double> signature_symmetrized_frequencies(
    Sequence const&                sequence,
    SignatureSpecifications const& settings
);

// =================================================================================================
//     Signature Ranks
// =================================================================================================

/**
 * @brief Calcuate the rank signature of a @p sequence according to the @p settings.
 *
 * That is, @link utils::ranking_standard() standard ranking@endlink is applied to the
 * k-mer counts of the Sequence.
 */
std::vector<size_t> signature_ranks(
    Sequence const&                sequence,
    SignatureSpecifications const& settings
);

/**
 * @brief Calcuate the symmetrized rank signature of a @p sequence according to the @p settings.
 *
 * That is, @link utils::ranking_standard() standard ranking@endlink is applied to the
 * symmetrized (combined reverse complement) k-mer counts of the Sequence.
 */
std::vector<size_t> signature_symmetrized_ranks(
    Sequence const&                sequence,
    SignatureSpecifications const& settings
);

// =================================================================================================
//     Signature Min Max Reverse Complements
// =================================================================================================

/**
 * @brief Calculate the signature of a @p sequence that uses the minimum frequency of reverse
 * complement k-mers.
 *
 * The function is calculated according to
 *
 * > F. Gori, D. Mavroedis, M. S. M. Jetten, and E. Marchiori,
 * > “Genomic signatures for metagenomic data analysis:
 * > Exploiting the reverse complementarity of tetranucleotides,”
 * > 2011 IEEE Int. Conf. Syst. Biol. ISB 2011, pp. 149–154, 2011.
 *
 * It excludes palindromic k-mers where the reverse complement is the k-mer itself.
 */
std::vector<double> signature_minimal_complementarity_frequencies(
    Sequence const&                sequence,
    SignatureSpecifications const& settings
);

/**
 * @brief Calculate the signature of a @p sequence that uses the maximum frequency of reverse
 * complement k-mers.
 *
 * The function is calculated according to
 *
 * > F. Gori, D. Mavroedis, M. S. M. Jetten, and E. Marchiori,
 * > “Genomic signatures for metagenomic data analysis:
 * > Exploiting the reverse complementarity of tetranucleotides,”
 * > 2011 IEEE Int. Conf. Syst. Biol. ISB 2011, pp. 149–154, 2011.
 *
 * It excludes palindromic k-mers where the reverse complement is the k-mer itself.
 */
std::vector<double> signature_maximal_complementarity_frequencies(
    Sequence const&                sequence,
    SignatureSpecifications const& settings
);

/**
 * @brief Calculate the signature of a @p sequence that uses only the frequencies of k-mers
 * whose reverse complement is the k-mer itself.
 *
 * The function is calculated according to
 *
 * > F. Gori, D. Mavroedis, M. S. M. Jetten, and E. Marchiori,
 * > “Genomic signatures for metagenomic data analysis:
 * > Exploiting the reverse complementarity of tetranucleotides,”
 * > 2011 IEEE Int. Conf. Syst. Biol. ISB 2011, pp. 149–154, 2011.
 *
 * It excludes k-mers where the reverse complement is a different k-mer.
 */
std::vector<double> signature_reverse_identity_frequencies(
    Sequence const&                sequence,
    SignatureSpecifications const& settings
);

// =================================================================================================
//     Signature Misc
// =================================================================================================

/**
 * @brief Calculate the ratio 1 signature of a @p sequence.
 *
 * The function is calculated according to
 *
 * > F. Gori, D. Mavroedis, M. S. M. Jetten, and E. Marchiori,
 * > “Genomic signatures for metagenomic data analysis:
 * > Exploiting the reverse complementarity of tetranucleotides,”
 * > 2011 IEEE Int. Conf. Syst. Biol. ISB 2011, pp. 149–154, 2011.
 *
 * It excludes palindromic k-mers where the reverse complement is the k-mer itself.
 */
std::vector<double> signature_frequency_ratios_1(
    Sequence const&                sequence,
    SignatureSpecifications const& settings
);

/**
 * @brief Calculate the ratio 2 signature of a @p sequence.
 *
 * The function is calculated according to
 *
 * > F. Gori, D. Mavroedis, M. S. M. Jetten, and E. Marchiori,
 * > “Genomic signatures for metagenomic data analysis:
 * > Exploiting the reverse complementarity of tetranucleotides,”
 * > 2011 IEEE Int. Conf. Syst. Biol. ISB 2011, pp. 149–154, 2011.
 *
 * It excludes palindromic k-mers where the reverse complement is the k-mer itself.
 */
std::vector<double> signature_frequency_ratios_2(
    Sequence const&                sequence,
    SignatureSpecifications const& settings
);

/**
 * @brief Calculate the Jensen-Shannon (JS) signature of a @p sequence.
 *
 * The function is calculated according to
 *
 * > F. Gori, D. Mavroedis, M. S. M. Jetten, and E. Marchiori,
 * > “Genomic signatures for metagenomic data analysis:
 * > Exploiting the reverse complementarity of tetranucleotides,”
 * > 2011 IEEE Int. Conf. Syst. Biol. ISB 2011, pp. 149–154, 2011.
 *
 * using details of
 *
 * > J. Lin, “Divergence Measures Based on the Shannon Entropy,”
 * > IEEE Trans. Inf. Theory, vol. 37, no. 1, pp. 145–151, 1991.
 *
 * It excludes palindromic k-mers where the reverse complement is the k-mer itself.
 */
std::vector<double> signature_jensen_shannon(
    Sequence const&                sequence,
    SignatureSpecifications const& settings
);

} // namespace sequence
} // namespace genesis

#endif // include guard

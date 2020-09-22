#ifndef GENESIS_POPULATION_FORMATS_SIMPLE_PILEUP_READER_H_
#define GENESIS_POPULATION_FORMATS_SIMPLE_PILEUP_READER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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
 * @ingroup population
 */

#include "genesis/utils/io/input_source.hpp"
#include "genesis/sequence/functions/quality.hpp"

#include <string>
#include <vector>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace utils {
    class InputStream;
}

namespace population {

// =================================================================================================
//     Simple (m)pileup Reader
// =================================================================================================

/**
 * @brief Reader for line-by-line assessment of (m)pileup files.
 *
 * This simple reader processes (m)pileup files line by line. That is, it does not take into
 * consideration which read starts at which position, but instead gives a quick and simple
 * tally of the bases of all reads that cover a given position, as well as some other useful
 * information on read coverage, consensus character, and more.
 * This makes it fast in cases where only per-position, but no per-read information is needed.
 *
 * For each processed line, a SimplePileupReader::Record is produced, which captures the basic
 * information of the line, as well as a tally for each sample in the line, collected in
 * SimplePileupReader::Sample. One such sample consists of two or three columns in the file
 * (depending on whether the file uses quality scores or not).
 */
class SimplePileupReader
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    /**
     * @brief One sample in a pileup line/record.
     *
     * Each sample in a pileup file corresponds to the reads of one sample that cover a certain
     * position on a chromosome, and consists of two or three entries/columns in the file:
     *
     *  1. A read count.
     *  2. A list of bases (and some other information on read start and end etc) from the reads
     *     that cover the given position on the chromosome.
     *  3. (Optionally) A list of phread-scaled ASCII-encoded quality scores for the list of bases.
     *
     * As this here is a simple reader, we ignore the information on read starts/ends, as well as
     * potential insertions and deletions (indels), and instead simply tally up the number of
     * actual bases of the reads that cover a position. This Sample struct here collects this
     * information.
     */
    struct Sample
    {
        /**
         * @brief Total count of reads covering this position.
         *
         * This is the number given in the first column of each sample.
         * In a well-formed pileup file, this also corresponds to the number of actual bases
         * that are listed for the sample, that is `read_bases.size()`.
         *
         * In our simple reader here, this value is almost identical to the sum of all other
         * counters here (that is, `a_count + c_count + g_count + t_count + n_count + d_count`),
         * with the exception of RNA symbols (`<>`) in the string, that we ignore.
         * If those are needed as well, @ref read_bases can be scanned again to count them.
         */
        size_t read_coverage = 0;

        /**
         * @brief All bases (expect for indels) of the reads that cover the given position.
         *
         * These are the data of the second column of the sample, but without the read start/end
         * and indel data. Furthermore, the pileup notation for using the reference base
         * (`.` and `,`) is replaced by the actual reference base here.
         */
        std::string read_bases;

        /**
         * @brief Phread-scaled scores of the bases as given in `read_bases`.
         *
         * This is the data from the third column of the sample. It is only parsed and filled in
         * if with_quality_string() is set to `true` (default), in which case this data is expected
         * to be present in the file.
         */
        std::vector<unsigned char> phred_scores;

        /**
         * @brief Count of all `A` nucleotides that are present in the sample.
         */
        size_t a_count = 0;

        /**
         * @brief Count of all `C` nucleotides that are present in the sample.
         */
        size_t c_count = 0;

        /**
         * @brief Count of all `G` nucleotides that are present in the sample.
         */
        size_t g_count = 0;

        /**
         * @brief Count of all `T` nucleotides that are present in the sample.
         */
        size_t t_count = 0;

        /**
         * @brief Count of all `N` (undetermined/any) nucleotides that are present in the sample.
         */
        size_t n_count = 0;

        /**
         * @brief Count of all deleted (`*`) nucleotides that are present in the sample.
         */
        size_t d_count = 0;

        /**
         * @brief Count of the pure nucleotide bases at this position, that is,
         * the sum of all `A`, `C`, `G`, and `T`.
         *
         * This is simply the sum of `a_count + c_count + g_count + t_count`.
         *
         * NB: In PoPoolation, this variable is called `eucov`.
         */
        size_t nucleotide_count = 0;

        /**
         * @brief Is the Sample covered by enough reads/nucleotides?
         *
         * This value is set to `true` iff the total Sample::nucleotide_count (sum of Sample::a_count,
         * Sample::c_count, Sample::g_count, and Sample::t_count after testing that they are at least
         * min_count()) is in between the min_coverage() and max_coverage() values (inclusive),
         * and iff the amount of deletions (Sample::d_count) is not higher than min_count() as well
         * (unless, tolerate_deletions() is also set to `true`).
         *
         * That was a mouthful. Basically, a Sample is covered, if the sum of `A`, `C`, `G`, `T`
         * is in between min_coverage() and max_coverage(), and (unless we tolerate that) the amount
         * of deletions is not too high.
         */
        bool is_covered = false;

        /**
         * @brief Does the Sample have two or more alleles?
         *
         * That is the case if at least two of the `A`, `C`, `G`, `T` counts (Sample::a_count,
         * Sample::c_count, Sample::g_count, and Sample::t_count ) are above zero, after testing
         * that they are at least min_count().
         *
         * This value is also `false` if the amount of deletions (Sample::d_count) is too high
         * (using min_count() as the inclusive threshold), unless tolerate_deletions() is `true`.
         */
        bool is_snp = false;

        /**
         * @brief Is the Sample biallelic?
         *
         * This is closely related to Sample::is_snp, but only `true` iff the number of nucleotide
         * counts above zero is exactly two - that is, if there are only reads of two of `A`, `C`,
         * `G`, `T` in the sample.
         *
         * This value is also `false` if the amount of deletions (Sample::d_count) is too high
         * (using min_count() as the inclusive threshold), unless tolerate_deletions() is `true`.
         */
        bool is_biallelic = false;

        /**
         * @brief Is the Sample ignored due to high deletions count?
         *
         * This value is only set to `true` iff the Sample is well covered (as determined by
         * Sample::is_covered), but also has a high amount of deletions (at least min_count() many),
         * and if not also tolerate_deletions() is `true`.
         *
         * It is hence an indicator that there are too many deletions in the sample (if we decide
         * not to tolerate them).
         */
        bool is_ignored = false;

        /**
         * @brief Consensus character for the sample, if it is well covered.
         *
         * This is simply the character (out of `ACGT`) that appears most often (or, for ties,
         * the lexicographically smallest character). If the Sample is not well covered by reads
         * (that is, if Sample::is_covered is `false`), the consensus character is `N`.
         */
        char consensus_character = 'N';

        /**
         * @brief Confidence of the consensus character.
         *
         * The confidence is the count of the consensus character, divided by the total count
         * of all four nucleotides.
         */
        double consensus_confidence = 0.0;
    };

    /**
     * @brief Single line/record from a pileup file.
     *
     * A line consists of a chromosome and position, the reference base at that position,
     * as well as one or more Sample%s, that is, information on reads (and optionally, their quality
     * scores) that cover that position. See Sample for details on the latter.
     */
    struct Record
    {
        std::string         chromosome;
        size_t              position;
        char                reference_base;
        std::vector<Sample> samples;
    };

    using self_type         = SimplePileupReader;
    using value_type        = Record;
    using pointer           = value_type&;
    using reference         = value_type*;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    SimplePileupReader() = default;
    ~SimplePileupReader() = default;

    SimplePileupReader( self_type const& ) = delete;
    SimplePileupReader( self_type&& )      = default;

    self_type& operator= ( self_type const& ) = delete;
    self_type& operator= ( self_type&& )      = default;

    // ---------------------------------------------------------------------
    //     Reading
    // ---------------------------------------------------------------------

    std::vector<Record> read( std::shared_ptr< utils::BaseInputSource > source ) const;

    // -------------------------------------------------------------------------
    //     Parsing
    // -------------------------------------------------------------------------

    bool parse_line(
        utils::InputStream& input_stream,
        Record&             record
    ) const;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    bool with_quality_string() const
    {
        return with_quality_string_;
    }

    /**
     * @brief Set whether to expect a phred-scaled, ASCII-encoded quality code string per
     * sample.
     *
     * A typical line from a pileup file looks like
     *
     *     seq1 272 T 24  ,.$.....,,.,.,...,,,.,..^+. <<<+;<<<<<<<<<<<=<;<;7<&
     *
     * with the last field being quality codes. However, this last field is optional, and hence
     * we offer this option. If `true` (default), the field is expected to be there; if `false`,
     * it is expected not to be there. That is, at the moment, we have no automatic setting for this.
     */
    self_type& with_quality_string( bool value )
    {
        with_quality_string_ = value;
        return *this;
    }

    sequence::QualityEncoding quality_encoding() const
    {
        return quality_encoding_;
    }

    /**
     * @brief Set the type of encoding for the quality code string.
     *
     * If with_quality_string() is set to `true` (default), this encoding is used to transform
     * the ASCII-encoded string into actual phred-scaled scores. See
     * sequence::quality_decode_to_phred_score() for details.
     */
    self_type& quality_encoding( sequence::QualityEncoding value )
    {
        quality_encoding_ = value;
        return *this;
    }

    unsigned char min_phred_score() const
    {
        return min_phred_score_;
    }

    /**
     * @brief Set the minimum phred-scaled quality score for taking a base into the tally.
     *
     * For each pileup sample in a given record, we compute a tally, that is, we sum up the number
     * of occurences of each nucleotide at the given line, and store these as counts in Sample;
     * see there for details.
     *
     * Using this setting, only bases that have at least the given phred-scaled quality score
     * are taken into account when summing up that tally.
     */
    self_type& min_phred_score( unsigned char value )
    {
        min_phred_score_ = value;
        return *this;
    }

    size_t min_count() const
    {
        return min_count_;
    }

    /**
     * @brief Minimum count that we need for a type of nucleotide (`A`, `C`, `G`, `T`)
     * to be considered in the tally.
     *
     * After tallying up all nucleotides for a given sample, this value is used as a type
     * of quality control filter. All nucleotide counts (that is, Sample::a_count, Sample::c_count,
     * Sample::g_count, and Sample::t_count) that are below this value are set to zero.
     * Only after this step is then the value for their total sum (Sample::nucleotide_count)
     * computed.
     *
     * Furthermore, this value is used to determine whether a Sample has too many deletions,
     * and unless tolerate_deletions() is set to `true`, the Samole::is_ignored will be set to `true`
     * in that case (too many deletions, as given by Sample::d_count), while the values for
     * Sample::is_covered, Sample::is_snp, and Sample::is_biallelic will be set to `false`.
     */
    self_type& min_count( size_t value )
    {
        min_count_ = value;
        return *this;
    }

    size_t min_coverage() const
    {
        return min_coverage_;
    }

    /**
     * @brief Minimum coverage expected for a Sample to be considered "covered".
     *
     * The number of nucleotides (`A`, `C`, `G`, `T`) in the reads of a sample is stored in
     * Sample::nucleotide_count. If that value is less then the here provided min_coverage,
     * then the Sample is not considered sufficiently covered, and the Sample::is_covered flag
     * will be set to `false`.
     *
     * See also max_coverage() for the upper bound on coverage. Only if the nucleotide count is
     * in between (or equal to either) these two counds, it is considered to be covered, and
     * Sample::is_covered will be set to `true`.
     */
    self_type& min_coverage( size_t value )
    {
        min_coverage_ = value;
        return *this;
    }

    size_t max_coverage() const
    {
        return max_coverage_;
    }

    /**
     * @brief Minimum coverage expected for a Sample to be considered "covered".
     *
     * The number of nucleotides (`A`, `C`, `G`, `T`) in the reads of a sample is stored in
     * Sample::nucleotide_count. If that value is greater then the here provided max_coverage,
     * then the Sample is not considered sufficiently covered, and the Sample::is_covered flag
     * will be set to `false`.
     *
     * If provided with a value of `0` (default), max_coverage is not used.
     *
     * See also min_coverage() for the lower bound on coverage. Only if the nucleotide count is
     * in between (or equal to either) these two counds, it is considered to be covered, and
     * Sample::is_covered will be set to `true`.
     */
    self_type& max_coverage( size_t value )
    {
        max_coverage_ = value;
        return *this;
    }

    bool tolerate_deletions() const
    {
        return tolerate_deletions_;
    }

    /**
     * @brief Set whether we tolerate Sample%s with a high amount of deletions.
     *
     * If set to `false` (default), we do not tolerate deletions. In that case, if the number of
     * deletions in a Sample (given by Sample::d_count) is higher than or equal to min_count(),
     * the Sample will be considered ignored (Sample::is_ignored set to `true`), and considered
     * not covered (Sample::is_covered, Sample::is_snp, and Sample::is_biallelic will all be set
     * to `false`).
     *
     * If however set to `true`, we tolerate high amounts of deletions, and the values for the above
     * properties will be set as usual by considering the nucleotide counts (Sample::a_count,
     * Sample::c_count, Sample::g_count, and Sample::t_count) instead.
     */
    self_type& tolerate_deletions( bool value )
    {
        tolerate_deletions_ = value;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    bool parse_line_(
        utils::InputStream& input_stream,
        Record&             record
    ) const;

    void process_sample_(
        utils::InputStream& input_stream,
        Record&             record,
        size_t              index
    ) const;

    void parse_sample_fields_(
        utils::InputStream& input_stream,
        Record&             record,
        Sample&             sample
    ) const;

    void tally_sample_counts_(
        utils::InputStream& input_stream,
        Sample&             sample
    ) const;

    void compute_sample_consensus_(
        Sample&             sample
    ) const;

    void next_field_(
        utils::InputStream& input_stream
    ) const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // Set whether the file contains the base quality score column, and if so, how its encoded
    // (we default to Sanger with offset 33), and if we want to skip low quality bases.
    bool with_quality_string_ = true;
    sequence::QualityEncoding quality_encoding_ = sequence::QualityEncoding::kSanger;
    unsigned char min_phred_score_ = 0;

    // Other quality control settings.
    size_t min_count_ = 0;
    size_t min_coverage_ = 0;
    size_t max_coverage_ = 0;
    bool tolerate_deletions_ = false;

};

} // namespace population
} // namespace genesis

#endif // include guard

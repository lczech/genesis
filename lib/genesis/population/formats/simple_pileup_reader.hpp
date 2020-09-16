#ifndef GENESIS_POPULATION_FORMATS_SIMPLE_PILEUP_INPUT_ITERATOR_H_
#define GENESIS_POPULATION_FORMATS_SIMPLE_PILEUP_INPUT_ITERATOR_H_

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
 * @brief Reader for line-by-line assessment of pileup files.
 *
 * This simple reader processes pileup files line by line. That is, it does not take into
 * consideration which read starts at which position, but instead gives a quick and simple
 * tally of the bases of all reads that cover a given position.
 * This makes it fast in cases where no per-read information is needed.
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
     * position on a chromosome, and consists of two or three entries:
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
         * @brief Count of all `A` nucleotides that are present in the sample line.
         */
        size_t a_count = 0;

        /**
         * @brief Count of all `C` nucleotides that are present in the sample line.
         */
        size_t c_count = 0;

        /**
         * @brief Count of all `G` nucleotides that are present in the sample line.
         */
        size_t g_count = 0;

        /**
         * @brief Count of all `T` nucleotides that are present in the sample line.
         */
        size_t t_count = 0;

        /**
         * @brief Count of all `N` (undetermined/any) nucleotides that are present in the sample line.
         */
        size_t n_count = 0;

        /**
         * @brief Count of all deleted (`*`) nucleotides that are present in the sample line.
         */
        size_t d_count = 0;

        /**
         * @brief Count of the pure nucleotide bases at this position, that is,
         * the sum of all `A`, `C`, `G`, and `T`.
         *
         * This is simply the sum of `a_count + c_count + g_count + t_count`.
         */
        size_t nucleotide_count = 0;
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
     * @brief Minimum count that we need for a type of nucleotide to be considered.
     *
     * After tallying up all nucleotides for a given sample, this value is used as another type
     * of quality control filter. All nucleotide counts (that is, Sample::a_count, Sample::c_count,
     * Sample::g_count, and Sample::t_count) that are below this value are set to zero.
     */
    self_type& min_count( size_t value )
    {
        min_count_ = value;
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

    void parse_sample_(
        utils::InputStream& input_stream,
        Record&             record,
        size_t              index
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

};

} // namespace population
} // namespace genesis

#endif // include guard
